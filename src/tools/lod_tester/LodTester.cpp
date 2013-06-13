/*
 * LodTester.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreSkeletonManager.h>
#include <OgreMeshManager.h>
#include <OgreStringConverter.h>

#include "LodTester.h"

namespace tool {


void
LodTester::handleCameraInput()
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

LodTester::LodTester() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
{
    setUseDefaultInput(false);
}

LodTester::~LodTester()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
LodTester::loadAditionalData(void)
{
    // Load the meshes and LOD first!
    // load the lod meshes
    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    Ogre::MeshPtr hiMesh = meshManager.load("gusanohi.mesh", "Popular");
    Ogre::MeshPtr lowMesh = meshManager.load("gusanolow.mesh", "Popular");
    hiMesh->createManualLodLevel(25.0f, lowMesh->getName());

    // use the same skeleton
    Ogre::SkeletonPtr skeleton = hiMesh->getSkeleton();
    lowMesh->setSkeletonName(skeleton->getName());

    // Then create the entity using the already loaded mesh
    mEntity = mSceneMgr->createEntity("gusano", hiMesh->getName());
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(mEntity);
    mEntity->setMaterialName("gusano_real");

    // load the animation
    mActualAnim = mEntity->getAnimationState("camina");
    ASSERT(mActualAnim);
    mActualAnim->setLoop(true);
    mActualAnim->setEnabled(true);

    // configure the text for ogre
    mDistanceText.setPos(0.f, 0.f);
    mMeshText.setPos(0.f, 0.08f);

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
LodTester::update()
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

    // calculate distance and print it
	const Ogre::Real dist = mOrbitCamera.getCameraPosition().distance(mNode->getPosition());
	mDistanceText.setText("Distance to object: " + Ogre::StringConverter::toString(dist));
	mMeshText.setText("MeshName: " + mEntity->getMesh()->getName());

    handleCameraInput();

}

}
