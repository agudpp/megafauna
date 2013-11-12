/*
 * PathExporter.h
 *
 *  Created on: Nov 12, 2013
 *      Author: agustin
 */

#ifndef PATHEXPORTER_H_
#define PATHEXPORTER_H_

#include <string>
#include <list>

namespace gps {

// @brief This module will be used to export the data camming from blender or
//        whatever and we will convert that into a path.
//        If the file contains multiple paths then all of them will be exported
//        into different files.
//

namespace PathExporter {

// @brief Parse a file where we have the external path information and convert
//        that into Paths to be saved into different files. All this files
//        will be exported in the same directory where the tool is executed
//        and with the name convention: <path_name>.path, where path_name is
//        the name given to the path in the file.
// @param file      The file to be parsed.
// @param result    The resulting list of path files created. If empty no
//                  paths were parsed.
// @return true on succes and some path was parsed | false otherwise
//
bool
transform(const std::string& file, std::list<std::string>& result);



}

}

#endif /* PATHEXPORTER_H_ */
