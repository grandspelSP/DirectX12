//=========================================================
// @file SampleImage.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once
#include "Object2D.h"

class SampleImage : public Object2D
{
public:
	SampleImage() = default;
	virtual ~SampleImage() = default;

	virtual void enter();
	virtual void update();
	virtual void draw();
	virtual void leave();

private:
	
};

