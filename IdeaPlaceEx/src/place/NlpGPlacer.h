/**
 * @file NlpGPlacer.h
 * @brief The global placement solver with non-linear optimization
 * @author Keren Zhu
 * @date 03/29/2020
 */

#ifndef IDEAPLACE_NLPGPLACER_H_
#define IDEAPLACE_NLPGPLACER_H_

#include <Eigen/Dense>
#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
#include <taskflow/taskflow.hpp>
#endif // IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ
#include "db/Database.h"
#include "place/different.h"
#include "place/differentSecondOrder.hpp"
#include "place/nlp/nlpOuterOptm.hpp"
#include "place/nlp/nlpInitPlace.hpp"
#include "place/nlp/nlpTasks.hpp"
#include "place/nlp/nlpTypes.hpp"
#include "place/nlp/nlpOptmKernels.hpp"
#include "place/nlp/nlpFirstOrderKernel.hpp"
#include "place/nlp/nlpSecondOrderKernels.hpp"
#include "pinassign/VirtualPinAssigner.h"
PROJECT_NAMESPACE_BEGIN

namespace nlp 
{
    /* The wrapper of settings */

    struct nlp_default_hyperparamters
    {
    };

    struct nlp_default_types
    {
        typedef RealType nlp_coordinate_type;
        typedef RealType nlp_numerical_type;
        typedef Eigen::Matrix<nlp_numerical_type, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;
        typedef Eigen::Matrix<nlp_numerical_type, Eigen::Dynamic, 1> EigenVector;
        typedef Eigen::Map<EigenVector> EigenMap;
        typedef Eigen::DiagonalMatrix<nlp_numerical_type, Eigen::Dynamic> EigenDiagonalMatrix;
        typedef diff::LseHpwlDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_hpwl_type;
        typedef diff::CellPairOverlapPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_ovl_type;
        typedef diff::CellOutOfBoundaryPenaltyDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_oob_type;
        typedef diff::AsymmetryDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_asym_type;
        typedef diff::CosineDatapathDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_cos_type;
        typedef diff::PowerVerQuadraticWireLengthDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_power_wl_type;
        typedef diff::CurrentFlowDifferentiable<nlp_numerical_type, nlp_coordinate_type> nlp_crf_type;
    };
    
    struct nlp_default_zero_order_algorithms
    {
        typedef outer_stop_condition::stop_condition_list<
            outer_stop_condition::stop_after_violate_small,
            outer_stop_condition::stop_after_num_outer_iterations<1000>
            > stop_condition_type;
        typedef init_place::init_random_placement_with_normal_distribution_near_center init_place_type;

        /* multipliers */
        typedef outer_multiplier::init::hard_code_init mult_init_type;
        typedef outer_multiplier::update::subgradient_normalized_by_init<nlp_default_types::nlp_numerical_type> mult_update_type;
        //typedef outer_multiplier::update::direct_subgradient mult_update_type;
        typedef outer_multiplier::mult_const_hpwl_cos_and_penalty_by_type<nlp_default_types::nlp_numerical_type, mult_init_type, mult_update_type> mult_type;
    };

    struct nlp_default_first_order_algorithms
    {
        typedef converge::converge_list<
                    converge::converge_grad_norm_by_init<nlp_default_types::nlp_numerical_type>,
                    converge::converge_criteria_max_iter<3000>
                        >
                converge_type;
        //typedef optm::first_order::naive_gradient_descent<converge_type> optm_type;
        typedef optm::first_order::adam<converge_type, nlp_default_types::nlp_numerical_type> optm_type;
        //typedef optm::first_order::nesterov<converge_type, nlp_default_types::nlp_numerical_type> optm_type;
        //typedef optm::first_order::conjugate_gradient_wnlib optm_type;
        
        /* multipliers */
        typedef outer_multiplier::init::init_by_matching_gradient_norm mult_init_type;
        //typedef outer_multiplier::update::subgradient_normalized_by_init<nlp_default_types::nlp_numerical_type> mult_update_type;
        typedef outer_multiplier::update::direct_subgradient mult_update_type;
        typedef outer_multiplier::mult_const_hpwl_cos_and_penalty_by_type<nlp_default_types::nlp_numerical_type, mult_init_type, mult_update_type> mult_type;

        typedef outer_multiplier::update::match_grad_const_multipliers<nlp_default_types::nlp_numerical_type> mult_adjust_type;

        /* alpha */
        typedef alpha::alpha_hpwl_ovl_oob<nlp_default_types::nlp_numerical_type> alpha_type;
        typedef alpha::update::alpha_update_list<
                alpha::update::reciprocal_by_obj<nlp_default_types::nlp_numerical_type, 1>,
                alpha::update::reciprocal_by_obj<nlp_default_types::nlp_numerical_type, 2>,
                alpha::update::reciprocal_by_obj<nlp_default_types::nlp_numerical_type, 3>
            > alpha_update_type;
    };

    template<typename nlp_types>
    struct nlp_default_second_order_settings
    {
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_hpwl_type> hpwl_hessian_trait;
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_ovl_type> ovl_hessian_trait;
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_oob_type> oob_hessian_trait;
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_asym_type> asym_hessian_trait;
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_cos_type> cos_hessian_trait;
        typedef diff::jacobi_hessian_approx_trait<typename nlp_types::nlp_power_wl_type> power_wl_hessian_trait;
    };


    struct nlp_default_second_order_algorithms
    {
        typedef converge::converge_list<
                    converge::converge_grad_norm_by_init<nlp_default_types::nlp_numerical_type>,
                    converge::converge_criteria_max_iter<3000>
                        >
                converge_type;
        //typedef optm::second_order::naive_gradient_descent<converge_type> optm_type;
        typedef optm::second_order::adam<converge_type, nlp_default_types::nlp_numerical_type> optm_type;
        //typedef optm::second_order::nesterov<converge_type, nlp_default_types::nlp_numerical_type> optm_type;
        //typedef optm::first_order::conjugate_gradient_wnlib optm_type;
        
        /* multipliers */
        typedef outer_multiplier::init::init_by_matching_gradient_norm mult_init_type;
        //typedef outer_multiplier::update::subgradient_normalized_by_init<nlp_default_types::nlp_numerical_type> mult_update_type;
        typedef outer_multiplier::update::direct_subgradient mult_update_type;
        typedef outer_multiplier::mult_const_hpwl_cos_and_penalty_by_type<nlp_default_types::nlp_numerical_type, mult_init_type, mult_update_type> mult_type;
        typedef outer_multiplier::update::match_grad_const_multipliers<nlp_default_types::nlp_numerical_type> mult_adjust_type;

        /* alpha */
        typedef alpha::alpha_hpwl_ovl_oob<nlp_default_types::nlp_numerical_type> alpha_type;
        typedef alpha::update::alpha_update_list<
                alpha::update::reciprocal_by_obj<nlp_default_types::nlp_numerical_type, 1>,
                alpha::update::reciprocal_by_obj<nlp_default_types::nlp_numerical_type, 2>
            > alpha_update_type;
    };

    struct nlp_default_settings
    {
        typedef nlp_default_zero_order_algorithms nlp_zero_order_algorithms_type;
        typedef nlp_default_first_order_algorithms nlp_first_order_algorithms_type;
        typedef nlp_default_hyperparamters nlp_hyperparamters_type;
        typedef nlp_default_types nlp_types_type;
        typedef nlp_default_second_order_settings<nlp_types_type> nlp_second_order_setting_type;
        typedef nlp_default_second_order_algorithms nlp_second_order_algorithms_type;
    };


}// namespace nlp

/// @brief non-linear programming-based analog global placement
template<typename nlp_settings>
class NlpGPlacerBase
{
    public:
        typedef typename nlp_settings::nlp_types_type nlp_types;
        typedef typename nlp_settings::nlp_zero_order_algorithms_type nlp_zero_order_algorithms;
        typedef typename nlp_settings::nlp_hyperparamters_type nlp_hyperparamters;

        typedef typename nlp_types::EigenMatrix EigenMatrix;
        typedef typename nlp_types::EigenVector EigenVector;
        typedef typename nlp_types::EigenMap EigenMap;
        typedef typename nlp_types::nlp_coordinate_type nlp_coordinate_type;
        typedef typename nlp_types::nlp_numerical_type nlp_numerical_type;
        typedef typename nlp_types::nlp_hpwl_type nlp_hpwl_type;
        typedef typename nlp_types::nlp_ovl_type nlp_ovl_type;
        typedef typename nlp_types::nlp_oob_type nlp_oob_type;
        typedef typename nlp_types::nlp_asym_type nlp_asym_type;
        typedef typename nlp_types::nlp_cos_type nlp_cos_type;
        typedef typename nlp_types::nlp_power_wl_type nlp_power_wl_type;
        typedef typename nlp_types::nlp_crf_type nlp_crf_type;


        /* algorithms */
        typedef typename nlp_zero_order_algorithms::stop_condition_type stop_condition_type;
        typedef nlp::outer_stop_condition::stop_condition_trait<stop_condition_type> stop_condition_trait;
        template<typename _T>
        friend struct nlp::outer_stop_condition::stop_condition_trait;
        typedef typename nlp_zero_order_algorithms::init_place_type init_placement_type;
        typedef nlp::init_place::init_place_trait<init_placement_type> init_place_trait;
        friend init_place_trait;
        typedef typename nlp_zero_order_algorithms::mult_init_type mult_init_type;
        typedef nlp::outer_multiplier::init::multiplier_init_trait<mult_init_type> mult_init_trait;
        friend mult_init_trait;
        typedef typename nlp_zero_order_algorithms::mult_update_type mult_update_type;
        typedef nlp::outer_multiplier::update::multiplier_update_trait<mult_update_type> mult_update_trait;
        friend mult_update_trait;
        typedef typename nlp_zero_order_algorithms::mult_type mult_type;
        typedef nlp::outer_multiplier::multiplier_trait<mult_type> mult_trait;
        friend mult_trait;
    
    public:
        explicit NlpGPlacerBase(Database &db) : _db(db) {}
        IntType solve();

    protected:
        void assignIoPins();
        /* calculating obj */
        void calcObj()
        {
            _wrapObjAllTask.run();
        }
        /* Init functions */
        virtual void initProblem();
        void initHyperParams();
        void initBoundaryParams();
        void initVariables();
        void initPlace();
        void initOperators();
        void initOptimizationKernelMembers();
        /* Output functions */
        void writeOut();
        /* Util functions */
        IndexType plIdx(IndexType cellIdx, Orient2DType orient);
        void alignToSym();
        /* construct tasks */
        virtual void constructTasks();
        // Obj-related
        void constructObjTasks();
        void constructObjectiveCalculationTasks();
        void constructSumObjTasks();
#ifdef DEBUG_SINGLE_THREAD_GP
        void constructWrapObjTask();
#endif
        /* Optimization  kernel */
        virtual void optimize();
        /* build the computational graph */
        /* Debugging function */
#ifdef DEBUG_GR
#ifdef DEBUG_DRAW
        void drawCurrentLayout(const std::string &name);
#endif 
#endif
    protected:
        Database &_db; ///< The placement engine database
        /* NLP problem parameters */
        IndexType _numCells; ///< The number of cells
        RealType _alpha; ///< Used in LSE approximation hyperparameter
        Box<nlp_coordinate_type> _boundary; ///< The boundary constraint for the placement
        nlp_coordinate_type _scale = 0.01; /// The scale ratio between float optimization kernel coordinate and placement database coordinate unit
        nlp_coordinate_type _totalCellArea = 0; ///< The total cell area of the problem
        nlp_coordinate_type _defaultSymAxis = 0.0; ///< The default symmetric axis
        IndexType _numVariables = 0; ///< The number of variables
        /* Optimization internal results */
        nlp_numerical_type _objHpwl = 0.0; ///< The current value for hpwl
        nlp_numerical_type _objOvl = 0.0; ///< The current value for overlapping penalty
        nlp_numerical_type _objOob = 0.0; ///< The current value for out of boundary penalty
        nlp_numerical_type _objAsym = 0.0; ///< The current value for asymmetry penalty
        nlp_numerical_type _objCos = 0.0; ///< The current value for the cosine signal path penalty
        nlp_numerical_type _objPowerWl = 0.0; ///< power wire length
        nlp_numerical_type _objCrf = 0.0; ///< Current flow
        nlp_numerical_type _obj = 0.0; ///< The current value for the total objective penalty
        nlp_numerical_type _objHpwlRaw = 0.0; ///< The current value for hpwl
        nlp_numerical_type _objOvlRaw = 0.0; ///< The current value for overlapping penalty
        nlp_numerical_type _objOobRaw = 0.0; ///< The current value for out of boundary penalty
        nlp_numerical_type _objAsymRaw = 0.0; ///< The current value for asymmetry penalty
        nlp_numerical_type _objCosRaw = 0.0; ///< The current value for the cosine signal path penalty
        nlp_numerical_type _objPowrWlRaw = 0.0; ///< Power wire length
        nlp_numerical_type _objCrfRaw = 0.0; ///< Current flow
        /* NLP optimization kernel memebers */
        stop_condition_type _stopCondition;
        /* Optimization data */
        EigenVector _pl; ///< The placement solutions
        /* Tasks */
        // Evaluating objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaHpwlTasks; ///< The tasks for evaluating hpwl objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaOvlTasks; ///< The tasks for evaluating overlap objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaOobTasks; ///< The tasks for evaluating out of boundary objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaAsymTasks;  ///< The tasks for evaluating asymmetry objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaCosTasks;  ///< The tasks for evaluating signal path objectives
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaPowerWlTasks;  
        std::vector<nt::Task<nt::EvaObjTask<nlp_numerical_type>>> _evaCrfTasks; ///< The tasks for evaluating current flow objectives
        // Sum the objectives
        nt::Task<nt::FuncTask> _sumObjHpwlTask; ///< The task for summing hpwl objective
        nt::Task<nt::FuncTask> _sumObjOvlTask; ///< The task for summing the overlapping objective
        nt::Task<nt::FuncTask> _sumObjOobTask; ///< The task for summing the out of boundary objective
        nt::Task<nt::FuncTask> _sumObjAsymTask; ///< The task for summing the asymmetry objective
        nt::Task<nt::FuncTask> _sumObjCosTask; ///< The task for summing the cosine signal path objective
        nt::Task<nt::FuncTask> _sumObjPowerWlTask; ///< The task for summing the cosine signal path objective
        nt::Task<nt::FuncTask> _sumObjCrfTask; ///< The task for summing the current flow objective
        nt::Task<nt::FuncTask> _sumObjAllTask; ///< The task for summing the different objectives together
        // Wrapper tasks for debugging
        nt::Task<nt::FuncTask> _wrapObjHpwlTask; ///< The task for wrap the objective 
        nt::Task<nt::FuncTask> _wrapObjOvlTask;
        nt::Task<nt::FuncTask> _wrapObjOobTask;
        nt::Task<nt::FuncTask> _wrapObjAsymTask;
        nt::Task<nt::FuncTask> _wrapObjCosTask;
        nt::Task<nt::FuncTask> _wrapObjPowerWlTask;
        nt::Task<nt::FuncTask> _wrapObjCrfTask; ///< The wrapper for caculating the current flow objective
        nt::Task<nt::FuncTask> _wrapObjAllTask;
        /* Operators */
        std::vector<nlp_hpwl_type> _hpwlOps; ///< The HPWL cost 
        std::vector<nlp_ovl_type> _ovlOps; ///< The cell pair overlapping penalty operators
        std::vector<nlp_oob_type> _oobOps; ///< The cell out of boundary penalty operators 
        std::vector<nlp_asym_type> _asymOps; ///< The asymmetric penalty operators
        std::vector<nlp_cos_type> _cosOps; ///< The signal flow operators
        std::vector<nlp_power_wl_type> _powerWlOps;
        std::vector<nlp_crf_type> _crfOps; ///< The current flow operators
        /* run time */
        std::unique_ptr<::klib::StopWatch> _calcObjStopWatch;
};

template<typename nlp_settings>
inline IndexType NlpGPlacerBase<nlp_settings>::plIdx(IndexType cellIdx, Orient2DType orient)
{
    if (orient == Orient2DType::HORIZONTAL)
    {
        return cellIdx;
    }
    else if (orient == Orient2DType::VERTICAL)
    {
        return cellIdx + _numCells;
    }
    else
    {
#ifdef MULTI_SYM_GROUP
        return cellIdx + 2 *  _numCells; // here cell index representing the idx of sym grp
#else
        return 2 * _numCells;
#endif
    }
}

/// @brief first-order optimization
template<typename nlp_settings>
class NlpGPlacerFirstOrder : public NlpGPlacerBase<nlp_settings>
{
    public:
        typedef NlpGPlacerBase<nlp_settings> base_type;
        typedef typename base_type::EigenVector EigenVector;
        typedef typename base_type::nlp_hpwl_type nlp_hpwl_type;
        typedef typename base_type::nlp_ovl_type nlp_ovl_type;
        typedef typename base_type::nlp_oob_type nlp_oob_type;
        typedef typename base_type::nlp_asym_type nlp_asym_type;
        typedef typename base_type::nlp_cos_type nlp_cos_type;
        typedef typename base_type::nlp_power_wl_type nlp_power_wl_type;
        typedef typename base_type::nlp_crf_type nlp_crf_type;

        typedef typename nlp_settings::nlp_first_order_algorithms_type nlp_first_order_algorithms;
        typedef typename nlp_first_order_algorithms::converge_type converge_type;
        typedef typename nlp::converge::converge_criteria_trait<converge_type> converge_trait;
        typedef typename nlp_first_order_algorithms::optm_type optm_type;
        typedef typename nlp::optm::optm_trait<optm_type> optm_trait;
        
        friend converge_type;
        template<typename converge_criteria_type>
        friend struct nlp::converge::converge_criteria_trait;
        friend optm_type;
        friend optm_trait;

        typedef typename nlp_settings::nlp_first_order_algorithms_type::mult_init_type mult_init_type;
        typedef nlp::outer_multiplier::init::multiplier_init_trait<mult_init_type> mult_init_trait;
        friend mult_init_trait;
        typedef typename nlp_settings::nlp_first_order_algorithms_type::mult_update_type mult_update_type;
        typedef nlp::outer_multiplier::update::multiplier_update_trait<mult_update_type> mult_update_trait;
        friend mult_update_trait;
        typedef typename nlp_settings::nlp_first_order_algorithms_type::mult_type mult_type;
        typedef nlp::outer_multiplier::multiplier_trait<mult_type> mult_trait;
        friend mult_trait;

        typedef typename nlp_settings::nlp_first_order_algorithms_type::mult_adjust_type mult_adjust_type;
        typedef nlp::outer_multiplier::update::multiplier_update_trait<mult_adjust_type> mult_adjust_trait;
        friend mult_adjust_trait;

        /* updating alpha parameters */
        typedef typename nlp_settings::nlp_first_order_algorithms_type::alpha_type alpha_type;
        typedef nlp::alpha::alpha_trait<alpha_type> alpha_trait;
        template<typename T>
        friend struct nlp::alpha::alpha_trait;
        typedef typename nlp_settings::nlp_first_order_algorithms_type::alpha_update_type alpha_update_type;
        typedef nlp::alpha::update::alpha_update_trait<alpha_update_type> alpha_update_trait;
        template<typename T>
        friend struct nlp::alpha::update::alpha_update_trait;


        NlpGPlacerFirstOrder(Database &db) : NlpGPlacerBase<nlp_settings>(db) {}
        void writeoutCsv()
        {
            std::string ver1f = "ver1.csv";
            std::string ver2f = "ver2.csv";
            std::ofstream ver1(ver1f.c_str());
            std::ofstream ver2(ver2f.c_str());
            ver1 << "x y val\n";
            ver2 << "x y val\n";

            for (RealType x = -8; x < 8; x+=(16.0/300))
            {
                for (RealType y = -8; y < 8; y+=(16.0/300))
                {
                    this->_pl(this->plIdx(0, Orient2DType::HORIZONTAL)) = x;
                    this->_pl(this->plIdx(0, Orient2DType::VERTICAL)) = y;
                    this->_wrapObjAllTask.run();
                    auto obj = this->_obj;
                    ver1 << x << " "<< y << " " << obj << "\n";
                }
            }
            auto getLambda = [&](){ return 1.0; };
            for (auto &op : this->_hpwlOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_cosOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_ovlOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_oobOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_asymOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_powerWlOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (auto &op : this->_crfOps) { op._getLambdaFunc = [&](){ return 1.0; }; }
            for (RealType x = -8; x < 8; x+=(16.0/300))
            {
                for (RealType y = -8; y < 8; y+=(16.0/300))
                {
                    this->_pl(this->plIdx(0, Orient2DType::HORIZONTAL)) = x;
                    this->_pl(this->plIdx(0, Orient2DType::VERTICAL)) = y;
                    this->_wrapObjAllTask.run();
                    auto obj = this->_obj;
                    ver2 << x << " "<< y << " " << obj << "\n";
                }
            }
        }
    protected:
        /* calculating gradient */
        void calcGrad()
        {
            _wrapCalcGradTask.run();
        }
        /* Init */
        virtual void initProblem() override;
        void initFirstOrderGrad();
        /* Construct tasks */
        virtual void constructTasks() override;
        void constructFirstOrderTasks();
        void constructCalcPartialsTasks();
        void constructUpdatePartialsTasks();
        void constructClearGradTasks();
        void constructSumGradTask();
        void constructWrapCalcGradTask();
        /* optimization */
        virtual void optimize() override;
        /* Build the computational graph */
#ifdef IDEAPLACE_TASKFLOR_FOR_GRAD_OBJ_
        void regCalcHpwlGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcOvlGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcOobGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcAsymGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcCosGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcAllGradTaskFlow(tf::Taskflow &tfFlow);
        void regCalcGradForLoop(tf::Taskflow &tfFlow);
#endif
        

    protected:
        /* Optimization data */
        EigenVector _grad; ///< The first order graident
        EigenVector _gradHpwl; ///< The first order gradient of hpwl objective
        EigenVector _gradOvl; ///< The first order gradient  of overlapping objective
        EigenVector _gradOob; ///< The first order gradient of out of boundary objective
        EigenVector _gradAsym; ///< The first order gradient of asymmetry objective
        EigenVector _gradCos; ///< The first order gradient of cosine signal path objective
        EigenVector _gradPowerWl;
        EigenVector _gradCrf;
        /* Tasks */
        // Calculate the partials
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_hpwl_type, EigenVector>>> _calcHpwlPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_ovl_type,  EigenVector>>> _calcOvlPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_oob_type,  EigenVector>>> _calcOobPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_asym_type, EigenVector>>> _calcAsymPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_cos_type,  EigenVector>>> _calcCosPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_power_wl_type,  EigenVector>>> _calcPowerWlPartialTasks;
        std::vector<nt::Task<nt::CalculateOperatorPartialTask<nlp_crf_type,  EigenVector>>> _calcCrfPartialTasks;
        // Update the partials
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_hpwl_type, EigenVector>>> _updateHpwlPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_ovl_type,  EigenVector>>> _updateOvlPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_oob_type,  EigenVector>>> _updateOobPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_asym_type, EigenVector>>> _updateAsymPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_cos_type,  EigenVector>>> _updateCosPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_power_wl_type,  EigenVector>>> _updatePowerWlPartialTasks;
        std::vector<nt::Task<nt::UpdateGradientFromPartialTask<nlp_crf_type,  EigenVector>>> _updateCrfPartialTasks;
        // Clear the gradient. Use to clear the _gradxxx records. Needs to call before updating the partials
        nt::Task<nt::FuncTask> _clearGradTask; //FIXME: not used right noe
        nt::Task<nt::FuncTask> _clearHpwlGradTask;
        nt::Task<nt::FuncTask> _clearOvlGradTask;
        nt::Task<nt::FuncTask> _clearOobGradTask;
        nt::Task<nt::FuncTask> _clearAsymGradTask;
        nt::Task<nt::FuncTask> _clearCosGradTask;
        nt::Task<nt::FuncTask> _clearPowerWlGradTask;
        nt::Task<nt::FuncTask> _clearCrfGradTask;
        // Sum the _grad from individual
        nt::Task<nt::FuncTask> _sumGradTask;
        nt::Task<nt::FuncTask> _sumHpwlGradTask;
        nt::Task<nt::FuncTask> _sumOvlGradTask;
        nt::Task<nt::FuncTask> _sumOobGradTask;
        nt::Task<nt::FuncTask> _sumAsymGradTask;
        nt::Task<nt::FuncTask> _sumCosGradTask;
        nt::Task<nt::FuncTask> _sumPowerWlTaskGradTask;
        nt::Task<nt::FuncTask> _sumCrfGradTask;
        // all the grads has been calculated but have not updated
        nt::Task<nt::FuncTask> _wrapCalcGradTask; ///<  calculating the gradient and sum them
        /* run time */
        std::unique_ptr<::klib::StopWatch> _calcGradStopWatch;
        std::unique_ptr<::klib::StopWatch> _optimizerKernelStopWatch;
};



//// @brief some helper function for NlpGPlacerSecondOrder
namespace _nlp_second_order_details
{
    template<typename nlp_settings, BoolType is_diagonal>
    struct is_diagonal_select {};

    template<typename nlp_settings>
    struct is_diagonal_select<nlp_settings, true>
    {
        typedef typename nlp_settings::nlp_types_type::EigenMatrix matrix_type;
        static void resize(matrix_type &matrix, IntType size)
        {
            matrix.resize(size, size);
        }

        static decltype(auto) inverse(matrix_type &matrix)
        {
            return matrix.diagonal().cwiseInverse().asDiagonal();
        }
    };

    template<typename nlp_settings>
    struct is_diagonal_select<nlp_settings, false>
    {
        typedef typename nlp_settings::nlp_types_type::EigenMatrix matrix_type;
        static void resize(matrix_type &matrix, IntType size)
        {
            matrix.resize(size, size);
        }
        static decltype(auto) inverse(matrix_type &matrix)
        {
            Assert(false);
            return matrix.diagonal().cwiseInverse();
        }
    };

    template<typename hessian_target_type>
    struct update_hessian
    {
        hessian_target_type &target;
    };
};

/// @brief first-order optimization
template<typename nlp_settings>
class NlpGPlacerSecondOrder : public NlpGPlacerFirstOrder<nlp_settings>
{
    public:
        typedef typename NlpGPlacerFirstOrder<nlp_settings>::base_type base_type;
        typedef NlpGPlacerFirstOrder<nlp_settings> first_order_type;

        typedef typename first_order_type::nlp_numerical_type nlp_numerical_type;
        typedef typename first_order_type::nlp_coordinate_type nlp_coordinate_type;
        
        typedef typename nlp_settings::nlp_second_order_setting_type second_order_setting_type;

        typedef typename first_order_type::EigenMatrix EigenMatrix;

        typedef typename first_order_type::nlp_hpwl_type nlp_hpwl_type;
        typedef typename first_order_type::nlp_ovl_type nlp_ovl_type;
        typedef typename first_order_type::nlp_oob_type nlp_oob_type;
        typedef typename first_order_type::nlp_asym_type nlp_asym_type;
        typedef typename first_order_type::nlp_cos_type nlp_cos_type;
        typedef typename first_order_type::nlp_power_wl_type nlp_power_wl_type;
        
        typedef typename second_order_setting_type::hpwl_hessian_trait hpwl_hessian_trait;
        typedef typename second_order_setting_type::ovl_hessian_trait ovl_hessian_trait;
        typedef typename second_order_setting_type::oob_hessian_trait oob_hessian_trait;
        typedef typename second_order_setting_type::asym_hessian_trait asym_hessian_trait;
        typedef typename second_order_setting_type::cos_hessian_trait cos_hessian_trait;
        typedef typename second_order_setting_type::power_wl_hessian_trait power_wl_hessian_trait;

        /* figure out the types for storing the hessian */
        // Determine whether the operators are return a diagonal hessian
        constexpr static BoolType isHpwlHessianDiagonal = diff::is_diagnol_matrix<hpwl_hessian_trait>::value;
        constexpr static BoolType isOvlHessianDiagonal = diff::is_diagnol_matrix<ovl_hessian_trait>::value;
        constexpr static BoolType isOobHessianDiagonal = diff::is_diagnol_matrix<oob_hessian_trait>::value;
        constexpr static BoolType isAsymHessianDiagonal = diff::is_diagnol_matrix<asym_hessian_trait>::value;
        constexpr static BoolType isCosHessianDiagonal = diff::is_diagnol_matrix<cos_hessian_trait>::value;
        constexpr static BoolType isPowerWlHessianDiagonal = diff::is_diagnol_matrix<power_wl_hessian_trait>::value;
        constexpr static BoolType isHessianDiagonal = 
                isHpwlHessianDiagonal
                and isOvlHessianDiagonal
                and isOobHessianDiagonal
                and isAsymHessianDiagonal
                and isCosHessianDiagonal
                and isPowerWlHessianDiagonal;

        // define the supporting trait
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isHpwlHessianDiagonal> hpwl_hessian_diagonal_selector;
        friend hpwl_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isOvlHessianDiagonal> ovl_hessian_diagonal_selector;
        friend ovl_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isOobHessianDiagonal> oob_hessian_diagonal_selector;
        friend oob_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isAsymHessianDiagonal> asym_hessian_diagonal_selector;
        friend asym_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isCosHessianDiagonal> cos_hessian_diagonal_selector;
        friend cos_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isPowerWlHessianDiagonal> power_wl_hessian_diagonal_selector;
        friend cos_hessian_diagonal_selector;
        typedef _nlp_second_order_details::is_diagonal_select<nlp_settings, isHessianDiagonal> hessian_diagonal_selector;
        friend hessian_diagonal_selector;

        typedef typename hpwl_hessian_diagonal_selector::matrix_type hpwl_hessian_matrix;
        typedef typename ovl_hessian_diagonal_selector::matrix_type ovl_hessian_matrix;
        typedef typename oob_hessian_diagonal_selector::matrix_type oob_hessian_matrix;
        typedef typename asym_hessian_diagonal_selector::matrix_type asym_hessian_matrix;
        typedef typename cos_hessian_diagonal_selector::matrix_type cos_hessian_matrix;
        typedef typename power_wl_hessian_diagonal_selector::matrix_type power_wl_hessian_matrix;
        typedef typename hessian_diagonal_selector::matrix_type hessian_matrix;

        /* define the algorithms */
        typedef typename nlp_settings::nlp_second_order_algorithms_type nlp_second_order_algorithms;
        typedef typename nlp_second_order_algorithms::converge_type converge_type;
        typedef typename nlp::converge::converge_criteria_trait<converge_type> converge_trait;
        typedef typename nlp_second_order_algorithms::optm_type optm_type;
        typedef typename nlp::optm::optm_trait<optm_type> optm_trait;
        
        friend converge_type;
        template<typename converge_criteria_type>
        friend struct nlp::converge::converge_criteria_trait;
        friend optm_type;
        friend optm_trait;

        typedef typename nlp_settings::nlp_second_order_algorithms_type::mult_init_type mult_init_type;
        typedef nlp::outer_multiplier::init::multiplier_init_trait<mult_init_type> mult_init_trait;
        friend mult_init_trait;
        typedef typename nlp_settings::nlp_second_order_algorithms_type::mult_update_type mult_update_type;
        typedef nlp::outer_multiplier::update::multiplier_update_trait<mult_update_type> mult_update_trait;
        friend mult_update_trait;
        typedef typename nlp_settings::nlp_second_order_algorithms_type::mult_type mult_type;
        typedef nlp::outer_multiplier::multiplier_trait<mult_type> mult_trait;
        friend mult_trait;

        typedef typename nlp_settings::nlp_second_order_algorithms_type::mult_adjust_type mult_adjust_type;
        typedef nlp::outer_multiplier::update::multiplier_update_trait<mult_adjust_type> mult_adjust_trait;
        friend mult_adjust_trait;

        /* updating alpha parameters */
        typedef typename nlp_settings::nlp_second_order_algorithms_type::alpha_type alpha_type;
        typedef nlp::alpha::alpha_trait<alpha_type> alpha_trait;
        template<typename T>
        friend struct nlp::alpha::alpha_trait;
        typedef typename nlp_settings::nlp_second_order_algorithms_type::alpha_update_type alpha_update_type;
        typedef nlp::alpha::update::alpha_update_trait<alpha_update_type> alpha_update_trait;
        template<typename T>
        friend struct nlp::alpha::update::alpha_update_trait;


        static constexpr nlp_numerical_type hessianMinBound = 0.01;
        static constexpr nlp_numerical_type hessianMaxBound = 10;


        NlpGPlacerSecondOrder(Database &db) : NlpGPlacerFirstOrder<nlp_settings>(db) {}
    public:
        decltype(auto) inverseHessian()
        {
            return hessian_diagonal_selector::inverse(_hessian);
        }
        void calcHessian()
        {
            _clearHessian();
            _calcAllHessians();
            _updateAllHessian();
            clipHessian();
        }
    protected:
        virtual void initProblem() override
        {
            first_order_type::initProblem();
            initSecondOrder();
        }
        void initSecondOrder();
        /* Construct tasks */
        virtual void optimize() override
        {
            WATCH_QUICK_START();
            // setting up the multipliers
            this->assignIoPins();
            this->_wrapObjAllTask.run();
            this->_wrapCalcGradTask.run();
            calcHessian();

            optm_type optm;
            mult_type multiplier = mult_trait::construct(*this);
            mult_trait::init(*this, multiplier);
            mult_trait::recordRaw(*this, multiplier);

            mult_adjust_type multAdjuster = mult_adjust_trait::construct(*this, multiplier);
            mult_adjust_trait::init(*this, multiplier, multAdjuster);

            alpha_type alpha = alpha_trait::construct(*this);
            alpha_trait::init(*this, alpha);
            alpha_update_type alphaUpdate = alpha_update_trait::construct(*this, alpha);
            alpha_update_trait::init(*this, alpha, alphaUpdate);
            DBG("np \n");
            std::cout<<"nlp address " <<this <<std::endl;

            IntType iter = 0;
            do
            {
                std::string debugGdsFilename  = "./debug/";
                debugGdsFilename += "gp_iter_" + std::to_string(iter)+".gds";
                DBG("iter %d \n", iter);
                optm_trait::optimize(*this, optm);
                mult_trait::update(*this, multiplier);
                mult_trait::recordRaw(*this, multiplier);
                mult_adjust_trait::update(*this, multiplier, multAdjuster);

                alpha_update_trait::update(*this, alpha, alphaUpdate);
                this->assignIoPins();
                DBG("obj %f hpwl %f ovl %f oob %f asym %f cos %f \n", this->_obj, this->_objHpwl, this->_objOvl, this->_objOob, this->_objAsym, this->_objCos);
                ++iter;
            } while (not base_type::stop_condition_trait::stopPlaceCondition(*this, this->_stopCondition));
            auto end = WATCH_QUICK_END();
            //std::cout<<"grad"<<"\n"<< _grad <<std::endl;
            std::cout<<"time "<< end / 1000 <<" ms" <<std::endl;
            this->writeOut();
        }
        private:
        void constructCalcHessianTasks()
        {
            using hpwl = nt::CalculateOperatorHessianTask<nlp_hpwl_type, hpwl_hessian_trait, EigenMatrix, hpwl_hessian_matrix>;
            using ovl = nt::CalculateOperatorHessianTask<nlp_ovl_type, ovl_hessian_trait, EigenMatrix, ovl_hessian_matrix>;
            using oob = nt::CalculateOperatorHessianTask<nlp_oob_type, oob_hessian_trait, EigenMatrix, oob_hessian_matrix>;
            using asym = nt::CalculateOperatorHessianTask<nlp_asym_type, asym_hessian_trait, EigenMatrix, asym_hessian_matrix>;
            using cos = nt::CalculateOperatorHessianTask<nlp_cos_type, cos_hessian_trait, EigenMatrix, cos_hessian_matrix>;
            using pwl = nt::CalculateOperatorHessianTask<nlp_power_wl_type, power_wl_hessian_trait, EigenMatrix, power_wl_hessian_matrix>;
            auto getIdxFunc = [&](IndexType cellIdx, Orient2DType orient) { return this->plIdx(cellIdx, orient); }; // wrapper the convert cell idx to pl idx
            for (IndexType i = 0; i < this->_hpwlOps.size(); ++i)
            {
                _calcHpwlHessianTasks.emplace_back(hpwl(&this->_hpwlOps[i], &_hessianHpwl, getIdxFunc));
            }
            for (auto &op : this->_ovlOps)
            {
                _calcOvlHessianTasks.emplace_back(ovl(&op, &_hessianOvl, getIdxFunc));
            }
            for (auto &op : this->_oobOps)
            {
                _calcOobHessianTasks.emplace_back(oob(&op, &_hessianOob, getIdxFunc));
            }
            for (auto &op : this->_asymOps)
            {
                _calcAsymHessianTasks.emplace_back(asym(&op, &_hessianAsym, getIdxFunc));
            }
            for (auto &op : this->_cosOps)
            {
                _calcCosHessianTasks.emplace_back(cos(&op, &_hessianCos, getIdxFunc));
            }
            for (auto &op : this->_powerWlOps)
            {
                _calcPowerWlHessianTasks.emplace_back(pwl(&op, &_hessianPowerWl, getIdxFunc));
            }
        }
        void _clearHessian()
        {
            _hessian.setZero();
            _hessianHpwl.setZero();
            _hessianOvl.setZero();
            _hessianOob.setZero();
            _hessianAsym.setZero();
            _hessianCos.setZero();
            _hessianPowerWl.setZero();
        }
        void _calcAllHessians()
        {
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcHpwlHessianTasks.size(); ++i) { _calcHpwlHessianTasks[i].calc(); }
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcOvlHessianTasks.size(); ++i) { _calcOvlHessianTasks[i].calc(); }
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcOobHessianTasks.size(); ++i) { _calcOobHessianTasks[i].calc(); }
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcAsymHessianTasks.size(); ++i) { _calcAsymHessianTasks[i].calc(); }
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcCosHessianTasks.size(); ++i) { _calcCosHessianTasks[i].calc(); }
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < _calcPowerWlHessianTasks.size(); ++i) { _calcPowerWlHessianTasks[i].calc(); }
        }
        void _updateAllHessian()
        {
            #pragma omp parallel for schedule(static)
            for (IndexType i = 0; i < 6; ++i)
            {
                if (i == 0)
                {
                    for (auto & calc : _calcHpwlHessianTasks) { calc.update(); }
                }
                else if (i == 1)
                {
                    for (auto & calc : _calcOvlHessianTasks) { calc.update(); }
                }
                else if (i == 2)
                {
                    for (auto & calc : _calcOobHessianTasks) { calc.update(); }
                }
                else if (i == 3)
                {
                    for (auto & calc : _calcAsymHessianTasks) { calc.update(); }
                }
                else if (i == 4)
                {
                    for (auto & calc : _calcCosHessianTasks) { calc.update(); }
                }
                else
                {
                    for (auto & calc : _calcPowerWlHessianTasks) { calc.update(); }
                }
            }
            _hessian = _hessianHpwl + _hessianOvl + _hessianOob + _hessianAsym + _hessianCos + _hessianPowerWl;
        }

        void clipHessian()
        {
            _hessian = _hessian.cwiseMin(hessianMinBound).cwiseMax(hessianMaxBound);
        }
        virtual void constructTasks() override
        {
            first_order_type::constructTasks();

            this->constructCalcHessianTasks();
        }

        protected:
        hessian_matrix _hessian; ///< The hessian for the objective function
        hpwl_hessian_matrix _hessianHpwl; ///< The hessian for the hpwl function
        ovl_hessian_matrix _hessianOvl; ///< The hessian for the overlapping function
        oob_hessian_matrix _hessianOob; ///< The hessian for the out of boundary function
        asym_hessian_matrix _hessianAsym; ///< The hessian for the asymmetry function
        cos_hessian_matrix _hessianCos; ///< The hessian for the signal path function
        power_wl_hessian_matrix _hessianPowerWl;
        /* Tasks */
        std::vector<nt::CalculateOperatorHessianTask<nlp_hpwl_type, hpwl_hessian_trait, EigenMatrix, hpwl_hessian_matrix>> _calcHpwlHessianTasks; ///< calculate and update the hessian
        std::vector<nt::CalculateOperatorHessianTask<nlp_ovl_type, ovl_hessian_trait, EigenMatrix, ovl_hessian_matrix>> _calcOvlHessianTasks; ///< calculate and update the hessian
        std::vector<nt::CalculateOperatorHessianTask<nlp_oob_type, oob_hessian_trait, EigenMatrix, oob_hessian_matrix>> _calcOobHessianTasks; ///< calculate and update the hessian
        std::vector<nt::CalculateOperatorHessianTask<nlp_asym_type, asym_hessian_trait, EigenMatrix, asym_hessian_matrix>> _calcAsymHessianTasks; ///< calculate and update the hessian
        std::vector<nt::CalculateOperatorHessianTask<nlp_cos_type, cos_hessian_trait, EigenMatrix, cos_hessian_matrix>> _calcCosHessianTasks; ///< calculate and update the hessian
        std::vector<nt::CalculateOperatorHessianTask<nlp_power_wl_type, power_wl_hessian_trait, EigenMatrix, power_wl_hessian_matrix>> _calcPowerWlHessianTasks; ///< calculate and update the hessian


};


template<typename nlp_settings>
inline void NlpGPlacerSecondOrder<nlp_settings>::initSecondOrder()
{
    const IntType size = this->_numVariables;
    DBG("resize hessian to %d \n", size);
    hpwl_hessian_diagonal_selector::resize(_hessianHpwl, size);
    ovl_hessian_diagonal_selector::resize(_hessianOvl, size);
    oob_hessian_diagonal_selector::resize(_hessianOob, size);
    asym_hessian_diagonal_selector::resize(_hessianAsym, size);
    cos_hessian_diagonal_selector::resize(_hessianCos, size);
    power_wl_hessian_diagonal_selector::resize(_hessianPowerWl , size);
}


PROJECT_NAMESPACE_END
#endif //IDEAPLACE_NLPGPLACER_H_
