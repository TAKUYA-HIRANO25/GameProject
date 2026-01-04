#include "Model.h"
#include "ModelCommon.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename)
{
	this->modelCommon = modelCommon;

	// モデル読み込み
	modelData = LoadObjFile(directorypath, filename);

	// 頂点リソースを作る
	vertexResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	if (!modelData.vertices.empty()) {
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	}

	// （モデル側の共有マテリアルは残すが、描画時のバインドは呼び出し側に任せる）
	materialResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	Vector4 color{ 1.0f,1.0f,1.0f,1.0f };
	materialData->color = color;
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();

	// .objの参照しているテスクチャファイルの読み込み（あれば）
	if (!modelData.material.textureFilePath.empty()) {
		TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
		// 読み込んだテクスチャの番号を取得
		modelData.material.textureIndex =
			TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
	}
}

void Model::Draw()
{
	// VertexBufferViewを設定
	modelCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	// --- デバッグ: textureIndex を確認 ---
	{
		uint32_t texIdx = modelData.material.textureIndex;
		// textureIndex がロード済みテクスチャの範囲内か確認（GetSrvHandleGPU は範囲外で assert する）
		// 簡易ログ
		char buf[128];
		sprintf_s(buf, "Model::Draw(): textureIndex=%u\n", texIdx);
		OutputDebugStringA(buf);
	}

	// SRVのDescriptorTarbleの先頭を設定
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(
		2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureIndex));

	// 描画！（DrawCall/ドローコール)
	modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

// ------------------------------------------------------------
// 簡易 OBJ / MTL ローダ（最低限の TRIANGLE 出力を行う実装）
// - 頂点データ (v/vt/vn) を読み取り、フェースをトライアングル化して ModelData.vertices に格納する。
// - mtllib -> mtl ファイル内の map_Kd を探して material.textureFilePath を設定する。
// ※ 本実装はフル機能の OBJ ローダではありませんが、リンクエラー解消と簡易なモデル読込を目的としています。
// ------------------------------------------------------------
static std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) elems.push_back(item);
	return elems;
}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData out{};
	std::string fullpath = directoryPath + "/" + filename;
	std::ifstream ifs(fullpath);
	if (!ifs.is_open()) {
		// ファイル開けなければ空の ModelData を返す
		return out;
	}

	std::vector<Vector4> positions;
	std::vector<Vector2> texcoords;
	std::vector<Vector3> normals;

	std::string line;
	std::string lastMtllib;
	while (std::getline(ifs, line)) {
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v") {
			Vector4 p{ 0,0,0,1.0f };
			iss >> p.x >> p.y >> p.z;
			positions.push_back(p);
		}
		else if (prefix == "vt") {
			Vector2 uv{ 0,0 };
			iss >> uv.x >> uv.y;
			texcoords.push_back(uv);
		}
		else if (prefix == "vn") {
			Vector3 n{ 0,0,0 };
			iss >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}
		else if (prefix == "f") {
			// face: 三角形またはポリゴン（ここではポリゴンをトライアングルファンで分解）
			std::vector<std::string> tokens;
			std::string vtok;
			while (iss >> vtok) tokens.push_back(vtok);
			if (tokens.size() < 3) continue;
			// 三角形ファンで分解
			for (size_t i = 1; i + 1 < tokens.size(); ++i) {
				std::string t0 = tokens[0];
				std::string t1 = tokens[i];
				std::string t2 = tokens[i + 1];
				std::vector<std::string> tri = { t0,t1,t2 };
				for (const auto& tv : tri) {
					// v/vt/vn or v//vn or v/vt
					auto parts = split(tv, '/');
					int vi = 0, vti = 0, vni = 0;
					if (!parts.empty() && !parts[0].empty()) vi = std::stoi(parts[0]) - 1;
					if (parts.size() > 1 && !parts[1].empty()) vti = std::stoi(parts[1]) - 1;
					if (parts.size() > 2 && !parts[2].empty()) vni = std::stoi(parts[2]) - 1;

					VertexData vd{};
					// position
					if (vi >= 0 && vi < (int)positions.size()) vd.position = positions[vi];
					else vd.position = { 0,0,0,1.0f };
					// texcoord
					if (vti >= 0 && vti < (int)texcoords.size()) vd.texcoord = texcoords[vti];
					else vd.texcoord = { 0.0f, 0.0f };
					// normal
					if (vni >= 0 && vni < (int)normals.size()) vd.normal = normals[vni];
					else vd.normal = { 0.0f, 0.0f, 0.0f };

					out.vertices.push_back(vd);
				}
			}
		}
		else if (prefix == "mtllib") {
			// マテリアルファイル名
			std::string mtlname;
			iss >> mtlname;
			lastMtllib = mtlname;
		}
		else if (prefix == "usemtl") {
			// 現状は無視（シンプル実装）
		}
	}

	// mtl が見つかっていれば読み込む
	if (!lastMtllib.empty()) {
		// 重要: filename がサブディレクトリを含む場合、mtl の相対参照は
		// OBJ ファイルが置かれているディレクトリからの相対になる。
		// そのため OBJ の所在ディレクトリを組み立てて渡す。
		std::string objContainingDir = directoryPath;
		// filename にサブパスが含まれていればそれを追加 ("Player/Player.obj" -> "resources/Player")
		const size_t posSlash = filename.find_last_of("/\\");
		if (posSlash != std::string::npos) {
			std::string subdir = filename.substr(0, posSlash);
			objContainingDir += "/" + subdir;
		}
		// LoadMaterialTemplateFile は第一引数に「mtl が存在するディレクトリ」を期待するように変更せず使う
		out.material = LoadMaterialTemplateFile(objContainingDir, lastMtllib);
	}

	return out;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData out{};
	std::string fullpath = directoryPath + "/" + filename;
	std::ifstream ifs(fullpath);
	if (!ifs.is_open()) {
		return out;
	}

	// mtl ファイルが存在するディレクトリを取得（"resources/Player/Player.mtl" -> "resources/Player"）
	std::string mtlDirectory;
	{
		// fullpath の最後の '/' または '\\' の位置を探す
		const size_t posSlash = fullpath.find_last_of("/\\");
		if (posSlash != std::string::npos) {
			mtlDirectory = fullpath.substr(0, posSlash);
		}
		else {
			// 予備: directoryPath を使う（フォールバック）
			mtlDirectory = directoryPath;
		}
	}

	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		// map_Kd が diffuse テクスチャの指定（簡易対応）
		if (prefix == "map_Kd") {
			std::string texpath;
			iss >> texpath;

			// テクスチャパスが絶対パスやドライブ指定を含む場合、そのまま使う
			bool isAbsolute = false;
#if defined(_WIN32)
			if (texpath.size() >= 2 && texpath[1] == ':') isAbsolute = true; // C:\...
#endif
			if (!texpath.empty() && (texpath.front() == '/' || texpath.front() == '\\')) isAbsolute = true;

			if (isAbsolute) {
				out.textureFilePath = texpath;
			}
			else {
				// mtl が置かれているディレクトリを基準に相対パスを解決
				out.textureFilePath = mtlDirectory + "/" + texpath;
			}

			// 正常に見つけたのでループ終了
			break;
		}
	}
	return out;
}