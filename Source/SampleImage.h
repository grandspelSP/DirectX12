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
	SampleImage() = default;
	virtual ~SampleImage() = default;

	virtual void enter(const int id = -1);
	virtual void update();
	virtual void draw();
	virtual void leave();

private:
	int mID = -1;
};
//--------------------------------------------------------------------------------------