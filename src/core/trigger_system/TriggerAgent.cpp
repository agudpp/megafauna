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
}

TriggerCode
TriggerAgent::setPosition(const Vector2& position)
{
    return mTriggerSystem.updateAgentPos(this, position);
}


} /* namespace core */
