//=========================================================
// @file Transform.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "ComponentBase.h"
#include "../DIrectXIncluder.hpp"

using namespace DirectX;

class Transform : public ComponentBase
{
public:
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

	const void SetPosition2D(XMFLOAT2& position) {
		mPosition.x = position.x;
		mPosition.y = position.y;
	}
	const void SetPosition3D(XMFLOAT3& position) {
		mPosition = position;
	}
	const void SetScale2D(XMFLOAT2& scale) {
		scale.x = scale.x;
		scale.y = scale.y;
	}
	const void SetScale3D(XMFLOAT3& scale) {
		mScale = scale;
	}
	const void SetRotation2D(XMFLOAT2& rotation) {
		mRotation.x = rotation.x;
		mRotation.y = rotation.y;
	}
	const void SetRotation3D(XMFLOAT3& rotation) {
		mRotation = rotation;
	}

private:
	XMFLOAT3 mPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 mRotation{ 0.0f, 0.0f, 0.0f };
};
