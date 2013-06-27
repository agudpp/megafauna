/*
 * PrimitiveDrawer.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: agustin
 */

#include "PrimitiveDrawer.h"

#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#include <OgreSubEntity.h>

#include "OgreNameGen.h"

namespace core {

////////////////////////////////////////////////////////////////////////////////
void
Primitive::setColor(const Ogre::ColourValue& color)
{
    const Ogre::String& matName = (isManual) ? obj.manual->getSection(0)->getMaterialName() :
        obj.ent->getSubEntity(0)->getMaterialName();

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
    mat->setReceiveShadows(false);
    mat->getTechnique(0)->setLightingEnabled(true);
    mat->getTechnique(0)->getPass(0)->setDiffuse(color.r, color.g, color.b, color.a);
    mat->getTechnique(0)->getPass(0)->setAmbient(color.r, color.g, color.b);
    mat->getTechnique(0)->getPass(0)->setSelfIllumination(color.r, color.g, color.b);
}

////////////////////////////////////////////////////////////////////////////////
void
Primitive::setAlpha(Ogre::Real alpha)
{
    ASSERT(false);
}


PrimitiveDrawer::PrimitiveDrawer() :
    mSceneMngr(0)
{
    Ogre::SceneManagerEnumerator::SceneManagerIterator it =
        Ogre::Root::getSingleton().getSceneManagerIterator();
    mSceneMngr = it.getNext();

    // create base mat
    mBaseMat = Ogre::MaterialManager::getSingleton().create("PrimitiveDrawerBaseMat",
                                                            "General");
}

PrimitiveDrawer::~PrimitiveDrawer()
{
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createBox(const Ogre::Vector3& center,
                           const Ogre::Vector3& sizes,
                           const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual
    manual->begin(newMat->getName());

    const Ogre::Vector3 halfSize = sizes * 0.5f;



    // create the different vertices, first the top four and then the bottom
    // four
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, -halfSize.z));
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, -halfSize.z));

    // bottom
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, -halfSize.z));
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, -halfSize.z));

    // set the triangles
    manual->triangle(2, 1, 0); manual->triangle(0, 3, 2);   // top
    manual->triangle(4, 5, 6); manual->triangle(7, 4, 6);   // bottom
    manual->triangle(1, 2, 6); manual->triangle(1, 6, 5);   // left
    manual->triangle(2, 7, 6); manual->triangle(2, 3, 7);
    manual->triangle(3, 4, 7); manual->triangle(4, 3, 0);
    manual->triangle(4, 0, 1); manual->triangle(5, 4, 1);

    manual->end(); // and building the manual

    // create the scene node
    Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(manual);
    node->setPosition(center);

    PrimitivePtr box(new Primitive(node, manual));
    box->setColor(color);

    box->id = mPrimitives.size();
    mPrimitives.push_back(box);

    return box.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createPlane(const Ogre::Vector3& center,
                             const Ogre::Vector2& dims,
                             const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual (2 triangles)
    manual->begin(newMat->getName());

    const Ogre::Vector2 halfDims = dims * .5f;

    // 1---------2
    // |         |
    // |         |
    // 0---------3

    manual->position(-halfDims.x, 0, -halfDims.y);
    manual->position(-halfDims.x, 0, halfDims.y);
    manual->position(halfDims.x, 0, halfDims.y);
    manual->position(halfDims.x, 0, -halfDims.y);

    // create triangles
    manual->triangle(0, 1, 2);
    manual->triangle(2, 3, 0);

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);
   node->setPosition(center);

   PrimitivePtr plane(new Primitive(node, manual));
   plane->setColor(color);

   plane->id = mPrimitives.size();
   mPrimitives.push_back(plane);

   return plane.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createSphere(const Ogre::Vector3& center,
                              Ogre::Real radius,
                              const Ogre::ColourValue& color)
{
    ASSERT(false); return 0;
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createLine(const Ogre::Vector3& p1,
                            const Ogre::Vector3& p2,
                            const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual one line
    manual->begin(newMat->getName(), Ogre::RenderOperation::OT_LINE_LIST);

    manual->position(p1);
    manual->position(p2);

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);

   PrimitivePtr line(new Primitive(node, manual));
   line->setColor(color);

   line->id = mPrimitives.size();
   mPrimitives.push_back(line);

   return line.get();
}


////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createMultiline(const std::vector<Ogre::Vector3>& points,
                                 const Ogre::ColourValue& color)
{
    ASSERT(points.size() >= 2);

    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual one line
    manual->begin(newMat->getName(), Ogre::RenderOperation::OT_LINE_LIST);

    for (core::size_t i = 1, size = points.size(); i < size; ++i) {
        manual->position(points[i-1]);
        manual->position(points[i]);
    }

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);

   PrimitivePtr line(new Primitive(node, manual));
   line->setColor(color);

   line->id = mPrimitives.size();
   mPrimitives.push_back(line);

   return line.get();
}


////////////////////////////////////////////////////////////////////////////////
void
PrimitiveDrawer::deletePrimitive(Primitive* primitive)
{
    ASSERT(primitive);
    ASSERT(primitive->id < mPrimitives.size());

    if (mPrimitives.size() == 1) {
        mPrimitives.clear();
        return;
    }

    // swap with last
    mPrimitives.back()->id = primitive->id;
    mPrimitives[primitive->id] = mPrimitives.back();
    mPrimitives.pop_back();
}


} /* namespace core */
