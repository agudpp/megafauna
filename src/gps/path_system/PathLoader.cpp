/*
 * PathLoader.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: agustin
 */

#include "PathLoader.h"

#include <string.h>
#include <fstream>

#include <types/StackVector.h>
#include <types/basics.h>
#include <debug/DebugUtil.h>

namespace gps {

namespace PathLoader {

#define HEADER_SIZE 10
#define HEADER_DATA     "PATH0.1"


////////////////////////////////////////////////////////////////////////////////
bool
savePath(const Path& path, const std::string& fname)
{
    core::StackVector<Path::Point, Path::MAX_NUM_NODES> nodes;
    std::ofstream outFile(fname.c_str(), std::ofstream::out);

    if (!outFile) {
        debugERROR("Error openning file %s\n", fname.c_str());
        return false;
    }

    // write the header file
    char header[HEADER_SIZE];
    memcpy(header, HEADER_DATA, strlen(HEADER_DATA));
    outFile.write(header, HEADER_SIZE);

    // write the node data information
    core::uint32_t size = nodes.size();
    outFile.write(static_cast<const char*>(&size), sizeof(core::uint32_t));
    outFile.write(static_cast<const char*>(nodes.begin()),
                  sizeof(Path::Point) * nodes.size());

    outFile.close();

    return outFile.good();
}

////////////////////////////////////////////////////////////////////////////////
bool
loadPath(const std::string& fname, Path& path)
{
    std::ifstream file(fname.c_str(), std::ifstream::binary);

    if (!file) {
        debugERROR("Error openning the file %s\n", fname.c_str());
        return false;
    }

    // now read the header and check if we are ok
    char header[HEADER_SIZE];
    file.read(header, HEADER_SIZE);
    if (strcmp(header, HEADER_DATA) != 0) {
        debugERROR("Invalid version of file or wrong file, we expected %s as"
                " the header and we read %s\n", header, HEADER_DATA);
        return false;
    }

    // now we will read all the information we have into the an auxiliary buffer
    core::uint32_t size;
    file.read(&size, sizeof(core::uint32_t));

    if (size == 0) {
        debugERROR("Path with no nodes?\n");
        return false;
    }

    Path::Point points[Path::MAX_NUM_NODES];
    file.read(points, sizeof(Path::Point) * size);

    // now we will put it in the path
    path.clear();
    for (unsigned int i = 0; i < size; ++i) {
        path.pushNode(points[i]);
    }

    // everything is fine
    return true;
}

}

}
