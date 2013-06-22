/*
 * TriggerSystem.h
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#ifndef TRIGGERSYSTEM_H_
#define TRIGGERSYSTEM_H_


#include <boost/signals.hpp>

#include <memory>
#include <vector>

#include <debug/DebugUtil.h>

#include "TriggerZone.h"
#include "TriggerAgent.h"
#include "TriggerSystemDefines.h"

namespace core {

class TriggerSystem
{
public:
    // Define the callbacks interface
    enum EventType {
        Entering,       // when we are entering the zone
        Leaving,        // when we are leaving the zone
    };

    struct EventInfo {
        EventInfo(EventType t, core::uint16_t z, const TriggerAgent& a) :
            type(t), zoneID(z), agent(a)
        {}

        EventType type;
        core::uint16_t zoneID;
        const TriggerAgent& agent;
    };

    // TODO: remove boost and use stl instead
    // Each signal will receive a TriggerZone and an EventType
    typedef boost::signal<void (const EventInfo&)> Signal;
    typedef boost::signals::connection Connection;

public:
    TriggerSystem();
    ~TriggerSystem();

    ////////////////////////////////////////////////////////////////////////////
    //                    Characters associated methods
    //

    // @brief Create a new agent to be used to "trigger" the zones.
    // @returns the new agent created, this class will handle the memory of the
    //          agent. You can remove it when you want calling removeAgent().
    //
    TriggerAgent*
    createAgent(void);

    // @brief Remove a specific agent by ID or by ptr. This will remove the
    //        agent from the system and will free the memory also
    //
    void
    removeAgent(core::uint16_t id);
    inline void
    removeAgent(TriggerAgent* agent);

    // @brief Returns an agent by ID.
    // @param id The id of the agent we want to get
    // @returns the associated Agent from a given ID or 0 if not exists
    //
    inline TriggerAgent*
    getAgent(core::uint16_t id);

    // @brief Initialize the agent in a certaion position. We will find
    //        all the zones that intersects with the agent and we will start there.
    //        Call this method before start moving the agent
    // @param agent     The agent we want to initializate
    // @param position  The initial position of the agent
    //
    TriggerCode
    initializeAgent(TriggerAgent* agent, const Vector2& pos);

    // @brief Update the position of an agent, this is the same that calling
    //        agent->setPosition(pos).
    // @param agent The agent we want to update its position
    // @param pos   The new position of the agent
    // @return the associated error code.
    //
    TriggerCode
    updateAgentPos(TriggerAgent* agent, const Vector2& pos);



    ////////////////////////////////////////////////////////////////////////////
    //                       Building methods
    //

    // @brief Add a callback for an specific Zone. This callback will be called
    //        when an event occurs (see EventType).
    // @param zoneID    The zone for which we want to add the callback
    // @param callback  The callback itself
    // @return the associated connection
    //
    Connection
    addCallback(core::uint16_t zoneID, const Signal::slot_type& subscriber);

    // @brief Build the graph and the system. We will check for collisions between
    //        the zones and build the colored graph. This method is slow and will
    //        remove all the previous information.
    // @param zones     The zone list to be used when build the system.
    // @param ids       The resulting list of ids for the respective zones (the
    //                  zone[i] will have the ids[i] for each i).
    // @returns true on success | false otherwise (see debug info for more detailed
    //          explanation)
    //
    bool
    build(const std::vector<TriggerZone>& zones, std::vector<core::uint16_t>& ids);

    // @brief Destroy the current TriggerSystem. This will remove all the zones
    //        and callbacks and everything.
    //
    void
    destroy(void);

    // @brief Check if the system is already built
    //
    inline bool
    isAlreadyBuilt(void) const;

private:

    ////////////////////////////////////////////////////////////////////////////
    //                      Helper classes and methods                        //
    //

    class ZoneNode;
    typedef std::vector<ZoneNode*> ZoneNodePtrVec;

    struct ZoneNode {
        TriggerZone zone;
        ZoneNodePtrVec neighbors;
        Signal callbacks;
        TriggerColor_t color;
        core::uint16_t id;
    };

    // @brief Get all the zones that intersects a specific position.
    // @param pos   The position
    // @param zones The resulting zones
    //
    void
    getZonesFromPosition(const Vector2& pos, ZoneNodePtrVec& zones) const;

    // @brief Initialize the agent in its current position. We will find
    //        all the zones that matchs and set them as the closer zones
    // @param agent     The agent we want to initializate
    //
    TriggerCode
    initialize(TriggerAgent* agent);


    // @brief Method for coloring the "graph" (that is the vector of ZoneNode).
    //
    TriggerColor_t
    getColor(const ZoneNode& node);
    bool
    coloringTheGraph(void);

private:
    typedef std::vector<std::shared_ptr<TriggerAgent> > AgentPtrVec;
    typedef std::vector<TriggerZone> TriggerZoneVec;
    typedef std::vector<TriggerZone*> TriggerZonePtrVec;

    ZoneNode* mZoneNodes;
    core::size_t mZoneNodesSize;
    AgentPtrVec mAgents;
    ZoneNodePtrVec mZoneNodePtrs;

};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

////////////////////////////////////////////////////////////////////////////

inline void
TriggerSystem::removeAgent(TriggerAgent* agent)
{
    ASSERT(agent);
    removeAgent(agent->id());
}

inline TriggerAgent*
TriggerSystem::getAgent(core::uint16_t id)
{
    if (id >= mAgents.size()) {
        return 0;
    }
    return mAgents[id].get();
}


////////////////////////////////////////////////////////////////////////////

inline bool
TriggerSystem::isAlreadyBuilt(void) const
{
    return mZoneNodes != 0;
}

} /* namespace core */
#endif /* TRIGGERSYSTEM_H_ */
