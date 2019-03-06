//
// Created by anoynmous 09/16/2018
// 

#ifndef AROUTER_WRITEGDSLAYOUT_H_
#define AROUTER_WRITEGDSLAYOUT_H_

#include <memory>
#include "db/Database.h"
#include "WriteGds.h"
#include "dr/ShapeConstructor.h"

// ================================================================================ 
// Write the layout to gds
//
// Wrapper of wrapper of gdsWriter in Limbo
//
// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

class WriteGdsLayout
{
    public:
        explicit WriteGdsLayout(Database &db) : _db(db), _shapeComp(db) {};

        /// @brief One function to call
        bool writeLayout(std::string outputFile);

        /// @brief Start drawing fake functions
        bool writeLayoutFake(std::string outputFile);


    public:
        bool writeNetLayout(IndexType netIdx);
        /// @brief write a shape for via or wire
        /// @param the shapeID of the via or wire
        /// @param whether to write the net label on the segment
        /// @return if successful
        bool writeShape(ShapeID shapeID, bool writeLabel);
        //bool writeFixedViaNodeLayout(const FixedViaNode &viaNode) const;
       // bool writeViaPlacedLayout(const ViaPlaced &via) const;
        /// @brief write pins to a layer (for debug)
        void writeDummyPins();
        /// @brief write the intra cell connection
        bool writeIntraCell();
        /// @brief write terminals and pins
        bool writePins();

    private:
        std::shared_ptr<WriteGds> _wg = NULL;
        Database &_db; 
        ShapeConstructor _shapeComp;
};
PROJECT_NAMESPACE_END

#endif ///AROUTER_WRITEGDSLAYOUT_H_
