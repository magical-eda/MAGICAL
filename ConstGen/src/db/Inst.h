/*! @file db/Inst.h
    @brief Instance class.
    @author Mingjie Liu
    @date 11/24/2018
*/
#ifndef __INST_H__
#define __INST_H__

#include <string>
#include <vector>
#include "global/type.h"

PROJECT_NAMESPACE_BEGIN

/*! @class Inst
    @brief Inst class
*/
class Inst
{
public:
/*! @brief Default constructor */
    explicit Inst() = default;
/*! @brief Constructor for Inst.
    
    Constructor for netlist instances that does not have
    width and length attributes.

    @param name Name of Inst.
    @param type Type of Inst. Member of InstType. @see type.h
    @param id Id of Inst.
*/
    explicit Inst(const std::string &name, InstType type, IndexType id) 
        : _name(name), _type(type), _id(id) 
    {}
/*! @brief Constructor for Inst

    Constructor for netlist instances that have width and
    length attributes.

    @param name Name of Inst.
    @param type Type of Inst. Member of InstType.
    @param id Id of INst.
    @param wid Width of Inst.
    @param len Length of Inst.
    @param nf Number fingers of Inst.
*/
    explicit Inst(const std::string &name, InstType type, IndexType id, RealType wid, RealType len, RealType nf) 
        : _name(name), _type(type), _id(id), _wid(wid), _len(len), _nf(nf)
    {}

    // Getters
/*! Return name of Inst. */
    const std::string &             name() const                        { return _name; }
/*! @brief Return type of Inst. 
    @see #InstType
*/
    InstType                        type() const                        { return _type; } 
/*! @brief Return Id of Inst. */
    IndexType                       id() const                          { return _id; }
/*! @brief Return the index array for pins of the Inst. */
    const std::vector<IndexType> &  pinIdArray() const                  { return _pinIdArray; }
/*! @brief Return width of Inst. */
    RealType                        wid() const                         { return _wid; }
/*! @brief Return length of Inst. */
    RealType                        len() const                         { return _len; }
/*! @brief Return number fingers of Inst. */
    RealType                        nf() const                          { return _nf; }

    // Setters
/*! @brief Add pin index to Inst.
    @param pinId Added pin Id.
*/
    void                            addPinId(IndexType pinId)           { _pinIdArray.push_back(pinId); }
/*! @brief Assign width of Inst. */
    void                            setWid(RealType wid)                { _wid = wid; }
/*! @brief Assign length of Inst. */
    void                            setLen(RealType len)                { _len = len; }
/*! @brief Assign number fingers of Inst. */
    void                            setNf(RealType nf)                  { _nf = nf; }

private:
    std::string                     _name;
    InstType                        _type;
    IndexType                       _id;
    std::vector<IndexType>          _pinIdArray;                        
    RealType                        _wid;
    RealType                        _len;
    RealType                        _nf;
};


PROJECT_NAMESPACE_END

#endif
