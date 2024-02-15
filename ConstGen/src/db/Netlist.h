/*! @file db/Netlist.h
    @brief Netlist class
    @author Mingjie Liu
    @date 11/24/2018
*/
#ifndef __NETLIST_H__
#define __NETLIST_H__

#include <vector>
#include <string>
#include "global/type.h"
#include "db/Net.h"
#include "db/Pin.h"
#include "db/Inst.h"

PROJECT_NAMESPACE_BEGIN
/*! @class Netlist
    @brief Netlist class
*/
class Netlist
{
public:
/*! @struct InitNet
    @brief Net for instantiation.
*/
    struct InitNet
    {
        std::string                 name;
        IndexType                   id = INDEX_TYPE_MAX;
    };
/*! @struct InitInst
    @brief Inst for instantiation.
*/
    struct InitInst
    {
        InstType                    type = InstType::OTHER;
        std::vector<IndexType>      netIdArray;
        std::string                 name; 
        RealType                    wid = 0, len = 0, nf = 1;
    };
/*! @struct InitDataObj
    @brief Instantiate Netlist class
    @see init(InitDataObj &).
*/
    struct InitDataObj
    {
        std::vector<InitNet>        netArray;
        std::vector<InitInst>       instArray;
    };

public:
/*! @brief Default Constructor */
    explicit Netlist() = default;
    
/*! @brief Initialize Netlist class. */
    void                            init(InitDataObj &obj);

/*! @brief Add a net to netlist. */
    IndexType                       addNet(const std::string name, IndexType netIdx);

/*! @brief Add a instance to netlist. */
    IndexType                       addInst(const std::string name, InstType type, RealType wid, RealType len, RealType nf);

/*! @brief Add a pin to inst 
    @param Index of inst in _instArray
    @param Index of net connected to pin.
    @param PinType of added pin.
*/
    void                            addInstPin(IndexType instIdx, IndexType netIdx, PinType pinType);

/*! Print netlist. */
    void                            print_all() const;

/*! @brief Return true if InstType is a Mosfet.
    NMOS and PMOS are Mosfets.
*/
    bool                            isMos(InstType instType) const;
/*! @brief Return true if InstType is passive device.
    RES and CAP are passive devices.
*/
    bool                            isPasvDev(InstType instType) const;

/*! @brief Return true if corresponding net NetType::Signal. */
    bool                            isSignal(IndexType netId) const     { return _netArray[netId].netType() == NetType::SIGNAL; }

/*! @brief Return MosType of corresponding instance id. */
    MosType                         mosType(IndexType mosId) const;
/*! @brief Return Id of Net connected to Inst by certain PinType.

    Example: instNetId(0, PinType::DRAIN) would return the net index 
    connected to inst[0] through a pin which PinType::DRAIN. Or this 
    returns inst[0] drain net. If the Inst does not have a PinType 
    connected, INDEX_TYPE_MAX would be returned. Use at risk and 
    only if InstType is known. 

    @param instId Id of Inst.
    @param pinType Returned Net Id connected to this PinType.
*/
    IndexType                       instNetId(IndexType instId, PinType pinType) const;
/*! @brief Return Id of Pin with PinType connected to Inst.

    Example: instPinId(0,PinType::DRAIN) would return the pin index
    connected to inst[0] which is PinType::DRAIN. Or this returns
    inst[0] drain pin index. If Inst does not have a PinType connected,
    INDEX_TYPE_MAX would be returned. Use at risk and only if InstType 
    is known.

    @param instId Id of Inst.
    @param pinType Returned Pin Id should be this PinType.
*/
    IndexType                       instPinId(IndexType instId, PinType pinType) const;

/*! @brief Return Source Net Id of Inst mosId.
    Equivalent as instNetId(mosId, PinType::SOURCE);
    @see instNetId.
*/
    IndexType                       srcNetId(IndexType mosId) const     { return instNetId(mosId, PinType::SOURCE); }
/*! @brief Return Drain Net Id of Inst mosId.
    Equivalent as instNetId(mosId, PinType::DRAIN);
    @see instNetId
*/
    IndexType                       drainNetId(IndexType mosId) const   { return instNetId(mosId, PinType::DRAIN); }
/*! @brief Return Gate Net Id of Inst mosId.
    Equivalent as instNetId(mosId, PinType::GATE);
    @see instNetId.
*/
    IndexType                       gateNetId(IndexType mosId) const    { return instNetId(mosId, PinType::GATE); }

/*! @brief Get PinType of a pin such that Inst and Pin are connected through this pin.

    Example: Suppose pin[0] of inst[1] is connected to pin[2]
    (through some net). getPinTypeInstPinConn(1,2) would return 
    PinType of pin[0]. This function allows us to querry for 
    connection types and determine future search directions.

    By definition this pin must belong to instId and be connected
    to pinId through some net. If no such pin exists PinType::OTHER is returned.

    @param instId Id of Inst that returned pin is connected.
    @param pinId Id of Pin that returned pin is connected.
*/
    PinType                         getPinTypeInstPinConn(IndexType instId, IndexType pinId) const;
/*! @brief Get PinType of a pin such that Inst and Net are connected through this pin.

    Example: Suppose pin[0] of inst[1] is connected to net[2].
    getPinTypeInstNetConn(1,2) would return PinType of pin[0].
    This function allows us to querry for connection types 
    and determine future search directions.

    By definition this pin must belong to instId and be connected
    to netId. If no such pin exists PinType::OTHER is returned.

    @param instId Id of Inst that returned pin is connected.
    @param netId Id of Net that returned pin is connected.
*/
    PinType                         getPinTypeInstNetConn(IndexType instId, IndexType netId) const;

/*! @brief Get all Inst that are connected to netId.

    @param[out] instArray Array of the returned Inst Id.
    @param[in] netId Id of net.
*/
    void                            getInstNetConn(std::vector<IndexType> & instArray, IndexType netId) const;
/*! @brief Get all Inst that are connected to pinId(through some net).
    
    The instance that pinId itself belongs to is not returned.

    @param[out] instArray Array of the returned Inst Id.
    @param[in] pinId Id of pin.
*/
    void                            getInstPinConn(std::vector<IndexType> & instArray, IndexType pinId) const;

/*! @brief Remove from array, Inst that has pinId.

    O(n) complexity guaranteed. Similar implementation of std::remove().
    
    @param instArray Reference to instance Id array.
    @param pinId Id of pin.
*/
    void                            rmvInstHasPin(std::vector<IndexType> & instArray, IndexType pinId) const;

/*! @brief Filter instArray. Remove Inst that are connected to pinId through connPinType.

    Removed instId if getPinTypeInstPinConn(instId, pinId) == connPinType.
    O(n) complexity. Similar implementation of std::remove().
    
    @see getPinTypeInstPinConn.
*/
    void                            fltrInstPinConnPinType(std::vector<IndexType> & instArray, IndexType pinId, PinType connPinType) const;
/*! @brief Filter instArray. Remove Inst that are connected to netId through connPinType.

    Removed instId if getPinTypeInstNetConn(instId, pinId) == connPinType.
    O(n) complexity. Similar implementation of std::remove().
    
    @see getPinTypeInstNetConn.
*/
    void                            fltrInstNetConnPinType(std::vector<IndexType> & instArray, IndexType netId, PinType connPinType) const;
/*! @brief Filter instArray. Remove Mosfet Inst whose type are not mosType.

    Removed instId if mosType(instId) != mosType. 
    O(n) complexity. Similar implementation of std::remove().
    
    @see getPinTypeInstNetConn.
*/
    void                            fltrInstMosType(std::vector<IndexType> & instArray, MosType mosType) const;
/*! @brief Filter instArray. Remove Inst whose type are not type.

    Removed instId if InstType of instance 
    is different from input type.
*/
    void                            fltrInstType(std::vector<IndexType> & instArray, InstType type) const;
   
    // Getters
/*! @brief Return Pin of Id. */
    const Pin &                     pin(IndexType id) const             { return _pinArray.at(id); }
/*! @brief Return Net of Id. */
    const Net &                     net(IndexType id) const             { return _netArray.at(id); }
/*! @brief Return Inst of Id. */
    const Inst &                    inst(IndexType id) const            { return _instArray.at(id); }
/*! @brief Return number of Pin. */
    IndexType                       numPin() const                      { return _pinArray.size(); }
/*! @brief Return number of Net. */
    IndexType                       numNet() const                      { return _netArray.size(); }
/*! @brief Return number of Inst. */
    IndexType                       numInst() const                     { return _instArray.size(); }

    // Setters
/*! @brief Add Pin to Netlist. */
    void                            addPin(Pin & pin)                   { _pinArray.push_back(pin); }
/*! @brief Add Net to Netlist. */
    void                            addNet(Net & net)                   { _netArray.push_back(net); }
/*! @brief Add Inst to Netlist. */
    void                            addInst(Inst & inst)                { _instArray.push_back(inst); }

private:    
    std::vector<Net>                _netArray;
    std::vector<Pin>                _pinArray;
    std::vector<Inst>               _instArray;
};

PROJECT_NAMESPACE_END

#endif
