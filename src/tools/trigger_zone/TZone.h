/*
 * TZone.h
 *
 *  Created on: Jul 13, 2013
 *      Author: agustin
 */

#ifndef TZONE_H_
#define TZONE_H_

#include <OgreColourValue.h>

#include <debug/DebugUtil.h>
#include <trigger_system/TriggerZone.h>
#include <trigger_system/TriggerSystemDefines.h>


// forward
//
namespace core {
struct Primitive;
}

namespace tool {

class TZone {
public:
    // Constructor / destructor
    //
    TZone(const core::TriggerZone& zone, const Ogre::ColourValue& color);
    virtual ~TZone();

    // @brief Return the associated TriggerZone
    //
    inline const core::TriggerZone&
    triggerZone(void) const;


    ////////////////////////////////////////////////////////////////////////////


    // @brief Method called when the player get inside of this zone
    //
    void
    playerInside(void);

    // @brief Method called when the player leaves the zone
    //
    void
    playerOutside(void);

    // @brief Return the associated connection
    //
    inline core::TriggerCallback::Connection&
    getConnection(void);

    // @brief Method that will be called to receive the event associated to this
    //        zone
    //
    inline void
    eventHandler(const core::TriggerCallback::EventInfo&);


private:
    core::TriggerZone mTriggerZone;
    core::Primitive* mPrimitive;
    core::TriggerCallback::Connection mConnection;

};


////////////////////////////////////////////////////////////////////////////////
//

inline const core::TriggerZone&
TZone::triggerZone(void) const
{
    return mTriggerZone;
}

inline core::TriggerCallback::Connection&
TZone::getConnection(void)
{
    return mConnection;
}

inline void
TZone::eventHandler(const core::TriggerCallback::EventInfo& e)
{
    if (e.type == core::TriggerCallback::EventType::Entering) {
        playerInside();
    } else {
        ASSERT(e.type == core::TriggerCallback::EventType::Leaving);
        playerOutside();
    }
}

}

#endif /* TZONE_H_ */
