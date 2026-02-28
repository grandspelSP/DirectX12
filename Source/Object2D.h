//=========================================================
// @file Object2D.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "ObjectBase.h"

class Object2D : public ObjectBase
{
public:
	Object2D() = default;
	virtual ~Object2D() = default;

	virtual void enter() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void leave() = 0;

protected:
	// テクスチャ情報
	const wchar_t* mTexturePath = L"Texture/a.png";
};

