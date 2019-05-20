/**
 * @file   FMMultiWay.h
 * @brief  Implementation of the Multi-way FM partitioning algorithm 
 *
 * Refer to Fiduccia and Mattheyses,
 * "A Linear-time Heuristics for Improving Network Partitions", DAC 1982
 *
 * @author Yibo Lin
 * @date   Sep 2015
 */

#ifndef LIMBO_ALGORITHMS_PARTITION_FMMULTIWAY_H
#define LIMBO_ALGORITHMS_PARTITION_FMMULTIWAY_H

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Partition
namespace partition 
{

/// @class limbo::algorithms::partition::FMMultiWay
/// Assume vertices are represented by 0, 1, ... N array.  
/// Partitions are 0, 1, ... P array. 
/// Negative partition id denotes no partition assigned.  
/// @tparam GainCalcType a function object that calculate gains, refer to @ref limbo::algorithms::coloring::SDPColoringCsdp::FMGainCalcType for example.  
template <typename GainCalcType>
class FMMultiWay
{
    public:
        /// @nowarn 
        typedef GainCalcType gain_calc_type;
        typedef typename gain_calc_type::value_type gain_value_type;
        /// @endnowarn

        /// @class limbo::algorithms::partition::FMMultiWay::VertexMove
        /// @brief a class denotes movement of vertex 
        struct VertexMove
        {
            int vertex; ///< vertex id 
            signed char orig_partition; ///< original partition 
            gain_value_type gain; ///< gain 
            /// @brief constructor 
            /// @param v vertex 
            /// @param op original partition
            /// @param g gain value 
            VertexMove(int v, int op, gain_value_type g) : vertex(v), orig_partition(op), gain(g) {}
        };

        /// @brief constructor 
        /// @param gc function object of gain calculator 
        /// @param tn number of vertices 
        /// @param tp number of partitions 
        FMMultiWay(gain_calc_type const& gc, int tn, signed char tp)
            : m_gain_calc(gc)
            , m_num_vertice(tn)
            , m_num_partitions(tp)
            , m_vPartition(tn, -1)
            , m_vFixed(tn, false)
        {
            m_vVertexMove.reserve(tn);
        }
        /// @brief set vertex \a v to partition \a p 
        /// @param v vertex 
        /// @param p partition 
        void set_partition(int v, signed char p) {m_vPartition[v] = p;}
        /// @brief set partitions 
        /// @tparam Iterator iterator to the array of partitions 
        /// @param first, last begin and end iterator to the array of partitions 
        template <typename Iterator>
        void set_partitions(Iterator first, Iterator last) {m_vPartition.assign(first, last);}
        /// @brief get partition of a vertex 
        /// @param v vertex 
        /// @return partition 
        signed char partition(int v) const {return m_vPartition[v];}

        /// @brief API to run the algorithm 
        void operator()() {return run();}
    protected:
        /// @brief kernel function to run the algorithm 
        void run();
        /// traverse all vertices and partitions for the candidate with best gain 
        /// @param cv find vertex to move 
        /// @param cp target partition 
        /// @param max_gain best gain of the movement 
        void find_candidate(int& cv, signed char& cp, gain_value_type& max_gain) const;
        /// find best kth movement 
        /// @param k index of movement 
        /// @param improve cumulative improvement at kth movement 
        void best_kth_move(int& k, gain_value_type& improve) const;
        /// revert to kth movement 
        /// @param k index of movement 
        void revert_to_kth_move(int k);
        /// reset movements and fixed flags 
        void reset();

        gain_calc_type const& m_gain_calc; ///< function object to calculate gains 
        int m_num_vertice; ///< total number of vertices 
        int m_num_partitions; ///< total number of partitions 
        std::vector<signed char> m_vPartition; ///< an array storing partition of each vertex 
        std::vector<bool> m_vFixed; ///< whehter fixed during current iteration 
        std::vector<VertexMove> m_vVertexMove; ///< record vertex movement during each iteration 
};

template <typename GainCalcType>
void FMMultiWay<GainCalcType>::find_candidate(int& cv, signed char& cp, FMMultiWay<GainCalcType>::gain_value_type& max_gain) const
{
    max_gain = -std::numeric_limits<gain_value_type>::max();

    for (int v = 0; v != m_num_vertice; ++v)
    {
        // skip fixed vertex 
        if (m_vFixed[v]) continue; 
        for (signed char p = 0; p != m_num_partitions; ++p)
        {
            // skip its own partition
            if (p == m_vPartition[v]) continue;
            gain_value_type cur_gain = m_gain_calc(v, m_vPartition[v], p, m_vPartition);
            if (cur_gain > max_gain)
            {
                cv = v;
                cp = p;
                max_gain = cur_gain;
            }
        }
    }
}

template <typename GainCalcType>
void FMMultiWay<GainCalcType>::best_kth_move(int& k, FMMultiWay<GainCalcType>::gain_value_type& improve) const
{
    k = -1;
    improve = 0;
    gain_value_type sum = 0;
    for (int i = 0; i != m_num_vertice; ++i)
    {
        sum += m_vVertexMove[i].gain;
        if (sum > improve)
        {
            improve = sum;
            k = i;
        }
    }
}

template <typename GainCalcType>
void FMMultiWay<GainCalcType>::revert_to_kth_move(int k)
{
    for (int i = m_num_vertice-1; i > k; --i)
        m_vPartition[m_vVertexMove[i].vertex] = m_vVertexMove[i].orig_partition;
}

template <typename GainCalcType>
void FMMultiWay<GainCalcType>::reset()
{
    m_vFixed.assign(m_vFixed.size(), false);
    m_vVertexMove.clear();
}

template <typename GainCalcType>
void FMMultiWay<GainCalcType>::run()
{
    gain_value_type improve = 0;
    do 
    {
        for (int i = 0; i != m_num_vertice; ++i)
        {
            int v = -1;
            signed char p = -1;
            gain_value_type gain = 0; 
            find_candidate(v, p, gain);
            assert(v >= 0);

            // collect move 
            m_vVertexMove.push_back(VertexMove(v, m_vPartition[v], gain));
            // apply 
            m_vPartition[v] = p;
            m_vFixed[v] = true;
        }
        int k = -1;
        best_kth_move(k, improve);
        revert_to_kth_move(k);
        reset();
    } while (improve > 0);
}

} // namespace partition
} // namespace algorithms 
} // namespace limbo

#endif
