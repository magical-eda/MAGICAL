/**
 * @file   test_FM.cpp
 * @brief  test FM partitioning algorithms 
 * @author Yibo Lin
 * @date   Feb 2015
 */

#include <iostream>
#include <limbo/algorithms/partition/FM.h>

using std::cout;
using std::endl;

/// a class to describe graph vertex 
class Node 
{
	public:
        /// @nowarn
		typedef char tie_id_type;
		typedef int weight_type;
        /// @endnowarn

        /// constructor 
        /// @param w node weight 
        /// @param id node label  
		Node(weight_type const& w, tie_id_type const& id) 
		{
			m_weight = w;
			m_id = id;
		}

        /// @return node label  
		tie_id_type tie_id() const {return m_id;}
        /// @return node weight 
		weight_type weight() const {return m_weight;}

	protected:
		tie_id_type m_id; ///< node label  
		weight_type m_weight; ///< node weight 
};

/// main function \n
/// construct a graph and verify @ref limbo::algorithms::partition::FM
/// @return 0 
int main()
{
	array<Node*, 8> vNode;
	vNode[0] = new Node (1, 'a');
	vNode[1] = new Node (1, 'b');
	vNode[2] = new Node (1, 'c');
	vNode[3] = new Node (1, 'd');
	vNode[4] = new Node (1, 'e');
	vNode[5] = new Node (1, 'f');
	vNode[6] = new Node (1, 'g');
	vNode[7] = new Node (1, 'h');

	limbo::algorithms::partition::FM<Node, double> fm; 
	for (unsigned int i = 0; i < vNode.size(); ++i)
	{
#if 1
		if (i < 4)
			fm.add_node(vNode[i], 0);
		else 
			fm.add_node(vNode[i], 1);
#else
		if (i < 4)
			fm.add_node(vNode[i], i%2);
		else 
			fm.add_node(vNode[i], i%2);
#endif
	}

	// net n1
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[0]);
		vNodeNet.push_back(vNode[1]);
		vNodeNet.push_back(vNode[2]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	// net n2
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[1]);
		vNodeNet.push_back(vNode[3]);
		vNodeNet.push_back(vNode[4]);
		vNodeNet.push_back(vNode[5]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	// net n3
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[2]);
		vNodeNet.push_back(vNode[5]);
		vNodeNet.push_back(vNode[6]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	// net n4
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[0]);
		vNodeNet.push_back(vNode[6]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	// net n5
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[3]);
		vNodeNet.push_back(vNode[4]);
		vNodeNet.push_back(vNode[7]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	// net n6
	{
		vector<Node*> vNodeNet;
		vNodeNet.push_back(vNode[5]);
		vNodeNet.push_back(vNode[7]);
		fm.add_net(1, vNodeNet.begin(), vNodeNet.end());
		//fm.add_net(1.0/(vNodeNet.size()-1), vNodeNet.begin(), vNodeNet.end());
	}
	fm.print();
	fm.print_connection();
	fm(3.0/5, 5.0/3);
	fm.print();
	fm.print_connection();

	return 0;
}
