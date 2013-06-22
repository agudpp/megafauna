/*
 * TriggerSystemDefines.h
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#ifndef TRIGGERSYSTEMDEFINES_H_
#define TRIGGERSYSTEMDEFINES_H_

namespace core {

// The number of colors we will use
#define TS_NUM_COLORS       32

typedef core::uint32_t TriggerColor_t;



// Define the Possible error codes for this module
//
enum TriggerCode {
    Ok = 0,                 // means no error
    UnreachablePosition,    // for when the agent is outside of any possible
                            // zone
    BigJump,                // for when the agent did a big jump
    AlreadyInitialized,     // error, we are trying to init twice the same agent
};

}

#endif /* TRIGGERSYSTEMDEFINES_H_ */
