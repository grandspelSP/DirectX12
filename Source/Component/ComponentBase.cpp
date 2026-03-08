//=========================================================
// @file ComponentBase.cpp
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#include "ComponentBase.h"
#include "../ActorBase.h"

//--------------------------------------------------------------------------------------
ActorBase* ComponentBase::getParent() const
{
    return mParentObject;
}
//--------------------------------------------------------------------------------------
const void ComponentBase::setParent(ActorBase* parent)
{
    mParentObject = parent;
}
//--------------------------------------------------------------------------------------