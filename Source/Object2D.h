//=========================================================
// @file Object2D.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "ActorBase.h"

class Object2D : public ActorBase
{
public:
	Object2D() = default;
	virtual ~Object2D() = default;

	virtual void enter();
	virtual void update();
	virtual void draw();
	virtual void leave() = 0;
};

