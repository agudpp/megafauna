/*
 * SelectionHelper.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: agustin
 */

#include "SelectionHelper.h"

#include <OgreUserObjectBindings.h>

#include <types/basics.h>

namespace tool {

////////////////////////////////////////////////////////////////////////////////
SelectableObject::SelectableObject(Ogre::SceneNode* node) :
    mNode(node)
{
    ASSERT(mNode);
    Ogre::UserObjectBindings& binding = mNode->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(this));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::performRaycast()
{
    ASSERT(mRaySceneQuery);

    // configure the ray
    mMouseRay = mCamera.getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                               mMouseCursor.getYRelativePos());
    mRaySceneQuery->setRay(mMouseRay);
    mRaySceneQuery->setQueryMask(~0); // all
    mRaySceneQuery->setSortByDistance(true, 1); // the first one

    // execute the ray and get the results
    Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
    if (result.empty()) {
        // nothing to return
        return 0;
    }
    ASSERT(result.size() == 1); // we want only one

    Ogre::RaySceneQueryResultEntry& entry = result.back();
    if (entry.movable == 0) {
        debugWARNING("We pick a worldfragment object... that is ok?\n");
        return 0;
    }
    Ogre::SceneNode* node = entry.movable->getParentSceneNode();
    if (node == 0) {
        debugERROR("Some movable object without scene node associated!!\n");
        return 0;
    }

    // return the object if is a selectable one
    return Ogre::any_cast<SelectableObject*>(node->getUserObjectBindings().getUserAny());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

SelectionHelper::SelectionHelper(Ogre::SceneManager& sm,
                                 Ogre::Camera& camera,
                                 const ui::MouseCursor& mouseCursor) :
    mSceneMngr(sm)
,   mCamera(camera)
,   mMouseCursor(mouseCursor)
,   mLastSelection(0)
,   mLastRaycasted(0)
,   mRaySceneQuery(0)
{
    mRaySceneQuery = mSceneMngr.createRayQuery(mMouseRay,~0);
}

SelectionHelper::~SelectionHelper()
{
    mSceneMngr.destroyQuery(mRaySceneQuery);
}

////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::update(const OIS::MouseState& mouseState)
{
    // get the current position
    const Ogre::Vector2 currentPos(mMouseCursor.getXRelativePos(),
                                   mMouseCursor.getYRelativePos());

    // check if we are in the same position (and assuming that nothing is
    // moving)
    if (currentPos == mLastMousePos) {
        // do nothing, no new object intersected
        return 0;
    }

    // update the new position
    mLastMousePos = currentPos;

    // check if new object is raycasted
    SelectableObject* newObjectRaycasted = performRaycast();

    if (newObjectRaycasted != 0) {
        // no new object raycasted, if we had object raycasted before we need
        // to advise it
        if (newObjectRaycasted == mLastRaycasted) {
            // is the same than before, do nothing and return
            return 0;
        }

        // the new raycasted object is a new one, trigger the event
        newObjectRaycasted->mouseOver();

        // check if we have to advise to the old one
        if (mLastRaycasted != 0) {
            mLastRaycasted->mouseExit();
        }
    } else {
        // we have no object raycasted... check if we had one before
        if (mLastRaycasted != 0) {
            mLastRaycasted->mouseExit();
        }
    }

    // in any case we should update the mLastRaycasted with the new one
    return mLastRaycasted = newObjectRaycasted;
}

////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::getLastRaycasted(void)
{
    return mLastRaycasted;
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::getSelected(std::vector<SelectableObject*>& selected)
{
    selected = mSelectedObjects;
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::select(SelectableObject* object)
{
    ASSERT(object);

    // check if the object is already selected
    if (isSelected(object)) {
        // do nothing
        return;
    }
    // else add it to the list
    object->id = mSelectedObjects.size();
    mSelectedObjects.push_back(object);
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::unselect(SelectableObject* object)
{
    ASSERT(object);
    ASSERT(object->id < mSelectedObjects.size());
    ASSERT(mSelectedObjects[object->id] == object);
    object->objectUnselected();

    // swap with last
    mSelectedObjects[object->id] = mSelectedObjects.back();
    mSelectedObjects[object->id]->id = object->id;
    mSelectedObjects.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::unselectAll(void)
{
    for (core::size_t i = 0, size = mSelectedObjects.size(); i < size; ++i) {
        mSelectedObjects[i]->objectUnselected();
    }
    mSelectedObjects.clear();
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::setBaseIntersectionPlane(const Ogre::Plane& plane)
{
    mPlane = plane;
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionHelper::getPlaneIntersection(const Ogre::Vector2& mousePos, Ogre::Vector3& intPos)
{
    // set up the ray
    mMouseRay = mCamera.getCameraToViewportRay(mousePos.x, mousePos.y);

    // check if the ray intersects our plane
    // intersects() will return whether it intersects or not (the bool value) and
    // what distance (the Real value) along the ray the intersection is
    std::pair<bool, Ogre::Real> result = mMouseRay.intersects(mPlane);

    if (result.first)
    {
        // if the ray intersect the plane, we have a distance value
        // telling us how far from the ray origin the intersection occurred.
        // the last thing we need is the point of the intersection.
        // Ray provides us getPoint() function which returns a point
        // along the ray, supplying it with a distance value.

        // get the point where the intersection is
        Ogre::Vector3 v = mMouseRay.getPoint(result.second);
        intPos.x = v.x;
        intPos.y = v.z;
        return true;
    }

    return false;
}

} /* namespace tool */
