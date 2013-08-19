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
    gps::HeightMapUtils::configureFromMesh(mesh.get(), mHeightMap);

    Ogre::Entity* terrain = mSceneMgr->createEntity("terreno.mesh");
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(terrain);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::createPlayer()
{
    Ogre::Entity* ent = mSceneMgr->createEntity(Ogre::SceneManager::PrefabType::PT_CUBE);
    mPlayer = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mPlayer->attachObject(ent);
    mPlayerHalfHeight = ent->getBoundingBox().getSize().z/2.0f + 1;
    mPlayer->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);
}

////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::handlePlayer()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)) {
        mTranslationVec.x += 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
        mTranslationVec.y += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
        mTranslationVec.y -= 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        ASSERT(mPlayer);
        // check if we are inside
        mTranslationVec *= (200.f * mGlobalTimeFrame);
        Ogre::Vector3 resultPos = mPlayer->getPosition() + mTranslationVec;
        if (!mHeightMap.isPositionInside(core::Vector2(resultPos.x, resultPos.y))) {
            // nothing to do, we are outside
            return;
        }

        // calculate the height
        Ogre::Vector3 normal;
        resultPos.z = mHeightMap.getHeightAndNormal(
            core::Vector2(resultPos.x, resultPos.y), normal) + mPlayerHalfHeight;
        mPlayer->setPosition(resultPos);
        // get current orientation and the angle between two vectors
        //Ogre::Vector3 oldUP = mPlayer->getOrientation().zAxis();
        Ogre::Quaternion newUPRot = Ogre::Vector3::UNIT_Y.getRotationTo(normal);
        mPlayer->setOrientation(newUPRot);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
HeightMapTool::handleCameraInput()
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

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mBodySceneNode->translate(mTranslationVec * (200.f * mGlobalTimeFrame),
            Ogre::Node::TS_LOCAL);
    }

    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    // now we actualize the direction of the camera.
    const OIS::MouseState& lMouseState = mMouse->getMouseState();
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
    //float lRotCoeff = -5.0f;
    Ogre::Radian lAngleX(lMouseX * -0.01f);
    Ogre::Radian lAngleY(lMouseY * -0.01f);
    // If the 'player' don't make loopings, 'yaw in world' + 'pitch in local' is often enough for a camera controler.
    mBodySceneNode->roll(lAngleX, Ogre::Node::TS_WORLD);
//  mCameraScnNode->pitch(lAngleY);
    mBodySceneNode->pitch(lAngleY);

    ///////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
HeightMapTool::HeightMapTool() :
    core::AppTester(mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mPlayer(0)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);

    mPlayerPos.setPos(0.f, 0.0f);
    mPlayerPos.setColor(0, 0, 1, 1.f);

    setUseDefaultInput(false);
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
    createPlayer();

    // create a 3D axis
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    pd.create3DAxis(mCamera->getRealPosition(), 15);
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
    handlePlayer();
    handleCameraInput();

    // show the position of the player and the camera
 //   mPlayerPos.setText("PlayerPosition: " + toString(mPlayer.position()), 0.023f);

}

}
