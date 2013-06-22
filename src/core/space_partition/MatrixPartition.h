/*
 * MatrixPartition.h
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#ifndef MATRIXPARTITION_H_
#define MATRIXPARTITION_H_

#include <vector>

#include <types/basics.h>
#include <math/AABB.h>
#include <math/Vec2.h>
#include <debug/DebugUtil.h>

namespace core {

template <typename CellType>
class MatrixPartition
{
public:
    MatrixPartition() {};
    ~MatrixPartition() {};

    // @brief Set the size of the matrix. If we previously created the matrix
    //        we will remove all the data and initialize the new one
    // @param numColumns    The number of columns to use
    // @param numRows       The number of rows to use
    // @param aabb          The space of the bounding box we want to map
    //
    inline void
    construct(core::size_t numColumns, core::size_t numRows, const AABB& aabb);

    // @brief Return the number of rows and columns
    //
    inline core::size_t
    numColumns(void) const;
    inline core::size_t
    numRows(void) const;

    // @brief Return the bounding box that this matrix is mapping ("world")
    //
    inline const AABB&
    boundingBox(void) const;

    // @brief Get the index for an specific row and column
    // @param row   The row
    // @param col   The column
    //
    inline core::size_t
    getIndex(core::size_t row, core::size_t col) const;

    // @brief Get the cell of from an specific row and column or from an index
    // @param row   The row
    // @param col   The column
    //
    inline CellType&
    getCell(core::size_t row, core::size_t col);
    inline const CellType&
    getCell(core::size_t row, core::size_t col) const;
    inline CellType&
    getCell(core::size_t index);
    inline const CellType&
    getCell(core::size_t index) const;

    // @brief Check if a position is inside the matrix or not
    // @param position  The position to be checked
    //
    inline bool
    isPositionInside(const Vector2& position) const;

    // @brief Get a cell from a position (the position must be inside the boundingBox
    //        of this matrix). Check that before calling this method. If not
    //        the position will be clamped to the bounding
    // @param position  The position that will be mapped into a cell
    // @return the associated cell
    //
    inline const CellType&
    getCell(const Vector2& position) const;
    inline CellType&
    getCell(const Vector2& position);

private:
    AABB mBoundingBox;
    core::size_t mNumRows;
    core::size_t mNumColumns;
    float mInvXFactor;
    float mInvYFactor;
    std::vector<CellType> mMatrix;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template<typename CellType>
inline void
MatrixPartition<CellType>::construct(core::size_t numColumns, core::size_t numRows, const AABB& aabb)
{
    mMatrix.clear();
    mMatrix.resize(numColumns * numRows);

    mNumColumns = numColumns;
    mNumRows = numRows;
    mBoundingBox = aabb;

    // calculate inv factors
    const float worldWidth = aabb.getWidth();
    const float worldHeight = aabb.getHeight();
    mInvYFactor = static_cast<float>(numRows) / worldHeight; // = 1 / YCellSize
    mInvXFactor = static_cast<float>(numColumns) / worldWidth; // 1 / XCellSize

}

template<typename CellType>
inline core::size_t
MatrixPartition<CellType>::numColumns(void) const
{
    return mNumColumns;
}
template<typename CellType>
inline core::size_t
MatrixPartition<CellType>::numRows(void) const
{
    return mNumRows;
}

template<typename CellType>
inline const AABB&
MatrixPartition<CellType>::boundingBox(void) const
{
    return mBoundingBox;
}

template<typename CellType>
inline core::size_t
MatrixPartition<CellType>::getIndex(core::size_t row, core::size_t col) const
{
    return mNumColumns * row + col;
}

template<typename CellType>
inline CellType&
MatrixPartition<CellType>::getCell(core::size_t index)
{
    ASSERT(index < mMatrix.size());
    return mMatrix[index];
}
template<typename CellType>
inline const CellType&
MatrixPartition<CellType>::getCell(core::size_t index) const
{
    ASSERT(index < mMatrix.size());
    return mMatrix[index];
}

template<typename CellType>
inline CellType&
MatrixPartition<CellType>::getCell(core::size_t row, core::size_t col)
{
    return getCell(getIndex(row, col));
}
template<typename CellType>
inline const CellType&
MatrixPartition<CellType>::getCell(core::size_t row, core::size_t col) const
{
    return getCell(getIndex(row, col));
}

template<typename CellType>
inline bool
MatrixPartition<CellType>::isPositionInside(const Vector2& position) const
{
    return mBoundingBox.checkPointInside(position);
}

template<typename CellType>
inline const CellType&
MatrixPartition<CellType>::getCell(const Vector2& position) const
{
    // translate positions inside of our coordinate system and multiply by the
    // factor to get the index directly
    const core::size_t row = position.x <= mBoundingBox.tl.x ? 0 :
        position.x >= mBoundingBox.br.x ? mNumRows - 1 :
            static_cast<core::size_t>((position.x - mBoundingBox.tl.x) * mInvXFactor);
    const core::size_t col = position.y >= mBoundingBox.tl.y ? mNumRows -1 :
        position.y <= mBoundingBox.br.y ? 0 :
            static_cast<core::size_t>((position.y - mBoundingBox.br.y) * mInvYFactor);

    ASSERT(row < mNumRows);
    ASSERT(col < mNumColumns);

    return getCell(row, col);
}

template<typename CellType>
inline CellType&
MatrixPartition<CellType>::getCell(const Vector2& position)
{
    // translate positions inside of our coordinate system and multiply by the
    // factor to get the index directly
    const core::size_t row = position.x <= mBoundingBox.tl.x ? 0 :
        position.x >= mBoundingBox.br.x ? mNumRows - 1 :
            static_cast<core::size_t>((position.x - mBoundingBox.tl.x) * mInvXFactor);
    const core::size_t col = position.y >= mBoundingBox.tl.y ? mNumRows -1 :
        position.y <= mBoundingBox.br.y ? 0 :
            static_cast<core::size_t>((position.y - mBoundingBox.br.y) * mInvYFactor);

    ASSERT(row < mNumRows);
    ASSERT(col < mNumColumns);

    return getCell(row, col);
}

} /* namespace core */
#endif /* MATRIXPARTITION_H_ */
