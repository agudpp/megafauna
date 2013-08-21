/*
 * TriGraph.cpp
 *
 *  Created on: Aug 19, 2013
 *      Author: agustin
 */

#include "TriGraph.h"

#include <cstring>
#include <bitset>
#include <queue>

#include <types/BitMatrix.h>



// Helper methods
//
namespace {

// @brief Check if two triangles share some edges and returns the shared
//        vertices (indices).
// @return true if they share a edge | false otherwise
//
inline bool
shareEdge(const gps::Triangle& t1, const gps::Triangle& t2, unsigned short vertices[])
{
    unsigned int matchs = 0;
    for (unsigned int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (t1.vertex[i] == t2.vertex[j]) {
                vertices[matchs] = t1.vertex[i];
                ++matchs;
                if (matchs == 2) {
                    // done
                    return true;
                }
                ++i;
                --j;
            }
        }
    }
    return false;
}

}


namespace gps {

// @brief Remove the allocated memory
//
void
TriGraph::freeMemory(void)
{
    if (mVertices.data) {
        free(mVertices.data);
    }
    mVertices.size = mVertices.data = 0;
    mTris.size = mTris.data = 0;
    mNodes.size = mNodes.data = 0;
    mLinks.size = mLinks.data = 0;
}

////////////////////////////////////////////////////////////////////////////////
unsigned int
TriGraph::countSharedEdges(const core::ContiguousContainer<Vertex>& vertices,
                           const core::ContiguousContainer<Triangle>& triangles) const
{
    ASSERT(vertices.data);
    ASSERT(triangles.data);

    // what we will do is just count the number of shared edges... to do this
    // we will use a memory expensive but easy and fast to implement algorithm
    // We will create a matrix[NumVertices][NumVertices] and for every edge (x,y)
    // where x and y are indices of vertices representing the edge, we will
    // set to true the matrix[x][y] = matrix[y][x] = 1;
    // if before setting that value, it was already set, means that was shared.
    // @IMPORTANT: note that we are assuming that we have only one connected
    //             component in the graph
    //
    unsigned int shared = 0;

    // allocate in stack no more than 1000 vertices? there are a lot of vertices
    // we will get a crash if we have more than 1000, anyway we are allocating
    // 1000*1000/8 bytes.
    //
    core::BitMatrix<1000*1000> matrix;

    for (unsigned int i = 0; i < triangles.size; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            const unsigned int r = triangles[i].vertex[j];
            const unsigned int c = triangles[i].vertex[(j+1)%3];

            if (matrix.get(c, r)) {
                ++shared;
            } else {
                matrix.set(c, r, true);
                matrix.set(r, c, true);
            }
        }
    }

    return shared;
}

////////////////////////////////////////////////////////////////////////////////
bool
TriGraph::build(const core::ContiguousContainer<Vertex>& vertices,
                const core::ContiguousContainer<Triangle>& triangles)
{
    ASSERT(vertices.size > 0 && vertices.data != 0);
    ASSERT(triangles.size > 0 && triangles.size != 0);

    debugWARNING("Warning: This method should be only called in debug mode, use"
        " the importer / exporter methods for the final version\n");

    // check the number of triangles
    //
    if (triangles.size > MAX_NUM_NODES) {
        debugERROR("The number of triangles is bigger than the number of "
            "possible nodes we can have: %d\n", triangles.size);
        return false;
    }

    // we need first to know the number of edges we have to allocate..
    //
    const unsigned int numSharedEdges = countSharedEdges(vertices, triangles);
    if (numSharedEdges == 0) {
        debugWARNING("Empty graph\n?");
        return false;
    }

    // we will allocate the memory we need just to start working with that
    //
    freeMemory(); // just in case
    void* mem = malloc(vertices.size * sizeof(Vertex) +
                       triangles.size * sizeof(Triangle) +
                       triangles.size * sizeof(TriNode) + // We must ensure this
                       numSharedEdges * sizeof(TriLink));
    setMem(mem, vertices.size, triangles.size, triangles.size, numSharedEdges);
    std::memcpy(mVertices.data, vertices.data, vertices.size * sizeof(Vertex));
    std::memcpy(mTris.data, triangles.data, triangles.size * sizeof(Triangle));

    // now that we have loaded all the memory, we can create the graph
    // TODO: use stack_allocator instead of this.
    //

    // set all the nodes first
    for (unsigned int i = 0; i < mNodes.size; ++i) {
        mNodes[i].info.triangleIndex = i; // set exactly the same as the triangle
        mNodes[i].resetLinks(); // initialize
    }

    // now we need to configure all the edges and then set the corresponding
    // edges for each node... this is O(n^2)! but is not very important since
    // this method should be called only for debug
    //
    unsigned int realEdgesCount = 0;
    unsigned short sharedEdge[2];
    for (unsigned int i = 0, size = mNodes.size; i < size; ++i) {
        unsigned int edgesFound = 0;
        TriNode& node = mNodes[i];
        // now for each other node we will try to get the common edge if they have
        //
        for (unsigned int j = i+1; j < size; ++j) {
            TriNode& node2 = mNodes[j];
            if (shareEdge(mTris[node.info.triangleIndex],
                          mTris[node2.info.triangleIndex],
                          sharedEdge)) {
                // we have a new edge shared
                ASSERT(realEdgesCount < mLinks.size);
                ASSERT(node.info.triangleIndex != node2.info.triangleIndex);

                mLinks[realEdgesCount].nodes[0] = i;
                mLinks[realEdgesCount].nodes[1] = j;

                // here we will also calculate the squared lenght between the
                // vertices conforming the edge
                //
                ASSERT(sharedEdge[0] < mVertices.size);
                ASSERT(sharedEdge[1] < mVertices.size);
                const Vertex& v1 = mVertices[sharedEdge[0]];
                const Vertex& v2 = mVertices[sharedEdge[1]];
                mLinks[realEdgesCount].info.sqrLen = v1.squaredDistance(v2);

                // set to node and node2 the associated link!
                //
                node.setNewLink(realEdgesCount);
                node2.setNewLink(realEdgesCount);

                ++realEdgesCount;
                ++edgesFound;

                // check if we have to continue checking for the current node
                //
                if (edgesFound == 3) {
                    // no we have not
                    break;
                }
            }
        }
    }
    ASSERT(realEdgesCount == mLinks.size);
    ASSERT(realEdgesCount == numSharedEdges);

    // here we need to check if from one starting node we can visit all the
    // other nodes using the above information, if not, then, the graph
    // had non connected component...
    if (containNonConnectedComp()) {
        debugERROR("The graph contains non connected components... something "
            "is wrong with the inpu data?\n");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
TriGraph::setMem(const void* mem,
                 unsigned int vCount,
                 unsigned int triCount,
                 unsigned int nodesCount,
                 unsigned int edgesCount)
{
    ASSERT(mem != 0);

    unsigned int offset = 0;
    mVertices.data = mem;
    mVertices.size = vCount;

    offset += vCount * sizeof(Vertex);
    mTris.data = mem + offset;
    mTris.size = triCount;


    offset += nodesCount * sizeof(Triangle);
    mNodes.data = mem + offset;
    mNodes.size = nodesCount;

    offset += mNodes.size * sizeof(TriNode);
    mLinks.data = mem + offset;
    mLinks.size = edgesCount;
}

////////////////////////////////////////////////////////////////////////////////
bool
TriGraph::containNonConnectedComp(void) const
{
    if (mNodes.size <= 1) {
        return true;
    }

    // use the bitset to check for the already visited nodes
    std::bitset<MAX_NUM_NODES> alreadyVisited;
    // TODO use stack allocator here
    std::queue<unsigned short> toVisit;
    toVisit.push(0);
    unsigned int numNodesVisited = 1;

    while (!toVisit.empty()) {
        const unsigned short currentIndex = toVisit.back();
        toVisit.pop();
        if (alreadyVisited[currentIndex]) { // this should never happen
            // do nothing
            continue;
        }
        // visit this node
        alreadyVisited[currentIndex] = true;
        ++numNodesVisited;

        // get the neighbors
        ASSERT(currentIndex < mNodes.size);
        const TriNode& node = mNodes[currentIndex];
        for (unsigned int i = 0, size = node.validLinks(); i < size; ++i) {
            ASSERT(node.linkIndices[i] < mLinks.size);
            const TriLink& link = mLinks[node.linkIndices[i]];
            const unsigned int neighbor = link.getOppositeNode(currentIndex);

            if (!alreadyVisited[neighbor]) {
                toVisit.push(neighbor);
            }
        }
    }

    // here we need to have to being visited all the nodes...
    ASSERT(numNodesVisited <= mNodes.size); // this should never happen
    return mNodes.size != numNodesVisited;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TriGraph::TriGraph()
{
}

////////////////////////////////////////////////////////////////////////////////
TriGraph::~TriGraph()
{
    freeMemory();
}

////////////////////////////////////////////////////////////////////////////////
bool
TriGraph::buildGraph(const core::ContiguousContainer<Vertex>& vertices,
                     const core::ContiguousContainer<Triangle>& triangles)
{
    // proxy function
    return build(vertices, triangles);
}

////////////////////////////////////////////////////////////////////////////////
bool
TriGraph::importFromFile(const std::string& filename)
{
    debugERROR("TODO: implement this\n");
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
TriGraph::exportToFile(const std::string& filename) const
{
    debugERROR("TODO: implement this\n");
    return false;
}



} /* namespace gps */
