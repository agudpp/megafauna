/*
 * test.cpp
 *
 *  Created on: Aug 07, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>

#include <UnitTest++/UnitTest++.h>

#include <math/FloatComp.h>
#include <pathfinding/navmesh/TriGraph.h>
#include <pathfinding/navmesh/TriMesh.h>
#include <pathfinding/navmesh/NavMeshCommon.h>


using namespace core;

typedef Ogre::Vector3 V3;
typedef core::Vector2 V2;
typedef std::vector<float> VecF;
typedef core::AABB BB;


////////////////////////////////////////////////////////////////////////////////
// TriGraph tests
//

// check that the vertices order is maintained and the triangles too
TEST(CheckVerticesAndTrianglesOrder)
{

}

//


int
main(void)
{
    return UnitTest::RunAllTests();
}



