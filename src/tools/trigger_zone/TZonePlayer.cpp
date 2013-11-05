/*
 * TZonePlayer.cpp
 *
 *  Created on: Aug 6, 2013
 *      Author: agustin
 */

#include "TZonePlayer.h"

namespace tool {

TZonePlayer::TZonePlayer(Ogre::SceneManager* sm) :
     mAgent(0)
{
    ASSERT(sm);

    mNode = sm->getRootSceneNode()->createChildSceneNode();

    // create the entity we will use.. just for now it will be a cube..
    mEntity = sm->createEntity(Ogre::SceneManager::PrefabType::PT_SPHERE);
    mNode->attachObject(mEntity);
    mNode->yaw(Ogre::Radian(Ogre::Math::PI));
}

TZonePlayer::~TZonePlayer()
{

}

void
TZonePlayer::initialize(core::TriggerAgent* agent, const Ogre::Vector3& startPos)
{
    ASSERT(agent);
    ASSERT(mNode);

    mAgent = agent;
    mNode->setPosition(startPos);
    agent->setPosition(core::Vector2(startPos.x, startPos.y));
}


} /* namespace tool */
