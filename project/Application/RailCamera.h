#pragma once
#include "Vector3.h"

class RailCamera
{
public:
	void Initialize();

	void Update();

private:
	Vector3 position_;
	Vector3 target_;

};

