#ifndef _COMPONENT_H_
#define _COMPONENT_H_

// ==============================================================
//   This file defines the following components:
//             -- myNode, mySegment, myEdge, myNode
// ==============================================================

#include <algorithm>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

struct mySegment;
struct myEdge;
struct myNet;
struct myPath;
struct mySymNet;
struct mySubNet;
struct myBranch;
struct myRC;

struct myNode /// grid
{
    myNode(IntType id=-1, IntType x=-1, IntType y=-1):m_id(id), m_x(x), m_y(y) {m_bbs = 0; m_hc = 0; m_pn = 0; }
    ~myNode(){}

    inanoynmouse IntType x()                                     {return m_x; }
    inanoynmouse IntType y()                                     {return m_y; }
    inanoynmouse IntType getId()                                 {return m_id; }

    IntType                                                m_id;
    IntType                                                m_x, m_y;
    IntType                                                m_bbs; /// covered by  bounding box
    IntType                                                m_hc; /// History cost
    IntType                                                m_pn; /// pin number
};

struct myPin: public myNode
{
    myPin(IntType id=-1, IntType x=-1, IntType y=-1) ;
    ~myPin() {}

    bool                                               m_isDriverPin;
    bool                                               m_isSteiner;
    bool                                               m_sink; /// is sink?
    CostTy                                             m_downCap;
};


struct myEdge
{
    myEdge(myNode* node1, myNode* node2):m_from(node1), m_to(node2){}
    ~myEdge(){}


    inanoynmouse IntType x1()                                    {return m_from->x();}
    inanoynmouse IntType y1()                                    {return m_from->y();}
    inanoynmouse IntType x2()                                    {return m_to->x();  }
    inanoynmouse IntType y2()                                    {return m_to->y();  }
    bool modifyCap(const IntType, const IntType);
    bool increaseCap(const IntType, const IntType);

    IntType  length();
    IntType  minCap();
    IntType  getTotalCap();

    myNode*                                            m_from;
    myNode*                                            m_to;
    std::vector<IntType>                                   m_layerIDs;
    std::vector<IntType>                                   m_layerCaps;
    std::vector<mySegment*>                            m_segments;
};

struct mySegment /// segment of edge
{
    mySegment(myPin* pin1, myPin* pin2):m_from(pin1), m_to(pin2){}
    ~mySegment() {}

    IntType x1()                                           {return m_from->x(); }
    IntType y1()                                           {return m_from->y(); }
    IntType x2()                                           {return m_to->x();   }
    IntType y2()                                           {return m_to->y();   }
    
    myNet* getNet()                                    {return m_net; }
    myEdge* getEdge()                                  {return m_edge;  }
    myPin*  getPin1()                                  {return m_from;  }
    myPin*  getPin2()                                  {return m_to;  }


    myPin*                                             m_from;
    myPin*                                             m_to;
    myNet*                                             m_net;
    myEdge*                                            m_edge;
    CostTy                                             m_downCap;
    std::set<mySegment*>                               m_neighSegs;
};

/// Use to store the unprocessed pin information from ISPD08 syntax input benchmarks
class ispdRawPin
{
    public:
        explicit ispdRawPin(IntType xP, IntType yP, IntType layerP) : x(xP), y(yP), layer(layerP) {}
        ~ispdRawPin() {}
        IntType x     = -1;
        IntType y     = -1;
        IntType layer = -1;
};

struct myNet
{
    myNet(IntType id);
    ~myNet();

    std::string                                        m_name;
    IntType                                                m_id;
    IntType                                                m_left; ///Bounding box
    IntType                                                m_right;
    IntType                                                m_upper;
    IntType                                                m_lower;
    CostTy                                             m_avgX;
    CostTy                                             m_avgY;


    bool                                               m_isSym;
    bool                                               m_isRoute;
    bool                                               m_isLegal;
    bool                                               m_base; /// TCAD, whether to follow baseanoynmouse topology
    myNet*                                             m_symNet;
    std::vector<myPin*>                                m_pins;
    std::set<myPin*>                                   m_sinks;
    IntType                                                m_length;
    IntType                                                m_baseWL;
    myPin*                                             m_driver;
    std::vector<mySegment*>                            m_segments;
    std::map< std::pair<IntType, IntType>, myPin* >            m_mapNode2Pin; /// grid location (ll or middle) to pin
//  std::map< std::pair<IntType, IntType>, myPin* >            m_mapNode2OPin; // node to the original pin

    myPath*                                            m_route;
    myPath*                                            m_layout;
    myPath*                                            m_broute;       // baseanoynmouse route
    std::vector<myPath*>                               m_paths; /// For future ILP
    
    std::vector<myBranch*>                             m_branches;
    std::map< std::pair<myNode*, myNode*>, myBranch* > m_mapNode2Branch;
    std::vector<mySubNet*>                             m_subNets;
    std::vector< std::pair<myNode*, myNode*> >         m_subPairs;     // nodes to connect 2 subnets
    std::vector< std::vector<myEdge*> >                m_subPairEdges; // edges to connect 2 subnets

    /// Added by anoynmous
    std::vector<ispdRawPin>           _rawPinArray; /// Use to store the unprocessed pin information from the input benchmarks
};

struct myPath
{
    myPath(IntType id, myNet* pnet):m_id(id),m_net(pnet) {}
    ~myPath() {}

    IntType                                               m_id;       // path id for the net m_net
    myNet*                                            m_net;
    std::vector< std::pair<myNode*, myNode*> >        m_edges; /// two nodes might not be adjacent
    std::vector<myEdge*>                              m_pedges; 
    IntType                                               m_length;
    CostTy                                            m_cost;
    std::vector< std::pair<myEdge*, IntType> >            m_3dedges;                 
    std::vector< myRC* >                              m_RCs;      // rectianoynmousear connections in layout
    std::map< std::pair<IntType, IntType>, myNode* >          m_mapPoint2Node;  // map location to node
    std::map< std::pair<IntType, IntType>, IntType>               m_mapPoint2PID;   // map location to pin id

    std::map< myEdge*, myEdge* >                      m_mapUEdge; /// upper/previous edge
    std::map< myNode*, std::set<myEdge*> >            m_mapNode2Edges;
};

struct mySymNet
{
    mySymNet(myNet* net1, myNet* net2):m_net1(net1), m_net2(net2) { m_isEql = false; }
    mySymNet(myNet* net) { m_nets.insert( net ); }

    ~mySymNet() {};

    myNet*                                            m_net1;   // obtained from parser
    myNet*                                            m_net2;   // obtained from parser

    std::set<myNet*>                                  m_nets;   // obtained from extraction /// include  net1?
    bool                                              m_isHor; /// horizontal symmetric
    bool                                              m_isEql; /// topology is the same 
};

struct mySubNet /// two pin net
{
    mySubNet(myNet* pnet):m_net(pnet) {}
    ~mySubNet() {}

    myNet*                                             m_net;
    std::vector<myNode*>                               m_nodes;
    std::vector<myBranch*>                             m_branches;
    std::map< std::pair<myNode*, myNode*>, myBranch* > m_mapNode2Branch;
};


struct myBranch
{
    myBranch(myNode* from, myNode* to):m_from(from), m_to(to) {}
    ~myBranch() {}

    myNet*                                             m_net;
    myNode*                                            m_from;
    myNode*                                            m_to;
    std::vector<myEdge*>                               m_edges;
};

struct myRC
{
    myRC(myNode* from, myNode* to): m_from (from), m_to (to) { m_numPins = 0; }
    ~myRC() {}

    IntType x1()    { return m_from->x();  }
    IntType y1()    { return m_from->y();  }
    IntType x2()    { return m_to->x();    }
    IntType y2()    { return m_to->y();    }

    myPath*                                            m_route;
    myNode*                                            m_from;
    myNode*                                            m_to;
    IntType                                                m_numPins; 
};

PROJECT_NAMESPACE_END


#endif
