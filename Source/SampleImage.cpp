//=========================================================
// @file SampleImage.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "SampleImage.h"
#include "Transform.h"
#include "TextureRender.h"

//--------------------------------------------------------------------------------------
void SampleImage::enter()
{
	Super::enter();

	auto* texture_render = getComponent<TextureRender>();
	
	if (texture_render) {
		getComponent<TextureRender>()->Init("Texture/ENDFIELD_SHARE_1769687062.png");
	}

	auto* transform = getComponent<Transform>();
	transform->SetScale2D({ 128.0f, 72.0f });
}
//--------------------------------------------------------------------------------------
void SampleImage::update()
{
	Super::update();

	// “ü—ÍƒeƒXƒg
	if(GetAsyncKeyState(VK_UP) & 0x8000) {
		auto* transform = getComponent<Transform>();
		if(transform) {
			XMFLOAT2 position = transform->getPosition2D();
			position.y += 0.1f;
			transform->SetPosition2D(position);
		}
	}
}
//--------------------------------------------------------------------------------------
void SampleImage::draw()
{
	Super::draw();
}
//--------------------------------------------------------------------------------------
void SampleImage::leave()
{
	Super::leave();
}
//--------------------------------------------------------------------------------------