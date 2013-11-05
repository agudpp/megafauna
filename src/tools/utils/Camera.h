/*
 * Camera.h
 *
 *  Created on: Jul 10, 2013
 *      Author: agustin
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreAxisAlignedBox.h>

#include <debug/DebugUtil.h>
#include <debug/OgreText.h>

namespace tool {

class Camera {
public:
    Camera(Ogre::Camera* camera,
           Ogre::SceneManager* manager,
           float& globalTimeFrame);
    virtual ~Camera();

private:
    core::OgreText mName;
    Ogre::Camera* mCamera;
    float& mGlobalTimeFrame;
};

}

#endif /* CAMERA_H_ */
