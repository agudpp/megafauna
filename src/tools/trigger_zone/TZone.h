/*
 * TZone.h
 *
 *  Created on: Jul 13, 2013
 *      Author: agustin
 */

#ifndef TZONE_H_
#define TZONE_H_

#include <OgreColourValue.h>

#include <trigger_system/TriggerZone.h>
#include <utils/SelectionHelper.h>


// forward
//
namespace core {
struct Primitive;
}

namespace tool {

class TZone : public SelectableObject {
public:
    // Constructor / destructor
    //
    TZone(const core::TriggerZone& zone, const Ogre::ColourValue& color);
    virtual ~TZone();

    // @brief Return the associated TriggerZone
    //
    inline const core::TriggerZone&
    triggerZone(void) const;


    ////////////////////////////////////////////////////////////////////////////


    // @brief Method called when we pass the mouse over the object
    //
    virtual void
    mouseOver(void);

    // @brief Method called when the mouse is out of the object
    //
    virtual void
    mouseExit(void);

    // @brief Method called when the object is clicked (or selected)
    // @param type  Determines the selection type (button of the mouse)
    // @return true if the object should be selected or false if not. if we
    //         return false the object will be not took into account as if was
    //         selected (we will never call objectUnselected()). If we return true
    //         we will track it as a selected object.
    //
    virtual bool
    objectSelected(SelectType type);

    // @brief Method called when the object is unselected
    //
    virtual void
    objectUnselected(void);

private:
    core::TriggerZone mTriggerZone;
    core::Primitive* mPrimitive;
    bool mSelected;

};


////////////////////////////////////////////////////////////////////////////////
//

inline const core::TriggerZone&
TZone::triggerZone(void) const
{
    return mTriggerZone;
}

}

#endif /* TZONE_H_ */
