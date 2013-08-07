/*
 * TriggerZone.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TriggerZone_H_
#define TriggerZone_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include <vector>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <types/basics.h>
#include <utils/SatelliteCamera.h>
#include <cursor/MouseCursor.h>
#include <utils/SelectionHelper.h>
#include <math/AABB.h>
#include <input/InputHelper.h>
#include <trigger_system/TriggerSystem.h>

#include "TZonePlayer.h"
#include "TZone.h"

#define TRIGGER_ZONE_TOOL_FILE  "TriggerZone.xml"


// forward
//
class TiXmlElement;


namespace tool {

class TriggerZone : public core::AppTester
{
public:
    TriggerZone();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~TriggerZone();

private:

    // @brief Load the floor and the base plane using the xml file
    // @param xml   The root xml element of the xml file
    // @return true on success | false otherwise
    //
    bool
    loadFloor(const TiXmlElement* xml);

    // @brief Configure the camera to be used as Satellite mode
    //
    void
    configureCamera(void);

    // @brief Create the trigger system for this level
    //
    void
    configureTriggerSystem(void);

    void
    handleCameraInput(void);
    void
    handlePlayerInput(void);


private:
    float mTimeFrame;
    core::OgreText mCameraPos;
    core::OgreText mPlayerPos;
    SatelliteCamera mSatelliteCamera;
    ui::MouseCursor mMouseCursor;
    input::InputHelper mInputHelper;
    core::AABB mFloorAABB;
    core::TriggerSystem mTriggerSystem;

    TZonePlayer mPlayer;
    std::vector<TZone> mTZones;
    std::vector<core::uint16_t> mIDs; // zones ids
};

}

#endif /* TriggerZone_H_ */
