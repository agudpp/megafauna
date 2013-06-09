/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef AnimPlayer_H_
#define AnimPlayer_H_

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

class AnimPlayer : public core::AppTester
{
    static const Ogre::Real RANDOM_POSITION;
public:
    AnimPlayer();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~AnimPlayer();

private:
    void
    loadAnimations(void);
    void
    changeAnim(int i);
    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *node;
    Ogre::Entity *ent;
    std::vector<Ogre::AnimationState *> mAnims;
    core::size_t mCurrentIndex;
    Ogre::AnimationState *mActualAnim;
    std::vector<Ogre::AnimationState *> mCamina;
    float mTimeFrame;
    core::OgreText mAnimText;
    core::OgreText mModelLoadedText;
    OrbitCamera mOrbitCamera;
};

}

#endif /* AnimPlayer_H_ */
