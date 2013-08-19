/*
 * HeightMapTool.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef HeightMapTool_H_
#define HeightMapTool_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include <vector>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <types/basics.h>
#include <math/AABB.h>
#include <input/InputHelper.h>
#include <heightmap/HeightMap.h>


// forward
//
class TiXmlElement;


namespace tool {

class HeightMapTool : public core::AppTester
{
public:
    HeightMapTool();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~HeightMapTool();

private:

    // @brief Load the floor and the base plane using the xml file
    // @param xml   The root xml element of the xml file
    // @return true on success | false otherwise
    //
    bool
    loadFloor();

    // @brief Create Player
    //
    void
    createPlayer(void);

    void
    handlePlayer(void);
    void
    handleCameraInput(void);


private:
    float mTimeFrame;
    core::OgreText mPlayerPos;
    input::InputHelper mInputHelper;
    Ogre::SceneNode* mPlayer;
    Ogre::Real mPlayerHalfHeight;
    gps::HeightMap<Ogre::Vector3> mHeightMap;

};

}

#endif /* HeightMapTool_H_ */
