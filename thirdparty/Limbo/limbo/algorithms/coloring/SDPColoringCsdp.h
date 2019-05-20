/**
 * @file   SDPColoringCsdp.h
 * @brief  graph coloring algorithm based on semidefinite programming (SDP)
 *
 * "Layout decomposition for triple patterning lithography", 
 * Bei Yu, Kun Yuan, Duo Ding, and David Z. Pan, 
 * IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems (TCAD), 34(3):433–446, March 2015.
 *
 * See @ref limbo::algorithms::coloring::SDPColoringCsdp for details. 
 *
 * @author Yibo Lin
 * @date   Sep 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_SDPCOLORINGCSDP
#define LIMBO_ALGORITHMS_COLORING_SDPCOLORINGCSDP

#include <limbo/string/String.h>
#include <limbo/algorithms/coloring/GraphSimplification.h>
#include <limbo/algorithms/coloring/Coloring.h>
#include <limbo/algorithms/coloring/BacktrackColoring.h>
#include <limbo/algorithms/partition/FMMultiWay.h>
#include <limbo/containers/DisjointSet.h>

// as the original csdp easy_sdp api is not very flexible to printlevel
// I made small modification to support that 
#include <limbo/solvers/api/CsdpEasySdpApi.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

/// @class limbo::algorithms::coloring::SDPColoringCsdp
/// Thread control is not available as Csdp does not provide any API for that. \n
/// 
/// SDP formulation from Bei Yu's TCAD 2015 paper \cite TPL_TCAD2015_Yu 
/// 
/// \f{eqnarray*}{
/// & min. & C X, \\
/// & s.t. & x_{ii} = 1, \forall i \in V, \\
/// &      & x_{ij} \ge -0.5, \forall (i, j) \in E, \\
/// &      & X \succeq 0, \textrm{(PSD)}. 
/// \f}
/// Note that Csdp only solves equality problem for constraints, 
/// so it is necessary to introduce slack variables for each conflict edge.
/// The total number of variables are N = (vertex number + conflict edge number).
/// The variable matrix has dimension NxN. 
/// 
/// Based on the solution of X, we group vertices and construct merged graph. 
/// Then we color the merged graph. 
/// 
/// Edge weight is used to differentiate conflict edge and stitch edge.  
/// Non-negative weight implies conflict edge. 
/// Negative weight implies stitch edge. 
/// 
/// @tparam GraphType graph_type 
template <typename GraphType>
class SDPColoringCsdp : public Coloring<GraphType>
{
	public:
        /// @nowarn
		typedef Coloring<GraphType> base_type;
		using typename base_type::graph_type;
		using typename base_type::graph_vertex_type;
		using typename base_type::graph_edge_type;
		using typename base_type::vertex_iterator_type;
		using typename base_type::edge_iterator_type;
        using typename base_type::edge_weight_type;
		using typename base_type::ColorNumType;
        typedef typename base_type::EdgeHashType edge_hash_type;
        /// @endnowarn

        /// @class limbo::algorithms::coloring::SDPColoringCsdp::FMGainCalcType
        /// compute the gain when moving a vertex from one partition to another 
        struct FMGainCalcType
        {
            /// define edge_weight_type as value_type 
            typedef edge_weight_type value_type;
            graph_type const& graph; ///< graph 

            /// constructor 
            /// @param g graph 
            FMGainCalcType(graph_type const& g) : graph(g) {}
            /// compute the gain when moving a vertex from one partition to another 
            /// @param v vertex 
            /// @param origp original partition 
            /// @param newp new partition 
            /// @param vPartition array of partition for each vertex  
            /// @return gain 
            edge_weight_type operator()(int32_t v, int8_t origp, int8_t newp, std::vector<int8_t> const& vPartition) const
            {
                typedef typename boost::graph_traits<graph_type>::out_edge_iterator out_edge_iterator_type;
                out_edge_iterator_type ei, eie; 
                // if v is not partitioned, then any partition will have preassigned large gain 
                edge_weight_type gain = (origp >= 0)? 0 : boost::num_edges(graph)*boost::num_vertices(graph);
                for (boost::tie(ei, eie) = boost::out_edges(v, graph); ei != eie; ++ei)
                {
                    graph_vertex_type t = boost::target(*ei, graph);
                    int8_t pt = vPartition[t];
#ifdef DEBUG_SDPCOLORING
                    assert((int32_t)t != v);
#endif
                    // skip unpartitioned vertex 
                    if (pt < 0) continue;
                    edge_weight_type w = boost::get(boost::edge_weight, graph, *ei);
                    // assume origp != newp, pt >= 0 
                    gain += (pt == newp)? -w : (pt == origp)? w : 0;
                    //gain += w * ((vPartition[t] == origp && origp >= 0) - (vPartition[t] == newp));
                }
                return gain;
            }
        };
		/// constructor
        /// @param g graph 
		SDPColoringCsdp(graph_type const& g); 
		/// destructor
		virtual ~SDPColoringCsdp() {}

        /// for debug 
        /// write sdp solution to file 
        /// @param filename file name 
        /// @param X variable matrix X 
        void write_sdp_sol(std::string const& filename, struct blockmatrix const& X) const; 
        /// print data in blockrec 
        /// @param label to mark what \a block is used for 
        /// @param block compact representation of a matrix 
        void print_blockrec(const char* label, blockrec const& block) const; 
	protected:
        /// kernel coloring algorithm 
		/// @return objective value 
		virtual double coloring();

        /// helper functions 
        /// construct blockrec in C for objective 
        /// @param C objective matrix C 
        /// @param blocknum block number 
        /// @param blocksize size of block 
        /// @param blockcategory which type of compact representation, MATRIX or DIAG 
        void construct_objectve_blockrec(blockmatrix& C, int32_t blocknum, int32_t blocksize, blockcat blockcategory) const; 
        /// construct sparseblock for constraint 
        /// @param blocknum block number 
        /// @param blocksize size of block 
        /// @param constraintnum constraint number 
        /// @param entrynum number of entries 
        struct sparseblock* construct_constraint_sparseblock(int32_t blocknum, int32_t blocksize, int32_t constraintnum, int32_t entrynum) const; 
        /// set entry for sparseblock 
        /// @param block target block 
        /// @param entryid entry id 
        /// @param i, j indices 
        /// @param value data value 
        void set_sparseblock_entry(struct sparseblock& block, int32_t entryid, int32_t i, int32_t j, double value) const; 
        /// Round sdp solution. 
        /// Based on the solution of X, we group vertices and construct merged graph. 
        /// Then we color the merged graph. 
        /// @param X variable matrix X 
        void round_sol(struct blockmatrix const& X);
        /// coloring merged graph 
        /// @param mg graph 
        /// @param vMColor coloring solutions 
        void coloring_merged_graph(graph_type const& mg, std::vector<int8_t>& vMColor) const;
        /// use different algorithms to color merged graph 
        /// @param g graph 
        /// @param vColor coloring solutions 
        void coloring_algos(graph_type const& g, std::vector<int8_t>& vColor) const;
        /// use @ref limbo::algorithms::coloring::BacktrackColoring to color merged graph 
        /// @param mg graph 
        /// @param vColor coloring solutions 
        virtual void coloring_by_backtrack(graph_type const& mg, std::vector<int8_t>& vColor) const;
        /// use @ref limbo::algorithms::partition::FMMultiWay to color merged graph 
        /// @param mg graph 
        /// @param vColor coloring solutions 
        virtual void coloring_by_FM(graph_type const& mg, std::vector<int8_t>& vColor) const;

        double m_rounding_lb; ///< if SDP solution x < m_rounding_lb, take x as -0.5
        double m_rounding_ub; ///< if SDP solution x > m_rounding_ub, take x as 1.0
        const static uint32_t max_backtrack_num_vertices = 7; ///< maximum number of graph size that @ref limbo::algorithms::coloring::BacktrackColoring can handle
};

template <typename GraphType>
SDPColoringCsdp<GraphType>::SDPColoringCsdp(SDPColoringCsdp<GraphType>::graph_type const& g) 
    : base_type(g)
{
    m_rounding_lb = -0.4;
    m_rounding_ub = 0.9;
}

template <typename GraphType>
double SDPColoringCsdp<GraphType>::coloring()
{
    // Since Csdp is written in C, the api here is also in C 
    // Please refer to the documation of Csdp for different notations 
    // basically, X is primal variables, C, b, constraints and pobj are all for primal 
    // y, Z, and dobj are for dual problem 
    //
    // Csdp has very complex storage structure for matrix 
    // I still do not have a full understanding about the block concept, especially blocks.blocksize 
    // with some reverse engineering, for the coloring problem here, matrices in C, b, and constraints mainly consists of 2 blocks 
    // the first block is for vertex variables, and the second block is for slack variables introduced to resolve '>=' operators in the constraints

    assert_msg(!this->has_precolored(), "SDP coloring does not support precolored layout yet");
    // The problem and solution data.
    struct blockmatrix C; // objective matrix 
    double *b; // right hand side of constraints
    struct constraintmatrix *constraints; // constraint matrices
    // Storage for the initial and final solutions.
    struct blockmatrix X,Z;
    double *y;
    double pobj,dobj;

    // iterators used to traverse through the graph 
    vertex_iterator_type vi, vie; 
    edge_iterator_type ei, eie; 
    // compute total number of vertices and edges 
    uint32_t num_vertices = boost::num_vertices(this->m_graph);
    uint32_t num_edges = boost::num_edges(this->m_graph);
    // compute total number of conflict edges and stitch edges 
    uint32_t num_conflict_edges = 0;
    uint32_t num_stitch_edges = 0;
    for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
    {
        if (this->edge_weight(*ei) >= 0) // conflict edge 
            num_conflict_edges += 1;
        else // stitch edge 
            num_stitch_edges += 1;
    }
    assert_msg(num_edges > 0 && num_conflict_edges > 0, "no edges or conflict edges found, no need to solve SDP");
    // compute total number of variables and constraints
    uint32_t num_variables = num_vertices+num_conflict_edges;
    uint32_t num_constraints = num_conflict_edges+num_vertices;

    // setup blockmatrix C 
    C.nblocks = 2;
    C.blocks = (struct blockrec *)malloc((C.nblocks+1)*sizeof(struct blockrec));
    assert_msg(C.blocks, "Couldn't allocate storage for C");
    // C.blocks[0] is not used according to the example of Csdp
    // block 1 for vertex variables 
    construct_objectve_blockrec(C, 1, num_vertices, MATRIX);
    for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
    {
        graph_vertex_type s = boost::source(*ei, this->m_graph);
        graph_vertex_type t = boost::target(*ei, this->m_graph);
        // 1 for conflict edge, -alpha for stitch edge 
        // add unary negative operator, because Csdp solves maximization problem 
        // but we are solving minimization problem 
        edge_weight_type alpha = (this->edge_weight(*ei) >= 0)? -1 : this->stitch_weight();
        // variable starts from 1 instead of 0 in Csdp
        s += 1; t += 1;
        int32_t idx1 = ijtok(s,t,C.blocks[1].blocksize);
        int32_t idx2 = ijtok(t,s,C.blocks[1].blocksize);
        C.blocks[1].data.mat[idx1] = alpha; 
        C.blocks[1].data.mat[idx2] = alpha;
    }
    // block 2 for slack variables 
    // this block is all 0s, so we use diagonal format to represent  
    construct_objectve_blockrec(C, 2, num_conflict_edges, DIAG);
#ifdef DEBUG_SDPCOLORING
    print_blockrec("C.blocks[1].data.mat", C.blocks[1]);
    print_blockrec("C.blocks[2].data.vec", C.blocks[2]);
#endif

    // setup right hand side of constraints b
    // the order is first for conflict edges and then for vertices  
    // the order matters for constraint matrices 
    b = (double *)malloc((num_constraints+1)*sizeof(double));
    assert_msg(b, "Failed to allocate storage for right hand side of constraints b");
    // -1/(k-1) according to Bei Yu's DAC2014 paper
    // consider in the constraints, xij+xji >= beta, so beta should be -2/(k-1)
    double beta = -2.0/(this->color_num()-1.0); // right hand side of constraints for conflict edges 
    for (uint32_t i = 1, ie = num_constraints+1; i != ie; ++i)
    {
        if (i <= num_conflict_edges) // slack for each conflict edge, xij >= -0.5
            b[i] = beta;
        else // slack for each vertex, xii = 1
            b[i] = 1;
    }

    // setup constraint matrix constraints
    // the order should be the same as right hand side b 
    constraints=(struct constraintmatrix *)malloc((num_constraints+1)*sizeof(struct constraintmatrix));
    assert_msg(constraints, "Failed to allocate storage for constraints");
    // for conflict edges, xij 
    uint32_t cnt = 1;
    for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
    {
        if (this->edge_weight(*ei) >= 0) // conflict edge 
        {
            graph_vertex_type s = boost::source(*ei, this->m_graph);
            graph_vertex_type t = boost::target(*ei, this->m_graph);
            if (s > t) // due to symmetry, only need to create constraint matrices for upper-matrix 
                std::swap(s, t);
            // variable starts from 1 instead of 0 in Csdp
            s += 1; t += 1;
            struct constraintmatrix& constr = constraints[cnt];
            // Terminate the linked list with a NULL pointer.
            constr.blocks = NULL;
            // inverse order to initialize blocks, because linked list will reverse the order 
            // first set block 2 for diagonal values and then block 1 for upper-matrix values  
            for (uint32_t i = 2; i != 0; --i)
            {
                struct sparseblock* blockptr;
                if (i == 1) // block 1, vertex variables  
                {
                    blockptr = construct_constraint_sparseblock(i, num_vertices, cnt, 1);
                    set_sparseblock_entry(*blockptr, 1, s, t, 1);
                }
                else // block 2, slack variables 
                {
                    blockptr = construct_constraint_sparseblock(i, num_conflict_edges, cnt, 1);
                    set_sparseblock_entry(*blockptr, 1, cnt, cnt, -1);
                }
                // insert block to linked list 
                blockptr->next = constr.blocks;
                constr.blocks = blockptr;
            }
            ++cnt;
        }
    }
    // for vertices, xii 
    for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
    {
        graph_vertex_type v = *vi;
        v += 1;
        struct constraintmatrix& constr = constraints[cnt];
        // Terminate the linked list with a NULL pointer.
        constr.blocks = NULL;
        struct sparseblock* blockptr = construct_constraint_sparseblock(1, num_vertices, cnt, 1);
        set_sparseblock_entry(*blockptr, 1, v, v, 1);
        // insert block to linked list 
        blockptr->next = constr.blocks;
        constr.blocks = blockptr;
        ++cnt;
    }

#ifdef DEBUG_SDPCOLORING
    write_prob((char*)"problem.sdpa", num_variables, num_constraints, C, b, constraints);
#endif

    // after all configuration ready 
    // start solving sdp 
    // set initial solution 
    initsoln(num_variables, num_constraints, C, b, constraints, &X, &y, &Z);
    // use default params 
    // only set printlevel to zero 
    struct paramstruc params; 
    int printlevel;
    initparams(&params, &printlevel);
//#ifndef DEBUG_SDPCOLORING
    printlevel = 0;
//#endif
    // A return code for the call to easy_sdp().
    // solve sdp 
    // objective value is (dobj+pobj)/2
    //int ret = easy_sdp(num_variables, num_constraints, C, b, constraints, 0.0, &X, &y, &Z, &pobj, &dobj);
    int ret = limbo::solvers::easy_sdp_ext<int>(num_variables, num_constraints, C, b, constraints, 0.0, &X, &y, &Z, &pobj, &dobj, params, printlevel);
    assert_msg(ret == 0, "SDP failed");

    // round result to get colors 
    round_sol(X);

    // Free storage allocated for the problem and return.
    free_prob(num_variables, num_constraints, C, b, constraints, X, y, Z);

#ifdef DEBUG_LPCOLORING
    this->write_graph("final_output");
#endif
    // return objective value 
    //return (dobj+pobj)/2;
    return this->calc_cost(this->m_vColor);
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::construct_objectve_blockrec(blockmatrix& C, int32_t blocknum, int32_t blocksize, blockcat blockcategory) const 
{
    struct blockrec& cblock = C.blocks[blocknum];
    cblock.blocksize = blocksize;
    cblock.blockcategory = blockcategory;
    if (blockcategory == MATRIX)
    {
        cblock.data.mat = (double *)malloc(blocksize*blocksize*sizeof(double));
        assert_msg(cblock.data.mat, "Couldn't allocate storage for cblock.data.mat");
        // initialize to all 0s
        std::fill(cblock.data.mat, cblock.data.mat+blocksize*blocksize, 0); 
    }
    else if (blockcategory == DIAG)
    {
        cblock.data.vec = (double *)malloc((blocksize+1)*sizeof(double));
        assert_msg(cblock.data.vec, "Couldn't allocate storage for cblock.data.vec");
        // initialize to all 0s
        std::fill(cblock.data.vec, cblock.data.vec+blocksize+1, 0); 
    }
}

template <typename GraphType>
struct sparseblock* SDPColoringCsdp<GraphType>::construct_constraint_sparseblock(int32_t blocknum, int32_t blocksize, int32_t constraintnum, int32_t entrynum) const 
{
    struct sparseblock* blockptr = (struct sparseblock *)malloc(sizeof(struct sparseblock));
    assert_msg(blockptr, "Allocation of constraint block failed for blockptr");
    blockptr->blocknum = blocknum;
    blockptr->blocksize = blocksize;
    blockptr->constraintnum = constraintnum;
    blockptr->next = NULL;
    blockptr->nextbyblock = NULL;
    blockptr->entries = (double *) malloc((entrynum+1)*sizeof(double));
    assert_msg(blockptr->entries, "Allocation of constraint block failed for blockptr->entries");
    blockptr->iindices = (int *) malloc((entrynum+1)*sizeof(int));
    assert_msg(blockptr->iindices, "Allocation of constraint block failed for blockptr->iindices");
    blockptr->jindices = (int *) malloc((entrynum+1)*sizeof(int));
    assert_msg(blockptr->jindices, "Allocation of constraint block failed for blockptr->jindices");
    blockptr->numentries = entrynum;

    return blockptr;
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::set_sparseblock_entry(struct sparseblock& block, int32_t entryid, int32_t i, int32_t j, double value) const 
{
    block.iindices[entryid] = i;
    block.jindices[entryid] = j;
    block.entries[entryid] = value;
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::round_sol(struct blockmatrix const& X)
{
#ifdef DEBUG_SDPCOLORING
    write_sdp_sol("problem.sol", X);
#endif
    // merge vertices with SDP solution with disjoint set 
    std::vector<graph_vertex_type> vParent (boost::num_vertices(this->m_graph));
    std::vector<uint32_t> vRank (vParent.size());
    typedef limbo::containers::DisjointSet disjoint_set_type;
    disjoint_set_type::SubsetHelper<graph_vertex_type, uint32_t> gp (vParent, vRank);
    // check SDP solution in X 
    // we are only interested in block 1 
    struct blockrec const& block = X.blocks[1];
    assert_msg(block.blockcategory == MATRIX, "mismatch of block category");
    for (int32_t i = 1; i <= block.blocksize; ++i)
        for (int32_t j = i+1; j <= block.blocksize; ++j)
        {
            double ent = block.data.mat[ijtok(i, j, block.blocksize)];
            if (ent > m_rounding_ub) // merge vertices if SDP solution rounded to 1.0
                disjoint_set_type::union_set(gp, i-1, j-1); // Csdp array starts from 1 instead of 0
        }
    // construct merged graph 
    // for vertices in merged graph 
    std::vector<graph_vertex_type> vG2MG (vParent.size(), std::numeric_limits<graph_vertex_type>::max()); // mapping from graph to merged graph
    uint32_t mg_count = 0; // count number of vertices in merged graph 
    for (uint32_t i = 0, ie = vParent.size(); i != ie; ++i) // check subset elements and compute mg_count
        if (vParent[i] == i)
            vG2MG[i] = mg_count++;
    for (uint32_t i = 0, ie = vParent.size(); i != ie; ++i) // check other elements 
        if (vParent[i] != i)
            vG2MG[i] = vG2MG.at(disjoint_set_type::find_set(gp, i));
#ifdef DEBUG_SDPCOLORING
    assert(mg_count == disjoint_set_type::count_sets(gp));
#endif
    graph_type mg (mg_count); // merged graph 
    // for edges in merged graph 
    edge_iterator_type ei, eie; 
    for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
    {
        graph_edge_type const& e = *ei;
        graph_vertex_type s = boost::source(e, this->m_graph);
        graph_vertex_type t = boost::target(e, this->m_graph);
        graph_vertex_type ms = vG2MG.at(s);
        graph_vertex_type mt = vG2MG.at(t);
        std::pair<graph_edge_type, bool> me = boost::edge(ms, mt, mg);
        // need to consider if this setting is still reasonable when stitch is on 
        edge_weight_type w = (this->edge_weight(e) >= 0)? 1 : -this->stitch_weight();
        if (me.second) // already exist, update weight 
            w += boost::get(boost::edge_weight, mg, me.first);
        else // not exist, add edge 
            me = boost::add_edge(ms, mt, mg);
        boost::put(boost::edge_weight, mg, me.first, w);
#ifdef DEBUG_SDPCOLORING
        assert(boost::get(boost::edge_weight, mg, me.first) != 0);
#endif
    }
#ifdef DEBUG_SDPCOLORING
    //this->print_edge_weight(this->m_graph);
    //this->check_edge_weight(this->m_graph, this->stitch_weight()/10, 4);
    //this->print_edge_weight(mg);
    this->check_edge_weight(mg, this->stitch_weight()/10, boost::num_edges(this->m_graph));
#endif
    // coloring for merged graph 
    std::vector<int8_t> vMColor (mg_count, -1); // coloring solution for merged graph 
    coloring_merged_graph(mg, vMColor);
    // apply coloring solution from merged graph to graph 
    // first, map colors to subsets 
    vertex_iterator_type vi, vie; 
    for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
    {
        graph_vertex_type v = *vi;
        this->m_vColor[v] = vMColor.at(vG2MG.at(v));
#ifdef DEBUG_SDPCOLORING
        assert(this->m_vColor[v] >= 0 && this->m_vColor[v] < this->color_num());
#endif
    }
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::coloring_merged_graph(graph_type const& mg, std::vector<int8_t>& vMColor) const
{
    uint32_t num_vertices = boost::num_vertices(mg);
    // if small number of vertices or no vertex merged, no need to simplify graph 
    if (num_vertices <= max_backtrack_num_vertices || num_vertices == boost::num_vertices(this->m_graph)) 
        coloring_algos(mg, vMColor);
    else 
    {
        // simplify merged graph 
        typedef GraphSimplification<graph_type> graph_simplification_type;
        graph_simplification_type gs (mg, this->color_num());
        gs.simplify(graph_simplification_type::HIDE_SMALL_DEGREE);

        // in order to recover color from articulation points 
        // we have to record all components and mappings 
        // but graph is not necessary 
        std::vector<std::vector<int8_t> > mSubColor (gs.num_component());
        std::vector<std::vector<graph_vertex_type> > mSimpl2Orig (gs.num_component());
        for (uint32_t sub_comp_id = 0; sub_comp_id < gs.num_component(); ++sub_comp_id)
        {
            graph_type sg;
            std::vector<int8_t>& vSubColor = mSubColor[sub_comp_id];
            std::vector<graph_vertex_type>& vSimpl2Orig = mSimpl2Orig[sub_comp_id];

            gs.simplified_graph_component(sub_comp_id, sg, vSimpl2Orig);

            vSubColor.assign(boost::num_vertices(sg), -1);

#ifdef DEBUG_SDPCOLORING
            this->write_graph("initial_merged_graph", sg, vSubColor);
#endif
            // solve coloring 
            coloring_algos(sg, vSubColor);
#ifdef DEBUG_SDPCOLORING
            this->write_graph("final_merged_graph", sg, vSubColor);
#endif
        }

        // recover color assignment according to the simplification level set previously 
        // HIDE_SMALL_DEGREE needs to be recovered manually for density balancing 
        gs.recover(vMColor, mSubColor, mSimpl2Orig);

        // recover colors for simplified vertices without balanced assignment 
        gs.recover_hide_small_degree(vMColor);
    }
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::coloring_algos(graph_type const& g, std::vector<int8_t>& vColor) const
{
    if (boost::num_vertices(g) <= max_backtrack_num_vertices)
        coloring_by_backtrack(g, vColor);
    else 
        coloring_by_FM(g, vColor);
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::coloring_by_backtrack(SDPColoringCsdp<GraphType>::graph_type const& mg, std::vector<int8_t>& vColor) const
{
    // currently backtrack coloring is used 
    // TO DO: add faster coloring approach like FM partition based 
    BacktrackColoring<graph_type> bc (mg);
    bc.stitch_weight(1); // already scaled in edge weights 
    bc.color_num(this->color_num());
    bc();
    for (uint32_t i = 0, ie = vColor.size(); i != ie; ++i)
        vColor[i] = bc.color(i);
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::coloring_by_FM(SDPColoringCsdp<GraphType>::graph_type const& mg, std::vector<int8_t>& vColor) const
{
    limbo::algorithms::partition::FMMultiWay<FMGainCalcType> fmp (FMGainCalcType(mg), boost::num_vertices(mg), this->color_num());
    fmp.set_partitions(vColor.begin(), vColor.end());
    fmp();
    for (uint32_t i = 0, ie = vColor.size(); i != ie; ++i)
        vColor[i] = fmp.partition(i);
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::write_sdp_sol(std::string const& filename, struct blockmatrix const& X) const 
{
    // compute dimensions of matrix 
    uint32_t matrix_size = 0;
    for (int32_t blk = 1; blk <= X.nblocks; ++blk)
        matrix_size += X.blocks[blk].blocksize;

    // collect data from X and store to mSol 
    std::vector<std::vector<double> > mSol (matrix_size, std::vector<double>(matrix_size, 0.0));
    // as i and j starts from 1, set index_offset to -1 
    int32_t index_offset = 0; 
    for (int32_t blk = 1; blk <= X.nblocks; ++blk)
    {
        switch (X.blocks[blk].blockcategory)
        {
            case DIAG:
                for (int32_t i = 1; i <= X.blocks[blk].blocksize; ++i)
                {
                    double ent = X.blocks[blk].data.vec[i];
                    if (ent != 0.0)
                        mSol[index_offset+i-1][index_offset+i-1] = ent;
                };
                break;
            case MATRIX:
                for (int32_t i = 1; i <= X.blocks[blk].blocksize; ++i)
                    for (int32_t j = i; j <= X.blocks[blk].blocksize; ++j)
                    {
                        double ent = X.blocks[blk].data.mat[ijtok(i, j, X.blocks[blk].blocksize)];
                        if (ent != 0.0)
                            mSol[index_offset+i-1][index_offset+j-1] = ent;
                    };
                break;
            case PACKEDMATRIX:
            default: assert_msg(0, "Invalid Block Type");
        }
        index_offset += X.blocks[blk].blocksize; 
    }

    // write to file 
    std::ofstream out (filename.c_str());
    assert_msg(out.good(), "failed to open file " << filename << " for write");
    for (std::vector<std::vector<double> >::const_iterator it1 = mSol.begin(), it1e = mSol.end(); it1 != it1e; ++it1)
    {
        const char* prefix = "";
        for (std::vector<double>::const_iterator it2 = it1->begin(), it2e = it1->end(); it2 != it2e; ++it2)
        {
            out << prefix << *it2;
            prefix = ",";
        }
        out << std::endl;
    }
    out.close();
}

template <typename GraphType>
void SDPColoringCsdp<GraphType>::print_blockrec(const char* label, blockrec const& block) const 
{
    printf("%s", label);
    if (block.blockcategory == MATRIX)
    {
        printf("[M]: "); // show it is a matrix 
        for (int32_t i = 0, ie = block.blocksize*block.blocksize; i != ie; ++i)
            printf("%g ", block.data.mat[i]);
    }
    else if (block.blockcategory == DIAG)
    {
        printf("[V]: "); // show it is a vector 
        for (int32_t i = 0; i != block.blocksize; ++i)
            printf("%g ", block.data.vec[i+1]);
    }
    printf("\n");
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
