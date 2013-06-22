/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>

#include <UnitTest++/UnitTest++.h>

#include <space_partition/MatrixPartition.h>
#include <types/basics.h>

using namespace core;

typedef MatrixPartition<int> IntMat;


    // Simple graph checking with one agent
//
TEST(SimpleMatrix)
{
    IntMat mat;
    mat.construct(10,10,AABB(10,-10,-10,10));
    CHECK_EQUAL(10, mat.numColumns());
    CHECK_EQUAL(10, mat.numRows());

    Vector2 p(0,0);
    CHECK_EQUAL(true, mat.isPositionInside(p));
    p.x = 100; p.y = 10;
    CHECK_EQUAL(false, mat.isPositionInside(p));

    // set all the values
    for (uint i = 0, size = 10*10; i < size; ++i) {
        mat.getCell(i) = i;
    }
    // set all the values
    for (uint i = 0, size = 10*10; i < size; ++i) {
        CHECK_EQUAL(i, mat.getCell(i));
    }
}

TEST(CheckValueFromPosition)
{
    IntMat mat;
    mat.construct(10,10,AABB(10,-10,-10,10));
    CHECK_EQUAL(10, mat.numColumns());
    CHECK_EQUAL(10, mat.numRows());


    const float cellSizeX = mat.boundingBox().getWidth() / mat.numColumns();
    const float cellSizeY = mat.boundingBox().getHeight() / mat.numRows();
    const float halfX = cellSizeX * 0.5f;
    const float halfY = cellSizeY * 0.5f;

    CHECK(cellSizeX > 0.f);
    CHECK(cellSizeY > 0.f);

    // set all the values
    for (uint i = 0, size = 10; i < size; ++i) {
        for (uint j = 0, size2 = 10; j < size2; ++j) {
            Vector2 pos(-10.0f + halfX + i*cellSizeX, -10.0f + halfY + j * cellSizeY);
            CHECK_EQUAL(true, mat.isPositionInside(pos));
            mat.getCell(pos) = j*10 + i;
        }
    }

    const uint numSamples = 15;
    const float deltaX = halfX / (numSamples+1);
    const float deltaY = halfY / (numSamples+1);

    // get the same values for a lot of samples
    for (uint i = 0, size = 10; i < size; ++i) {
        for (uint j = 0, size2 = 10; j < size2; ++j) {
            for (uint samplesX = 0; samplesX < numSamples; ++samplesX) {
                for (uint samplesY = 0; samplesY < numSamples; ++samplesY) {
                    Vector2 pos(-10.0f + i * cellSizeX + deltaX * samplesX,
                                -10.0f + j * cellSizeY + deltaY * samplesY);
                    CHECK_EQUAL(true, mat.isPositionInside(pos));
                    CHECK_EQUAL(j*10 + i, mat.getCell(pos));
                }
            }
        }
    }
}

int
main(void)
{
    return UnitTest::RunAllTests();
}



