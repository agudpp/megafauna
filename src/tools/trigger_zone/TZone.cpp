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
{
    // create the primitive using the zone (assuming that are AABB)
    const core::TZType& aabb = zone.zone();
    const core::Vector2 center = (aabb.tl + aabb.br) * 0.5f;
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    mPrimitive = pd.createPlane(Ogre::Vector3(center.x, center.y, 1.f ),
                                Ogre::Vector2(aabb.getWidth(), aabb.getHeight()),
                                color);
    ASSERT(mPrimitive);

    // We will configure the zone with an alpha value of 0.5 (as if it is
    // unselected)
    mPrimitive->setAlpha(0.5f);
}

TZone::~TZone()
{
}

////////////////////////////////////////////////////////////////////////////////
void
TZone::playerInside(void)
{
    debugBLUE("TZone::playerInside\n");
    mPrimitive->setAlpha(0.75f);
}

////////////////////////////////////////////////////////////////////////////////
void
TZone::playerOutside(void)
{
    debugBLUE("TZone::playerOutside\n");
    mPrimitive->setAlpha(0.5f);
}


}
