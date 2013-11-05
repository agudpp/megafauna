/*
 * TZonePlayer.h
 *
 *  Created on: Aug 6, 2013
 *      Author: agustin
 */

#ifndef TZONEPLAYER_H_
#define TZONEPLAYER_H_

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <debug/DebugUtil.h>
#include <math/Vec2.h>
#include <trigger_system/TriggerAgent.h>

namespace tool {

// Representation of the "Player/Character" for the trigger zone tool
//

class TZonePlayer
{
public:
    // Constructor
    TZonePlayer(Ogre::SceneManager* sm);
    ~TZonePlayer();

    // @brief Configure and initiate the Player
    // @param agent     The trigger agent to be used
    // @param startPos  The start position where the agent should be
    //
    void
    initialize(core::TriggerAgent* agent, const Ogre::Vector3& startPos);

    // @brief Translate the player over x and z (not y)
    //
    inline void
    translate(Ogre::Real x, Ogre::Real y);

    // @brief Set a new position
    //
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief Get the current position
    //
    inline const Ogre::Vector3&
    position(void) const;

    // @brief Return the associated core::TriggerAgent*
    //
    inline core::TriggerAgent*
    triggerAgent(void);

private:
    Ogre::Entity* mEntity;
    Ogre::SceneNode* mNode;
    core::TriggerAgent* mAgent;
};

//
// INLINE
//

inline void
TZonePlayer::translate(Ogre::Real x, Ogre::Real y)
{
    ASSERT(mNode);
    ASSERT(mAgent);
    mNode->translate(x, y, 0);
    mAgent->setPosition(mAgent->position() + core::Vector2(x,y));
}

inline void
TZonePlayer::setPosition(const Ogre::Vector3& pos)
{
    ASSERT(mNode);
    ASSERT(mAgent);
    mNode->setPosition(pos);
    mAgent->setPosition(core::Vector2(pos.x,pos.y));
}

inline const Ogre::Vector3&
TZonePlayer::position(void) const
{
    ASSERT(mNode);
    return mNode->getPosition();
}

inline core::TriggerAgent*
TZonePlayer::triggerAgent(void)
{
    return mAgent;
}

} /* namespace tool */
#endif /* TZONEPLAYER_H_ */
