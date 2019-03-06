/**
 * @file ParseTerminalFile.h
 * @brief Parser for terminal file
 * @author anoynmous anoynmous
 * @date 12/10/2018
 */

#ifndef AROUTER_PARSER_TERMINAL_FILE_H_
#define AROUTER_PARSER_TERMINAL_FILE_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @brief the type of devices each pin belonging to
enum class TerminalDeviceType
{
    DEFAULT = 0,
    CMOS = 1
};

/// @class AROUTER::CmosTerminalPin
/// @brief the pin/terminal settings for a CMOS 
struct CmosTerminalPin
{
    public:
        explicit CmosTerminalPin() = default;
        std::vector<IndexType> gatePins; ///< The pin indices for the gate terminal
        std::vector<IndexType> drainPins; ///< The pin indices for the drain terminal
        std::vector<IndexType> sourcePins; ///< The pin indices for the source terminal
};

/// @class AROUTER::DefaultTerminalPin
/// @brief the pin/terminal settings for a default pin
struct DefaultTerminalPin
{
    public: 
        explicit DefaultTerminalPin() = default;
        IndexType pinIdx = INDEX_TYPE_MAX; ///< The default pin index for the terminal
};
/// @class AROUTER::ParseTerminalFile
/// @brief parser for the terminal setting file
class ParseTerminalFile
{
    public:
        explicit ParseTerminalFile(IndexType numPins);
        /// @brief read the file
        bool read(const std::string &fileName);
        /// @brief get the device types for the pins
        /// @return device type for each pin
        const std::vector<TerminalDeviceType> & deviceTypeArray() const { return _deviceTypeArray; }
        /// @brief get the pin to individual device type index
        /// @return the array specifying which device each pin corresponding to
        const std::vector<IndexType> & deviceIdxArray() const { return _deviceIdxArray; }
        /// @brief get the pin attributes index for each pin. eg. 0 for gate, 1 for drain, 2 for source
        /// @return the pin attributes index for each pin. eg. 0 for gate, 1 for drain, 2 for source
        const std::vector<IndexType> & attributesArray() const { return _terminalAttriArray; }
        /// @brief get the array for the default devices
        /// @return the array for the default devices
        const std::vector<DefaultTerminalPin> & defaultArray() const { return _default; }
        /// @brief get the array for the cmos
        /// @return the cmos
        const std::vector<CmosTerminalPin> & cmosArray() const { return _cmos; }
        /// @brief number of defaults
        /// @return number of defaults
        IndexType numDefault() const { return _numDefault; }
        /// @brief number of cmos
        /// @return number of cmos
        IndexType numCMOS() const { return _numCMOS; }
        
    private:
        std::vector<TerminalDeviceType> _deviceTypeArray; ///< The device type for each pin. size = number of pins
        std::vector<IndexType> _deviceIdxArray; ///< The indices for the pins in the individual device/terminal array
        std::vector<IndexType> _terminalAttriArray; ///< The type of attributes for each pin
        std::vector<DefaultTerminalPin> _default; ///< The pin/terminal settings for the default pin. NOT USED!
        std::vector<CmosTerminalPin> _cmos; ///< The pin/terminal settings for the cmos devices. NOT USED!
        IndexType _numDefault = 0;
        IndexType _numCMOS = 0;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_PARSER_TERMINAL_FILE_H_
