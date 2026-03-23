//=========================================================
// @file Transform.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "Transform.h"

void Transform::debugDraw()
{
	const auto& id = mParentObject->getBaseProcID();

	ImGui::Begin("Transform");
	ImGui::Text("Enemy %d", id);
	ImGui::PushID(id);

	float positionArray[] = { mPosition.x, mPosition.y, mPosition.z };
	float rotationArray[] = { mRotation.x, mRotation.y, mRotation.z };
	float scaleArray[] = { mScale.x, mScale.y, mScale.z };

	if (ImGui::DragFloat3("Position ## Inspector", positionArray, 1.0f))
	{
		SetPosition3D({ positionArray[0], positionArray[1], positionArray[2] });
	}

	if (ImGui::DragFloat3("Rotation ## Inspector", rotationArray, 1.0f))
	{
		SetRotation3D({ rotationArray[0], rotationArray[1], rotationArray[2] });
	}

	if (ImGui::DragFloat3("Scale ## Inspector", scaleArray, 1.0f))
	{
		SetScale3D({ scaleArray[0], scaleArray[1], scaleArray[2] });
	}

	if (ImGui::Button("Reset")) {
		ResetTransform();
	}

	ImGui::PopID();
	ImGui::End();
}