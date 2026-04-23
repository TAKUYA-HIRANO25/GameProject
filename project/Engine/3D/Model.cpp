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
	// 頂点バッファビューを作成
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	if (!modelData.vertices.empty()) {
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	}

	// モデル側の共有マテリアルは残すが、描画時のバインドは呼び出し側に任せる
	materialResource = modelCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	Vector4 color{ 1.0f,1.0f,1.0f,1.0f };
	materialData->color = color;
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();

	// objの参照しているテスクチャファイルの読み込み
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

	// SRVのDescriptorTarbleの先頭を設定
	modelCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->
		GetSrvHandleGPU(modelData.material.textureIndex));

	// 描画
	modelCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

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
		// ファイル開けなければ空のModelDataを返す
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
			// 三角形またはポリゴン
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
					if (vti >= 0 && vti < (int)texcoords.size()) {
                        // OBJのVは下原点の場合が多いDirectXのテクスチャ座標と合わせるため反転
                        vd.texcoord = texcoords[vti];
                        vd.texcoord.y = 1.0f - vd.texcoord.y;
                    }
                    else {
                        vd.texcoord = { 0.0f, 0.0f };
                    }
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

	// mtlが見つかっていれば読み込む
	if (!lastMtllib.empty()) {
		std::string objContainingDir = directoryPath;
		//filename にサブパスが含まれていればそれを追加
		const size_t posSlash = filename.find_last_of("/\\");
		if (posSlash != std::string::npos) {
			std::string subdir = filename.substr(0, posSlash);
			objContainingDir += "/" + subdir;
		}
		// LoadMaterialTemplateFileは第一引数にmtlが存在するディレクト使う
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

	// mtlファイルが存在するディレクトリを取得
	std::string mtlDirectory;
	{
		// fullpath最後の/または\\の位置を探す
		const size_t posSlash = fullpath.find_last_of("/\\");
		if (posSlash != std::string::npos) {
			mtlDirectory = fullpath.substr(0, posSlash);
		}
		else {

			mtlDirectory = directoryPath;
		}
	}

	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		// map_Kdがdiffuseテクスチャの指定
		if (prefix == "map_Kd") {
			std::string texpath;
			iss >> texpath;

			// テクスチャパスが絶対パスやドライブ指定を含む場合、そのまま使う
			bool isAbsolute = false;
#if defined(_WIN32)
			if (texpath.size() >= 2 && texpath[1] == ':') isAbsolute = true;
#endif
			if (!texpath.empty() && (texpath.front() == '/' || texpath.front() == '\\')) isAbsolute = true;

			if (isAbsolute) {
				out.textureFilePath = texpath;
			}
			else {
				// mtlが置かれているディレクトリを基準に相対パスを解決
				out.textureFilePath = mtlDirectory + "/" + texpath;
			}

			// 正常に見つけたのでループ終了
			break;
		}
	}
	return out;
}