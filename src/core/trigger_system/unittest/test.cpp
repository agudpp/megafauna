/*
 * test.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: agustin
 */

#include <UnitTest++/UnitTest++.h>

#include <trigger_system/TriggerZone.h>
#include <trigger_system/TriggerSystem.h>
#include <trigger_system/TriggerSystemDefines.h>
#include <trigger_system/TriggerAgent.h>
#include <types/basics.h>

using namespace core;

TEST(CheckTriggerZone)
{
    TriggerZone z1,z2,z3,z4;
    AABB ab1(10,0,0,10), ab2(5,1,2,4), ab3(9,3,5.1,4), ab4(-100,-10,-200, 4);
    z1.setZone(ab1);
    z2.setZone(ab2);
    z3.setZone(ab3);
    z4.setZone(ab4);

    Vector2 p1(-4,-4), p2(0.1,0.1), p3(100,100);

    // check point inside
    CHECK(!z1.isPointInside(p1));
    CHECK(z1.isPointInside(p2));
    CHECK(!z1.isPointInside(p3));

    CHECK(!z2.isPointInside(p1));
    CHECK(!z2.isPointInside(p2));
    CHECK(!z2.isPointInside(p3));

    CHECK(!z3.isPointInside(p1));
    CHECK(!z3.isPointInside(p2));
    CHECK(!z3.isPointInside(p3));

    CHECK(!z4.isPointInside(p1));
    CHECK(!z4.isPointInside(p2));
    CHECK(!z4.isPointInside(p3));

    // check intersection
    CHECK(z1.intersects(z2));
    CHECK(z1.intersects(z3));
    CHECK(z2.intersects(z1));
    CHECK(z3.intersects(z1));
    CHECK(!z2.intersects(z3));
    CHECK(!z3.intersects(z2));
    CHECK(!z1.intersects(z4));
    CHECK(!z2.intersects(z4));
    CHECK(!z3.intersects(z4));
}

int
main(void)
{
    return UnitTest::RunAllTests();
}
