/*
 * PathLoader.h
 *
 *  Created on: Nov 12, 2013
 *      Author: agustin
 */

#ifndef PATHLOADER_H_
#define PATHLOADER_H_

#include <string>

#include "Path.h"

namespace gps {

// @brief This class will load a path from a .path file and will save a .path
//        file from a path.
//


namespace PathLoader {

// @brief Save a path into a path file.
// @param path      The path we want to save
// @param fname     The filename
// @return true on success | false otherwise
//
bool
savePath(const Path& path, const std::string& fname);

// @brief Load a path from a path file.
// @param fname     The file name where we will load the path.
// @param path      The path to build.
// @return true on success | false otherwise
//
bool
loadPath(const std::string& fname, Path& path);

};

}

#endif /* PATHLOADER_H_ */
