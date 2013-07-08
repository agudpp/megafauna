/*
 * TriggerZone.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include "TriggerZone.h"

namespace tool {

const Ogre::Real TriggerZone::RANDOM_POSITION = 2000.0f;

void
TriggerZone::loadAnimations(void)
{
    Ogre::AnimationStateSet *allAnim = ent->getAllAnimationStates();
    Ogre::AnimationStateIterator it = allAnim->getAnimationStateIterator();

    while(it.hasMoreElements()){
        Ogre::AnimationState *anim = it.getNext();
        if (!anim) {
            break;
        }
        mAnims.push_back(anim);
    }
    mCurrentIndex = 0;
    // set enable the current anim
    mAnims.back()->setEnabled(true);
}

void
TriggerZone::changeAnim(int i)
{
    ASSERT(i < mAnims.size());

    mActualAnim->setEnabled(false);
    mActualAnim = mAnims[i];
    mActualAnim->setTimePosition(0);
    mActualAnim->setEnabled(true);

    // update the text
    if (mActualAnim != 0) {
        mAnimText.setText("Animation: " + mActualAnim->getAnimationName());
    }
}

void
TriggerZone::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
            lMouseState.X.abs <= 0)
    {
        mTranslationVec.x -= 1.0f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
            lMouseState.X.abs >= lMouseState.width)
    {
        mTranslationVec.x += 1.0f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_Q))
    {
        zoom += 0.5f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_E))
    {
        zoom -= 0.5f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) ||
            lMouseState.Y.abs <= 0)
    {
        mTranslationVec.z -= 1.0f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
            lMouseState.Y.abs >= lMouseState.height)
    {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO)
    {
        mOrbitCamera.moveCamera(mTranslationVec);
    }
    if(zoom != mOrbitCamera.zoom()){
        mOrbitCamera.setZoom(zoom);
    }

    const float lMouseZ = float(lMouseState.Z.rel);
    float scrollZoom = mOrbitCamera.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if(scrollZoom != mOrbitCamera.zoom()){
        mOrbitCamera.setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
    if(lMouseState.buttonDown(OIS::MB_Right)){
        const float factor = -0.01 * 1.5f;
        mOrbitCamera.rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));
    }

    // check for the type of camera we want to use
    if (mKeyboard->isKeyDown(OIS::KC_1)) {
        mOrbitCamera.setCameraType(OrbitCamera::CameraType::FreeFly);
    } else if (mKeyboard->isKeyDown(OIS::KC_2)) {
        mOrbitCamera.setCameraType(OrbitCamera::CameraType::Orbit);
    }

}

TriggerZone::TriggerZone() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
{
    setUseDefaultInput(false);
}

TriggerZone::~TriggerZone()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
TriggerZone::loadAditionalData(void)
{
    ent = mSceneMgr->createEntity("gusanohi.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);

    loadAnimations();

    // configure the text for ogre
    mModelLoadedText.setPos(0.f, 0.f);
    mAnimText.setPos(0.f, 0.05f);

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    mModelLoadedText.setText("EntityLoaded: " + ent->getName());
    mActualAnim = mAnims.empty() ? 0 : mAnims[0];
    if (mActualAnim == 0) {
        mAnimText.setText("No animations found\n");
    } else {
        mAnimText.setText("Animation: " + mActualAnim->getAnimationName());
    }

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
TriggerZone::update()
{
    static bool keyNextPressed = false;
    static bool keyBackPressed = false;

    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
    }

    if (mActualAnim == 0) {
        return;
    }
	mActualAnim->addTime(mGlobalTimeFrame);

    if (mActualAnim->hasEnded()) {
        // start the animation again
        mActualAnim->setTimePosition(0.f);
    }

    // check for user input
    if (mKeyboard->isKeyDown(OIS::KC_ADD)) {
        if (!keyNextPressed) {
            keyNextPressed = true;
            // add one to the counter
            mCurrentIndex = (mCurrentIndex + 1) % mAnims.size();
            changeAnim(mCurrentIndex);
        }
    } else {
        keyNextPressed = false;
    }
    if (mKeyboard->isKeyDown(OIS::KC_MINUS)) {
        if (!keyBackPressed) {
            keyBackPressed = true;
            // decrement one to the counter
            mCurrentIndex = (mCurrentIndex == 0) ? mAnims.size() - 1 : mCurrentIndex - 1;
            changeAnim(mCurrentIndex);
        }
    } else {
        keyBackPressed = false;
    }

    handleCameraInput();

}

}
