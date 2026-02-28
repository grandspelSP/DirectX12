//=========================================================
// @file Camera.cpp
// @auth Kota Tamaoki
// @date 2026/02/28
//=========================================================

#include "Camera.h"
#include "Device.h"

Camera* Camera::mInstance = nullptr;

bool Camera::Init() {
    mView = XMMatrixLookAtLH({ 0.f, 0.f, -5.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
    mProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), GetDevice()->mAspectRatio, 1.f, 20.f);

    return true;
}