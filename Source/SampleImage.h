//=========================================================
// @file SampleImage.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "Object2D.h"

//--------------------------------------------------------------------------------------
class SampleImage : public Object2D
{
public:
	using Super = Object2D;

	SampleImage() = default;
	virtual ~SampleImage() = default;

	virtual void enter() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void leave() override;
};
//--------------------------------------------------------------------------------------