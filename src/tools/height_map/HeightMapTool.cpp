/*
 * HeightMapTool.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "HeightMapTool.h"

#include <memory>
#include <cmath>

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
#include <input/InputMouse.h>
#include <input/InputKeyboard.h>
#include <debug/PrimitiveDrawer.h>
#include <heightmap/HeightMapUtils.h>
#include <heightmap/HeightMap.h>



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
HeightMapTool::loadFloor()
{
    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load("terreno.mesh",
        Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    ASSERT(mesh.get());
    gps::HeightMap<Ogre::Vector3> hm;
    gps::HeightMapUtils::configureFromMesh(mesh.get(), hm);

    Ogre::Entity* terrain = mSceneMgr->createEntity("terreno.mesh");
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(terrain);

    return true;
}


////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::handlePlayerInput()
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
        //mPlayer.translate(mTranslationVec.x, mTranslationVec.y);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
HeightMapTool::HeightMapTool() :
    core::AppTester(mTimeFrame)
//,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
//,   mPlayer(mSceneMgr)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);

    mPlayerPos.setPos(0.f, 0.0f);
    mPlayerPos.setColor(0, 0, 1, 1.f);
}

////////////////////////////////////////////////////////////////////////////////
HeightMapTool::~HeightMapTool()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::loadAditionalData(void)
{
    loadFloor();
}

////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update player movement
    handlePlayerInput();

    // show the position of the player and the camera
 //   mPlayerPos.setText("PlayerPosition: " + toString(mPlayer.position()), 0.023f);

}

}
