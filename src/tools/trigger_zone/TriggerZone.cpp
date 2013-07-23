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

#include <OIS/OISMouse.h>

#include <xml/XMLHelper.h>
#include <tinyxml/tinyxml.h>
#include <trigger_system/TriggerZone.h>
#include <input/InputMouse.h>
#include <input/InputKeyboard.h>

#include "TZone.h"



// helper stuff
//
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);

    return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_A);
    buttons.push_back(input::KeyCode::KC_S);
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_W);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_DOWN);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_UP);

    return buttons;
}

}


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
TriggerZone::createZone(void)
{
    ASSERT(false); // TODO
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

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT) ||
            mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT) ||
            mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x -= 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP) ||
            mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN) ||
            mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z -= 1.0f;
    }

    const float lMouseZ = float(input::Mouse::relZ());
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
,   mSelectionHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mState(InternalState::S_Normal)

{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);

    setUseDefaultInput(false);
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
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


    // TODO: remove this, is just for test
    core::TriggerZone tz;
    tz.setZone(core::TZType(100,100, 0, 500));
    SelectableObject* zone = new TZone(tz, Ogre::ColourValue::Green);
    tz.setZone(core::TZType(500, 400, 20, 800));
    SelectableObject* zone2 = new TZone(tz, Ogre::ColourValue::Red);

}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update mouse cursor
    mMouseCursor.updatePosition(input::Mouse::absX(), input::Mouse::absY());

    // depending on the internal state we should do different things
    switch (mState) {
    case InternalState::S_Normal:
        mSelectionHelper.update();
        break;

    case InternalState::S_CreateZone:

        break;

    default:
        ASSERT(false && "Invalid internal state");
    }


    // update camera
    handleCameraInput();

}

}
