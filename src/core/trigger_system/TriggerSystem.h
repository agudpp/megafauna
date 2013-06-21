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
    // TODO: remove boost and use stl instead
    // Each signal will receive a TriggerZone and an EventType
    typedef boost::signal<void (const TriggerZone&, EventType)> Signal;
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

    // @brief Update the position of an agent, this is the same that calling
    //        agent->setPosition(pos).
    // @param agent The agent we want to update its position
    // @param pos   The new position of the agent
    //
    void
    updateAgentPos(TriggerAgent* agent, const Vector2& pos);



    ////////////////////////////////////////////////////////////////////////////
    //                       Building methods
    //

    // @brief Add a new zone to the system. Note that you must call the build()
    //        method after you finish adding all the zones you want to track
    // @param tz    The TriggerZone to be tracked.
    //
    inline void
    addZone(const TriggerZone& zone);

    // @brief Remove all the zones
    //
    inline void
    removeAllZones(void);

    // @brief Add a callback for an specific Zone. This callback will be called
    //        when an event occurs (see EventType).
    // @param zoneID | zone     The zone for which we want to add the callback
    // @param callback          The callback itself
    // @return the associated connection
    Connection
    addCallback(core::uint16_t zoneID, const Signal::slot_type& subscriber);
    inline Connection
    addCallback(const TriggerZone& zone, const Signal::slot_type& subscriber);


    // @brief Build the graph and the system. We will check for collisions between
    //        the zones and build the colored graph. This method is slow and will
    //        remove all the previous information.
    // @returns true on success | false otherwise (see debug info for more detailed
    //          explanation)
    //
    bool
    build(void);


private:

    ////////////////////////////////////////////////////////////////////////////
    //                      Helper classes and methods                        //
    //

    class ZoneNode;
    typedef std::vector<ZoneNode*> ZoneNodePtrVec;

    struct ZoneNode {
        TriggerZone zone;
        TriggerColor_t color;
        ZoneNodePtrVec neighbors;
        Signal callbacks;
    };

    // @brief Method for coloring the "graph" (that is the vector of ZoneNode).
    //
    TriggerColor_t
    getColor(const ZoneNode& node);
    bool
    coloringTheGraph(void);

private:
    typedef std::vector<std::shared_ptr<TriggerAgent> > AgentPtrVec;
    typedef std::vector<TriggerZone> TriggerZoneVec;

    ZoneNode* mZoneNodes;
    core::size_t mZoneNodesSize;
    AgentPtrVec mAgents;
    TriggerZoneVec mTmpZones;
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
    if (id > mAgents.size()) {
        return 0;
    }
    return mAgents[id].get();
}


////////////////////////////////////////////////////////////////////////////

inline void
TriggerSystem::addZone(const TriggerZone& zone)
{
    mTmpZones.push_back(zone);
    mTmpZones.back().setID(mTmpZones.size() - 1);
}

// @brief Remove all the zones
//
inline void
TriggerSystem::removeAllZones(void)
{
    mTmpZones.clear();
}

inline TriggerSystem::Connection
TriggerSystem::addCallback(const TriggerZone& zone, const Signal::slot_type& subscriber)
{
    return addCallback(zone.id(), subscriber);
}

} /* namespace core */
#endif /* TRIGGERSYSTEM_H_ */
