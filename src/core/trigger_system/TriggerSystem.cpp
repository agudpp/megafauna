/*
 * TriggerSystem.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#include "TriggerSystem.h"

#include "GraphColoringHelper.h"

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
    (1 << 24), (1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 29), (1 << 30),
    static_cast<core::TriggerColor_t>(1 << 31)
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
void
TriggerSystem::getZonesFromPosition(const Vector2& pos, ZoneNodePtrVec& zones) const
{
    debugOPTIMIZATION("Really slow, change this and use KD-trees or some other"
        " partition space structure\n");

    zones.clear();
    for (core::size_t i = 0, size = mZoneNodesSize; i < size; ++i) {
        if (mZoneNodes[i].zone.isPointInside(pos)) {
            zones.push_back(&(mZoneNodes[i]));
        }
    }
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::initialize(TriggerAgent* agent)
{
    ASSERT(agent);

    if (!agent->closerZones.empty()) {
        return TriggerCode::AlreadyInitialized;
    }

    // get the possible zones
    getZonesFromPosition(agent->position(), mZoneNodePtrs);
    if (mZoneNodePtrs.empty()) {
        // the agent is nowhere, we will return an error
        return TriggerCode::UnreachablePosition;
    }

    // else set the zones
    RefContainer<core::uint16_t>& closerZones = agent->closerZones;
    for (core::size_t i = 0, size = mZoneNodePtrs.size(); i < size; ++i) {
        closerZones.addElement(mZoneNodePtrs[i]->id);
    }
    return TriggerCode::Ok;

}

////////////////////////////////////////////////////////////////////////////
bool
TriggerSystem::coloringTheGraph(void)
{
    if (mZoneNodesSize == 0) {
        debugWARNING("Trying to color a empty graph\n");
        return false;
    }

    // construct the Adjacency matrix used
    GraphColoringHelper::AdjacencyMatrix matrix;
    matrix.setNumberNodes(mZoneNodesSize);

    for (core::size_t i = 0; i < mZoneNodesSize; ++i) {
        const core::size_t currentId = mZoneNodes[i].id;

        ZoneNodePtrVec& neighbors = mZoneNodes[i].neighbors;
        for (core::size_t j = 0, size = neighbors.size(); j < size; ++j) {
            const core::size_t neighborId = neighbors[j]->id;
            ASSERT(neighborId != currentId);

            // configure the matrix
            matrix.set(currentId, neighborId, true);
        }
    }

    // now get the number of colors for the different zones
    std::vector<int> colors;
    int numColors = GraphColoringHelper::graphColoring(matrix, colors);
    if (numColors > TS_NUM_COLORS) {
        debugERROR("We couldn't color the graph with less than %d colors, we got "
            "%d colors\n", TS_NUM_COLORS, numColors);
        return false;
    }

    // now that we could color the graph we will assign the colors to the different
    // zones.
    // TODO: This is very inefficient! but is called only once in the beginning
    //       we can let this to the future
    for (core::size_t j = 0, count = colors.size(); j < count; ++j) {
        const TriggerColor_t currentColor = COLOR_BITS[colors[j]];

        // find the zone with id j
        for (core::size_t i = 0; i < mZoneNodesSize; ++i) {
            const core::size_t currentId = mZoneNodes[i].id;
            if (currentId == j) {
                // we found the zone, set the color
                mZoneNodes[i].color = currentColor;
                break;
            }
        }
    }

#ifdef DEBUG
    // double checking for adjacent zones
    for (core::size_t i = 0; i < mZoneNodesSize; ++i) {
        const TriggerColor_t currentColor = mZoneNodes[i].color;

        ZoneNodePtrVec& neighbors = mZoneNodes[i].neighbors;
        for (core::size_t j = 0, size = neighbors.size(); j < size; ++j) {
            const TriggerColor_t neighborColor = neighbors[j]->color;
            ASSERT(neighborColor != currentColor);
        }
    }

#endif

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
    agent->setID(mAgents.size() - 1);
    return agent.get();
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::removeAgent(core::uint16_t id)
{
    if (id >= mAgents.size()) {
        debugWARNING("Trying to remove agent %ui and we have %ul agents\n",
            id, mAgents.size());
        return;
    }

    // if we have only one..
    if (mAgents.size() == 1) {
        mAgents.clear();
        return;
    }

    // swap with the last and remove this one
    core::size_t size = mAgents.size()-1;
    mAgents[id] = mAgents[size];
    mAgents[id]->setID(id);
    mAgents.pop_back();
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::initializeAgent(TriggerAgent* agent, const Vector2& pos)
{
    ASSERT(agent);
    ASSERT(agent->id() < mAgents.size());
    ASSERT(agent->id() == mAgents[agent->id()]->id());

    agent->mPosition = pos;
    return initialize(agent);
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::updateAgentPos(TriggerAgent* agent, const Vector2& pos)
{
    ASSERT(agent);
    ASSERT(agent->id() < mAgents.size());
    ASSERT(agent->id() == mAgents[agent->id()]->id());

    // update the position of the agent
    agent->mPosition = pos;

    // first check for "leaving" zones, then for entering zones.
    RefContainer<core::uint16_t>& closerZones = agent->closerZones;
    TriggerColor_t agentColor = agent->currentColors;
    TriggerColor_t currentColors = 0;

    debugBLUE("CloserZones: %ul\n", closerZones.size());

    for (core::size_t i = 0, size = closerZones.size(); i < size; ++i) {
        ASSERT(closerZones[i] < mZoneNodesSize);
        ZoneNode& node = mZoneNodes[closerZones[i]];

        if (node.zone.isPointInside(pos)) {
            currentColors |= node.color;
            debugBLUE("Inside: %u\n", node.id);

            // the player is inside of this zone, check if we was before
            if (node.color & agentColor) {
                // we still inside of the same zone, do nothing
            } else {
                // we are entering in a new zone! call the callbacks
                node.callbacks(EventInfo(EventType::Entering, node.id, *agent));

                // add new possible neighbors for this node
                ZoneNodePtrVec& neighbors = node.neighbors;
                for (core::size_t j = 0, count = neighbors.size(); j < count; ++j) {
                    closerZones.addElement(neighbors[i]->id);
                }
            }
        } else {
            debugBLUE("Outside: %u\n", node.id);
            // we are not anymore in this zone... check if we was before
            if (node.color & agentColor) {
                // we was before, but now we leave it, send the event
                node.callbacks(EventInfo(EventType::Leaving, node.id, *agent));

                // remove the neighbors
                ZoneNodePtrVec& neighbors = node.neighbors;
                for (core::size_t j = 0, count = neighbors.size(); j < count; ++j) {
                    closerZones.removeElement(neighbors[i]->id);
                }
            } else {
                // do nothing..
            }
        }
    }

    agent->currentColors = currentColors;
    // check if the agent is outside of all the closer zones (probably a big jump)
    if (currentColors == 0 || closerZones.empty()) {
        debugBLUE("currentColors: %u\n", currentColors);
        // get the possible zones
        getZonesFromPosition(agent->position(), mZoneNodePtrs);
        if (mZoneNodePtrs.empty()) {
            // the agent is nowhere, we will return an error
            closerZones.clear();
            return TriggerCode::UnreachablePosition;
        }

        // else the agent is in a valid position but we did a big jump...
        // we will set the new color for the agent, the closerZones and we will
        // call the associated callbacks (this way we avoid to wait until the
        // next updatePosition() call)
        closerZones.clear();
        currentColors = 0;
        for (core::size_t i = 0, size = mZoneNodePtrs.size(); i < size; ++i) {
            ASSERT(mZoneNodePtrs[i]);
            ZoneNode& node = *(mZoneNodePtrs[i]);
            ZoneNodePtrVec& neighbors = node.neighbors;
            for (core::size_t j = 0, count = neighbors.size(); j < count; ++j) {
                closerZones.addElement(neighbors[i]->id);
            }

            // call the associated callback
            currentColors |= node.color;
            node.callbacks(EventInfo(EventType::Entering, node.id, *agent));
        }
        return TriggerCode::BigJump;
    }

    // everything was ok
    return TriggerCode::Ok;
}


////////////////////////////////////////////////////////////////////////////
//                       Building methods
//


TriggerSystem::Connection
TriggerSystem::addCallback(core::uint16_t zoneID, const Signal::slot_type& subscriber)
{
    ASSERT(zoneID < mZoneNodesSize);
    ASSERT(zoneID == mZoneNodes[zoneID].id);
    return mZoneNodes[zoneID].callbacks.connect(subscriber);
}

////////////////////////////////////////////////////////////////////////////
bool
TriggerSystem::build(const std::vector<TriggerZone>& zones, std::vector<core::uint16_t>& ids)
{
    if (isAlreadyBuilt()) {
        debugERROR("The system is already built, call destroy() first\n");
        return false;
    }

    if (zones.empty()) {
        debugERROR("No zones were added to build the system\n");
        return false;
    }

    ids.clear();
    ids.reserve(zones.size());

    // create a new vector
    mZoneNodes = new ZoneNode[zones.size()];
    mZoneNodesSize = zones.size();
    // TODO: clear graph here (kdtree or whatever we are using).

    debugOPTIMIZATION("We are doing this very slow, use a kd-tree or something "
        "like that to optimize this\n");
    debugOPTIMIZATION("Other possible optimization should be put all the closer "
        "zones in the same part of the array to avoid cache misses. If we do this "
        "we must be careful with the IDs, since it will be changed the order ("
        "we can use a map from old IDs to new IDs\n");

    for (core::size_t i = 0, zoneCount = zones.size(); i < zoneCount; ++i) {
        // put the zone into the ZoneNode
        ZoneNode& node = mZoneNodes[i];
        node.zone = zones[i];
        node.color = 0;
        node.id = i;

        // push the new id that matches with this zone
        ids.push_back(node.id);

        // iterate over all the tmpZones to check which are their neighbors
        for (core::size_t j = 0; j < zoneCount; ++j) {
            // if we are not ourself and we intersect with the zone
            if (i != j && node.zone.intersects(zones[j])) {
                // we need to add j-th ZoneNode as neighbor of i-th ZoneNode
                node.neighbors.push_back(&(mZoneNodes[j]));
            }
        }
    }

    // color the nodes
    if (!coloringTheGraph()) {
        debugERROR("We couldn't color the graph.. something goes wrong? the number "
            "of overlapped zones is greater than %d?\n", TS_NUM_COLORS);
        destroy();
        return false;
    }

    // success
    return true;
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::destroy(void)
{
    delete []mZoneNodes; mZoneNodes = 0;
    mZoneNodesSize = 0;
}

} /* namespace core */
