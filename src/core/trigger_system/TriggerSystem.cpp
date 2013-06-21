/*
 * TriggerSystem.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#include "TriggerSystem.h"

////////////////////////////////////////////////////////////////////////////
//
namespace {
////////////////////////////////////////////////////////////////////////////
// coloring helpers methods

// possible Colors
static core::TriggerColor_t COLOR_BITS[TS_NUM_COLORS] = {
    (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7),
    (1 << 8), (1 << 9), (1 << 10), (1 << 11), (1 << 12), (1 << 13), (1 << 14), (1 << 15),
    (1 << 16), (1 << 17), (1 << 18), (1 << 19), (1 << 20), (1 << 21), (1 << 22), (1 << 23),
    (1 << 24), (1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 29), (1 << 30), (1 << 31)
};

}


namespace core {


// Get a new empty color from a node (checking the adjacencies)
core::TriggerColor_t
TriggerSystem::getColor(const core::TriggerSystem::ZoneNode& node)
{
    const core::TriggerSystem::ZoneNodePtrVec& neighbors = node.neighbors;
    ASSERT(neighbors.size() < TS_NUM_COLORS);
    core::TriggerColor_t accumColor = 0;
    for (core::size_t i = 0; i < neighbors.size(); ++i) {
        accumColor |= neighbors[i]->color;
    }

    // get the first free color
    for (core::size_t i = 0; i < TS_NUM_COLORS; ++i) {
        if (!(COLOR_BITS[i] & accumColor)) {
            return COLOR_BITS[i];
        }
    }
    // no color found!?
    ASSERT(false);
    return 0;
}

////////////////////////////////////////////////////////////////////////////
bool
TriggerSystem::coloringTheGraph(void)
{
    debugWARNING("This method is not right since there could exists a way to"
        " color the nodes with the same number of colors. If this fails it only"
        " means that we need to change the algorithm for a good one\n");

    for (core::size_t i = 0; i < mZoneNodesSize; ++i) {
        mZoneNodes[i].color = getColor(mZoneNodes[i]);
    }

    // check for correctness
    for (core::size_t i = 0; i < mZoneNodesSize; ++i) {
        ZoneNode& currentNode = mZoneNodes[i];
        ZoneNodePtrVec& neighbors = currentNode.neighbors;

        if (currentNode.color == 0) {
            debugERROR("Node %ul has no color assigned!\n", i);
            ASSERT(false);
        }

        // check that we have a unique color
        for (core::size_t j = 0, nc = neighbors.size(); j < nc; ++j) {
            if (currentNode.color & neighbors[j]->color) {
                debugERROR("Node %ul and node %ul has the same color!\n",
                    i, neighbors[j]->zone.id());
                ASSERT(false);
            }
        }
    }

    // everything is ok, nodes colored fine
    return true;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
TriggerSystem::TriggerSystem() :
    mZoneNodes(0)
,   mZoneNodesSize(0)
{
}

////////////////////////////////////////////////////////////////////////////
TriggerSystem::~TriggerSystem()
{
    delete []mZoneNodes;
    mZoneNodesSize = 0;
}

////////////////////////////////////////////////////////////////////////////
//                    Characters associated methods
//

////////////////////////////////////////////////////////////////////////////
TriggerAgent*
TriggerSystem::createAgent(void)
{
    std::shared_ptr<TriggerAgent> agent(new TriggerAgent(*this));
    mAgents.push_back(agent);
    return agent.get();
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::removeAgent(core::uint16_t id)
{
    if (id > mAgents.size()) {
        debugWARNING("Trying to remove agent %ui and we have %ul agents\n",
            id, mAgents.size());
        return;
    }
    // swap with the last and remove this one
    core::size_t size = mAgents.size()-1;
    mAgents[id] = mAgents[size];
    mAgents[id]->setID(id);
    mAgents.pop_back();
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::updateAgentPos(TriggerAgent* agent, const Vector2& pos)
{
    ASSERT(agent);
    ASSERT(agent->id() < mAgents.size());
    ASSERT(agent->id() == mAgents[agent->id()]->id());
    ASSERT(false); // to implement
}


////////////////////////////////////////////////////////////////////////////
//                       Building methods
//

////////////////////////////////////////////////////////////////////////////
TriggerSystem::Connection
TriggerSystem::addCallback(core::uint16_t zoneID, const Signal::slot_type& subscriber)
{
    ASSERT(zoneID < mZoneNodesSize);
    ASSERT(zoneID == mZoneNodes[zoneID].zone.id());
    return mZoneNodes[zoneID].callbacks.connect(subscriber);
}

////////////////////////////////////////////////////////////////////////////
bool
TriggerSystem::build(void)
{
    if (mTmpZones.empty()) {
        debugERROR("No zones were added to build the system\n");
        return false;
    }

    // clear everything
    delete []mZoneNodes; mZoneNodes = 0;
    mZoneNodesSize = 0;
    mZoneNodes = new ZoneNode[mTmpZones.size()];
    mZoneNodesSize = mTmpZones.size();
    // TODO: clear graph here (kdtree or whatever we are using).

    debugOPTIMIZATION("We are doing this very slow, use a kd-tree or something "
        "like that to optimize this\n");
    debugOPTIMIZATION("Other possible optimization should be put all the closer "
        "zones in the same part of the array to avoid cache misses\n");
    for (core::size_t i = 0, zoneCount = mTmpZones.size(); i < zoneCount; ++i) {
        // put the zone into the ZoneNode
        mZoneNodes[i].zone = mTmpZones[i];
        mZoneNodes[i].color = 0;

        // iterate over all the tmpZones to check which are their neighbors
        for (core::size_t j = 0; j < zoneCount && i != j; ++j) {
            if (mTmpZones[i].intersects(mTmpZones[j])) {
                // we need to add j-th ZoneNode as neighbor of i-th ZoneNode
                mZoneNodes[i].neighbors.push_back(&(mZoneNodes[j]));
            }
        }
    }

    // color the nodes
    if (!coloringTheGraph()) {
        debugERROR("We couldn't color the graph.. something goes wrong? the number "
            "of overlapped zones is greater than %d?\n", TS_NUM_COLORS);
        return false;
    }

    // success
    mTmpZones.clear(); // clear stuff
    return true;
}

} /* namespace core */
