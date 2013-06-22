/*
 * test.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: agustin
 */

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <vector>

#include <UnitTest++/UnitTest++.h>

#include <trigger_system/TriggerZone.h>
#include <trigger_system/TriggerSystem.h>
#include <trigger_system/TriggerSystemDefines.h>
#include <trigger_system/TriggerAgent.h>
#include <types/basics.h>

using namespace core;

typedef std::vector<core::TriggerZone> TZoneVec;
typedef std::vector<core::uint16_t> TZoneIDs;

// Define the structure used to receive signals event from the TriggerZones
//
struct EventReceiver {
    EventReceiver() : called(false) {};

    bool called;
    TriggerSystem::EventType event;
    core::uint16_t agentId;
    core::Vector2 agentPos;
    core::uint16_t zoneId;

    void
    receiveEvent(const TriggerSystem::EventInfo& evt)
    {
        debugGREEN("receiveEvent called zone id: %u\n", evt.zoneID);
        called = true;
        event = evt.type;
        agentId = evt.agent.id();
        agentPos = evt.agent.position();
        zoneId = evt.zoneID;
    }

    void
    registerSignal(TriggerSystem& ts, core::uint16_t zoneId)
    {
        connection = ts.addCallback(zoneId,
            boost::bind(&EventReceiver::receiveEvent, this, _1));
    }

    TriggerSystem::Connection connection;
};


static void
checkReciver(TriggerSystem::EventType event,
             core::uint16_t agentId,
             const core::Vector2& agentPos,
             core::uint16_t zoneId,
             const EventReceiver& rcv)
{
    CHECK_EQUAL(event, rcv.event);
    CHECK_EQUAL(agentId, rcv.agentId);
    CHECK_EQUAL(agentPos.x, rcv.agentPos.x);
    CHECK_EQUAL(agentPos.y, rcv.agentPos.y);
    CHECK_EQUAL(zoneId, rcv.zoneId);
}

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

///////////////////////////////////////////////////////////////////////////////

TEST(CreateAndRemoveAgent)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TriggerAgent* nullAgent = 0;

    CHECK(agent != 0);
    uint16_t id = agent->id();
    CHECK_EQUAL(agent, ts.getAgent(id));
    ts.removeAgent(agent);
    CHECK_EQUAL(nullAgent, ts.getAgent(id));

    agent = ts.createAgent();
    id = agent->id();
    CHECK_EQUAL(agent, ts.getAgent(id));
    ts.removeAgent(id);
    CHECK_EQUAL(nullAgent, ts.getAgent(id));
}

// Building the system
//

TEST(BuildingTheSystemSimple)
{
    TriggerSystem ts;

    TZoneVec zones;
    TZoneIDs ids;

    zones.resize(2);
    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(zones.size(), ids.size());
    CHECK(ids[0] != ids[1]);

    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    ts.destroy();
    CHECK_EQUAL(false, ts.isAlreadyBuilt());

    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(zones.size(), ids.size());
    CHECK(ids[0] != ids[1]);
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
}

TEST(BuildingTheSystemWrong)
{
    TriggerSystem ts;
    TriggerZone z1;
    AABB bb(100,0,-100,100);
    z1.setZone(bb);

    TZoneVec zones;
    TZoneIDs ids;
    zones.resize(100, z1);

    // build and ensure that we cannot build that since we have more nodes in the
    // same place than the colors supported (the maximum overlapped zones)
    // colors or the implementation that is used
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(false, ts.isAlreadyBuilt());

    zones.resize(25, z1);

    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());

    // check that we always have new ids
    core::size_t currentSize = ids.size();
    std::vector<uint16_t>::iterator it = std::unique(ids.begin(), ids.end());
    core::size_t uniqSize = std::distance(ids.begin(), it);
    CHECK_EQUAL(currentSize, uniqSize);

}

// Simple graph checking with one agent
//
TEST(SimpleGraphOneAgent)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;

    std::vector<EventReceiver> rcv;

    for (uint i = 0; i < 3; ++i) {
        float size = 5;
        float xOffset = size * i * 0.5;
        float height = 30;
        AABB aabb(height, xOffset - size*0.4, 0, size*0.4 + xOffset);
        TriggerZone z(aabb);
        std::cout << "Creating box: \n" << aabb << std::endl;
        zones.push_back(z);
    }

    CHECK_EQUAL(false, ts.isAlreadyBuilt());
    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());

    // register callbacks
    rcv.resize(ids.size());
    for (uint i = 0; i < rcv.size(); ++i) {
        rcv[i].registerSignal(ts, ids[i]);
    }

    CHECK_EQUAL(core::TriggerCode::Ok,
                    ts.initializeAgent(agent, Vector2(-1,1)));

    // now move the agent
    CHECK_EQUAL(core::TriggerCode::UnreachablePosition,
                agent->setPosition(Vector2(555,555)));
    for (uint i = 0; i < rcv.size(); ++i) {CHECK_EQUAL(false, rcv[i].called);}

    CHECK_EQUAL(core::TriggerCode::UnreachablePosition,
                agent->setPosition(Vector2(-555,555)));
    for (uint i = 0; i < rcv.size(); ++i) {CHECK_EQUAL(false, rcv[i].called);}

    // put the agent in the first one only
    CHECK_EQUAL(core::TriggerCode::BigJump,
                agent->setPosition(Vector2(-1, 1)));
    CHECK_EQUAL(true, rcv[0].called);
    checkReciver(TriggerSystem::EventType::Entering,
                 agent->id(),
                 agent->position(),
                 ids[0],
                 rcv[0]);
    for (uint i = 1; i < rcv.size(); ++i) {CHECK_EQUAL(false, rcv[i].called);}

    rcv[0].called = false;

    // move the agent to the next position
    debugGREEN("\n\nStarting here\n");
    CHECK_EQUAL(core::TriggerCode::Ok,
                agent->setPosition(Vector2(2.5, 1)));
    CHECK_EQUAL(true, rcv[0].called);
    CHECK_EQUAL(true, rcv[1].called);
    CHECK_EQUAL(false, rcv[2].called);
    debugGREEN("\n--------------ENDS here here\n");

    checkReciver(TriggerSystem::EventType::Leaving,
                 agent->id(),
                 agent->position(),
                 ids[0],
                 rcv[0]);
    checkReciver(TriggerSystem::EventType::Entering,
                 agent->id(),
                 agent->position(),
                 ids[1],
                 rcv[1]);

    // move to the third area
    rcv[0].called = false;
    rcv[1].called = false;

    CHECK_EQUAL(core::TriggerCode::Ok,
                agent->setPosition(Vector2(5, 1)));
    CHECK_EQUAL(false, rcv[0].called);
    CHECK_EQUAL(true, rcv[1].called);
    CHECK_EQUAL(true, rcv[2].called);
    checkReciver(TriggerSystem::EventType::Leaving,
                 agent->id(),
                 agent->position(),
                 ids[1],
                 rcv[1]);
    checkReciver(TriggerSystem::EventType::Entering,
                 agent->id(),
                 agent->position(),
                 ids[2],
                 rcv[2]);


    // move to the first position now should be a big jump
    rcv[1].called = false;
    rcv[2].called = false;

    CHECK_EQUAL(core::TriggerCode::BigJump,
                agent->setPosition(Vector2(-1, 1)));
    CHECK_EQUAL(true, rcv[0].called);
    CHECK_EQUAL(false, rcv[1].called);
    CHECK_EQUAL(true, rcv[2].called);
    checkReciver(TriggerSystem::EventType::Entering,
                 agent->id(),
                 agent->position(),
                 ids[0],
                 rcv[0]);
    checkReciver(TriggerSystem::EventType::Leaving,
                 agent->id(),
                 agent->position(),
                 ids[2],
                 rcv[2]);
}

int
main(void)
{
    return UnitTest::RunAllTests();
}
