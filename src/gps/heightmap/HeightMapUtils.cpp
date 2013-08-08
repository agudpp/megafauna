/*
 * HeightMapUtils.cpp
 *
 *  Created on: Aug 8, 2013
 *      Author: agustin
 */

#include "HeightMapUtils.h"

#include <vector>
#include <algorithm>

#include <OgreMesh.h>
#include <OgreString.h>

#include <ogre_utils/OgreUtil.h>
#include <debug/DebugUtil.h>
#include <types/basics.h>
#include <math/AABB.h>
#include <math/FloatComp.h>


// helper methods
//
namespace {

// Some typedefs
//
typedef std::vector<Ogre::Vector3> VecV3;

// @brief Method used to calculate the BB of a list of Ogre vertices only
//        in the x and y coordinates (not Z).
//
core::AABB
getBoundingBox(const std::vector<Ogre::Vector3>& vertices)
{
    core::AABB bb;
    for (core::size_t i = 0, size = vertices.size(); i < size; ++i) {
        const Ogre::Vector3& v = vertices[i];
        bb.increaseToContain(core::Vector2(v.x, v.y));
    }
    return bb;
}

// @brief Method used to check if two vectors are the same using an epsilon
//        as error and discarding the z value.
//
inline bool
areEqual(const Ogre::Vector3& v1, const Ogre::Vector3& v2)
{
    return core::fcomp_equal(v1.x, v2.x, FLOAT_COMP_THRESHOLD) &&
           core::fcomp_equal(v1.y, v2.y, FLOAT_COMP_THRESHOLD);
}

// @brief Swap the i-th element of the vector with the last one and pop the last.
//
template <typename T>
inline void
swapAndPop(std::vector<T>& vec, core::size_t i)
{
    ASSERT(i < vec.size());
    vec[i] = vec.back();
    vec.pop_back();
}

// @brief Remove duplicated items (slow)
//
inline void
removeDuplicated(std::vector<Ogre::Vector3>& vec)
{
    for (core::size_t i = 0; i < vec.size(); ++i) {
        Ogre::Vector3& v1 = vec[i];
        for (core::size_t j = i+1; j < vec.size(); ++j){
            if (areEqual(v1, vec[j])) {
                swapAndPop(vec,j);
                --j;
            }
        }
    }
}

// @brief Sorting functions for vectors
inline bool
sortVecByX(const Ogre::Vector3& a, const Ogre::Vector3&b)
{
    return a.x < b.x;
}
inline bool
sortVecByY(const Ogre::Vector3& a, const Ogre::Vector3&b)
{
    return a.y < b.y;
}

// @brief Check that the distances from each point in X are the same in the vector
//
inline bool
checkDistancesX(const std::vector<Ogre::Vector3>& vec)
{
    if (vec.size() < 2) {
        return true;
    }

    const float distance = std::abs(vec[0].x - vec[1].x);
    for (core::size_t i = 2, size = vec.size(); i < size; ++i) {
        const float d = std::abs(vec[i].x - vec[i-1].x);
        if (!core::fcomp_equal(distance, d, 0.1f)) {
            // some distances differ...
            debugERROR("Distance 0: %f, d: %f, index: %d\n", distance, d, i);
            return false;
        }
    }
    return true;
}

// @brief Method that return an array of indices with the sorted values.
//        And we also check for the distances (after we sort them)
//
inline bool
sortAndCheckMatrix(const std::vector<VecV3>& matrix,
           std::vector<unsigned int>& resultIndices)
{
    struct ValueIndex {
        float value;
        unsigned int index;
        bool operator<(const ValueIndex& other) const {return value < other.value;}
    };

    std::vector<ValueIndex> toSort(matrix.size());

    for (unsigned int i = 0, size = matrix.size(); i < size; ++i) {
        ASSERT(!matrix[i].empty());
        toSort[i].value = matrix[i].front().y;
        toSort[i].index = i;
    }

    std::sort(toSort.begin(), toSort.end());

    resultIndices.resize(toSort.size());
    for (unsigned int i = 0, size = toSort.size(); i < size; ++i) {
        resultIndices[i] = toSort[i].index;
    }
    ASSERT(toSort.size() == matrix.size());
    if (toSort.size() < 2) {
        return true;
    }

    // check for distances
    const float distance = std::abs(toSort[0].value -
        toSort[1].value);
   for (core::size_t i = 2, size = toSort.size(); i < size; ++i) {
       const float d = std::abs(toSort[i].value -
           toSort[i-1].value);
       if (!core::fcomp_equal(distance, d, 0.1f)) {
           // some distances differ...
           debugERROR("Distance 0: %f, d: %f, index: %d\n", distance, d, resultIndices[i]);
           return false;
       }
   }

    return true;
}

}


namespace gps {
namespace HeightMapUtils {

bool
configureFromMesh(const Ogre::Mesh* mesh, HeightMap<Ogre::Vector3>& hm)
{
    ASSERT(mesh);
    core::size_t vCount = 8192;
    std::vector<Ogre::Vector3> vertices(vCount);
    core::size_t iCount = 10000;
    std::vector<unsigned long> indices(iCount);

    if (!core::OgreUtil::getMeshInformation(mesh,
                                            vCount,
                                            &(vertices[0]),
                                            iCount,
                                            &(indices[0]))) {
        debugERROR("Some problem occur when trying to get the mesh information\n");
        return false;
    }
    // resize the vectors to the real size
    vertices.resize(vCount);

    // now that we have the vertices we can remove the duplicated items
    debugGREEN("before: %d\n", vertices.size());
    removeDuplicated(vertices);
    debugGREEN("after: %d\n", vertices.size());

    // the indices doesn't matters, calculate the bounding box
    const core::AABB bb = getBoundingBox(vertices);

    // now while we still have elements in the vector, we need to perform this
    // operation

    std::vector<VecV3> matrix;
    matrix.reserve(512); // a lot of memory...
    while (!vertices.empty()) {
        // allocate the new vector we will use (to avoid further slow copies)
        matrix.push_back(VecV3());
        VecV3& row = matrix.back();
        row.reserve(512);

        // fill the row
        const Ogre::Vector3 vert = vertices.front();
        row.push_back(vert);
        swapAndPop(vertices,0);
        for (core::size_t i = 0; i < vertices.size(); ++i) {
            if (core::fcomp_equal(vert.y, vertices[i].y, 0.1f)) {
                // they are equal, we need to put it in the row and remove it
                // from the vertices
                row.push_back(vertices[i]);
                swapAndPop(vertices, i);
                --i;
            }
        }

        // check that we have the same number of elements for each new row
        if (matrix.back().size() != matrix.front().size()) {
            debugERROR("Current vector contains different number of elements "
                "than the first one: last: %d, first: %d\n",
                matrix.back().size(), matrix.front().size());
            return false;
        }

        // now we have all the vertices for the first row, we need to sort them
        // by x
        std::sort(row.begin(), row.end(), sortVecByX);

        // check that the distance between them are the same
        if (!checkDistancesX(row)) {
            debugERROR("Distance fails\n");
            return false;
        }

    }

    // now we need to sort all the vectors for the matrix by Y value
    std::vector<unsigned int> rowIndices;
    if (!sortAndCheckMatrix(matrix, rowIndices)) {
        debugERROR("The rows are not equally separated\n");
        return false;
    }

    // now we need to create the real data to be used to built the HeightMap
    std::vector<float> data;
    const unsigned int numColumns = matrix.front().size();
    const unsigned int numRows = matrix.size();
    data.reserve(numRows * numColumns);

    for (unsigned int row = 0; row < numRows; ++row) {
        VecV3& rowVec = matrix[rowIndices[row]];
        for (unsigned int col = 0; col < numColumns; ++col) {
            data.push_back(rowVec[col].z);
        }
    }

    // configure the hm
    return hm.build(bb, numColumns-1, numRows-1, data);
}

} /* namespace HeightMapUtils */
} /* namespace gps */
