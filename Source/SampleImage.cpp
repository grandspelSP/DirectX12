//=========================================================
// @file SampleImage.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "SampleImage.h"
#include "Component/Transform.h"
#include "Component/TextureRender.h"

void SampleImage::enter()
{
	addComponent<Transform>();
	addComponent<TextureRender>();
	getComponent<TextureRender>()->Init();
}

void SampleImage::update()
{
}

void SampleImage::draw()
{
	getComponent<TextureRender>()->RenderWICTexture();
}

void SampleImage::leave()
{
}
