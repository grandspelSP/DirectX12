//=========================================================
// @file SampleImage.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "SampleImage.h"
#include "Component/Transform.h"
#include "Component/TextureRender.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"

void SampleImage::enter(const int id)
{
	Object2D::enter();

	mID = id;
	if(mID == 1)
		getComponent<TextureRender>()->Init("Texture/a.png");
	else
		getComponent<TextureRender>()->Init("Texture/ENDFIELD_SHARE_1769687062.png");
}

void SampleImage::update()
{
	Object2D::update();
}

void SampleImage::draw()
{
#if DEBUG
	auto* transform = getComponent<Transform>();
	if (!transform) {
		return;
	}

	ImGui::Begin("Transform Enemy");
	ImGui::Text("Enemy %d", mID);
	ImGui::PushID(mID);
	XMFLOAT3 position = transform->getPosition3D();
	XMFLOAT3 rotation = transform->getRotation3D();
	XMFLOAT3 scale = transform->getScale3D();
	
	float positionArray[] = { position.x, position.y, position.z };
	float rotationArray[] = { rotation.x, rotation.y, rotation.z };
	float scaleArray[] = { scale.x, scale.y, scale.z };
	
	if (ImGui::DragFloat3("Position ## Inspector", positionArray, 0.01f))
	{
		transform->SetPosition3D({ positionArray[0], positionArray[1], positionArray[2] });
	}
	
	if (ImGui::DragFloat3("Rotation ## Inspector", rotationArray, 0.01f))
	{
		transform->SetRotation3D({ rotationArray[0], rotationArray[1], rotationArray[2] });
	}
	
	if (ImGui::DragFloat3("Scale ## Inspector", scaleArray, 0.01f))
	{
		transform->SetScale3D({ scaleArray[0], scaleArray[1], scaleArray[2] });
	}
	ImGui::PopID();
	ImGui::End();
#endif // DEBUG

	Object2D::draw();
}

void SampleImage::leave()
{
}
