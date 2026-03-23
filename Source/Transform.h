//=========================================================
// @file Transform.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "ComponentBase.h"
#include "DirectXIncluder.hpp"

//--------------------------------------------------------------------------------------
using namespace DirectX;
//--------------------------------------------------------------------------------------
class Transform : public ComponentBase
{
public:
	virtual void debugDraw() override final;

	const XMFLOAT2& getPosition2D() const {
		return XMFLOAT2{ mPosition.x, mPosition.y };
	}
	const XMFLOAT3& getPosition3D() const {
		return mPosition;
	}
	const XMFLOAT2& getScale2D() const {
		return XMFLOAT2{ mScale.x, mScale.y };
	}
	const XMFLOAT3& getScale3D() const {
		return mScale;
	}
	const XMFLOAT2& getRotation2D() const {
		return XMFLOAT2{ mRotation.x, mRotation.y };
	}
	const XMFLOAT3& getRotation3D() const {
		return mRotation;
	}

	void SetPosition2D(const XMFLOAT2& position) {
		mPosition.x = position.x;
		mPosition.y = position.y;
	}
	void SetPosition3D(const XMFLOAT3& position) {
		mPosition = position;
	}
	void SetScale2D(const XMFLOAT2& scale) {
		mScale.x = scale.x;
		mScale.y = scale.y;
	}
	void SetScale3D(const XMFLOAT3& scale) {
		mScale = scale;
	}
	void SetRotation2D(const XMFLOAT2& rotation) {
		mRotation.x = rotation.x;
		mRotation.y = rotation.y;
	}
	void SetRotation3D(const XMFLOAT3& rotation) {
		mRotation = rotation;
	}

	void ResetTransform() {
		mPosition = mInitialPosition;
		mScale = mInitialScale;
		mRotation = mInitialRotation;
	}

private:
	XMFLOAT3 mPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 mRotation{ 0.0f, 0.0f, 0.0f };

	XMFLOAT3 mInitialPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mInitialScale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 mInitialRotation{ 0.0f, 0.0f, 0.0f };
};
//--------------------------------------------------------------------------------------