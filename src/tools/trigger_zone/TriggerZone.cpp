/*
 * TriggerZone.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "TriggerZone.h"

#include <memory>
#include <cmath>

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>

#include <xml/XMLHelper.h>
#include <tinyxml/tinyxml.h>

namespace tool {

const Ogre::Real TriggerZone::RANDOM_POSITION = 2000.0f;



////////////////////////////////////////////////////////////////////////////////
bool
TriggerZone::loadFloor(const TiXmlElement* xml)
{
    ASSERT(xml);

    // get the xml
    //<Floor materialName="floor_trigger_zone" sizeX="5000" sizeY="4000" />
    //
    const TiXmlElement* floor = xml->FirstChildElement("Floor");
    if (!floor) {
        debugWARNING("No floor set\n");
        return false;
    }

    // get the material name and floor sizes
    float sizeX, sizeY;
    core::XMLHelper::parseFloat(floor, "sizeX", sizeX);
    core::XMLHelper::parseFloat(floor, "sizeX", sizeY);

    // get the material name
    const char* matName = floor->Attribute("materialName");
    if (!matName) {
        debugWARNING("No material name was set\n");
        return false;
    }

    // create the floor entity and node (we will leak here, is not important)
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3::UNIT_Y;
    plane.d = 0;

    Ogre::MeshManager& mm = Ogre::MeshManager::getSingleton();
    mm.createPlane("triggerFloor",
                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                   plane,
                   sizeX, sizeY,
                   20, 20,
                   true,
                   1,
                   1, 1,
                   Ogre::Vector3::UNIT_Z);
    Ogre::Entity* planeEnt = mSceneMgr->createEntity("triggerPlane", "triggerFloor");
    planeEnt->setMaterialName(matName);
    Ogre::SceneNode* planeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    planeNode->attachObject(planeEnt);

    // move a little bit down to be able to show the zones above the floor
    planeNode->translate(0, -5, 0);

    mFloorAABB.tl.x = -sizeX * .5f;
    mFloorAABB.tl.y = sizeY * .5f;
    mFloorAABB.br.x = sizeX * .5f;
    mFloorAABB.br.y = -sizeY * .5f;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::configureCamera(void)
{
    // set the zone for where we will be moving on
    Ogre::AxisAlignedBox moveZone(mFloorAABB.tl.x,
                                  50,
                                  mFloorAABB.br.y,
                                  mFloorAABB.br.x,
                                  std::max(mFloorAABB.getHeight(), mFloorAABB.getWidth()),
                                  mFloorAABB.tl.y);
    // configure the velocity taking into account the size of the level
    mSatelliteCamera.configure(moveZone, 10);

}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
            lMouseState.X.abs <= 0) {
        mTranslationVec.x += 1.0f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
            lMouseState.X.abs >= lMouseState.width) {
        mTranslationVec.x -= 1.0f;
    }

    if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) ||
            lMouseState.Y.abs <= 0) {
        mTranslationVec.z += 1.0f;
    }
    if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
            lMouseState.Y.abs >= lMouseState.height) {
        mTranslationVec.z -= 1.0f;
    }

    const float lMouseZ = float(lMouseState.Z.rel);
    if (lMouseZ > 0.0f) {
        mTranslationVec.y += 10.f;
    } else if (lMouseZ < 0.0f) {
        mTranslationVec.y -= 10.f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mSatelliteCamera.translate(mTranslationVec * (200.f * mGlobalTimeFrame));
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TriggerZone::TriggerZone() :
    core::AppTester(mTimeFrame)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
{
    setUseDefaultInput(false);
}

////////////////////////////////////////////////////////////////////////////////
TriggerZone::~TriggerZone()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::loadAditionalData(void)
{
    // load the xml
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(TRIGGER_ZONE_TOOL_FILE));
    if (doc.get() == 0) {
        debugWARNING("Error loading file %s\n", TRIGGER_ZONE_TOOL_FILE);
        return;
    }

    // load the floor
    if (!loadFloor(doc->RootElement())) {
        debugWARNING("Some error occur when loading the floor\n");
    }

    // configure the stallite camera
    configureCamera();
}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::update()
{
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
    }

    handleCameraInput();

}

}
