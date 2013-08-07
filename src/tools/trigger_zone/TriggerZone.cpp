/*
 * TriggerZone.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "TriggerZone.h"

#include <memory>
#include <cmath>

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
#include <OgreStringConverter.h>

#include <OIS/OISMouse.h>

#include <xml/XMLHelper.h>
#include <tinyxml/tinyxml.h>
#include <trigger_system/TriggerZone.h>
#include <input/InputMouse.h>
#include <input/InputKeyboard.h>
#include <debug/PrimitiveDrawer.h>



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

// Create a string from a vector
//
Ogre::String
toString(const Ogre::Vector3& pos)
{
    return Ogre::StringConverter::toString(pos);
}

}


namespace tool {


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
    plane.normal = Ogre::Vector3::UNIT_Z;
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
                   Ogre::Vector3::UNIT_Y);
    Ogre::Entity* planeEnt = mSceneMgr->createEntity("triggerPlane", "triggerFloor");
    planeEnt->setMaterialName(matName);
    Ogre::SceneNode* planeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    planeNode->attachObject(planeEnt);
//    planeNode->yaw(Ogre::Radian(Ogre::Math::PI));

    // move a little bit down to be able to show the zones above the floor
    planeNode->translate(0, 0, -5);

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
                                  mFloorAABB.br.y,
                                  50,
                                  mFloorAABB.br.x,
                                  mFloorAABB.tl.y,
                                  std::max(mFloorAABB.getHeight(), mFloorAABB.getWidth()));
    // configure the velocity taking into account the size of the level
    mSatelliteCamera.configure(moveZone, 10);

}

////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::configureTriggerSystem(void)
{
    // TODO: HARD-coded zones:
    std::vector<core::TriggerZone> zones;

    // TODO: remove this, is just for test
    zones.push_back(core::TriggerZone(core::TZType(100,100, 0, 500)));
    mTZones.push_back(TZone(zones.back(), core::PrimitiveDrawer::instance().getFreshColour()));
    zones.push_back(core::TriggerZone(core::TZType(500, 400, 20, 800)));
    mTZones.push_back(TZone(zones.back(), core::PrimitiveDrawer::instance().getFreshColour()));
    zones.push_back(core::TriggerZone(core::TZType(1600, 1200, 1000, 2000)));
    mTZones.push_back(TZone(zones.back(), core::PrimitiveDrawer::instance().getFreshColour()));

    // create the zones
    mTriggerSystem.build(zones, mIDs);

    ASSERT(mIDs.size() == mTZones.size());

    // create now the callbacks
    for (core::size_t i = 0, size = mTZones.size(); i < size; ++i) {
        mTZones[i].getConnection() = mTriggerSystem.addCallback(mIDs[i],
            boost::bind(&TZone::eventHandler, &(mTZones[i]), _1));
    }

    // configure the agent
   core::TriggerAgent* agent = mTriggerSystem.createAgent();
   ASSERT(agent);
   mTriggerSystem.initializeAgent(agent, core::Vector2(0,0));
   mPlayer.initialize(agent, Ogre::Vector3(0,0,0));

    debugGREEN("TriggerSystem configured..\n");
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

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)) {
        mTranslationVec.x -= 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
        mTranslationVec.y -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
        mTranslationVec.y += 1.0f;
    }

    const float lMouseZ = float(input::Mouse::relZ());
    if (lMouseZ > 0.0f) {
        mTranslationVec.z += 10.f;
    } else if (lMouseZ < 0.0f) {
        mTranslationVec.z -= 10.f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mSatelliteCamera.translate(mTranslationVec * (200.f * mGlobalTimeFrame));
    }

}
////////////////////////////////////////////////////////////////////////////////
void
TriggerZone::handlePlayerInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.y += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.y -= 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mTranslationVec *= (200.f * mGlobalTimeFrame);
        mPlayer.translate(mTranslationVec.x, mTranslationVec.y);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TriggerZone::TriggerZone() :
    core::AppTester(mTimeFrame)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mPlayer(mSceneMgr)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);

    setUseDefaultInput(false);
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());

    mCameraPos.setPos(0.f, 0.05f);
    mCameraPos.setColor(0, 0, 1, 1.f);
    mPlayerPos.setPos(0.f, 0.0f);
    mPlayerPos.setColor(0, 0, 1, 1.f);
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

    // configure the TriggerSystem
    configureTriggerSystem();
    mPlayer.translate(2000, 1000);
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

    // update camera
    handleCameraInput();

    // update player movement
    handlePlayerInput();

    // show the position of the player and the camera
    mCameraPos.setText("Camera Position: " + toString(mSatelliteCamera.getWorldCamPos()), 0.023f);
    mPlayerPos.setText("PlayerPosition: " + toString(mPlayer.position()), 0.023f);

}

}
