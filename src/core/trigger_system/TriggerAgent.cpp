/*
 * TriggerAgent.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#include "TriggerAgent.h"

#include "TriggerSystem.h"

namespace core {

TriggerAgent::TriggerAgent(TriggerSystem& ts) :
    mTriggerSystem(ts)
,   currentColors(0)
{
    std::memset(closerZones, 0, sizeof(TriggerZone *) * (TS_NUM_COLORS+1));
}

void
TriggerAgent::setPosition(const Vector2& position)
{
    mTriggerSystem.updateAgentPos(this, position);
}


} /* namespace core */
