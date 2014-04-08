/*
 * PathExporter.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: agustin
 */

#include "PathExporter.h"

#include <map>
#include <algorithm>
#include <sstream>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>

#include <tinyxml/tinyxml.h>

#define CHECK_CREATE_ATTR(varAttrName, xml, retVal) \
    const char* varAttrName = xml->Attribute(#varAttrName);\
    if(varAttrName == 0) { \
        debugERROR("Attribute " #varAttrName " not found\n");\
        return retVal;\
    }

// Helper classes
//
namespace {


// This path info structure will hold the temporary information to be used later
// to build the readl paths
struct TempPathInfo {
    core::StackVector<gps::Path::Point, gps::Path::MAX_NUM_NODES> nodes;
};

// @brief Parse a ogre vector into a Path::Point
inline bool
parseNodePos(const TiXmlElement* xml, gps::Path::Point& point)
{
    if (xml == 0) {
        debugERROR("xml null\n");
        return false;
    }

    CHECK_CREATE_ATTR(x, xml, false);
    CHECK_CREATE_ATTR(y, xml, false);

    ss << x;
    ss >> point.x;
    ss.clear();
    ss.str("");
    ss << y;
    ss >> point.y;

    return true;
}

// @brief Extract name and index from a string
// @param str       The string to be parsed
// @param name      The name parsed
// @param index     The index parsed
// @return true on success | false if it is illformed or some error occur
//
inline bool
parseString(const std::string& str, std::string& name, int& index)
{
    core::size_t sepPos = str.find('_');
    if (sepPos == std::string::npos) {
        debugERROR("Wrong node name, it is not following our norm: %s, we couldn't"
                " find the separator operator: %s\n", str.c_str(), "_");
        return false;
    }
    name = str.substr(0, sepPos);
    std::stringstream ss;
    ss << str.substr(sepPos + 1, str.size());
    ss >> index;

    return true;
}


// @brief Load an .scene xml file and parse the nodes (names and position).
//        We will also transform its id into numbers and sort its elements.
//        For example, if we have the nodes: tigre_1, tigre_2, tigre_3, we will
//        return the nodes already sorteds (node1, node2, node3).
// @param fname     The scene file name
// @param result    The resulting map of pathName -> pathNodes
// @return true on success | false on error
//
bool
parseScene(const std::string& fname, std::map<std::string, TempPathInfo>& result)
{
    TiXmlDocument doc;
    if (!doc.LoadFile(fname.c_str())) {
        debugERROR("Error loading scene file %s\n", fname.c_str());
        return false;
    }

    struct TempPoint {
        int index;
        gps::Path::Point point;

        TempPoint(){};
        TempPoint(const gps::Path::Point& p, int i) : index(i), point(p){}
        inline bool operator<(const TempPoint& o) const {return index < o.index; }
    };

    // for each xml element:
    //  - parse node name and node number
    //  - parse its associated point
    //  - put the point and number in the correspoding temporaryVector

    // now parse each of the nodes
    const TiXmlElement* root = doc.RootElement();
    if (!root || strcmp(root->Value(), "scene") != 0) {
        debugERROR("Invalid root element in the file %s\n", fname.c_str());
        return false;
    }

    const TiXmlElement* nodes = root->FirstChildElement("nodes");
    if (nodes == 0 || (nodes = nodes->FirstChildElement("node")) == 0) {
        debugERROR("No nodes where found in the scene file %s\n", fname.c_str());
        return false;
    }

    std::string tmpName;
    int tmpIndex;
    std::map<std::string, std::vector<TempPoint> > nameToPoints;
    while (nodes != 0) {
        // get name
        CHECK_CREATE_ATTR(name, nodex, false);

        // parse name and index
        if (!parseString(name, tmpName, tmpIndex)){
            return false;
        }

        // parse the point
        TempPoint tmpPoint;
        if (!parseNodePos(nodes->FirstChildElement("position"), tmpPoint.point)) {
            debugERROR("Error parsing point position for %s\n", tmpName.c_str());
            return false;
        }

        // put the point where belongs
        tmpPoint.index = tmpIndex;
        nameToPoints[tmpName].push_back(tmpPoint);

        nodes = nodes->NextSiblingElement("node");
    }

    // now sort all the temporary points and ensure that all the points are
    // different in each path (has different indices)
    // and construct the resulting path
    //
    result.clear();
    for (auto it = nameToPoints.begin(), end = nameToPoints.end(); it != end; ++it) {
        std::vector<TempPoint>& vec = it->second;

        if (vec.empty()) {
            debugERROR("Some path has no nodes? %s\n", it->first.c_str());
            return false;
        }

        std::sort(vec.begin(), vec.end());

        // ensure that the points are unique and fill the temporary object
        TempPathInfo tpi;
        tpi.nodes.push_back(vec[0].point);
        for (unsigned int i = 1; i < vec.size(); ++i) {
            if (vec[i-1].index == vec[i].index) {
                debugERROR("Two or more points have the same indices in the path "
                        "%s\n", it->first.c_str());
                return false;
            }
            tpi.nodes.push_back(vec[i].point);
        }
        result[it->second] = tpi;
    }

    return true;
}

}


namespace gps {

namespace PathExporter {


bool
transform(const std::string& file, std::list<std::string>& result)
{

}


}

}
