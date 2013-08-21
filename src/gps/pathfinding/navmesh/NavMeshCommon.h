/*
 * NavMeshCommon.h
 *
 *  Created on: Aug 19, 2013
 *      Author: agustin
 */

#ifndef NAVMESHCOMMON_H_
#define NAVMESHCOMMON_H_


#include <math/Vec2.h>

namespace gps {

// define a vertex and a triangle using indices
//

typedef core::Vector2 Vertex;

struct Triangle {
    unsigned short vertex[3];
};



}


#endif /* NAVMESHCOMMON_H_ */
