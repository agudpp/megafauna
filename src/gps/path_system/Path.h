/*
 * Path.h
 *
 *  Created on: Nov 12, 2013
 *      Author: agustin
 */

#ifndef PATH_H_
#define PATH_H_

#include <math/Vec2.h>

#include <types/StackVector.h>

namespace gps {

// @brief We will define here the interface for the paths and how we can interact
//        with them.
//        Also the interface to build them.
//

class Path {
public:
    // Here we will define some of the auxiliar classes we will use to interact
    // with the path
    //

    // define the type of node positions we will use (2D / 3D)
    typedef core::Vector2 Point;

    // define the maximum number of nodes we will support per parth
    static const unsigned int MAX_NUM_NODES = 45;

    // define the maximum number of neighbors per node (2)
    //
    static const unsigned int NUM_NEIGHBORS_PER_NODE = 2;

private:

    // Internal classes

    // @brief Edge structure (TODO: not used now)
    //
    struct Edge {
        unsigned short nodeIndex1;
        unsigned short nodeIndex2;
    };

    // @brief The Node class used by the path.
    //
    struct Node {
        Point position;

        // TODO: not used now.
        unsigned short edges[NUM_NEIGHBORS_PER_NODE];
        unsigned short edgesCount;

        // Methods
        Node(){}
        Node(const Point& p) : position(p) {}
    };

public:
    Path();
    ~Path();

    ////////////////////////////////////////////////////////////////////////////
    // Methods to use the path
    //

    // @brief Return the number of nodes this path has.
    // @return the total number of nodes
    //
    inline unsigned int
    numberNodes(void) const;

    // @brief Return a position for a particular node in the path.
    // @param nodeIndex     The node index we want to get its position
    // @return node position reference
    // @requires nodeIndex < numberNodes()
    //
    inline const Point&
    nodePos(unsigned int nodeIndex) const;

    ////////////////////////////////////////////////////////////////////////////
    // Methods to create / modify the path
    //

    // @brief Push a node position (connect the last one with the one we are adding)
    // @param pos       The new position we will add and connect with the last one
    //                  we added.
    //
    void
    pushNode(const Point& pos);

    // @brief Remove the last node from the path.
    //
    void
    removeLast(void);

    // @brief Clear the current path information
    //
    void
    clear(void);

private:
    core::StackVector<Node, MAX_NUM_NODES> mNodes;
    // TODO: add edges here
};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
inline unsigned int
Path::numberNodes(void) const
{
    return mNodes.size();
}

inline const Point&
Path::nodePos(unsigned int nodeIndex) const
{
    ASSERT(nodeIndex < mNodes.size());
    return mNodes[nodeIndex].position;
}

}

#endif /* PATH_H_ */
