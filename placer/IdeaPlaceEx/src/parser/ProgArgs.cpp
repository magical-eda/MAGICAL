#include <boost/algorithm/string.hpp> /// boost:iequals
#include "ProgArgs.h"

PROJECT_NAMESPACE_BEGIN

/* Command Line member function */


/// Construct and parse command line options
CommandLineOptions::CommandLineOptions(int argc, char** argv)
{
    // Define options 
    // 1st argument is long name
    // 2nd argument is short name (no short name if '\0' specified)
    // 3rd argument is description
    // 4th argument is mandatory (optional. default is false)
    // 5th argument is default value  (optional. it used when mandatory is false)

    // 6th argument is extra constraint.
    //     example: cmdline::range(1, 65535)
    //              cmdline::oneof<std::string>("http", "https", "ssh", "ftp")
    _parser.add <std::string> ("pin", '\0', ".pin file", false);
    _parser.add <std::string> ("netwgt", '\0', ".netwgt file", false);
    _parser.add <std::string> ("connection", '\0', ".connection file", false);
    _parser.add <std::string> ("techsimple", '\0', "techsimple file", false);
    _parser.add <std::string> ("sym", '\0', ".sym file", false);
    _parser.add <std::string> ("log", '\0', "log file", false, "");

    // boolean options don't need template
    //_parser.add               ("mute", '\0', "mute screen output");

    // Parse and check command line
    // It returns only if command line arguments are valid.
    // If arguments are invalid, a parser output error msgs then exit program.
    // If help flag ('--help' or '-?') is specified, a parser output usage message then exit program.
    _parser.parse_check(argc, argv);

    populateAndCheckOptions();
}


/*! Check command line arguments and update options for regular parsing
 */
void CommandLineOptions::populateAndCheckOptions()
{
    // Populate options
    pinFile        = _parser.get<std::string>("pin");
    netwgtFile     = _parser.get<std::string>("netwgt");
    connectionFile = _parser.get<std::string>("connection");
    techsimpleFile = _parser.get<std::string>("techsimple");
    symFile = _parser.get<std::string>("sym");
    log            = _parser.get<std::string>("log");
    for (const auto &rest : _parser.rest())
    {
        gdsFiles.emplace_back(rest);
    }
    //mute = _parser.exist("mute");
}


namespace ProgArgsDetails
{
    ProgArgs parseProgArgsCMD(IntType argc, char** argv)
    {
        CommandLineOptions opt(argc, argv);
        ProgArgs progArgs;
        progArgs.setPinFile(opt.pinFile);
        progArgs.setNetwgtFile(opt.netwgtFile);
        progArgs.setConnectionFile(opt.connectionFile);
        progArgs.setTechsimpleFile(opt.techsimpleFile);
        progArgs.setSymFile(opt.symFile);
        progArgs.gdsFiles() = opt.gdsFiles;

        return progArgs;
    }
}

PROJECT_NAMESPACE_END
