#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include "Input.h"
#include "WinApp.h"
#include "ModelManager.h"
#include "Model.h"
#include "ModelCommon.h"

class Player
{
public:
	Player();
	~Player();


	void Initialize(ObJect3dCommon* object3dCommon, Input* input);

	void Update();

	void Draw();
private:

	Transform modelTransform_;
	Object3d* object3d = nullptr; // 3Dオブジェクト
	Vector3 position; // 位置
	Vector3 rotation; // 回転
	Vector3 scale; // 拡大縮小
	float speed; // 移動速度
	// 入力関連
	Input* input_;


};