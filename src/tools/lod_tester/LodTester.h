/*
 * LodTester.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef LodTester_H_
#define LodTester_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include <vector>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <types/basics.h>
#include <utils/OrbitCamera.h>

namespace tool {

class LodTester : public core::AppTester
{
public:
    LodTester();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~LodTester();

private:
    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    Ogre::AnimationState *mActualAnim;
    float mTimeFrame;
    OrbitCamera mOrbitCamera;
    core::OgreText mDistanceText;
    core::OgreText mMeshText;
};

}

#endif /* LodTester_H_ */
