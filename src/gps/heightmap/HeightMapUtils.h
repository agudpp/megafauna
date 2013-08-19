/*
 * HeightMapUtils.h
 *
 *  Created on: Aug 8, 2013
 *      Author: agustin
 */

#ifndef HEIGHTMAPUTILS_H_
#define HEIGHTMAPUTILS_H_

#include <string>

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

// @brief Export a current built HeightMap into a file (binary file).
// @param hm            The heightMap we want to export.
// @param fileName      The output file name.
// @return true on success | false otherwise
//
bool
exportToFile(const HeightMap<Ogre::Vector3>& hm, const std::string& fileName);

// @brief Import a HeightMap from a binary file. We will construct the hm from
//        the contents of a file.
// @param fileName      The file name (path) to be read.
// @param hm            The HeightMap to be built.
// @return true on success | false otherwise
//
bool
importFromFile(const std::string& fileName, HeightMap<Ogre::Vector3>& hm);


} /* namespace HeightMapUtils */
} /* namespace gps */
#endif /* HEIGHTMAPUTILS_H_ */
