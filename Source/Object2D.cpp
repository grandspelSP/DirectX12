//=========================================================
// @file Object2D.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "Object2D.h"
#include "Component/Transform.h"
#include "Component/TextureRender.h"

void Object2D::enter()
{
	addComponent<Transform>();
	addComponent<TextureRender>();
}

void Object2D::update()
{
	auto* transform = getComponent<Transform>();
	getComponent<TextureRender>()->SetPosition(transform->getPosition3D());
	getComponent<TextureRender>()->SetRotation(transform->getRotation3D());
}

void Object2D::draw()
{
	getComponent<TextureRender>()->RenderWICTexture();
}