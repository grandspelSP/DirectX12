//=========================================================
// @file Camera.h
// @auth Kota Tamaoki
// @date 2026/02/28
//=========================================================

#pragma once

#include "DirectXIncluder.hpp"

using namespace DirectX;

class Camera
{
private:
	Camera() : mView(), mProjection() {}
	~Camera() { 
		delete mInstance; 
		mInstance = nullptr;
	}

public:
	bool Init();

public:
	static Camera* GetInstance()
	{
		if (!mInstance) {
			mInstance = new Camera();
			mInstance->Init();
		}
		return mInstance;
	}

	inline XMMATRIX GetViewProjection() const { return mView * mProjection; }

private:
	static Camera* mInstance;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

inline Camera* GetCamera(void) { return Camera::GetInstance(); }