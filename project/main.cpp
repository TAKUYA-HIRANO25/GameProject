#define _USE_MATH_DEFINES
#include <math.h>
#include "MyMath.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "D3DResourceLeakChecker.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "ParticleManager.h"
#include "RailCamera.h"
#pragma comment(lib,"dxcompiler.lib")

//球
struct Sphere {
	Vector3 center;
	float radius;
};
//球
void DrawSphere(Sprite::VertexData* vertexData, uint32_t Subdivision) {
	const uint32_t kSubdivision = Subdivision;
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);//経度 φ
	const float kLatEvery = float(M_PI) / float(kSubdivision);	//緯度 θ


	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = float(M_PI) / 2.0f + kLatEvery * latIndex;//θ

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//φ

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			Sprite::VertexData vertA = {
				{
					cos(lat) * cos(lon) ,
					sin(lat),
					cos(lat) * sin(lon),
					1.0f
				},
				{ float(lonIndex) / float(kSubdivision), 1.0f + float(latIndex) / float(kSubdivision) },
				{
					cos(lat) * cos(lon) ,
					sin(lat),
					cos(lat) * sin(lon),
				},
			};
			Sprite::VertexData vertB = {
				{
					cos(lat + kLatEvery) * cos(lon) ,
					sin(lat + kLatEvery),
					cos(lat + kLatEvery) * sin(lon),
					1.0f
				} ,
				{ float(lonIndex) / float(kSubdivision), 1.0f + float(latIndex + 1) / float(kSubdivision) },
				{
					cos(lat + kLatEvery) * cos(lon) ,
					sin(lat + kLatEvery),
					cos(lat + kLatEvery) * sin(lon),
				} ,
			};
			Sprite::VertexData vertC = {
				{
					cos(lat) * cos(lon + kLonEvery) ,
					sin(lat),
					cos(lat) * sin(lon + kLonEvery),
					1.0f
				},
				{ float(lonIndex + 1) / float(kSubdivision), 1.0f + float(latIndex) / float(kSubdivision) },
				{
					cos(lat) * cos(lon + kLonEvery) ,
					sin(lat),
					cos(lat) * sin(lon + kLonEvery),
				},
			};
			Sprite::VertexData vertD = {
				{
					cos(lat + kLatEvery) * cos(lon + kLonEvery),
					sin(lat + kLatEvery),
					cos(lat + kLatEvery) * sin(lon + kLonEvery),
					1.0f
				},
				{ float(lonIndex + 1) / float(kSubdivision), 1.0f + float(latIndex + 1) / float(kSubdivision) } ,
				{
					cos(lat + kLatEvery) * cos(lon + kLonEvery),
					sin(lat + kLatEvery),
					cos(lat + kLatEvery) * sin(lon + kLonEvery),
				},
			};

			vertexData[start + 5] = vertA;  //左下 A

			vertexData[start + 4] = vertB;  //上 B

			vertexData[start + 3] = vertC;  //右下 C

			vertexData[start + 2] = vertC;  //左下2 C

			vertexData[start + 1] = vertB;  //上2 B

			vertexData[start + 0] = vertD;  //右下2 D

		}
	}
}
//Transform
Sprite::Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
Sprite::Transform cameraTransfprm{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3DResourceLeakChecker leakChek;
	//WinAPI
#pragma region 
	WinApp* winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();

#pragma endregion
	//DirectCommon
#pragma region 
	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
#pragma endregion
	//キー入力
#pragma region
	Input* input;
	input = new Input();
	input->Initialize(winApp);
#pragma endregion
	//テクスチャー
#pragma region
	TextureManager::GetInstance()->Initialize(dxCommon);
	// Textureを読んで転送する
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");

#pragma endregion
	//スプライト
#pragma region
	SpriteCommon* spriteCommon;
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

#pragma endregion
	//モデル
#pragma region
	ModelCommon* modelCommon = nullptr;
	modelCommon = new ModelCommon;
	modelCommon->Initialize(dxCommon);

	Model* model = nullptr;
	model = new Model;
	//model->Initialize(modelCommon, "resources", "plane.obj");

	ObJect3dCommon* object3dCommon = nullptr;
	object3dCommon = new ObJect3dCommon;
	object3dCommon->Initialize(dxCommon);
#pragma endregion
	//カメラ
#pragma region
	Camera* camera_ = new Camera;
	camera_->SetRotate({ 0.0f, 6.28f,0.0f });
	camera_->SetTranslate({ 0.0f,0.0f,-20.0f });
	object3dCommon->SetDefaultCamera(camera_);

#pragma endregion
	//レールカメラ
#pragma region
	RailCamera* railCamera = new RailCamera();
	railCamera->Initialize(camera_);
	object3dCommon->SetDefaultCamera(railCamera->GetCamera());
#pragma endregion
	//タイトル
#pragma region
	bool isTitle = true;
	TextureManager::GetInstance()->LoadTexture("resources/title.png");
	TextureManager::GetInstance()->LoadTexture("resources/titleUI.png");
	TextureManager::GetInstance()->LoadTexture("resources/backGround.png");
	Sprite* title = new Sprite();
	title->Initialize(spriteCommon, "resources/title.png");
	Sprite* titleUI = new Sprite();
	titleUI->Initialize(spriteCommon, "resources/titleUI.png");
	Sprite* backGround = new Sprite();
	backGround->Initialize(spriteCommon, "resources/backGround.png");
	backGround->SetSize(Vector2(1280, 720));
	int titleTime = 0;
#pragma endregion
	//フェード
#pragma region
	bool isFade = false;
	bool endFade = false;
	Sprite* fadeSprite = new Sprite();
	Vector2 size = { 0,0 };
	Vector2 position = { 630,360 };
	float speedx = 16.0f;
	float speedy = 10.0f;
	float time = 0.0f;
	TextureManager::GetInstance()->LoadTexture("resources/Fade.png");
	fadeSprite->Initialize(spriteCommon, "resources/Fade.png");
	fadeSprite->SetSize(Vector2(0, 0));
	fadeSprite->SetPosition(Vector2(630, 360));
	//fadeSprite->SetColor(Vector4(0, 0, 0, 1));
#pragma endregion
	// スタート演出
#pragma region
	TextureManager::GetInstance()->LoadTexture("resources/Ready.png");
	TextureManager::GetInstance()->LoadTexture("resources/GO.png");
	Sprite* Ready = new Sprite();
	Ready->Initialize(spriteCommon, "resources/Ready.png");
	Sprite* Go = new Sprite();
	Go->Initialize(spriteCommon, "resources/GO.png");
	bool isStart = false;
	bool isGo = false;
	int startTime = 0;
	int goTime = 0;
#pragma endregion
	//ゲーム画面
#pragma region
	bool isGame = false;
	bool reup = false;
#pragma endregion
	//ゲームオーバー
#pragma region
	TextureManager::GetInstance()->LoadTexture("resources/GameOver.png");
	TextureManager::GetInstance()->LoadTexture("resources/Over.png");
	Sprite* gameOver = new Sprite();
	gameOver->Initialize(spriteCommon, "resources/GameOver.png");
	bool isOver = false;

	bool GameOverFlag = false;

	Sprite* Black = new Sprite();
	Black->Initialize(spriteCommon, "resources/backGround.png");
	Black->SetSize(Vector2(1280, 720));
	Black->SetColor(Vector4(1, 1, 1, 0));
	float blackAlpha = 0.0f;
#pragma endregion
	//クリア
#pragma region
	TextureManager::GetInstance()->LoadTexture("resources/Clear.png");
	Sprite* clear = new Sprite();
	clear->Initialize(spriteCommon, "resources/Clear.png");
	bool isClear = false;
#pragma endregion
	//モデル
#pragma region
	ModelManager::GetInstance()->Initialize(dxCommon);

	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("box.obj");
	ModelManager::GetInstance()->LoadModel("Bullet.obj");
	ModelManager::GetInstance()->LoadModel("Particle.obj");
	// 異なるモデルを持つオブジェクトを生成
	/*
	Object3d* planeObject = new Object3d;
	planeObject->Initialize(object3dCommon);
	planeObject->SetModel("plane.obj");
	planeObject->SetTranslate(Vector3(-2.0f, -1.0f, 0.0f));

	Object3d* axisObject = new Object3d;
	axisObject->Initialize(object3dCommon);
	axisObject->SetModel("axis.obj");
	axisObject->SetTranslate(Vector3(2.0f, 1.0f, 0.0f));
	*/

#pragma endregion
	//プレイヤー
#pragma region
	Player* player = new Player();
	player->Initialize(object3dCommon,input);
	player->SetRailCameraVelocity(railCamera->GetVelocity());
#pragma endregion
	//敵
#pragma region
	Enemy* enemy = new Enemy();
	Vector3 enemyPos = { 0.0f,-4.0f,40.0f };
	enemy->Initialize(object3dCommon, enemyPos);
	enemy->setPlayer(player);
#pragma endregion
	//当たり判定
#pragma region

#pragma endregion
	//パーティクル
#pragma region
	TextureManager::GetInstance()->LoadTexture("resources/Particle.png");
	ParticleManager* particleManager = new ParticleManager(object3dCommon);
	player->SetParticleManager(particleManager);
	enemy->SetParticleManager(particleManager);
#pragma endregion
	//天球
#pragma region
	TextureManager::GetInstance()->LoadTexture("resources/sky.png");
	ModelManager::GetInstance()->LoadModel("sphere.obj");
	Object3d* skyDome = new Object3d;
	skyDome->Initialize(object3dCommon);
	skyDome->SetModel("sphere.obj");
#pragma endregion
	//スフィア用リソース
#pragma region
	/*const uint32_t Subdivision = 16;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere = dxCommon->CreateBufferResource( sizeof(Sprite::VertexData) * Subdivision * Subdivision * 6);
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSphere = dxCommon->CreateBufferResource( sizeof(Matrix4x4));
	Matrix4x4* wvpDataSphere = nullptr;
	wvpResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSphere));
	*wvpDataSphere = MakeIdentity4x4();
	//頂点バッファビューを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};
	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
	vertexBufferViewSphere.SizeInBytes = sizeof(Sprite::VertexData) * Subdivision * Subdivision * 6;
	vertexBufferViewSphere.StrideInBytes = sizeof(Sprite::VertexData);
	//頂点リソースに書き込み
	Sprite::VertexData* vertexDataSphere = nullptr;
	vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));
	DrawSphere(vertexDataSphere, Subdivision);
	//Transform
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSphere = dxCommon->CreateBufferResource( sizeof(Sprite::TransformationMatrix));
	Sprite::TransformationMatrix* transformationMatrixDataSphere = nullptr;
	transformationMatrixResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSphere));
	transformationMatrixDataSphere->World = MakeIdentity4x4();*/

	//平行光源
	/*Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectiomalLight));
	DirectiomalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;*/

	//スフィア用Transform
	/*struct Sprite::Transform transformSphere { { 1.0f, 1.0f, 1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,5.0f} };

	//スフィア用インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSphere = dxCommon->CreateBufferResource( sizeof(uint32_t) * 6);
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};
	indexBufferViewSphere.BufferLocation = indexResourceSphere->GetGPUVirtualAddress();
	indexBufferViewSphere.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewSphere.Format = DXGI_FORMAT_R32_UINT;
	//データを送る
	uint32_t* indexDataSphere = nullptr;
	indexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSphere));
	indexDataSphere[0] = 0;
	indexDataSphere[1] = 1;
	indexDataSphere[2] = 2;
	indexDataSphere[3] = 1;
	indexDataSphere[4] = 3;
	indexDataSphere[5] = 2;*/
#pragma endregion
	//デバッグ画面初期化
#pragma region
	float materialDataVector[4] = { 1,1,1,1 };
	float TransformScale[3] = { 1.0f,1.0f,1.0f };
	float TransformRotae[3] = { 0.0f, 6.28f, 0.0f };
	float TransformTranslate[3] = { 0.0f,0.0f,-20.0f };
	float directionalLight[3] = { 0.0f,-1.0f,0.0f };
	//uvTransform
	struct Sprite::Transform uvTransformSprite {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f },
	};
	bool useMonsterball = false;
	bool bulletShot = false;
	bool EnemybulletShot = false;
	float mousePos[3] = {};
	bool mouseLeft = false;
	bool mouseRight = false;
#pragma endregion

	//ゲーム処理
	while (true)
	{
		if (winApp->ProcessMessage()) {
			break;
		}
		else {
			bulletShot = player->bulletActive;
			EnemybulletShot = enemy->bulletActive;
			//imgui
#ifdef USE_IMGUI
			mousePos[0] = input->GetCursorClientPos3().x;
			mousePos[1] = input->GetCursorClientPos3().y;
			mousePos[2] = input->GetCursorClientPos3().z;
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow();
			//ImGui::Checkbox("useMonsterBall", &useMonsterball);
			//ImGui::DragFloat4("materialData", materialDataVector);
			ImGui::DragFloat3("Scale", TransformScale);
			ImGui::DragFloat3("Rotae", TransformRotae, 0.1f);
			ImGui::DragFloat3("Translate", TransformTranslate);
			ImGui::DragFloat3("MousePos", mousePos);
			ImGui::Checkbox("bullet", &bulletShot);
			ImGui::Checkbox("enemyBullet", &EnemybulletShot);
			ImGui::Checkbox("mouseLeft", &mouseLeft);
			ImGui::Checkbox("mouseRight", &mouseRight);
			//ImGui::DragFloat3("directionalLight", directionalLight, 0.1f);
			//ImGui::DragFloat2("UVTransform", &uvTransformSprite.transform.x, 0.01f, -10.0f, 10.0f);
			//ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			//ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
#endif 
			TransformScale[0] = railCamera->GetTransform().scale.x;
			TransformScale[1] = railCamera->GetTransform().scale.y;
			TransformScale[2] = railCamera->GetTransform().scale.z;
			TransformRotae[0] = railCamera->GetTransform().rotate.x;
			TransformRotae[1] = railCamera->GetTransform().rotate.y;
			TransformRotae[2] = railCamera->GetTransform().rotate.z;
			TransformTranslate[0] = railCamera->GetTransform().translate.x;
			TransformTranslate[1] = railCamera->GetTransform().translate.y;
			TransformTranslate[2] = railCamera->GetTransform().translate.z;
			//camera_->SetTransform(transform_);

			input->Update();
			if (input->TriggerKey(DIK_0)) {
				OutputDebugStringA("HIT0\n");
			}

			/*for (Sprite* sprite : sprites) {
				sprite->Update();
			}*/

			object3dCommon->SettingCommonDraw();

			if(input->PushMouse(0)){
				mouseLeft = true;
			}
			else if(input->PushMouse(1)){
				mouseRight = true;
			}

			if (isTitle) {

				title->Update();
				titleUI->Update();
				backGround->Update();
				if (input->TriggerKey(DIK_RETURN)) {
					isFade = true;
					isOver = false;
					GameOverFlag = false;
				}

				if (reup) {
					player->Initialize(object3dCommon, input);
					enemy->Initialize(object3dCommon, enemyPos);
				}

			}
			else {
				if (isStart) {
					startTime++;
					if (startTime >= 120) {
						isStart = false;
						isGo = true;
					}
				}
				if (isGo) {
					goTime++;
					if (goTime >= 120) {
						isGo = false;
						isGame = true;
					}
				}

				if (isGame) {
					//天球
					railCamera->Update();
					skyDome->Updata();

					if (isOver == false && isClear == false) {
						//プレイヤー更新
						player->Update();
						//敵更新
						enemy->Update();
					}

					//当たり判定
					Vector3 posA, posB;
					const std::list<PlayerBullet*>& playerBullets = player->GetBullets();
					const std::list<EnemyBullet*>& enemyBullets = enemy->GetBullets();
#pragma region player
					posA = player->GetWorldPosition();
					for (EnemyBullet* bullet : enemyBullets)
					{
						posB = bullet->GetWorldPosition();
						float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
						//	半径は0.5
						if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
							player->OnCollision();

							bullet->OnCollision();
						}
					}
#pragma endregion
#pragma region Enemy
					posA = enemy->GetWorldPosition();
					for (PlayerBullet* bullet : playerBullets)
					{
						posB = bullet->GetWorldPosition();
						float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
						//	半径は0.5
						if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
							enemy->OnCollision();
							bullet->OnCollision();
						}

					}
#pragma endregion
#pragma region Bullet
					for (PlayerBullet* bulletP : playerBullets)
					{
						posA = bulletP->GetWorldPosition();

						for (EnemyBullet* bulletE : enemyBullets)
						{
							posA = bulletP->GetWorldPosition();
							posB = bulletE->GetWorldPosition();

							float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);

							//	半径0.5
							if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
								bulletE->OnCollision();
								bulletP->OnCollision();

							}
						}
					}
#pragma endregion
					if (player->IsDead()) {
						isOver = true;
						// アルファ値を徐々に増加（最大値は1.0f）
						blackAlpha += 0.01f;
						if (blackAlpha > 1.0f) {
							blackAlpha = 1.0f; // 最大値を超えないように制限
						}

						// Blackスプライトの色を更新
						Black->SetColor(Vector4(1.0f, 1.0f, 1.0f, blackAlpha)); // 黒色でアルファ値を適用

						if (input->TriggerKey(DIK_T)) {
							isTitle = true;
							isGame = false;
							isOver = false;
							reup = true;
							blackAlpha = 0.0f; // アルファ値をリセット
						}
					}
					if(enemy->IsDead()){
						isClear = true;
						// アルファ値を徐々に増加（最大値は1.0f）
						blackAlpha += 0.01f;
						if (blackAlpha > 1.0f) {
							blackAlpha = 1.0f; // 最大値を超えないように制限
						}

						// Blackスプライトの色を更新
						Black->SetColor(Vector4(1.0f, 1.0f, 1.0f, blackAlpha)); // 黒色でアルファ値を適用

						if (input->TriggerKey(DIK_T)) {
							isTitle = true;
							isGame = false;
							isClear = false;
							reup = true;
							blackAlpha = 0.0f; // アルファ値をリセット
						}
					}

					//パーティクル更新
					particleManager->Update();
				}
				Ready->Update();
				Go->Update();
				gameOver->Update();
				clear->Update();
				Black->Update();

				//uvTransform
				Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
				uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
				uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));


			}
			fadeSprite->Update();
			//フェード処理
			if (isFade) {
				if (!endFade) {
					size.x += speedx;
					size.y += speedy;
					position.x -= speedx / 2;
					position.y -= speedy / 2;

					fadeSprite->SetSize(Vector2(size.x, size.y));
					fadeSprite->SetPosition(Vector2(position.x, position.y));

					if (fadeSprite->GetSize().x >= 1800) {
						isTitle = false;
						endFade = true;
					}
				}
			}
			if (endFade) {
				size.x -= speedx;
				size.y -= speedy;
				position.x += speedx / 2;
				position.y += speedy / 2;
				fadeSprite->SetSize(Vector2(size.x, size.y));
				fadeSprite->SetPosition(Vector2(position.x, position.y));
				if (fadeSprite->GetSize().x <= 0) {
					endFade = false;
					isFade = false;
					isStart = true;
					isGo = false;
					startTime = 0;
					goTime = 0;
					size = { 0,0 };
					position = { 630,360 };
					fadeSprite->SetSize(Vector2(0, 0));
					fadeSprite->SetPosition(Vector2(630, 360));
				}
			}
		
			//uvTransform
			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));

			//球の３次元化 WVPスフィア用
			/*transformSphere.rotate.y += 0.03f;
			Matrix4x4 worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
			Matrix4x4 viewMatrixSphere = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrixSphere = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrixSphere, projectionMatrixSphere));
			transformationMatrixDataSphere->WVP = worldViewProjectionMatrixSphere;
			transformationMatrixDataSphere->World = worldMatrixSphere;*/
#ifdef USE_IMGUI
			ImGui::Render();
#endif 
			//画面色変更
#pragma region

			dxCommon->PreDraw();

			spriteCommon->SettingCommonDraw();

			if (isTitle) {
				backGround->Draw();
				title->Draw();
			}
			else {
				//3D描画
				skyDome->Draw();
				if (isOver == false) {
					player->Draw();
				}
				if (isClear == false) {
					enemy->Draw();
				}
				if (isStart) {
					Ready->Draw();
				}
				else if (isGo) {
					Go->Draw();
				}
				if (isOver) {
					Black->Draw();
					gameOver->Draw();
				}
				if (isClear)
				{
					Black->Draw();
					clear->Draw();
				}

				particleManager->Draw();
			}
			if (isFade || endFade) {
				fadeSprite->Draw();
			}

			//planeObject->Draw();
			//axisObject->Draw();
#ifdef USE_IMGUI
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
#endif 
			dxCommon->PostDrow();
#pragma endregion
		}
	}
	//解放
	/*for (Sprite* sprite : sprites) {
		delete sprite;
	}*/
	delete spriteCommon;
	delete input;
	delete dxCommon;
	delete particleManager;
	delete enemy;
	delete player;
	delete skyDome;
	delete clear;
	delete Go;
	delete Ready;
	delete Black;
	delete gameOver;
	delete fadeSprite;
	delete title;
	delete titleUI;
	delete backGround;
	//delete axisObject;
	//delete planeObject;
	ModelManager::GetInstance()->Finalize();
	delete camera_;
	delete object3dCommon;
	delete model;
	delete modelCommon;
	TextureManager::GetInstance()->Finalize();

#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif 

#ifndef _DEBUG
	
#endif _DEBUG

	winApp->Finalize();
	delete winApp;

	return 0;
};