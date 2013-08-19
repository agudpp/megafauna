/*
 * test.cpp
 *
 *  Created on: Aug 07, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>

#include <OgreVector3.h>
#include <OgreMeshManager.h>
#include <OgreMesh.h>
#include <OgreResourceGroupManager.h>

#include <UnitTest++/UnitTest++.h>

#include <heightmap/HeightMap.h>
#include <heightmap/HeightMapUtils.h>
#include <math/FloatComp.h>

using namespace core;

typedef Ogre::Vector3 V3;
typedef core::Vector2 V2;
typedef std::vector<float> VecF;
typedef core::AABB BB;
typedef gps::HeightMap<Ogre::Vector3> HM;



// Check for a simple plane
//
TEST(CheckBuild)
{
    // simple correct build
    {
        HM hm;
        VecF data;
        data.resize(4);
        CHECK_EQUAL(true, hm.build(BB(100,0,0,100), 1,1,data));
    }

    // wrong build
    {
        HM hm;
        VecF data;
        data.resize(5);
        CHECK_EQUAL(false, hm.build(BB(100,0,0,100), 1,1,data));
    }
    {
        HM hm;
        VecF data;
        data.resize(3);
        CHECK_EQUAL(false, hm.build(BB(100,0,0,100), 1,1,data));
    }
}

// check for a simple plane
TEST(SimplePlane)
{
    HM hm;
    VecF data;
    data.resize(4);
    for (unsigned int i = 0; i < data.size(); ++i) {
        data[i] = 1234.f;
    }

    CHECK_EQUAL(true, hm.build(BB(100,0,0,100), 1,1,data));

    // check for 2 different possitions
    V2 pos(-10,10);
    CHECK_EQUAL(false, hm.isPositionInside(pos));

    pos.x = 55; pos.y = 55;
    CHECK_EQUAL(true, hm.isPositionInside(pos));

    // get the height
    CHECK_EQUAL(1234.f, hm.getHeight(pos));

    // get the perpendicular value
    pos.x = 1; pos.y = 99;
    V3 normal;
    CHECK_EQUAL(1234.f, hm.getHeightAndNormal(pos, normal));

    // check that the normal value is pointing to up
    CHECK_EQUAL(0.f, normal.x);
    CHECK_EQUAL(0.f, normal.y);
    CHECK_EQUAL(1.f, normal.z);
}

// Test for a plane 45 degrees of pending
//
TEST(PendingSimplePlane)
{
    HM hm;
    VecF data;
    data.resize(16);
    int counter = -1;

    // the first 4 elements will be at 0.0, the second group of 4 elements will be
    // at 10, then 20, and then 30...
    for (unsigned int i = 0; i < data.size(); ++i) {
        if ((i % 4) == 0) {
            counter++;
        }
        data[i] = 10.f * counter;
    }

    // build a 3x3 heightmap
    CHECK_EQUAL(true, hm.build(BB(100,0,0,100), 3,3,data));

    const float cellSize = 100.f / 3.f;
    const float ep = 1.e-6;
    const float halfCell = cellSize / 3.f;

    std::vector<V3> normals;

    // now I will be moving throw all the cells and calculating 3 positions
    // in each step
    for (unsigned int y = 0; y < 3; ++y) {
        for (unsigned int x = 0; x < 3; ++x) {
            const float begX = x*cellSize;
            const float begY = y*cellSize;

            const V2 p1(begX + ep, begY + ep);
            const V2 p2(begX + halfCell, begY + halfCell);
            const V2 p3(begX + cellSize - ep, begY + cellSize - ep);

            // check the height
            CHECK_EQUAL(true, core::fcomp_equal(10.f * ep/cellSize + 10.f * y, hm.getHeight(p1)));
            CHECK_EQUAL(true, core::fcomp_equal(10.f * halfCell/cellSize + 10.f * y, hm.getHeight(p2)));
            CHECK_EQUAL(true, core::fcomp_equal(10.f * (cellSize - ep)/cellSize + 10.f * y, hm.getHeight(p3)));

            // the normals should be the same for all the points
            V3 n1, n2, n3;
            hm.getNomral(p1, n1);
            hm.getNomral(p2, n2);
            hm.getNomral(p3, n3);
            normals.push_back(n1); normals.push_back(n2); normals.push_back(n3);
        }
    }

    // now check that all the normals are the same
    V3& n = normals.front();
    for (unsigned int i = 1; i < normals.size(); ++i) {
        CHECK_EQUAL(true, core::fcomp_equal(n.x, normals[i].x));
        CHECK_EQUAL(true, core::fcomp_equal(n.y, normals[i].y));
        CHECK_EQUAL(true, core::fcomp_equal(n.z, normals[i].z));
    }
}

// TODO:
TEST(ParseMesh)
{
    // TODO:
    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load("test.mesh",
        Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    CHECK(mesh.get() != 0);

    HM hm;

    // now parse the mesh and create the heightmap
    CHECK_EQUAL(true, gps::HeightMapUtils::configureFromMesh(mesh.get(), hm));
    CHECK_EQUAL(10, hm.numOfColumns());
    CHECK_EQUAL(10, hm.numOfRows());

    // check that the data is what we want
    unsigned int size = 0;
    const float* data = hm.data(size);
    CHECK(data != 0);
    CHECK_EQUAL((10+1)*(10+1), size);

    core::AABB bb(100,0,0,100);
    CHECK_EQUAL(bb, hm.aabb());

    for (unsigned int i = 0; i < size; ++i) {
        CHECK_EQUAL(123.f, data[i]);
    }
}

TEST(ImporterExporter)
{
    // we will test the importer exporter right here
    HM hm;
    VecF data;
    data.resize(11*11);
    for (unsigned int i = 0; i < data.size(); ++i) {
        data[i] = 1234.f;
    }

    CHECK_EQUAL(true, hm.build(BB(100,0,0,100), 10, 10, data));
    CHECK_EQUAL(true, gps::HeightMapUtils::exportToFile(hm, "test.hm"));

    // import it
    HM hmImported;
    CHECK_EQUAL(true, gps::HeightMapUtils::importFromFile("test.hm", hmImported));

    // check that both are equal
    CHECK_EQUAL(hm, hmImported);
}

int
main(void)
{
    return UnitTest::RunAllTests();
}



