/*
 * TZone.cpp
 *
 *  Created on: Jul 13, 2013
 *      Author: agustin
 */

#include "TZone.h"

#include <debug/PrimitiveDrawer.h>
#include <math/Vec2.h>
#include <math/AABB.h>

namespace tool {

TZone::TZone(const core::TriggerZone& zone, const Ogre::ColourValue& color) :
    mTriggerZone(zone)
,   mPrimitive(0)
,   mSelected(false)
{
    // create the primitive using the zone (assuming that are AABB)
    const core::TZType& aabb = zone.zone();
    const core::Vector2 center = (aabb.tl + aabb.br) * 0.5f;
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    mPrimitive = pd.createPlane(Ogre::Vector3(center.x, 0.f, center.y),
                                Ogre::Vector2(aabb.getWidth(), aabb.getHeight()),
                                color);
    ASSERT(mPrimitive);

    // configure the SelectableObject
    setSceneNode(mPrimitive->node);

    // We will configure the zone with an alpha value of 0.5 (as if it is
    // unselected)
    mPrimitive->setAlpha(0.5f);
}

TZone::~TZone()
{
}

////////////////////////////////////////////////////////////////////////////////
void
TZone::mouseOver(void)
{
    debugBLUE("TZone::mouseOver\n");
    if (mSelected) {
        // do nothing
        return;
    }
    mPrimitive->setAlpha(0.75f);
}

////////////////////////////////////////////////////////////////////////////////
void
TZone::mouseExit(void)
{
    debugBLUE("TZone::mouseExit\n");
    if (mSelected) {
        // do nothing
        return;
    }
    mPrimitive->setAlpha(0.5f);
}

////////////////////////////////////////////////////////////////////////////////
bool
TZone::objectSelected(SelectType type)
{
    debugBLUE("TZone::objectClicked\n");
    mPrimitive->setAlpha(1.f);
    mSelected = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
TZone::objectUnselected(void)
{
    debugBLUE("TZone::objectUnselected\n");
    mPrimitive->setAlpha(0.5f);
    mSelected = false;
}

}
