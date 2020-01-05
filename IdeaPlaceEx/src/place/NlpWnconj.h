/**
 * @file NlpWnconj.h
 * @brief The "global placement" solver with non-linear optimization + wnlib conjugated gradient
 * @author Keren Zhu
 * @date 10/12/2019
 */

#ifndef IDEAPLACE_NLP_WNCONJ_H_
#define IDEAPLACE_NLP_WNCONJ_H_

#include "db/Database.h"
#include "wnconj.h"
#include <math.h>

PROJECT_NAMESPACE_BEGIN

namespace NlpWnconjDetails
{
    /// @brief calculate the log sum exp, used to smooth min max function
    /// @return the calculated log sum exp
    inline double logSumExp(double var1, double var2, double alpha)
    {
        return alpha * log(exp(var1 / alpha) + exp(var2 / alpha));
    }

    /// @brief Some smooth function from Biying I cannot understand
    /// @return the smoothed result
    inline double logSumExp0(double var, double alpha)
    {
        return alpha * log(exp(var / alpha) + 1);
        // modify it to handle overflow issue
        // avg > 709.8 => overflow; avg < -709.8 => underflow
        if (var / alpha < 709.8)
        {
            return alpha * log(exp(var / alpha) + 1);
        }
        else
        {
            // -var + 32
            return alpha * log(exp(32 / alpha) + exp((-var + 32)/alpha)) + var - 32;
        }
    }
    /// @brief the gradient of LSE
    /// @return the gradient of log sum exp
    inline double gradLogSumExp(double var1, double var2, double alpha)
    {
        return ( exp(var1 / alpha) - exp(var2 / alpha) ) / ( exp(var1 / alpha) + exp(var2 / alpha) );
    }

    /// @brief The gradient of some smooth function from Biying I cannot understand
    /// @return the gradient of logSumExp0
    inline double gradLogSumExp0(double var, double alpha)
    {
        return exp( var / alpha ) / ( exp(var / alpha) + 1);
    }
}

/// @class IDEAPLACE::NlpWnconj
/// @brief the non-linear optimization for global placement with wnlib conjugated gradient solver
class NlpWnconj
{
    public:
        /// @brief default constructor
        explicit NlpWnconj(Database &db) : _db(db) {}
        /// @brief run the NLP placer
        /// @return if successful
        bool solve();
        /// @brief set tough model
        void setToughMode(bool set) { _toughModel = set; }
    private:
        /*------------------------------*/ 
        /* Flow                         */
        /*------------------------------*/ 
        /// @brief initializing the variables
        /// @return if successful
        bool initVars();
        /// @brief nonlinear optimization kernel
        /// @return if successful
        bool nlpKernel();
        /// @brief write out the solution to the database
        /// @return if successful
        bool writeOut();
        /// @brief clean up the pointers
        /// @return if successful
        bool cleanup();
        /*------------------------------*/ 
        /* Supporting functions         */
        /*------------------------------*/ 
        /// @brief calculate the total overlap area
        /// @return the total overlap area
        double totalOvlArea(double *values);
        /// @brief get total asymmetric distance
        /// @return the total asymmetric distance
        double totalAsymDist() { return 0; }
        /// @brief evalute the current solution. Specifically, calculating _curOvlRatio, _curOOBRatio, _curAsymDist
        void evaluteSolution();
        /*------------------------------*/ 
        /* Math                         */
        /*------------------------------*/ 
        /// @brief calculate the smoothed HPWL of each net
        /// @param first: the array of the current values
        /// @param second: the net index
        /// @param third: alpha parameter in the smooth function
        double logSumExpHPWL(double *values, IndexType netIdx, double alpha)
        {
            double xmax = 0, xmin = 0, ymax = 0, ymin = 0;
            const auto &net = _db.net(netIdx);
            if (net.numPinIdx() <= 1)
            {
                return 0;
            }
            for (IndexType idx = 0; idx < net.numPinIdx(); ++idx)
            {
                // Get the pin location referenced to the cell
                IndexType pinIdx = net.pinIdx(idx);
                const auto &pin = _db.pin(pinIdx);
                IndexType cellIdx = pin.cellIdx();
                const auto &cell = _db.cell(cellIdx);
                // Get the cell location from the input arguments
                XY<double> cellLoc = XY<double>(values[cellIdx * 2], values[cellIdx * 2 + 1]);
                XY<double> midLoc = XY<double>(pin.midLoc().x(), pin.midLoc().y()) * _scale;
                XY<double> cellLoLoc = XY<double>(cell.cellBBox().xLo(), cell.cellBBox().yLo()) * _scale;
                XY<double> pinLoc = cellLoc + midLoc - cellLoLoc;
                /*
                DBG("cell loc %s mid loc %s cellloloc %s pinloc %s \n", 
                        cellLoc.toStr().c_str(),
                        midLoc.toStr().c_str(),
                        cellLoLoc.toStr().c_str(),
                        pinLoc.toStr().c_str());
                        */
                xmax += exp(pinLoc.x() / alpha);
                xmin += exp(-pinLoc.x() / alpha);
                ymax += exp(pinLoc.y() / alpha);
                ymin += exp(-pinLoc.y() / alpha);
            }
            xmax = log(xmax);
            xmin = log(xmin);
            ymax = log(ymax);
            ymin = log(ymin);
            return alpha * (xmax + xmin + ymax + ymin);
        }
        /// @brief the gradient if hpwl with respect to x
        /// @param first: the current variables values
        /// @param second: the index of net
        /// @param third: the alpha constant in LSE
        /// @param fourth: a reference to a vector to store the calculated gradient for dx
        /// @param fourth: a reference to a vector to store the calculated gradient for dy
        void gradLogSumExpHpwl(double *values, IndexType netIdx, double alpha, std::vector<double> &gradX, std::vector<double> &gradY)
        {
            double xmax = 0, xmin = 0;
            double ymax = 0, ymin = 0;
            const auto &net = _db.net(netIdx);
            if (net.numPinIdx() <= 1)
            {
                for (IndexType idx = 0; idx < net.numPinIdx(); ++idx)
                {
                    gradX.emplace_back(0);
                    gradY.emplace_back(0);
                }
            }
            for (IndexType idx = 0; idx < net.numPinIdx(); ++idx)
            {
                // Get the pin location referenced to the cell
                IndexType pinIdx = net.pinIdx(idx);
                const auto &pin = _db.pin(pinIdx);
                IndexType cellIdxTemp = pin.cellIdx();
                const auto &cell = _db.cell(cellIdxTemp);
                // Get the cell location from the input arguments
                XY<double> cellLoc = XY<double>(values[cellIdxTemp * 2], values[cellIdxTemp * 2 + 1]);
                XY<double> midLoc = XY<double>(pin.midLoc().x(), pin.midLoc().y()) * _scale;
                XY<double> cellLoLoc = XY<double>(cell.cellBBox().xLo(), cell.cellBBox().yLo()) * _scale;
                XY<double> pinLoc = cellLoc + midLoc - cellLoLoc;
                xmax += exp(pinLoc.x() / alpha);
                xmin += exp(- pinLoc.x() / alpha);
                ymax += exp(pinLoc.y() / alpha);
                ymin += exp(- pinLoc.y() / alpha);
            }
            // Calculate the pin-wise terms
            for (IndexType idx = 0; idx < net.numPinIdx(); ++idx)
            {
                // Get the pin location referenced to the cell
                IndexType pinIdx = net.pinIdx(idx);
                const auto &pin = _db.pin(pinIdx);
                IndexType cellIdxTemp = pin.cellIdx();
                // Get the cell location from the input arguments
                XY<double> cellLoc = XY<double>(values[cellIdxTemp * 2], values[cellIdxTemp * 2 + 1]);
                XY<double> midLoc = XY<double>(pin.midLoc().x(), pin.midLoc().y()) * _scale;
                XY<double> pinLoc = cellLoc + midLoc;
                double pinXmax = exp( pinLoc.x() / alpha ) / xmax;
                double pinXmin = exp( - pinLoc.x() / alpha) / xmin;
                double pinYmax = exp( pinLoc.y() / alpha) / ymax;
                double pinYmin = exp( - pinLoc.y() / alpha) / ymin;
                // Push back the calculated gradient to the vectors
                gradX.emplace_back(pinXmax - pinXmin);
                gradY.emplace_back(pinYmax - pinYmin);
            }
            
            
        }
    public:
        /*------------------------------*/ 
        /* For WNLIB interface          */
        /*------------------------------*/ 
        /// @brief the objective function
        /// @param the pointer to the current variables
        /// @return the evaluated objective function
        double objFunc(double *values);
        /// @brief the gradient function
        /// @param first: the pointer to the gradients
        /// @param second: the pointer to the values
        void gradFunc(double *grad, double *values);
    private:
        /*------------------------------*/ 
        /* Update penalty multiplier    */
        /*------------------------------*/ 
        /// @brief calculate the step size
        RealType stepSize();
        /// @brief update penalty terms
        /// @return true if the global routing should be terminated
        bool updateMultipliers();
        bool updateMultipliers2();

        void alignSym();

        
    private:
        Database &_db; ///< The placement engine database
        int _code = -1; ///< The wnlib status of the completed search. WN_SUCCESS WN_SUBOPTIMAL WN_UNBOUNDED
        double _valMin = 0; ///< The wnlib objective function value at the final solution
        double *_solutionVect = nullptr; ///< Loaded as beginning points and return containing the final solution
        int _len = -1; ///< The number of variables in "_solutionVect"
        double _totalCellArea = 0; ///< The total cell area
        double _curOvlRatio = 1; ///< The current overlapping ratio
        double _curOOBRatio = 1; ///< The current out of boundry ratio
        double _curAsymDist = 1; ///< The current asymmetric distance
        double _lambda1; ///< The coefficient for x and y overlap penalty 
        double _lambda2; ///< The cofficient for out of boundry penalty
        double _lambda3; ///< The coefficient for wire length penalty
        double _lambda4; ///< The coefficient for asymmetry penalty
        RealType _maxWhiteSpace;
        RealType _lambdaMaxOverlap;
        double _alpha; ///< Used in objective function
        Box<double> _boundary; ///< The boundary constraint for the placement
        double _overlapThreshold = NLP_WN_CONJ_OVERLAP_THRESHOLD; ///< Threshold for whether increase penalty for overlapping penalty
        double _oobThreshold = NLP_WN_CONJ_OOB_THRESHOLD; ///< The threshold for wehther increasing the penalty for out of boundry
        double _asymThreshold = NLP_WN_CONJ_ASYM_THRESHOLD; ///< The threshold for whether increasing the penalty for asymmetry
        double _scale = 0.01;
        RealType _fOverlap = 0;
        RealType _fOOB = 0;
        RealType _fHpwl = 0;
        RealType _fAsym = 0;
        RealType _fMaxOver = 0;
        RealType _epsilon = NLP_WN_CONJ_EPISLON;///< For updating penalty multipliers
        IndexType _iter = 0; ///< Current iteration
        double _tao = 0.5; ///< The exponential decay factor for step size
        IndexType _maxIter = NLP_WN_CONJ_DEFAULT_MAX_ITER; ///< The maximum iterations
        RealType _defaultSymAxis = 0;
        bool _toughModel = false; ///< Whether try hard to be feasible
};

inline double NlpWnconj::objFunc(double *values)
{
    //values[2 * _db.numCells()] = 0;
    //values[2 * _db.numCells() + 1] = 0;
    // Initial the objective to be 0 and add the non-zero to it
    double result = 0;
    _fOverlap = 0;
    _fOOB = 0;
    _fHpwl = 0;
    _fAsym = 0;
    _fMaxOver = 0;
    // log-sum-exp model for overlap penalty
    // Calculate the cell-wise overlapping area penalty
    // TODO: improve the codes below from O(n^2) to O(nlogn)
    for (IndexType cellIdxI = 0; cellIdxI < _db.numCells(); ++cellIdxI)
    {
        const auto &bboxI = _db.cell(cellIdxI).cellBBox();
        for (IndexType cellIdxJ = cellIdxI + 1; cellIdxJ < _db.numCells(); ++cellIdxJ)
        {
            const auto &bboxJ = _db.cell(cellIdxJ).cellBBox();
            // Values arrangement x0, y0, x1, y1...
            double xLoI = values[2 * cellIdxI]; double xHiI = xLoI + bboxI.xLen() * _scale;
            double xLoJ = values[2 * cellIdxJ]; double xHiJ = xLoJ + bboxJ.xLen() * _scale;
            double yLoI = values[2 * cellIdxI + 1]; double yHiI = yLoI + bboxI.yLen() * _scale;
            double yLoJ = values[2 * cellIdxJ + 1]; double yHiJ = yLoJ + bboxJ.yLen() * _scale;
            // max (min(xHiI - xLoJ, xHiJ - xLoI), 0)
            double var1X = xHiI - xLoJ;
            double var2X = xHiJ - xLoI;
            double overlapX = NlpWnconjDetails::logSumExp(var1X, var2X, -_alpha);
            overlapX = NlpWnconjDetails::logSumExp0(overlapX, _alpha);
            // max (min(yHiI - yLoJ, yHiJ - yLoI), 0)
            double var1Y = yHiI - yLoJ;
            double var2Y = yHiJ - yLoI;
            double overlapY = NlpWnconjDetails::logSumExp(var1Y, var2Y, -_alpha);
            overlapY = NlpWnconjDetails::logSumExp0(overlapY, _alpha);
            // find the max overlap
            RealType overlapArea = overlapX * overlapY;
            RealType areaI = (xHiI - xLoI) * (yHiI - yLoI);
            RealType areaJ = (xHiJ - xLoJ) * (yHiJ - yLoJ);
            RealType minArea = overlapArea / std::min(areaI, areaJ);
            if (_fMaxOver < minArea)
            {
                _fMaxOver = minArea;
            }
            
            if (0)
            {
                if (overlapArea / areaI > 1.0)
                {
                    result += _lambdaMaxOverlap * overlapX *overlapY ;
                    DBG("1:  %f \n" ,_lambdaMaxOverlap * overlapX *overlapY  );
                }
                if (overlapArea / areaJ > 1.0)
                {
                    result += _lambdaMaxOverlap * overlapX *overlapY ;
                }
            }
            
            // Add to the objective
            result += _lambda1 * overlapX * overlapY;
            //DBG("Overlap add %f, lambda %f \n", _lambda1 * overlapX * overlapY, _lambda1);
            _fOverlap += _lambda1 * overlapX * overlapY ;
        }
    }


    // record max overlap penalty
    result += _lambdaMaxOverlap * _fMaxOver;

    //double oobCost = 0;

    // Out of boundary penalty
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &bbox = _db.cell(cellIdx).cellBBox();
        double xLo = values[2 * cellIdx]; double xHi = xLo + bbox.xLen() * _scale;
        double yLo = values[2 * cellIdx + 1]; double yHi = yLo + bbox.yLen() * _scale;
        // max (-xLo +boundary_xLo, 0), max(xHi - boundary_xHi, 0)
        double obXLo = NlpWnconjDetails::logSumExp0(_boundary.xLo() - xLo, _alpha);
        double obXHi = NlpWnconjDetails::logSumExp0(xHi - _boundary.xHi(), _alpha);
        // also y
        double obYLo = NlpWnconjDetails::logSumExp0(_boundary.yLo() - yLo, _alpha);
        double obYHi = NlpWnconjDetails::logSumExp0(yHi - _boundary.yLo(), _alpha);
        // Add to the objective
        result += _lambda2 * (obXLo + obXHi + obYLo + obYHi);
        _fOOB +=  _lambda2 * (obXLo + obXHi + obYLo + obYHi);
        //oobCost += (obXLo + obXHi + obYLo + obYHi);
        //DBG("OOB add %f, lambda %f \n",_lambda2 * (obXLo + obXHi + obYLo + obYHi), _lambda2);
    }
    // Wire length penalty
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        double smoothHPWL = this->logSumExpHPWL(values, netIdx, _alpha);
        result += _lambda3 * _db.net(netIdx).weight() * smoothHPWL;
        _fHpwl +=  _lambda3 * _db.net(netIdx).weight() * smoothHPWL;
        //DBG("HPWL add %f, lambda %f \n", _lambda3 * _db.net(netIdx).weight() * smoothHPWL, _lambda3);
    }
    
    // ASYMMETRY
    RealType asymPenalty = 0;
    for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
    {
        const auto & symGrp = _db.symGroup(symGrpIdx);
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++ symPairIdx)
        {
            const auto & symPair = symGrp.symPair(symPairIdx);
            IndexType cellIdx1 = symPair.firstCell();
            IndexType cellIdx2 = symPair.secondCell();
            RealType cell1Lo = values[2 * cellIdx1];
            RealType cell1Hi = _db.cell(cellIdx1).cellBBox().xLen() * _scale + values[2 * cellIdx1];
            RealType cell2Lo = values[2 * cellIdx2];
            RealType cell2Hi = _db.cell(cellIdx2).cellBBox().xLen() * _scale + values[2 * cellIdx2];
            asymPenalty += pow(values[cellIdx1 * 2 +1] - values[2 * cellIdx2 + 1], 2.0); // (y1 -y2) ^ 2
#ifdef MULTI_SYM_GROUP
            asymPenalty += pow(cell1Lo / 2 + cell1Hi / 2 
                    + cell2Lo / 2 + cell2Hi / 2
                    - 2 *values[2 * _db.numCells() + symGrpIdx], 2.0);
#else
            asymPenalty += pow(cell1Lo / 2 + cell1Hi / 2 
                    + cell2Lo / 2 + cell2Hi / 2
                    - 2 * _defaultSymAxis, 2.0);
#endif
        }
        for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
        {
            IndexType selfSymCellIdx = symGrp.selfSym(selfSymIdx);
            RealType cell1Lo = values[2 * selfSymCellIdx];
            RealType cell1Hi = _db.cell(selfSymCellIdx).cellBBox().xLen() * _scale + values[2 * selfSymCellIdx];
#ifdef MULTI_SYM_GROUP
            asymPenalty += pow(cell1Lo / 2 + cell1Hi / 2 
                    -  values[2 * _db.numCells() + symGrpIdx], 2.0);
#else
            asymPenalty += pow(cell1Lo / 2 + cell1Hi / 2 
                    -  _defaultSymAxis, 2.0);
#endif
        }
    }
    result += _lambda4 * asymPenalty;
    _fAsym +=  _lambda4 * asymPenalty ;
    //DBG("foverlap %f, foob %f fhpwl %f fasym %f \n", _fOverlap, _fOOB, _fHpwl, _fAsym);
    
    return result;
}

inline void NlpWnconj::gradFunc(double *grad, double *values)
{
    RealType maxOverlapRatio = 0;
    IndexType maxOverLapIndex = 0;
    RealType maxOverLapGradX = 0;
    RealType maxOverLapGradY = 0;
    //RealType cellAreaMaxOverlap = 0;
    // log-sum-exp model for overlap penalty
    for (IndexType cellIdxI = 0; cellIdxI < _db.numCells(); ++cellIdxI)
    {
        grad[2 * cellIdxI] = 0; // df/dx
        grad[2 * cellIdxI + 1] = 0; // df/dy
        const auto &bboxI = _db.cell(cellIdxI).cellBBox();
        for (IndexType cellIdxJ = 0; cellIdxJ < _db.numCells(); ++cellIdxJ)
        {
            if (cellIdxI == cellIdxJ)
            {
                continue;
            }
            const auto &bboxJ = _db.cell(cellIdxJ).cellBBox();
            // Values arrangement x0, y0, x1, y1...
            double xLoI = values[2 * cellIdxI]; double xHiI = xLoI + bboxI.xLen() * _scale;
            double xLoJ = values[2 * cellIdxJ]; double xHiJ = xLoJ + bboxJ.xLen() * _scale;
            double yLoI = values[2 * cellIdxI + 1]; double yHiI = yLoI + bboxI.yLen() * _scale;
            double yLoJ = values[2 * cellIdxJ + 1]; double yHiJ = yLoJ + bboxJ.yLen() * _scale;
            // max( min (xHiI - xLoJ, xHiJ - xLoi), 0)
            double var1x = xHiI - xLoJ;
            double var2x = xHiJ - xLoI;
            double ox = NlpWnconjDetails::logSumExp(var1x, var2x, -_alpha); // smoothed min
            double gradOx = NlpWnconjDetails::gradLogSumExp0(ox, _alpha); // sommothed max with 0
            // max ( min(yHiI - yLoj, yHiJ - yLoI), 0 )
            double var1y = yHiI - yLoJ;
            double var2y = yHiJ - yLoI;
            double oy = NlpWnconjDetails::logSumExp(var1y, var2y, -_alpha); // smoothed min
            oy = NlpWnconjDetails::logSumExp0(oy, _alpha); // smoothed max with 0
            RealType gradX = oy * gradOx * NlpWnconjDetails::gradLogSumExp(var1x, var2x, -_alpha);
            // Record gradOy
            grad[2 * cellIdxI] += _lambda1 * gradX;
            double gradOy = NlpWnconjDetails::gradLogSumExp0(oy, _alpha);
            RealType gradY = ox * gradOy * NlpWnconjDetails::gradLogSumExp(var1y, var2y, -_alpha);
            // Record gradOy
            grad[2 * cellIdxI + 1] += _lambda1 * gradY;
            // maxoverlap parts
            RealType overlapArea= ox * oy;
            RealType areaI = (xHiI - xLoI) * (yHiI - yLoI);
            if (maxOverlapRatio < overlapArea / areaI)
            {
                maxOverLapIndex = cellIdxI;
                maxOverLapGradX = gradX;
                maxOverLapGradY = gradY;
                //cellAreaMaxOverlap = areaI;
            }
            
            if (0)
            {
                RealType lambda;
                RealType threshold = 0.7;
                if (_toughModel)
                {
                    lambda = _lambdaMaxOverlap ;
                    threshold = 0.5;
                }
                else
                {
                    lambda = _lambda1 ;
                }
                if (overlapArea / areaI > threshold)
                {
                    RealType ratio = overlapArea / areaI;
                    ratio /= threshold;
                    ratio *= ratio;
                    ratio *= ratio;
                    grad[2 * cellIdxI] += lambda * gradX * ratio;
                    grad[2 * cellIdxI + 1] += lambda * gradY  * ratio;
                }
            }
            
        }
    }
    // Update the max overlap
    grad[2 * maxOverLapIndex] += _lambdaMaxOverlap * maxOverLapGradX ;
    grad[2 * maxOverLapIndex + 1] += _lambdaMaxOverlap * maxOverLapGradY ;
    // out of boundary penalty
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &bbox = _db.cell(cellIdx).cellBBox();
        double xLo = values[2 * cellIdx] ; double xHi = xLo + bbox.xLen() * _scale;
        double yLo = values[2 * cellIdx + 1]; double yHi = yLo + bbox.yLen() * _scale;
        // max(lower - x/yLo, 0), max (x/yHi - upper, 0)
        double gradObX = - NlpWnconjDetails::gradLogSumExp0(_boundary.xLo() - xLo, _alpha); // the negative is from derivative
        gradObX += NlpWnconjDetails::gradLogSumExp0(xHi - _boundary.xHi(), _alpha);
        // Record
        grad[2 * cellIdx] +=  _lambda2 * gradObX;
        double gradObY = - NlpWnconjDetails::gradLogSumExp0(_boundary.yLo() - yLo, _alpha); // the negative is from derivative
        gradObY += NlpWnconjDetails::gradLogSumExp0(yHi - _boundary.yHi(), _alpha);
        // Record
        grad[2 * cellIdx + 1] += _lambda2 * gradObY;
    }
    // Wire length penalty
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        const auto &net = _db.net(netIdx);
        auto netWgt = net.weight();
        std::vector<double> gradX, gradY;
        // Calculate the gradients for the net. idx by pin
        this->gradLogSumExpHpwl(values, netIdx, _alpha, gradX, gradY);
        for (IndexType pinInNetIdx = 0; pinInNetIdx < net.numPinIdx(); ++pinInNetIdx)
        {
            IndexType pinIdx = net.pinIdx(pinInNetIdx);
            const auto &pin = _db.pin(pinIdx);
            IndexType cellIdx = pin.cellIdx();
            // Record
            grad[2 * cellIdx] += netWgt * _lambda3 * gradX[pinInNetIdx];
            grad[2 * cellIdx + 1] += netWgt * _lambda3 * gradY[pinInNetIdx];
        }
    }
    
    // ASYMMETRY
    for (IndexType idx = 0; idx < _db.numCells(); ++idx)
    {
        grad[idx + 2 * _db.numCells()] = 0; // initialize gradient
    }
    for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
    {
        const auto & symGrp = _db.symGroup(symGrpIdx);
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            IndexType cellIdx1 = symGrp.symPair(symPairIdx).firstCell();
            IndexType cellIdx2 = symGrp.symPair(symPairIdx).secondCell();
            RealType cell1Lo =  values[2 * cellIdx1];
            RealType cell1Hi = _db.cell(cellIdx1).cellBBox().xLen() * _scale + values[2 * cellIdx1];
            RealType cell2Lo =  values[2 * cellIdx2];
            RealType cell2Hi = _db.cell(cellIdx2).cellBBox().xLen() * _scale + values[2 * cellIdx2];
#ifdef MULTI_SYM_GROUP
            RealType gradX = cell1Lo / 2 + cell1Hi /2 
                    + cell2Lo / 2 + cell2Hi / 2
                    - 2 *values[2 * _db.numCells() + symGrpIdx]; // (x1 + x2 + const - 2 xsym)
#else
            RealType gradX = cell1Lo / 2 + cell1Hi /2 
                    + cell2Lo / 2 + cell2Hi / 2
                    - 2 * _defaultSymAxis; // (x1 + x2 + const - 2 xsym)
#endif
            grad[2 * cellIdx1] += _lambda4 * gradX * 2; // for x1. 
            grad[2 * cellIdx2] +=  _lambda4 * gradX * 2; // for x2
#ifdef MULTI_SYM_GROUP
            grad[2 * _db.numCells() + symGrpIdx] += _lambda4 * (- 2 * gradX) / symGrp.numConstraints(); // for xsym
#endif
            RealType gradY = 2.0 * (values[2 * cellIdx1 + 1] - values[2 * cellIdx2 + 1]);
            grad[2 * cellIdx1 + 1] += _lambda4 * gradY;
            grad[2 * cellIdx2 + 1] += _lambda4 * (- gradY); 
        }
        for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
        {
            IndexType cellIdx = symGrp.selfSym(selfSymIdx);
            RealType cell1Lo =  values[2 * cellIdx];
            RealType cell1Hi = _db.cell(cellIdx).cellBBox().xLen() * _scale + cell1Lo;
#ifdef MULTI_SYM_GROUP
            RealType gradSS = 2.0 * (cell1Lo / 2 + cell1Hi / 2  - values[2 * _db.numCells() + symGrpIdx]);
#else
            RealType gradSS = 2.0 * (cell1Lo / 2 + cell1Hi / 2  - _defaultSymAxis);
#endif
            grad[2 * cellIdx] += _lambda4 * gradSS;
#ifdef MULTI_SYM_GROUP
            grad[2 * _db.numCells() + symGrpIdx]  += _lambda4 * (- gradSS) / symGrp.numConstraints(); // for xsym
#endif
        }
    }
    
}

inline double NlpWnconj::totalOvlArea(double *values)
{
    // naive implementation
    // TODO: better implementation
    double area = 0;
    for (IndexType cellIdxI = 0; cellIdxI < _db.numCells(); ++cellIdxI)
    {
        const auto &bboxI = _db.cell(cellIdxI).cellBBox();
        for (IndexType cellIdxJ = cellIdxI + 1; cellIdxJ < _db.numCells(); ++cellIdxJ)
        {
            const auto &bboxJ = _db.cell(cellIdxJ).cellBBox();
            // Values arrangement x0, y0, x1, y1...
            double xLoI = values[2 * cellIdxI]; double xHiI = xLoI + bboxI.xLen() * _scale;
            double xLoJ = values[2 * cellIdxJ]; double xHiJ = xLoJ + bboxJ.xLen() * _scale;
            double yLoI = values[2 * cellIdxI + 1]; double yHiI = yLoI + bboxI.yLen() * _scale;
            double yLoJ = values[2 * cellIdxJ + 1]; double yHiJ = yLoJ + bboxJ.yLen() * _scale;
            // max (min(xHiI - xLoJ, xHiJ - xLoI), 0)
            double var1X = xHiI - xLoJ;
            double var2X = xHiJ - xLoI;
            double overlapX = std::min(var1X, var2X);
            overlapX = std::max(overlapX, 0.0);
            // max (min(yHiI - yLoJ, yHiJ - yLoI), 0)
            double var1Y = yHiI - yLoJ;
            double var2Y = yHiJ - yLoI;
            double overlapY = std::min(var1Y, var2Y);
            overlapY = std::max(overlapY, 0.0);
            if (overlapX * overlapY >= 0.000001)
            area +=  overlapX * overlapY;
        }
    }
    return area;
}

/// @brief evalute the current solution. Specifically, calculating _curOvlRatio, _curOOBRatio, _curAsymDist
inline void NlpWnconj::evaluteSolution()
{
    double totalOvlArea = this->totalOvlArea(_solutionVect);
    _curOvlRatio = totalOvlArea / (_totalCellArea );//* _scale * _scale);
    // out of boundary
    double totalOOBarea = 0.0;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &bbox = _db.cell(cellIdx).cellBBox();
        double xLo = _solutionVect[2 * cellIdx]; double xHi = xLo + bbox.xLen() * _scale;
        double yLo = _solutionVect[2 * cellIdx + 1]; double yHi = yLo + bbox.yLen() * _scale;
        /*
        double varX = 0;
        varX += std::max(-xLo + _boundary.xLo(), 0.0);
        varX += std::max(xHi - _boundary.xHi(), 0.0);
        double varY = 0;
        varY += std::max(_boundary.yLo() - yLo, 0.0);
        varY += std::max(yHi - _boundary.yHi(), 0.0);
        totalOOBarea +=  varX * varY;
        */
        double varX = 0;
        varX += std::max(-xLo + _boundary.xLo(), 0.0);
        varX += std::max(xHi - _boundary.xHi(), 0.0);
        double varY = 0;
        varY += std::max(_boundary.yLo() - yLo, 0.0);
        varY += std::max(yHi - _boundary.yHi(), 0.0);
        totalOOBarea +=  (varX - _boundary.xLen()) * (varY - _boundary.yLen());
        //DBG("total oob area %f \n", totalOOBarea);
    }
    _curOOBRatio = totalOOBarea / ( _totalCellArea);
    // Asymmetry
    _curAsymDist = 0;
    for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
    {
        const auto &symGrp = _db.symGroup(symGrpIdx);
#ifdef MULTI_SYM_GROUP
        RealType symAxis = _solutionVect[2 * _db.numCells() + symGrpIdx];
#else
        RealType symAxis = _defaultSymAxis;
#endif
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            const auto &symPair = symGrp.symPair(symPairIdx);
            IndexType cell1 = symPair.firstCell();
            IndexType cell2 = symPair.secondCell();
            RealType cellWidth = _db.cell(cell1).cellBBox().xLen() * _scale;
            _curAsymDist += std::abs(_solutionVect[2 * cell1 + 1] - _solutionVect[2 * cell2 + 1]);
            _curAsymDist += std::abs(_solutionVect[2 * cell1] + _solutionVect[2 * cell2] + cellWidth
                    - 2 * symAxis);
        }
        for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
        {
            IndexType cellIdx = symGrp.selfSym(selfSymIdx);
            RealType cellWidth = _db.cell(cellIdx).cellBBox().xLen() * _scale;
            _curAsymDist += std::abs(_solutionVect[2 * cellIdx] +  cellWidth / 2 - symAxis);
        }
    }
}
PROJECT_NAMESPACE_END

#endif
