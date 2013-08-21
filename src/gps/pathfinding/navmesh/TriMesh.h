/*
 * TriMesh.h
 *
 *  Created on: Aug 19, 2013
 *      Author: agustin
 */

#ifndef TRIMESH_H_
#define TRIMESH_H_

#include "NavMeshCommon.h"

#include <types/ContiguousCont.h>
#include <debug/DebugUtil.h>

namespace gps {

class TriMesh
{
public:
    inline TriMesh();
    inline ~TriMesh();

    // @brief Build the TriMesh from a list of vertices and a list of triangles.
    //        This method will package all the information into one common
    //        block of memory to improve cache efficiency.
    // @param vertices  The list of vertices to be used.
    // @param vCount    The number of vertices in the array
    // @param triangles The list of triangles to be used, each triangle
    //                  should contain the indices of the vertices passed in the
    //                  first parameter
    // @param triCount  The number of triangles in the array.
    //
    inline void
    buildMesh(const Vertex* vertices, unsigned int vCount,
              const Triangle* triangles, unsigned int triCount);

    // @brief Returns the associated containers
    //
    inline const core::ContiguousContainer<Vertex>&
    vertices(void) const;
    inline const core::ContiguousContainer<Triangle>&
    triangles(void) const;

private:
    // avoid copying
    //
    TriMesh(const TriMesh&);
    TriMesh& operator=(const TriMesh&);

private:
    // the container of triangles and the vertices list
    //
    core::ContiguousContainer<Vertex> mVertices;
    core::ContiguousContainer<Triangle> mTriangles;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline TriMesh::TriMesh()
{}

inline TriMesh::~TriMesh()
{
    if (mVertices.data) {
        free(data);
    }
}

inline void
TriMesh::buildMesh(const Vertex* vertices, unsigned int vCount,
          const Triangle* triangles, unsigned int triCount)
{
    // check if we need to remove the old data
    if (mVertices.data) {
        free (mVertices.data); mVertices.data = 0;
        free (mTriangles.data); mTriangles.data = 0;
    }

    // allocate a whole block for everything
    void* data = malloc(vCount * sizeof(Vertex) + triCount * sizeof(Triangle));
    ASSERT(data);

    // point the new data now
    mVertices.data = data;
    mVertices.size = vCount;

    mTriangles.data = data + (vCount * sizeof(Vertex));
    mTriangles.size = triCount;
}

// @brief Returns the associated containers
//
inline const core::ContiguousContainer<Vertex>&
TriMesh::vertices(void) const
{
    return mVertices;
}
inline const core::ContiguousContainer<Triangle>&
TriMesh::triangles(void) const
{
    return mTriangles;
}


} /* namespace gps */
#endif /* TRIMESH_H_ */
