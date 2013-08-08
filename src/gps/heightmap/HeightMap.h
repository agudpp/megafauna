/*
 * HeightMap.h
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#ifndef HEIGHTMAP_H_
#define HEIGHTMAP_H_

#include <vector>
#include <cstring>

#include <math/AABB.h>
#include <math/Vec2.h>
#include <debug/DebugUtil.h>

// This class will be in charge of handle the height map information. It will bring
// a interface to get the information of the map and some other querys.
//

namespace gps {

// Note that VEC_TYPE should contain x, y, z members
//

template <typename VEC_TYPE>
class HeightMap
{
public:
    // Empty constructor / destructor
    inline HeightMap();
    inline ~HeightMap();

    // @brief Define the size of the height map and the cell sizes
    // @param bb        The bounding box of the area of the heightmap.
    // @param columns   Number of cells in X (width / columns)
    // @param rows      Number of cells in Y (height / rows).
    // @param data      The value of the heights for each point in the matrix.
    //                  This should contain exactly (rows+1)*(columns+1) elements
    //                  and the elements should be sorted by rows from the bottom
    //                  left (increasing to the right) and the last element should
    //                  be the top right like this:
    //                  N-M.........N-1
    //                  |            |
    //                  |            |
    //                  0............M
    //
    // @return true on success | false otherwise
    //
    inline bool
    build(const core::AABB& bb,
          unsigned int columns,
          unsigned int rows,
          const std::vector<float>& data);

    // @brief Check if a position is inside the area of the height map
    // @param pos       The position
    // @returns true if it is | false otherwise
    //
    inline bool
    isPositionInside(const core::Vector2& pos) const;

    // @brief Get the height in a certain position.
    // @param pos       The position
    // @return the associated height
    // @note That this method requires that the point is inside of the heightmap
    //       bounding box.
    //
    inline float
    getHeight(const core::Vector2& pos) const;

    // @brief Get the normal of a current position.
    // @param pos       The position
    // @param normal    The resulting normal
    // @note That this method requires that the point is inside of the heightmap
    //       bounding box.
    //
    inline void
    getNomral(const core::Vector2& pos, VEC_TYPE& normal) const;

    // @brief Method that get the height and normal at the same time to
    //        save some time in matters of performance.
    // @param pos       The position
    // @param normal    The resulting normal
    // @returns the height in the given position
    // @note That this method requires that the point is inside of the heightmap
    //       bounding box.
    //
    inline float
    getHeightAndNormal(const core::Vector2& pos, VEC_TYPE& normal) const;

private:
    // avoid copy
    HeightMap(const HeightMap&);
    HeightMap& operator=(const HeightMap&);

    // @brief Get the index for an specific row and column
    // @param row   The row
    // @param col   The column
    //
    inline unsigned int
    getIndex(unsigned int row, unsigned int col) const;

private:
    float* mData;
    unsigned int mSize;
    core::AABB mBB;
    core::Vector2 mOffset;
    float mCellSizeX;
    float mCellSizeY;
    float mInvFactorX;
    float mInvFactorY;
    unsigned int mNumColumns;
    unsigned int mNumRows;

};



//
// INLINE STUFF
//
template<typename VEC_TYPE>
inline unsigned int
HeightMap<VEC_TYPE>::getIndex(unsigned int row, unsigned int col) const
{
    return mNumColumns * row + col;
}


template <typename VEC_TYPE>
inline HeightMap<VEC_TYPE>::HeightMap() :
    mData(0)
{}

template <typename VEC_TYPE>
inline HeightMap<VEC_TYPE>::~HeightMap()
{
    delete mData;
}

template <typename VEC_TYPE>
inline bool
HeightMap<VEC_TYPE>::build(const core::AABB& bb,
                           unsigned int columns,
                           unsigned int rows,
                           const std::vector<float>& data)
{
    const unsigned int size = (columns + 1) * (rows + 1);
    if (data.size() != size) {
        return false;
    }

    // copy the BB
    ASSERT(bb.getHeight() > 0.f && bb.getWidth() > 0.f);
    mBB = bb;

    // remove old data and copy the new one
    delete mData; mData = 0;
    mData = new float[size];
    std::memcpy(mData, &(data[0]), size * sizeof(float));
    mSize = size;

    // calculate values
    mOffset.x = -bb.tl.x; mOffset.y = -bb.br.y; // offset
    const float worldWidth = bb.getWidth();
    const float worldHeight = bb.getHeight();
    mInvFactorX = static_cast<float>(rows) / worldHeight; // = 1 / YCellSize
    mInvFactorY = static_cast<float>(columns) / worldWidth; // 1 / XCellSize
    mNumColumns = columns + 1;
    mNumRows = rows + 1;
    mCellSizeX = 1.f / mInvFactorX;
    mCellSizeY = 1.f / mInvFactorY;

    return true;
}

template <typename VEC_TYPE>
inline bool
HeightMap<VEC_TYPE>::isPositionInside(const core::Vector2& pos) const
{
    return mBB.checkPointInside(pos);
}

template <typename VEC_TYPE>
inline float
HeightMap<VEC_TYPE>::getHeight(const core::Vector2& pos) const
{
    ASSERT(isPositionInside(pos));
    VEC_TYPE dummy;
    return getHeightAndNormal(pos, dummy);
}

template <typename VEC_TYPE>
inline void
HeightMap<VEC_TYPE>::getNomral(const core::Vector2& pos, VEC_TYPE& normal) const
{
    ASSERT(isPositionInside(pos));
    getHeightAndNormal(pos, normal);
}

template <typename VEC_TYPE>
inline float
HeightMap<VEC_TYPE>::getHeightAndNormal(const core::Vector2& pos, VEC_TYPE& normal) const
{
    ASSERT(isPositionInside(pos));

    // move the position to the corresponding place (offset)
    const core::Vector2 realPos(pos + mOffset);

    // get the corresponding cells
    const unsigned int cellX = realPos.x * mInvFactorX;
    const unsigned int cellY = realPos.y * mInvFactorY;

    // check that we are inside of the indices
    ASSERT(cellX + 1 < mNumColumns);
    ASSERT(cellY + 1 < mNumRows);

    // now we should get the real positions in the array
    //   p2...........p3
    //   |            |
    //   |            |
    //   p0...........p1
    //
    const unsigned int p0 = getIndex(cellY, cellX),
                       p1 = getIndex(cellY, cellX+1),
                       p2 = getIndex(cellY+1, cellX),
                       p3 = getIndex(cellY+1, cellX+1);
    ASSERT(p0 < mSize); ASSERT(p1 < mSize);
    ASSERT(p2 < mSize); ASSERT(p3 < mSize);

    // now we will check which is the triangle we need
    //   p2..p3  -
    //   |1 / |  |  H is the height of the
    //   | / 2|  |    cell
    //   p0..p1  -
    //   |----|
    //      W  is the width of the cell
    // we can check if the dx (delta X) that is the % € [0,1] of x =
    // (pos.x - p0.x)/W
    // and dy = (pos.y - p0.y) / H
    // we can determine if pos € triangle1, or triangle2...
    const VEC_TYPE p0v(cellX * mCellSizeX, cellY * mCellSizeY, mData[p0]);
    const VEC_TYPE p3v((cellX+1) * mCellSizeX, (cellY+1) * mCellSizeY, mData[p3]);
    const float dx = (realPos.x - p0v.x) * mInvFactorX;
    const float dy = (realPos.y - p0v.y) * mInvFactorY;
    ASSERT(dx >= 0 && dx <= 1.f);
    ASSERT(dy >= 0 && dy <= 1.f);

    // determine the 2 vectors that define the triangle to calculate the normal
    // later as cross product of a x b
    VEC_TYPE a;
    VEC_TYPE b;
    if (dx > dy) {
        // we are in triangle 2
        const VEC_TYPE p1v((cellX+1) * mCellSizeX, cellY * mCellSizeY, mData[p1]);
        a = p1v - p0v;
        b = p3v - p0v;
    } else {
        // we are in triangle 1
        const VEC_TYPE p2v(cellX * mCellSizeX, (cellY+1) * mCellSizeY, mData[p2]);
        a = p3v - p0v;
        b = p2v - p0v;
    }

    // calculate normal
    normal.x = a.y * b.z - a.z * b.y,
    normal.y = a.z * b.x - a.x * b.z,
    normal.z = a.x * b.y - a.y * b.x;

    normal.normalise();
    const float d = -(p0v.x * normal.x + p0v.y * normal.y + p0v.z * normal.z);
    return (-normal.x * realPos.x - normal.y * realPos.y - d) / normal.z;
}


} /* namespace gps */
#endif /* HEIGHTMAP_H_ */
