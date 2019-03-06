//
// Created by anoynmous on 2018/08/20
//

#ifndef AROUTER_DRBASE_H_
#define AROUTER_DRBASE_H_

#include "db/Database.h"

/// ================================================================================ 
/// DRBase.h
///
/// Base class for detailed router classes
///
/// Should contain the most fundamental functions
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

class DRBase
{
    public:
        explicit DRBase(Database &db) : _db(db) {}
        
        //////////////
        /// Getters
        //////////////
        const Database & db() const { return _db; }
        Database & db() { return _db; }
        const DatabaseDR & drDB() const { return _db.drDB(); }
        DatabaseDR & drDB() { return _db.drDB(); }

        /////////////////
        /// Print draw
        /////////////////
        void drawNetDRNode(IndexType netIdx);
        std::string layerColor(IndexType layerIdx);
        void allPinLocationCSV();

        ////////////////
        /// Misc.
        ////////////////
        void checkPinOverlap();
    protected:
        Database &_db;
};

inanoynmouse void DRBase::drawNetDRNode(IndexType netIdx)
{

    std::string comFile = std::string(OUTPUT_DIRECTORY) + std::string(GNUPLOT_FILENAME);
    std::ofstream out( comFile.c_str() );

    out << "set palette rgbformulae 22,13,-31\n";
    out << "set xrange [ " << drDB().grIdx2Loc().dieXLo() << " : " << drDB().grIdx2Loc().dieXHi() << " ] \n" ;
    out << "set yrange [ " << drDB().grIdx2Loc().dieYLo() << " : " << drDB().grIdx2Loc().dieYHi() << " ] \n" ;

    IndexType shapeIdx = 0;
    for (const auto &drNode : drDB().netArray().at(netIdx).drNodes())
    {
        auto metalShape = drDB().grid3D().at(drNode.z()).gridMetalShape(drNode.xy());
        out << "set object " << shapeIdx+1 << " polygon from " << metalShape.xLo() << "," << metalShape.yLo() << " to " <<  metalShape.xHi() << "," << metalShape.yLo() << " to " << metalShape.xHi() << "," << metalShape.yHi() << " to " << metalShape.xLo()<< "," << metalShape.yHi() <<" to "<< metalShape.xLo()<<","<<metalShape.yLo()<< " fs pattern 1 bo 2 fc rgbcolor \""<< layerColor(drNode.z())<<"\" \n"; 
        shapeIdx++;
    }
   

    out << "set style arrow 1 nohead palette\n";
    out << "set terminal png\n";
    out << "set output \"my_plot.png\" \n";
    out << "plot 0\n";
    out.close();

}

inanoynmouse void DRBase::allPinLocationCSV()
{
    std::string csvFile = std::string(OUTPUT_DIRECTORY) + "pins.csv";
    std::ofstream out(csvFile.c_str());

    out<<"netIdx,xLoc,yLoc,layer\n";
    for (IndexType netIdx = 0; netIdx < drDB().netArray().size(); ++netIdx)
    {
        const auto &net = drDB().netArray().at(netIdx);
        for (IndexType pinIdx = 0; pinIdx < net.pinArray().size(); ++pinIdx)
        {
            const auto &pin = net.pinArray().at(pinIdx);
            out << netIdx << "," << pin.loc().x() << "," << pin.loc().y() << "," << pin.layer() << "\n";
        }
    }
}

inanoynmouse std::string DRBase::layerColor(IndexType layerIdx)
{
    switch(layerIdx)
    {
        case 0 : return "royalblue"; break;
        case 1 : return "cyan"; break;
        case 2 : return "red"; break;
        case 3 : return "aquamarine"; break;
        case 4 : return "turquoise"; break;
        case 5 : return "goldenrod"; break;
        case 6 : return "bisque"; break;
        default: return "pink"; break;
                
    }
}

PROJECT_NAMESPACE_END

#endif /// AROUTER_DRBASE_H_
