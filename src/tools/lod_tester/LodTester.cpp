/*
 * LodTester.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "LodTester.h"

#include <memory>
#include <string>
#include <vector>

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreSkeletonManager.h>
#include <OgreMeshManager.h>
#include <OgreStringConverter.h>
#include <OgreMeshSerializer.h>
#include <OgreAxisAlignedBox.h>
#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <types/basics.h>



namespace tool {

////////////////////////////////////////////////////////////////////////////////
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

    if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A))
    {
        mTranslationVec.x -= 1.0f * increaseFactor;
    }
    if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D))
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
    if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W))
    {
        mTranslationVec.z -= 1.0f * increaseFactor;
    }
    if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
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

////////////////////////////////////////////////////////////////////////////////
bool
LodTester::loadLODxml(Ogre::MeshPtr& resultMesh)
{
    // Load the xml document
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(LOD_XML_FILE));
    if (doc.get() == 0) {
        return false;
    }

    // parse the document
    //    <LODEntity>
    //        <Level meshName="mesh1.mesh" />
    //        <Level meshName="mesh2.mesh" distance="50" />
    //        <Level meshName="mesh3.mesh" distance="230" />
    //    </LODEntity>
    //
    const TiXmlElement* root = doc->RootElement();
    if (root == 0) {
        debugERROR("Ill formed xml, no root element\n");
        return false;
    }

    if (std::string(root->Value()) != "LODEntity") {
        debugERROR("Ill formed xml, we spect LODEntity and we get %s\n",
            root->Value());
        return false;
    }
    std::vector<const TiXmlElement*> levels;
    levels.reserve(10);
    core::XMLHelper::getFirstElements(root, levels);

    if (levels.empty()) {
        debugWARNING("No Levels found for LODEntity\n");
        return false;
    }

    // get the main mesh
    const char* value = levels.front()->Attribute("meshName");
    if (value == 0) {
        debugERROR("No meshname given for the main mesh in the first Level\n");
        return false;
    }

    // load meshes
    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    resultMesh = meshManager.load(value, "Popular");
    resultMesh->removeLodLevels();
    Ogre::SkeletonPtr skeleton = resultMesh->getSkeleton();

    // iterate over all the elements
    for (core::size_t i = 1, size = levels.size(); i < size; ++i) {
        value = levels[i]->Attribute("meshName");
        if (value == 0) {
            debugERROR("No meshname given for the mesh level %d\n",
                static_cast<int>(i));
            continue;
        }
        const char* distanceStr = levels[i]->Attribute("distance");
        if (distanceStr == 0) {
            debugERROR("No distance found for mesh in Level %d\n",
                static_cast<int>(i));
            continue;
        }
        const Ogre::Real distance = Ogre::StringConverter::parseReal(distanceStr);

        // parse the mesh and add it to the mesh1
        Ogre::MeshPtr mesh = meshManager.load(value, "Popular");
        mesh->setSkeletonName(skeleton->getName());
        resultMesh->createManualLodLevel(distance, mesh->getName());
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////
LodTester::LodTester() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
{
    setUseDefaultInput(false);
    mOrbitCamera.setCameraType(OrbitCamera::CameraType::FreeFly);
}

////////////////////////////////////////////////////////////////////////////////
LodTester::~LodTester()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
LodTester::loadAditionalData(void)
{
    // Load the meshes and LOD first!
    Ogre::MeshPtr lodMesh;
    if (!loadLODxml(lodMesh)) {
        debugERROR("Error loading lod?\n");
        return;
    }

    // Then create the entity using the already loaded mesh
    mEntity = mSceneMgr->createEntity("lodMesh", lodMesh->getName());
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(mEntity);

    // load the animation
    mActualAnim = mEntity->getAnimationState("camina");
    ASSERT(mActualAnim);
    mActualAnim->setLoop(true);
    mActualAnim->setEnabled(true);

    // configure the text for ogre
    mDistanceText.setPos(0.f, 0.f);

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }
}

////////////////////////////////////////////////////////////////////////////////
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

    handleCameraInput();

}

}
