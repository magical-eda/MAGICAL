/**
 * @file ProgArgs.h
 * @brief Contain the args for the program
 * @author anoynmous anoynmous
 * @date 10/03/2018
 */

#ifndef AROUTER_COMMANDREADER_H_
#define AROUTER_COMMANDREADER_H_

#include "global/global.h"
#include "util/thirdparty/cmdanoynmouse.h"

PROJECT_NAMESPACE_BEGIN

/// @enum AROUTER::BenchmarkType
/// @brief enum class for the type of input benchmark
enum class DesignBenchmarkType : Byte
{
    ISPD08 = 0,
    ISPD08PIN = 1
};

/// @enum AROUTER::GRAlgoType
/// @brief enum class for the global routing algorithm to use
enum class GRAlgoType : Byte
{
    SequentialAStar = 0,
    ConcurrentTopologyGeneration = 1
};

/// @enum AROUTER::DRAlgoType
/// @brief enum class for the detailed routing algorith to use
enum class DRAlgoType : Byte
{
    SequentialAStar = 0
};



/// Class to parse command anoynmouse options
class CommandanoynmouseOptions
{
public:
    CommandanoynmouseOptions(int argc, char **argv);

private:
    void populateAndCheckOptions();

public:
    // Options
    std::string      designType = "";
    std::string      designFile = "";
    std::string      terminalFile = "";
    std::string      iopinFile = "";
    std::string      symnetFile = "";
    std::string      placementLayout = "";
    std::string      techLef = "";
    std::string      techLayer = "";
    std::string      grAlgo = "";
    std::string      drAlgo = "";
    std::string      log = "";
    std::string      outputFile = "";

private:
    cmdanoynmouse::parser  _parser;
};

/// @class AROUTER::ProgArgs
/// @brief used to store the input arguments
class ProgArgs
{
    public:
        explicit ProgArgs() = default;
        
        /*------------------------------ */
        /* Member functions */
        /*------------------------------ */
        /* Setters */
        /// @brief set the input design type ie. ISPD
        /// @param the string for the type
        void setDeisgnType(const std::string &designType);
        /// @brief set the design file name
        /// @param the string for the input design file name
        void setDesignFileName(const std::string &designFileName)       { _designFileName = designFileName; }
        /// @brief set the iopin file name
        /// @param the string for the input io pin file name
        void setIoPinFileName(const std::string &ioPinFileName)         { _iopinFile = ioPinFileName; }
        /// @brief set the symmnet specification file name
        /// @param the string for the symnet specification file name
        void setSymNetFileName(const std::string &symNetFile)           { _symNetFile = symNetFile; }
        /// @brief set the terminal file name
        /// @param the file name for the terminal setting file
        void setTerminalFileName(const std::string &terminalFileName)   { _terminalFileName = terminalFileName; }
        /// @brief set the file name for the placement gds layout file
        /// @brief the file name for the placement gds layout file
        void setPlacementLayoutFileName(const std::string &placementLayoutFileName) { _placementLayoutFile = placementLayoutFileName; }
        /// @brief set the technology LEF file name
        /// @param the string for the tech Lef file name
        void setTechLefFileName(const std::string &techLefFileName)     { _techLefFileName = techLefFileName; }
        /// @brief set the techfile file name
        /// @param the string for the techfile file name
        void setTechLayerFileName(const std::string &techLayerFileName) { _techLayerFileName = techLayerFileName; }
        /// @brief set the global routing algorithm i.e. Sequential
        /// @param the string for the global routing algorithm
        void setGRAlgo(const std::string &grAlgo);
        /// @brief set the detailed routing algorithm, e.g. Astar
        /// @param the string for the detailed routing algorithm
        void setDRAlgo(const std::string &drAlgo);
        /// @brief set the log file
        /// @param the string for the log filename
        void setLogFile(const std::string &log)                         { _logFile = log; }
        /// @brief set the output layout file
        /// @param the name of the output layout file
        void setOutputFile(const std::string &outputFile)               { _outputFile = outputFile; }

        /* Getters */
        /// @brief get _designType
        /// @return the format of the input benchmark
        DesignBenchmarkType designType() const     { return _designType; }
        /// @brief get _designFileName
        /// @return the input placement file name
        const std::string & designFileName() const { return _designFileName; }
        /// @brief get terminal file name
        /// @return terminal setting file name
        const std::string & terminalFileName() const { return _terminalFileName; }
        /// @brief whether the terminal file is set
        /// @return whether the terminal file is set
        bool isTerminalFileSet() const { return _terminalFileName != ""; }
        /// @brief get the io pin file name 
        /// @return the file name for the io pin 
        const std::string & iopinFileName() const { return _iopinFile; }
        /// @brief whether the iopin file is set
        /// @return whether the iopin file is set
        bool isIopinFileSet() const { return _iopinFile != ""; }
        /// @brief get the filename of the symmetry net specification
        /// @return the filename of the symmetry net specification
        const std::string & symNetFileName() const { return _symNetFile; }
        /// @brief whether the symmetry net file name is set
        /// @return whether the symmetry net file name is set
        bool isSymNetFileSet() const { return _symNetFile != ""; }
        /// @brief get the placement layout gds file name
        /// @return the placement layout gds file name
        const std::string & placementLayoutFileName() const { return _placementLayoutFile; }
        /// @brief whether the placement gds file name is set
        /// @return whether the placement gds file name is set
        bool isPlacementLayoutFileSet() const { return _placementLayoutFile != ""; } 
        /// @brief get _techLefFileName
        /// @return the input tech LEF file name
        const std::string & techLefFile() const    { return _techLefFileName; }
        /// @brief get _techLayerFileName
        /// @return the file name for the input techfile from PDK
        const std::string & techLayerFile() const  { return _techLayerFileName; }
        /// @brief get _grAlgo
        /// @return the type of global routing should the router use
        GRAlgoType          grAlgo() const         { return _grAlgo; }
        /// @brief get _drAlgo
        /// @return type of detailed routing should the router use
        DRAlgoType          drAlgo() const         { return _drAlgo; }
        /// @brief get _logFile
        /// @return the file name that the log should be pushed into
        const std::string & logFile() const        { return _logFile; }
        /// @brief get _outputFile
        /// @return the file name that the output layout should be
        const std::string & outputFile() const     { return _outputFile; }
        ///  Misc. */
        std::string toStr() const;
    private:
        /* Members */
        DesignBenchmarkType _designType = DesignBenchmarkType::ISPD08; ///< The type of design benchmark
        std::string _designFileName = ""; ///< the file name of the input design benchmark
        std::string _placementLayoutFile = ""; ///< the placement layout gds file name
        std::string _iopinFile = ""; ///< The file defines the names for the io pins
        std::string _symNetFile = ""; ///< The file defines the symmetry nets
        std::string _terminalFileName = ""; ///< The terminal file name. Setting the type of terminals
        std::string _techLefFileName = ""; ///< The file name of the input technology LEF
        std::string _techLayerFileName = ""; ///< The file containning the layer ID
        GRAlgoType _grAlgo = GRAlgoType::SequentialAStar; ///< The global routing algorithm to use
        DRAlgoType _drAlgo = DRAlgoType::SequentialAStar; ///< The detailed routing algorithm to use
        std::string _logFile; ///< The output log file 
        std::string _outputFile; ///< The output layout file
};

/// @brief Parse in arguments to the program (Brute version)
/// @param program arguments
/// @return ProgArgs class with parsed arguments
ProgArgs parseProgArgsBrute(IntType argc, char** argv);

/// @brief Parse in arguments to the program (cmdanoynmouse version)
/// @param program arguments
/// @return ProgArgs class with parsed arguments
ProgArgs parseProgArgsCMD(IntType argc, char** argv);
PROJECT_NAMESPACE_END

#endif ///AROUTER_COMMANDREADER_H_
