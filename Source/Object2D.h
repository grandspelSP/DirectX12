//=========================================================
// @file Object2D.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "ActorBase.h"

//--------------------------------------------------------------------------------------
class Object2D : public ActorBase
{
public:
	using Super = ActorBase;

	Object2D() = default;
	virtual ~Object2D() = default;

	virtual void enter() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void leave() override;
};
//--------------------------------------------------------------------------------------