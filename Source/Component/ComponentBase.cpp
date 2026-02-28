//=========================================================
// @file ComponentBase.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "ComponentBase.h"
#include "../ObjectBase.h"

ObjectBase* ComponentBase::getParent() const {
    return mParentObject;
}
const void ComponentBase::setParent(ObjectBase* parent) {
    mParentObject = parent;
}