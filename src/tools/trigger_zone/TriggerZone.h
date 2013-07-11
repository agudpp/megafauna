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


#define TRIGGER_ZONE_TOOL_FILE  "TriggerZone.xml"


// forward
//
class TiXmlElement;


namespace tool {

class TriggerZone : public core::AppTester
{
    static const Ogre::Real RANDOM_POSITION;
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

    void
    handleCameraInput(void);

private:
    float mTimeFrame;
    core::OgreText mAnimText;
    core::OgreText mModelLoadedText;
    SatelliteCamera mSatelliteCamera;
    ui::MouseCursor mMouseCursor;
    //SelectionHelper mSelectionHelper;
    core::AABB mFloorAABB;
};

}

#endif /* TriggerZone_H_ */
