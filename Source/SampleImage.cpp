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
	Object2D::enter();
}

void SampleImage::update()
{
	auto* transform = getComponent<Transform>();
	if (!transform) {
		return;
	}
	transform->SetPosition2D(XMFLOAT2{ 1.0f, 1.0f });

	Object2D::update();
}

void SampleImage::draw()
{
	Object2D::draw();
}

void SampleImage::leave()
{
}
