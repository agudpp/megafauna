/*
 * TriggerZone.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "TriggerZone.h"

#include <memory>

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
                   sizeX,
                   sizeY);
    Ogre::Entity* planeEnt = mSceneMgr->createEntity("triggerPlane", "triggerFloor");
    planeEnt->setMaterialName(matName);
    Ogre::SceneNode* planeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    planeNode->attachObject(planeEnt);

    return true;
}


////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TriggerZone::TriggerZone() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
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



}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::update()
{
    handleCameraInput();

}

}
