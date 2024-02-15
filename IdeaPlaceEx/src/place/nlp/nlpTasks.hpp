/**
 * @file nlpTasks.hpp
 * @brief The non-lnear programming tasks for global plaement
 * @author Keren Zhu
 * @date 04/02/2020
 */

#pragma once
#include "global/global.h"
#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
#include <taskflow/taskflow.hpp>
#endif // IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ
#include "place/different.h"

PROJECT_NAMESPACE_BEGIN

/// @brief namespace for nlp tasks
namespace nt
{
    template<typename task_type>
    class Task
    {
        public: 
            explicit Task() {}
            explicit Task(const task_type &) =  delete;
            explicit Task(task_type &&task) : _task(std::make_shared<task_type>(std::move(task))) {}
            void run() { task_type::run(*_task); }
            const task_type &taskData() const { return *_task; } 
            task_type &taskData() { return *_task; }
            std::shared_ptr<task_type> taskDataPtr() { return _task; }
#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
            template<typename _task_type>
            explicit Task(const tf::Task &tfTask) = delete;
            void regTask(tf::Taskflow &taskflow)
            {
                Assert(!_isReg);
                _tfTask = taskflow.emplace([&](){ this->run(); });
                _isReg = true;
            }
            tf::Task &tfTask() { return _tfTask; }
            void precede(Task<_task_type> &other) { _tfTask.precede(other.tfTask()); }
            void precede(tf::Task &tfTask) { _tfTask.precede(tfTask); }
            void unReg()
            {
                _isReg = false;
                // _tfTask is not been changed. The outer should manage to deprecate the taskflow
            }
#endif // IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ
        protected:
            std::shared_ptr<task_type> _task;
#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
            tf::Task _tfTask; ///< the cpp-taskflow task
            bool _isReg = false;
#endif // IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ
    };

    /// @brief an empty task. Usually for indicating status
    class EmptyTask
    {
        public:
            static void run(EmptyTask &) { }
    };

#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
    // allow generate a empty task from taskflow task
    template<>
    inline Task<EmptyTask>::Task(const tf::Task &tfTask)
    {
        _tfTask = tfTask;
        _isReg = true;
    }
#endif // IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ

    /// @brief Evaluating objective tasks
    template<typename nlp_numerical_type>
    class EvaObjTask
    {
        public:
            EvaObjTask(const std::function<nlp_numerical_type(void)> &func) : _evaFunc(func) {}
            EvaObjTask(const EvaObjTask & other) { _evaFunc = other._evaFunc; }
            static void run(EvaObjTask & task) { task._obj =  task._evaFunc(); }
            nlp_numerical_type obj() const { return _obj; }
        private:
            std::function<nlp_numerical_type(void)> _evaFunc; 
            nlp_numerical_type _obj;
    };

    /// @brief The tasks for only wrapping a function
    class FuncTask
    {
        public:
            FuncTask() { _func = [](){}; }
            FuncTask(const std::function<void(void)> &func) : _func(func) {}
            FuncTask(const FuncTask &other) { _func = other._func; }
            static void run(FuncTask &task) { task._func(); }
        private:
            std::function<void(void)> _func;
    };

    /// @brief The tasks for only wrapping a function returning integer
    class ConditionTask
    {
        public:
            ConditionTask() { _func = [](){ return 0; }; _cond = 0; }
            ConditionTask(const std::function<IntType(void)> &func) : _func(func) { _cond = 0;}
            ConditionTask(const ConditionTask &other) { _func = other._func; _cond = other._cond; }
            static void run(ConditionTask &task) { task._cond = task._func(); }
            IntType cond() const { return _cond; }
        private:
            std::function<IntType(void)> _func;
            IntType _cond = 0;
    };

    /// @brief The tasks for calculating the partials from a operator.
    /// @tparam the differentiable operator type
    template<typename op_type, typename eigen_vector_type>
    class CalculateOperatorPartialTask;

    // @brief The tasks for transfer from CalculateOperatorPartialTask to a target matrix
    template<typename nlp_op_type, typename eigen_vector_type>
    class UpdateGradientFromPartialTask
    {
        typedef typename nlp_op_type::numerical_type nlp_numerical_type;
        typedef typename nlp_op_type::coordinate_type nlp_coordiante_type;
        typedef eigen_vector_type EigenVector;
        public:
            UpdateGradientFromPartialTask() = delete;
            UpdateGradientFromPartialTask(const UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &other)  = delete;
            UpdateGradientFromPartialTask(UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &other)  = delete;
            UpdateGradientFromPartialTask(UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &&other)
                : _calcTask(std::move(other._calcTask)), _target(std::move(other._target)), _idxFunc(std::move(other._idxFunc))
            {
            }
            UpdateGradientFromPartialTask(std::shared_ptr<CalculateOperatorPartialTask<nlp_op_type, eigen_vector_type>> calcTask, EigenVector *target,
                    const std::function<IndexType(IndexType, Orient2DType)> &idxFunc) 
            { 
                _calcTask = calcTask; 
                _target = target; 
                _idxFunc = idxFunc;  
            }
            static void run(UpdateGradientFromPartialTask &task)
            {
                for (IndexType idx = 0; idx < task._calcTask->numCells(); ++idx)
                {
                    IndexType cellIdx = task._calcTask->_inverseCellMap[idx];
                    (*task._target)(task._idxFunc(cellIdx, Orient2DType::HORIZONTAL)) += task._calcTask->_partialsX(idx);
                    (*task._target)(task._idxFunc(cellIdx, Orient2DType::VERTICAL)) += task._calcTask->_partialsY(idx);
                }
            }
        private:
            std::shared_ptr<CalculateOperatorPartialTask<nlp_op_type, eigen_vector_type>> _calcTask;
            EigenVector *_target;
            std::function<IndexType(IndexType, Orient2DType)> _idxFunc; //< convert cell idx to eigen vector idx
    };

#ifdef MULTI_SYM_GROUP
    template<typename nlp_numerical_type, typename nlp_coordinate_type, typename eigen_vector_type>
    class UpdateGradientFromPartialTask<diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type>, eigen_vector_type>
    {
        typedef diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        typedef eigen_vector_type EigenVector;
        public:
            UpdateGradientFromPartialTask() {}
            UpdateGradientFromPartialTask(const UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &other)  = delete;
            UpdateGradientFromPartialTask(UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &other)  = delete;
            UpdateGradientFromPartialTask(UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type> &&other)
                : _calcTask(std::move(other._calcTask)), _target(std::move(other._target)), _idxFunc(std::move(other._idxFunc))
            {
            }
            UpdateGradientFromPartialTask(std::shared_ptr<CalculateOperatorPartialTask<nlp_op_type, eigen_vector_type>> calcTask, EigenVector *target,
                    const std::function<IndexType(IndexType, Orient2DType)> &idxFunc) 
            { 
                _calcTask = calcTask; 
                _target = target; 
                _idxFunc = idxFunc;  
            }
            static void run(UpdateGradientFromPartialTask &task)
            {
                for (IndexType idx = 0; idx < task._calcTask->numCells(); ++idx)
                {
                    IndexType cellIdx = task._calcTask->_inverseCellMap[idx];
                    (*task._target)(task._idxFunc(cellIdx, Orient2DType::HORIZONTAL)) = task._calcTask->_partialsX(idx);
                    (*task._target)(task._idxFunc(cellIdx, Orient2DType::VERTICAL)) = task._calcTask->_partialsY(idx);
                }
                (*task._target)(task._idxFunc(task._calcTask->_symGrpIdx, Orient2DType::HORIZONTAL)) = task._calcTask->_partialsX(idx);
            }
        private:
            std::shared_ptr<CalculateOperatorPartialTask<nlp_op_type, eigen_vector_type>> _calcTask;
            EigenVector *_target;
            std::function<IndexType(IndexType, Orient2DType)> _idxFunc; //< convert cell idx to eigen vector idx
    };
#endif

    /// @brief trait template for building the _cellMap and _inverseCellMap from the different types opeartor. This template need partial specification.
    /// @tparam the differentiable operator type
    template<typename op_type>
    struct calc_operator_partial_build_cellmap_trait 
    {
        typedef op_type nlp_op_type;
        template<typename calc_type>
        static void build(op_type *, calc_type *) {}
    };


    template<typename op_type, typename eigen_vector_type>
    class CalculateOperatorPartialTask
    {
        typedef op_type nlp_op_type;
        typedef typename nlp_op_type::numerical_type nlp_numerical_type;
        typedef typename nlp_op_type::coordinate_type nlp_coordiante_type;
        typedef eigen_vector_type EigenVector;
        friend UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type>;
        friend calc_operator_partial_build_cellmap_trait<nlp_op_type>;
        static constexpr IntType MAX_NUM_CELLS = IDEAPLACE_DEFAULT_MAX_NUM_CELLS;
        public:
            CalculateOperatorPartialTask() = delete;
            CalculateOperatorPartialTask(CalculateOperatorPartialTask &other) = delete;
            CalculateOperatorPartialTask(CalculateOperatorPartialTask &&other)
                : _partialsX(std::move(other._partialsX)), _partialsY(std::move(other._partialsY)),
                _op(std::move(other._op)), _cellMap(std::move(other._cellMap)), _inverseCellMap(std::move(other._inverseCellMap)), 
                _numCells(std::move(other._numCells))
            {
                setAccumulateGradFunc();
            }
            CalculateOperatorPartialTask(nlp_op_type *op) 
            { 
                _op = op; 
                // Use this trait to speficify different number of cells for different operators
                calc_operator_partial_build_cellmap_trait<nlp_op_type>::build(*op, *this); 
                _partialsX.resize(_inverseCellMap.size());
                _partialsY.resize(_inverseCellMap.size());
                clear();
                setAccumulateGradFunc();
            }
            virtual void accumatePartial(nlp_numerical_type num, IndexType cellIdx, Orient2DType orient)
            {
                if (orient == Orient2DType::HORIZONTAL)
                {
                    _partialsX(_cellMap[cellIdx]) += num;
                }
                else if (orient == Orient2DType::VERTICAL)
                {
                    _partialsY(_cellMap[cellIdx]) += num;
                }
            }
            void setAccumulateGradFunc()
            {
                _op->setAccumulateGradFunc([&](nlp_numerical_type num, IndexType cellIdx, Orient2DType orient){ accumatePartial(num, cellIdx, orient);});
            }
            virtual void clear() 
            { 
                for (IndexType idx = 0; idx < numCells(); ++idx)
                {
                    _partialsX(idx) = 0.0;
                    _partialsY(idx) = 0.0;
                }
            }
            IndexType numCells() const { return _numCells; }
            static void run(CalculateOperatorPartialTask &task) 
            { 
                task.clear(); 
                diff::placement_differentiable_traits<nlp_op_type>::accumlateGradient(*(task._op)); 
            }
        protected:
            EigenVector _partialsX;
            EigenVector _partialsY;
            nlp_op_type* _op = nullptr;
            std::array<IndexType, MAX_NUM_CELLS> _cellMap; ///< From db cell index to this class index
            std::vector<IndexType> _inverseCellMap;
            IndexType _numCells;
    };

#ifdef MULTI_SYM_GROUP
    template<typename nlp_numerical_type, typename nlp_coordinate_type, typename eigen_vector_type>
    class CalculateOperatorPartialTask<diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type>> 
    : public CalculateOperatorPartialTask<diff::LseHpwlDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        typedef eigen_vector_type EigenVector;
        friend UpdateGradientFromPartialTask<nlp_op_type, eigen_vector_type>;
        friend calc_operator_partial_build_cellmap_trait<nlp_op_type, eigen_vector_type>;
        static constexpr IntType MAX_NUM_CELLS = IDEAPLACE_DEFAULT_MAX_NUM_CELLS;
        typedef CalculateOperatorPartialTask<diff::LseHpwlDifferentiable<nlp_numerical_type, nlp_coordinate_type>, eigen_vector_type> base_type;
        public:
            virtual void accumatePartial(nlp_numerical_type num, IndexType cellIdx, Orient2DType orient)
            {
                if (orient == Orient2DType::HORIZONTAL)
                {
                    _partialsX(base_type::_cellMap[cellIdx]) += num;
                }
                else if (orient == Orient2DType::VERTICAL)
                {
                    _partialsY(base_type::_cellMap[cellIdx]) += num;
                }
                else
                {
                    _sym += num;
                }
            }
            void clear() override
            { 
                for (IndexType idx = 0; idx < base_type::numCells(); ++idx)
                {
                    base_type::_partialsX(idx) = 0.0;
                    base_type::_partialsY(idx) = 0.0;
                }
                _sym = 0.0;
            }
        protected:
            RealType _sym;
            IndexType _symGrpIdx;
    };
#endif

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::LseHpwlDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::LseHpwlDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = op._cells.size(); // dx and dy for each cells
            calc._inverseCellMap.resize(op._cells.size());
            for (IndexType idx = 0; idx < op._cells.size(); ++idx)
            {
                calc._cellMap[op._cells[idx]] = idx;
                calc._inverseCellMap[idx] = op._cells[idx];
            }
        }
    };

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::CellPairOverlapPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::CellPairOverlapPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = 2; // Always have exactly two cells
            calc._inverseCellMap.resize(2);
            calc._cellMap[op._cellIdxI] = 0;
            calc._inverseCellMap[0] = op._cellIdxI;
            calc._cellMap[op._cellIdxJ] = 1;
            calc._inverseCellMap[1] = op._cellIdxJ;
        }
    };

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::CellOutOfBoundaryPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::CellOutOfBoundaryPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = 1; // Always have exactly two cells
            calc._inverseCellMap.resize(1);
            calc._cellMap[op._cellIdx] = 0;
            calc._inverseCellMap[0] = op._cellIdx;
        }
    };


    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = op._pairCells.size() * 2 + op._selfSymCells.size(); // Always have exactly two cells
            calc._inverseCellMap.resize(calc._numCells);
            IndexType idx = 0;
            for (IndexType pairIdx = 0; pairIdx < op._pairCells.size(); ++pairIdx)
            {
                calc._cellMap[op._pairCells[pairIdx][0]] = idx;
                calc._inverseCellMap[idx] = op._pairCells[pairIdx][0];
                ++idx;
                calc._cellMap[op._pairCells[pairIdx][1]] = idx;
                calc._inverseCellMap[idx] = op._pairCells[pairIdx][1];
                ++idx;
            }
            for (IndexType ssIdx = 0; ssIdx < op._selfSymCells.size(); ++ssIdx)
            {
                calc._cellMap[op._selfSymCells[ssIdx]] = idx;
                calc._inverseCellMap[idx] = op._selfSymCells[ssIdx];
                ++idx;
            }
#ifdef MULTI_SYM_GROUP
            calc._symGrpIdx = op._symGrpIdx;
#endif
        }
    };

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::CosineDatapathDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::CosineDatapathDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = 3; // Always have exactly two cells
            calc._inverseCellMap.resize(3);
            calc._cellMap[op._sCellIdx] = 0;
            calc._inverseCellMap[0] = op._sCellIdx;
            calc._cellMap[op._midCellIdx] = 1;
            calc._inverseCellMap[1] = op._midCellIdx;
            calc._cellMap[op._tCellIdx] = 2;
            calc._inverseCellMap[2] = op._tCellIdx;
        }
    };

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::PowerVerQuadraticWireLengthDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::PowerVerQuadraticWireLengthDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = op._cells.size(); // dx and dy for each cells
            calc._inverseCellMap.resize(op._cells.size());
            for (IndexType idx = 0; idx < op._cells.size(); ++idx)
            {
                calc._cellMap[op._cells[idx]] = idx;
                calc._inverseCellMap[idx] = op._cells[idx];
            }
        }
    };

    template<typename nlp_numerical_type, typename nlp_coordinate_type>
    struct calc_operator_partial_build_cellmap_trait<diff::CurrentFlowDifferentiable<nlp_numerical_type, nlp_coordinate_type>>
    {
        typedef diff::CurrentFlowDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_op_type;
        template<typename calc_type>
        static void build(nlp_op_type &op, calc_type &calc)
        {
            calc._numCells = 2; // Always have exactly two cells
            calc._inverseCellMap.resize(2);
            calc._cellMap[op._sCellIdx] = 0;
            calc._inverseCellMap[0] = op._sCellIdx;
            calc._cellMap[op._tCellIdx] = 1;
            calc._inverseCellMap[1] = op._tCellIdx;
        }
    };

} //namespace nt




namespace nt
{

    template<typename op_type, typename hessian_type, typename Matrix, typename TargetMatrix>
    class CalculateOperatorHessianTask
    {
        typedef op_type nlp_op_type;
        typedef typename nlp_op_type::numerical_type nlp_numerical_type;
        typedef typename nlp_op_type::coordinate_type nlp_coordiante_type;
        friend calc_operator_partial_build_cellmap_trait<nlp_op_type>;
        static constexpr IntType MAX_NUM_CELLS = IDEAPLACE_DEFAULT_MAX_NUM_CELLS;
        public:
            CalculateOperatorHessianTask() = delete;
            CalculateOperatorHessianTask(CalculateOperatorHessianTask &other) = delete;
            CalculateOperatorHessianTask(CalculateOperatorHessianTask &&other)
                : _hessian(std::move(other._hessian)), 
                _cellMap(std::move(other._cellMap)), _inverseCellMap(std::move(other._inverseCellMap)),
                _numCells(std::move(other._numCells)),
                _target(std::move(other._target)), _idxFunc(std::move(other._idxFunc))
            {
                createAccumulateFunc();
                _op = other._op;
            }
            CalculateOperatorHessianTask(nlp_op_type *op, TargetMatrix *target, const std::function<IndexType(IndexType, Orient2DType)> &idxFunc) 
            { 
                _op = op; 
                // Use this trait to speficify different number of cells for different operators
                calc_operator_partial_build_cellmap_trait<nlp_op_type>::build(*op, *this); 
                _hessian.resize(2 * _numCells, 2 * _numCells);

                _target = target;
                _idxFunc = idxFunc;
                clear();
                createAccumulateFunc();
            }
            void createAccumulateFunc()
            {
                _accumulateFunc = [&](nlp_numerical_type num, IndexType i, IndexType j, Orient2DType iOrient, Orient2DType jOrient)
                {
                    this->accumulateHessian(num, i, j, iOrient, jOrient);
                };
            }
            virtual void accumulateHessian(nlp_numerical_type num, IndexType cellIdxI, IndexType cellIdxJ, Orient2DType orientI, Orient2DType orientJ)
            {
                IndexType i = _cellMap[cellIdxI];
                IndexType j = _cellMap[cellIdxJ];
                if (orientI == Orient2DType::VERTICAL)
                {
                    i += _numCells;
                }
                if (orientJ == Orient2DType::VERTICAL)
                {
                    j += _numCells;
                }
                _hessian(i, j) += num;
            }
            virtual void clear() 
            { 
                _hessian.setZero();
            }
            IndexType numCells() const { return _numCells; }
            void calc() 
            { 
                clear(); 
                hessian_type::accumulateHessian(*_op, _accumulateFunc);
            }
            void update()
            {
                for (IndexType i = 0; i < _numCells * 2; ++i)
                {
                    Orient2DType iOrient;
                    IndexType iCellIdx;
                    if (i < _numCells)
                    {
                        iOrient = Orient2DType::HORIZONTAL;
                        iCellIdx = _inverseCellMap[i];
                    }
                    else
                    {
                        iOrient = Orient2DType::VERTICAL;
                        iCellIdx = _inverseCellMap[i - _numCells];
                    }
                    for (IndexType j = 0; j < _numCells * 2; ++j)
                    {
                        Orient2DType jOrient;
                        IndexType jCellIdx;
                        if (j < _numCells)
                        {
                            jOrient = Orient2DType::HORIZONTAL;
                            jCellIdx = _inverseCellMap[j];
                        }
                        else
                        {
                            jOrient = Orient2DType::VERTICAL;
                            jCellIdx = _inverseCellMap[j - _numCells];
                        }
                        (*_target)(_idxFunc(iCellIdx, iOrient), _idxFunc(jCellIdx, jOrient)) += _hessian(i, j);
                    }
                }
            }
        protected:
            Matrix _hessian;
            nlp_op_type* _op = nullptr;
            std::array<IndexType, MAX_NUM_CELLS> _cellMap; ///< From db cell index to this class index
            std::vector<IndexType> _inverseCellMap;
            IndexType _numCells;
            TargetMatrix *_target;
            std::function<IndexType(IndexType, Orient2DType)> _idxFunc; //< convert cell idx to eigen vector idx
            std::function<void(nlp_numerical_type, IndexType, IndexType, Orient2DType, Orient2DType)> _accumulateFunc;
    };
} // namespace nt

PROJECT_NAMESPACE_END
