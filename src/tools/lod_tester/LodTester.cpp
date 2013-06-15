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
#include <OgreMeshSerializer.h>
#include <OgreAxisAlignedBox.h>


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

    const Ogre::Real increaseFactor = (mKeyboard->isKeyDown(OIS::KC_LSHIFT)) ?
        0.2f : 1.0f;


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
            lMouseState.X.abs <= 0)
    {
        mTranslationVec.x -= 1.0f * increaseFactor;
    }
    if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
            lMouseState.X.abs >= lMouseState.width)
    {
        mTranslationVec.x += 1.0f * increaseFactor;
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
        mTranslationVec.z -= 1.0f * increaseFactor;
    }
    if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
            lMouseState.Y.abs >= lMouseState.height)
    {
        mTranslationVec.z += 1.0f * increaseFactor;
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
        scrollZoom += 5.f * increaseFactor;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f * increaseFactor;
    }
    if(scrollZoom != mOrbitCamera.zoom()){
        mOrbitCamera.setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
    if(lMouseState.buttonDown(OIS::MB_Right)){
        const float factor = -0.01 * 1.5f * increaseFactor;
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
    const Ogre::Real lodValue = 100.0f;

    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    Ogre::MeshPtr mesh1 = meshManager.load("mesh1.mesh", "Popular");
    Ogre::MeshPtr mesh2 = meshManager.load("mesh2.mesh", "Popular");
    Ogre::MeshPtr mesh3 = meshManager.load("mesh3.mesh", "Popular");
    Ogre::MeshPtr mesh4 = meshManager.load("mesh4.mesh", "Popular");
    mesh1->removeLodLevels();
    mesh1->createManualLodLevel(lodValue, mesh2->getName());
    mesh1->createManualLodLevel(lodValue + 50.0f, mesh3->getName());
    mesh1->createManualLodLevel(lodValue + 100.0f, mesh4->getName());

    // use the same skeleton
    Ogre::SkeletonPtr skeleton = mesh1->getSkeleton();
    mesh2->setSkeletonName(skeleton->getName());
    mesh3->setSkeletonName(skeleton->getName());
    mesh4->setSkeletonName(skeleton->getName());

    // export it
//    Ogre::MeshSerializer serializer;
//    serializer.exportMesh(mesh1.get(), "serializedMesh.mesh");

    // Then create the entity using the already loaded mesh
    mEntity = mSceneMgr->createEntity("gusano", mesh1->getName());
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(mEntity);
    mEntity->setMaterialName("gusano_real");


    // do some debug info
    Ogre::MeshPtr entMesh = mEntity->getMesh();
    std::cout << "isLodManual(ent): " << entMesh->isLodManual() << std::endl <<
        "isLodManual(real): " << mesh1->isLodManual() << std::endl;
    std::cout << "SceneManager: " << mSceneMgr->getName() << std::endl;
    std::cout << "Mesh1->lodCount(): "  << mesh1->getNumLodLevels() << std::endl;

    const Ogre::AxisAlignedBox& aabbm1 = mesh1->getBounds();
    const Ogre::AxisAlignedBox& aabbm2 = mesh2->getBounds();
    const Ogre::AxisAlignedBox& aabbm3 = mesh3->getBounds();
    const Ogre::AxisAlignedBox& aabbm4 = mesh4->getBounds();
    std::cout << "box1: " << aabbm1.getSize().length() << " and distance to camera: " <<
        aabbm1.distance(mOrbitCamera.getCameraPosition()) << std::endl;
    std::cout << "box2: " << aabbm2.getSize().length() << " and distance to camera: " <<
        aabbm2.distance(mOrbitCamera.getCameraPosition()) << std::endl;
    std::cout << "box3: " << aabbm3.getSize().length() << " and distance to camera: " <<
        aabbm3.distance(mOrbitCamera.getCameraPosition()) << std::endl;
    std::cout << "box4: " << aabbm4.getSize().length() << " and distance to camera: " <<
        aabbm4.distance(mOrbitCamera.getCameraPosition()) << std::endl;

    const Ogre::LodStrategy* strategy = mesh1->getLodStrategy();
    ASSERT(strategy);
    std::cout << "strategy->transformUserValue(lodValue): " << strategy->transformUserValue(lodValue) << std::endl;
    std::cout << "strategy->name(): " << strategy->getName().c_str() << std::endl;
    ushort lodIndex = mesh1->getLodIndex(strategy->transformUserValue(lodValue));
    std::cout << "lodIndex:" << lodIndex << std::endl;

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
