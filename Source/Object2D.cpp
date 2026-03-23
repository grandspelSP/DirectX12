//=========================================================
// @file Object2D.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "Object2D.h"
#include "Transform.h"
#include "TextureRender.h"

//--------------------------------------------------------------------------------------
void Object2D::enter()
{
	Super::enter();

	addComponent<Transform>();
	addComponent<TextureRender>();
}
//--------------------------------------------------------------------------------------
void Object2D::update()
{
	Super::update();

	auto* transform = getComponent<Transform>();
	getComponent<TextureRender>()->SetPosition(transform->getPosition3D());
	getComponent<TextureRender>()->SetRotation(transform->getRotation3D());
	getComponent<TextureRender>()->SetScale(transform->getScale3D());
}
//--------------------------------------------------------------------------------------
void Object2D::draw()
{
	Super::draw();

	getComponent<TextureRender>()->RenderWICTexture();
}
//--------------------------------------------------------------------------------------
void Object2D::leave()
{
	Super::leave();
}
//--------------------------------------------------------------------------------------