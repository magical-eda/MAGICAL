/*! @file db/Pin.h
    @brief Pin class.
    @author Mingjie Liu
    @date 11/24/2018
*/
#ifndef __PIN_H__
#define __PIN_H__

#include <string>
#include "global/type.h"

PROJECT_NAMESPACE_BEGIN
/*! @class Pin
    @brief Pin class
*/
class Pin
{
public:
/*! Default Constructor. */
    explicit Pin() = default;
/*! @brief Constructor for Pin
    @param id Id of Pin.
    @param instId Id of connected Inst.
    @param netId Id of connected Net.
    @param type Type of Pin.
*/
    explicit Pin(IndexType id, IndexType instId, IndexType netId, PinType type)  
        : _id(id), _instId(instId), _netId(netId), _type(type)
    {}

    // Getters
/*! Return id of Pin. */
    IndexType                       id() const          { return _id; }
/*! Return id of connected Inst. */
    IndexType                       instId() const      { return _instId; }
/*! Return id of connected Net. */
    IndexType                       netId() const       { return _netId; }
/*! @brief Return type of Pin. 
    @see #PinType
*/
    PinType                         type() const        { return _type; }

    //No Setters

/*! @brief Return the next search PinType for DFS. */   
    static PinType                  nextPinType(PinType type);
/*! @breif Return true if PinType belongs to a passive device. */
    static bool                     isPasvDev(PinType type);

private:
    IndexType                       _id;
    IndexType                       _instId;
    IndexType                       _netId;          
    PinType                         _type;
};

PROJECT_NAMESPACE_END

#endif

