/*
 * HeightMapUtils.h
 *
 *  Created on: Aug 8, 2013
 *      Author: agustin
 */

#ifndef HEIGHTMAPUTILS_H_
#define HEIGHTMAPUTILS_H_


#include <OgreVector3.h>

#include "HeightMap.h"



// General Forward
//
namespace Ogre {
class Mesh;
}

namespace gps {
namespace HeightMapUtils {

// @brief Configure a HeightMap structure from a given mesh.
// @param mesh          The mesh we want to "parse".
// @param hm            The HeightMap structure to be configured (built).
// @returns             True on success | false otherwise.
// @note This method will be used for fast checkings, the idea is to NOT use
//       this method on the final version, since we are loading and performing
//       a lot of operations that could be done "off-line". For that, used the
//       exporter / importer and the associated data instead.
//       We are also assuming that the mesh is already positioned where it should
//       (we can pass a transformation matrix if needed, not for now)
//
bool
configureFromMesh(const Ogre::Mesh* mesh, HeightMap<Ogre::Vector3>& hm);


} /* namespace HeightMapUtils */
} /* namespace gps */
#endif /* HEIGHTMAPUTILS_H_ */
