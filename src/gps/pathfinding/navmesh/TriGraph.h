/*
 * TriGraph.h
 *
 *  Created on: Aug 19, 2013
 *      Author: agustin
 */

#ifndef TRIGRAPH_H_
#define TRIGRAPH_H_

#include <limits>
#include <string>

#include <types/ContiguousCont.h>
#include <debug/DebugUtil.h>

#include "NavMeshCommon.h"


namespace gps {

static const unsigned short INVALID_INDEX = std::numeric_limits<unsigned short>::max();

// Define the structures used for this class
//
struct TriNode {
    // The associated data of the Node, in this case the triangle index and
    // probably something else like some kind of flag...
    //
    struct {
        unsigned short triangleIndex;
        // define here anything else we want to put in the node for example some
        // flags (is water? or whatever we want)
    } info;

    // the associated edges, in this case the indices of the edges... probably
    // we are losing cache performance here, we need to verify which is better
    //
    unsigned short linkIndices[3]; // at least 3, no more

    ////////////////////////////////////////////////////////////////////////////
    // Some helper methods
    //

    // @brief Reset all the link indices to invalid indices
    //
    inline void
    resetLinks(void);

    // @brief Get the number of links associated to this node
    // @returns the number of valid links
    //
    inline unsigned int
    validLinks(void) const;

    // @brief Check if have some valid link.
    // @return true if it has | false otherwise
    //
    inline bool
    hasValidLinks(void) const;

    // @brief Set a new link to this node. It will be "appended" in the last
    //        free position
    // @param lindex    The link index.
    // @note This method will crash if all the links are already set (3).
    //
    inline void
    setNewLink(unsigned short lindex);


};

struct TriLink {
    // This links represent the connection between the nodes, note that only connect
    // two nodes per link because we are connecting 2 triangles with one edge.
    // We can define the information of the edge here, for example, squared length?
    // we also can set some kind of weight if needed, now we will use just the
    // length to know if some object could pass or not through this edge.
    //
    struct {
        float sqrLen;
    } info;

    // the indices of the associated nodes
    //
    unsigned short nodes[2];    // if some of the nodes is empty = INVALID_INDEX
                                // is set

    ////////////////////////////////////////////////////////////////////////////
    // Some helper methods
    //

    // @brief Get the other index different from the given.
    // @param index The current index we want to avoid
    // @return the other associated node index : other != index && other != INVALID.
    //
    inline unsigned short
    getOppositeNode(unsigned short index) const;
};


// We are assuming for now that the graph is fully connected, so there are
// only one component in the graph... this is probably a limitation, but not
// for now
//

class TriGraph
{
public:
    // define some constants
    static const unsigned int MAX_NUM_NODES = 1000;

public:
    TriGraph();
    ~TriGraph();

    // @brief Build a graph from a list of vertices and a list of indices
    //        indicating the triangles.
    //        This method will create the graph where each node will be a single
    //        triangle and the links between the triangles will be the edges
    //        that share between them. So size(triangles) == size(nodes).
    //        We will also calculate the squared distance for each edge.
    //        We will allocate all the memory in a contiguous block (of the
    //        vertices and the triangles and everything).
    //        This method will use own memory for everything (vertices and triangles)
    //        The indices of the triangles are maintained as well as the vertices
    //
    // @param vertices  The vertices to be used
    // @param triangles The triangles to be used
    // @return true on success | false otherwise
    //
    bool
    buildGraph(const core::ContiguousContainer<Vertex>& vertices,
               const core::ContiguousContainer<Triangle>& triangles);

    // @brief Build a graph from an specific file.
    // @param filename  The filename to be read.
    // @return true on success | false otherwise
    //
    bool
    importFromFile(const std::string& filename);

    // @brief Export the current graph into file
    // @param filename  The filename to export the data.
    // @return true on success | false otherwise
    //
    bool
    exportToFile(const std::string& filename) const;

    // @brief Get the already built information of the graph (nodes and edges)
    //
    inline const core::ContiguousContainer<TriNode>&
    nodes(void) const;
    inline const core::ContiguousContainer<TriLink>&
    links(void) const;

private:
    // avoid copiable
    //
    TriGraph(const TriGraph&);
    TriGraph& operator=(const TriGraph&);

    // @brief Remove the allocated memory
    //
    void
    freeMemory(void);

    // @brief Count the number of shared edges of this graph since this is the
    //        only information we need (shared edges are the links between the
    //        nodes in the path)
    // @return the number of edges, or 0 on error
    //
    unsigned int
    countSharedEdges(const core::ContiguousContainer<Vertex>& vertices,
                     const core::ContiguousContainer<Triangle>& triangles) const;

    // @brief Build the graph from the already allocated vertices and tris but
    //        not our local memory, from outside.
    //        This method will allocate memory for the local vertices and tris
    //        copying the current information as well as creating the nodes and
    //        edges
    // @return true on success | false otherwise
    //
    bool
    build(const core::ContiguousContainer<Vertex>& vertices,
          const core::ContiguousContainer<Triangle>& triangles);

    // @brief Set the memory from an specific buffer.
    // @param buff  The memory address where we will set these values
    // @param sizes The different sizes of the data
    //
    void
    setMem(const void* mem,
           unsigned int vCount,
           unsigned int triCount,
           unsigned int nodesCount,
           unsigned int edgesCount);

    // @brief Check if the current graph has non connected components.
    // @return true if the graph contains non connected components | false if not
    //
    bool
    containNonConnectedComp(void) const;


private:
    core::ContiguousContainer<Vertex> mVertices;
    core::ContiguousContainer<Triangle> mTris;
    core::ContiguousContainer<TriNode> mNodes;
    core::ContiguousContainer<TriLink> mLinks;
};








////////////////////////////////////////////////////////////////////////////////
//                              Inline stuff                                  //
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// TriNode methods:
//

inline void
TriNode::resetLinks(void)
{
    for (unsigned int i = 0; i < 3; ++i) {
        linkIndices[i] = INVALID_INDEX;
    }
}

inline unsigned int
TriNode::validLinks(void) const
{
    return linkIndices[0] == INVALID_INDEX ? 0 :
        linkIndices[1] == INVALID_INDEX ? 1 :
            linkIndices[2] == INVALID_INDEX ? 2 : 3;
}


inline bool
TriNode::hasValidLinks(void) const
{
    return linkIndices[0] != INVALID_INDEX;
}

inline void
TriNode::setNewLink(unsigned short lindex)
{
    ASSERT(validLinks() < 3);
    linkIndices[validLinks()] = lindex;
}


////////////////////////////////////////////////////////////////////////////////
// TriLink methods:
//

inline unsigned short
TriLink::getOppositeNode(unsigned short index) const
{
    ASSERT(nodes[0] == index || nodes[1] == index);
    return nodes[0] == index ? nodes[1] : nodes[0];
}


////////////////////////////////////////////////////////////////////////////////
// TriGraph methods:
//

inline const core::ContiguousContainer<TriNode>&
TriGraph::nodes(void) const
{
    return mNodes;
}
inline const core::ContiguousContainer<TriLink>&
TriGraph::links(void) const
{
    return mLinks;
}

} /* namespace gps */
#endif /* TRIGRAPH_H_ */
