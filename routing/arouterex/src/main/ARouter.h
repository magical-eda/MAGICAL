/**
 * @file GlobalRouter.h
 * @brief wrapper of Everything
 * @author anoynmous anoynmous
 * @date 10/08/2018
 */

#ifndef AROUTER_AROUTER_H_
#define AROUTER_AROUTER_H_

#include "grdr/IntraCellRouter.h"
#include "gr/GlobalRouter.h"
#include "dr/DetailedRouter.h"
#include "parser/GrDB2DrDB.h"
#include "writer/gdsii/WriteGdsLayout.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ARouter
/// @brief Wrapper of everything
class ARouter
{
    public:
        /// @brief default constructor
        explicit ARouter() = default;
        /// @brief start routing
        /// @param the arguments to the program
        void operator()(int argc, char** argv);
    private:
        /// @brief read the commands/ program arguments
        /// @param the arguments to the program
        bool readCMD(int argc, char** argv);
        /// @brief parsing in the files
        bool read();
        /// @brief read terminal file 
        /// @return if sucessful
        bool readTerminalFile();
        /// @brief read placement layout file
        /// @return if successful
        bool readPlacementLayout();
        /// @brief the core function
        /// @return if successful
        bool solve();
        /// @brief data preprocessing
        /// @return if succesful
        bool preprocessing();
        /// @brief the core function for global router
        /// @return if successful
        bool solveGR();
        /// @brief the core function for detailed router
        /// @return if successful
        bool solveDR();
        /// @brief convert the global routing result to detailed router
        /// @return if succesful
        bool convertGR2DR();
        /// @brief write output layout
        /// @return if successful
        bool writeGDS();


        /*------------------------------*/ 
        /* Heuristics                   */
        /*------------------------------*/ 
        /// @brief ordering the terminals in nets in ordering of decending bbox area
        void orderTerminalDecendingBBox();

    protected:
        Database _db;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_AROUTER_H_
