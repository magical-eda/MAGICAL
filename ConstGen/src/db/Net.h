/*! @file db/Net.h
    @brief Net class.
    @author Mingjie LIu
    @date 11/24/2018
*/
#ifndef __NET_H__
#define __NET_H__

#include <string>
#include <vector>
#include "global/type.h"

PROJECT_NAMESPACE_BEGIN
/*! @class Net
    @brief Net class
*/
class Net
{
public:
/*! Default constructor. */
    explicit Net() = default;
/*! @brief Constructor of Net.
    @param name Name of Net.
    @param id Id of Net.
*/
    explicit Net(const std::string &name, IndexType id) 
        : _name(name), _id(id)
    {}    

    // Getters
/*! Return name of Net. */
    const std::string &             name() const        { return _name; }
/*! Return Id of Net. */
    IndexType                       id() const          { return _id; }
/*! Return index array of connected pins. */
    const std::vector<IndexType> &  pinIdArray() const  { return _pinIdArray; }
    
    // Setters
/*! Connect a pin to the net. */
    void                        addPinId(IndexType pinId)   { _pinIdArray.push_back(pinId); } 
/*! @brief Return net type.
    @see #NetType.
*/
    NetType                     netType() const; 
    
private:
    std::string                 _name;
    IndexType                   _id;
    std::vector<IndexType>      _pinIdArray;
};

PROJECT_NAMESPACE_END

#endif
