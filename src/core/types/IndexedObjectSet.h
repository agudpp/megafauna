/*
 * IndexedObjectSet.h
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#ifndef INDEXEDOBJECTSET_H_
#define INDEXEDOBJECTSET_H_

namespace core {


// @brief Here we will define the objects that should be used as base for the
//        indexed Object set..
//
typedef unsigned short IndexedObjectIDType;

template <typename CONTAINER>
struct IndexedObject
{
    IndexedObject() : indexedID(-1) {}
private:
    friend class CONTAINER;
    IndexedObjectIDType indexedID;   // to avoid collisions names..
};


// @brief For now the container cannot contain pointers, it should contain
//        real objects.

template<typename Container, typename Element>
struct IndexedObjectSet
{
    // @brief Check if an element exists in the container.
    // @param element   The element we want to check if exists or not in the
    //                  array.
    //
    inline bool
    exists(const Element& element) const;
};

} /* namespace core */
#endif /* INDEXEDOBJECTSET_H_ */
