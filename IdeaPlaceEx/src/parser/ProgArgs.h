/**
 * @file ProgArgs.h
 * @brief Parsing in the program arguments from system arguments
 * @author Keren Zhu
 * @date 10/04/2019
 */

#ifndef IDEAPLACE_PROGARGS_H_
#define IDEAPLACE_PROGARGS_H_

#include "global/global.h"
#include "util/thirdparty/cmdline.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::CommandLineOptions
/// @brief Class to parse command line options
class CommandLineOptions
{
public:
    CommandLineOptions(int argc, char **argv);

private:
    void populateAndCheckOptions();

public:
    // Options
    std::string pinFile = "";
    std::string netwgtFile = "";
    std::string connectionFile = "";
    std::string techsimpleFile = "";
    std::string symFile = "";
    std::string symnetFile = "";
    std::string sigpathFile = "";
    std::string log = "";
    std::vector<std::string> gdsFiles;

private:
    cmdline::parser  _parser;
};

/// @class IDEAPLACE::ProgArgs
/// @brief The program arguments
class ProgArgs
{
    public:
        /// @brief default constructor
        explicit ProgArgs() = default;
        /*------------------------------ */
        /* Getters                       */
        /*------------------------------ */
        /// @brief get the .pin file file name
        /// @return the .pin file file name
        const std::string pinFile() const { Assert(this->pinFileIsSet()); return _pinFile; }
        /// @brief determine whether the .pin file is set
        /// @return whether .pin file is set
        bool pinFileIsSet() const { return _pinFile != ""; }
        /// @brief get the .netwgt file file name
        /// @return the .netwgt file file name
        const std::string netwgtFile() const { Assert(this->netwgtFileIsSet()); return _netwgtFile; }
        /// @brief determine whether the .netwgt file is set
        /// @return whether .netwgt file is set
        bool netwgtFileIsSet() const { return _netwgtFile != ""; }
        /// @brief get the .connection file file name
        /// @return the .connection file file name
        const std::string connectionFile() const { Assert(this->connectionFileIsSet()); return _connectionFile; }
        /// @brief determine whether the .connection file is set
        /// @return whether .connection file is set
        bool connectionFileIsSet() const { return _connectionFile != ""; }
        /// @brief get the .connection file file name
        /// @return the .connection file file name
        const std::string symFile() const { Assert(this->symFileIsSet()); return _symFile; }
        /// @brief determine whether the .sym file is set
        /// @return whether .sym file is set
        bool symFileIsSet() const { return _symFile != ""; }
        /// @brief get the .symnet file
        /// @return the .symnet file
        const std::string symnetFile() const { Assert(this->symnetFileIsSet()); return _symnetFile; }
        /// @brief determine whether the .symnet file is set
        /// @return whether .symnet file is set
        bool symnetFileIsSet() const { return _symnetFile != ""; }
        /// @brief get the .sigpath file
        /// @return the .sigpath file
        const std::string sigpathFile() const { Assert(this->sigpathFileIsSet()); return _sigpathFile; }
        /// @brief determine whether the .sigpath file is set
        /// @return whether .sigpath file is set
        bool sigpathFileIsSet() const { return _sigpathFile != ""; }
        /// @brief get the techsimple file file name
        /// @return the techsimple file file name
        const std::string techsimpleFile() const { Assert(this->techsimpleFileIsSet()); return _techsimpleFile; }
        /// @brief determine whether the techsimple file is set
        /// @return whether techsimple file is set
        bool techsimpleFileIsSet() const { return _techsimpleFile != ""; }
        /// @brief get the gds files given from the arguments
        /// @return the gds files
        const std::vector<std::string> & gdsFiles() const { return _gdsFiles; }
        /// @brief get the gds files given from the arguments
        /// @return the gds files
        std::vector<std::string> & gdsFiles() { return _gdsFiles; }
        /*------------------------------ */
        /* Setters                       */
        /*------------------------------ */
        /// @brief set the .pin file
        /// @param the .pin file file name
        void setPinFile(const std::string &pinFile) { _pinFile = pinFile; }
        /// @brief set the .sym file
        /// @param the .sym file file name
        void setSymFile(const std::string &symFile) { _symFile = symFile; }
        /// @brief set the .netwgt file
        /// @param the .netwgt file file name
        void setNetwgtFile(const std::string &netwgtFile) { _netwgtFile = netwgtFile; }
        /// @brief set the .connection file
        /// @param the .connection file file name
        void setConnectionFile(const std::string &connectionFile) { _connectionFile = connectionFile; }
        /// @brief set the techsimple file
        /// @param the techsimple file file name
        void setTechsimpleFile(const std::string &techsimpleFile) { _techsimpleFile = techsimpleFile; }
        /// @brief set the symnet file
        /// @param the symnet file file name
        void setSymnetFile(const std::string &symnetFile) { _symnetFile = symnetFile; }
        /// @brief set the sigpath file
        /// @param the sigpath file file name
        void setSigpathFile(const std::string &sigpathFile) { _sigpathFile = sigpathFile; }
    private:
        std::string _pinFile = ""; ///< .pin file
        std::string _netwgtFile = ""; ///< .netwgt file
        std::string _connectionFile = ""; ///< .connection file
        std::string _techsimpleFile = ""; ///< The techfile simple file
        std::string _symFile = ""; ///< The .sym file
        std::string _symnetFile = ""; ///< The .symnet file
        std::string _sigpathFile = ""; ///< The .sigpath file
        std::vector<std::string> _gdsFiles; ///< The gds files for read
};

namespace ProgArgsDetails
{
/// @brief Parse in arguments to the program (cmdline version)
/// @param program arguments
/// @return ProgArgs class with parsed arguments
ProgArgs parseProgArgsCMD(IntType argc, char** argv);
}
PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PROGARGS_H_
