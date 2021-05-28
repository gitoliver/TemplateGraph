#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP

#include "Node.hpp"
#include "HalfAdjacencyMatrix.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"

#include <map>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace TemplateGraph
{

template<class T>
class Graph
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Graph();
	//TODO: Ensure we would like this functionality, current idea is pass root node then get all traversable from this node and store in our set
	Graph(Node<T> *const&initialNode);
	Graph(std::vector<Node<T>*> const &nodeList);

	//copy constructor
	Graph(const Graph<T> &rhs);

	//move constructor
	Graph(Graph<T> &&rhs);

	//copy assignment
	Graph<T>& operator=(const Graph<T> &rhs);

	//move assignment
	Graph<T>& operator=(Graph<T> &&rhs);

	virtual ~Graph();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	// TODO: Finalize what we would like to pass. Using weak_ptr is nice because we can easily check
	// 			if our node is even useful/alive still but this would get annoying to constantly pass
	// 			weak_ptr<Node<T>>. As of now, for our getNodes I will be using a raw ptr due to the fact
	// 			that there should be no deletions etc. when we run our algos. I will do a check prior to
	// 			returning the vector to ensure that all nodes in our node-list are still alive.
	//
	std::vector<Node<T>*> getNodes();
	HalfAdjacencyMatrix<T> getAdjMatrix() const;

	unsigned int getIndexFromNode(Node<T> *const&queryNode);
	Node<T>* getNodeFromIndex(unsigned int const &queryIndex);
	/************************************************
	 *  MUTATORS
	 ***********************************************/

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	std::string getGraphvizLink();

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	HalfAdjacencyMatrix<T> adjMatrix;
	std::vector<Node<T>*> allNodes;
	// TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
	std::unordered_map<unsigned int, Node<T>*> nodeLookup;
	std::unordered_map<Node<T>*, unsigned int> indexLookup;

	/************************************************
	 *  HELPER FUNCTIONS
	 ***********************************************/
	void populateAdjacencyMatrix();
	void populateLookups();
	void lazyExpiredFixer();

	std::vector<Node<T>*> getReachableNodes(Node<T> *const&startingNode);
	// NOTE: To be used when we are passed solely a root node.
	void getReachableHelper(Node<T> *const&currentNode,
			std::unordered_set<Node<T>*> &visistedNodeSet,
			std::vector<Node<T>*> &reachableNodes);

};

template<class T>
inline Graph<T>::Graph()
{
	badBehavior(__LINE__, __func__,
			"Warning calling default graph constructor");
}

template<class T>
inline Graph<T>::Graph(Node<T> *const&initialNode)
{

	// Lazy way to prevent dupes, cant use method used in get nodes
	// due to weak_ptr being useless in our typical stl algo functions

	std::vector<Node<T>*> tempNodeVec = this->getReachableNodes(initialNode);

	std::unordered_set<Node<T>*> tempNodeSet(tempNodeVec.begin(),
			tempNodeVec.end());

	for (Node<T> *currentNode : tempNodeSet)
	{
		this->allNodes.push_back(currentNode);
	}

	//populate our lookups
	this->populateLookups();
	this->populateAdjacencyMatrix();

}

template<class T>
inline Graph<T>::Graph(std::vector<Node<T>*> const &nodeList)
{
	if (nodeList.size() > 0)
	{
		//Lazy way to prevent dupes, again need to come up with
		//a more efficient way to actually prevent our dupes
		std::unordered_set<Node<T>*> tempNodeSet(nodeList.begin(),
				nodeList.end());
		for (Node<T> *currNode : tempNodeSet)
		{
			this->allNodes.push_back(currNode);
		}

		this->populateLookups();
		this->populateAdjacencyMatrix();
	}
	else
	{
		badBehavior(__LINE__, __func__, "Was passed a nodelist of size 0");
	}
}

template<class T>
inline Graph<T>::~Graph()
{
	lazyInfo(__LINE__, __func__, "Graph deleted");
}

template<class T>
inline std::vector<Node<T>*> Graph<T>::getNodes()
{
	return this->allNodes;
}

template<class T>
inline HalfAdjacencyMatrix<T> Graph<T>::getAdjMatrix() const
{
	return this->adjMatrix;
}

template<class T>
inline void Graph<T>::populateAdjacencyMatrix()
{
	if ((this->allNodes.size() > 0) && (this->indexLookup.size()))
	{
		this->adjMatrix.initializeWorkaround(this->getNodes());
		for (Node<T> *currNode : this->allNodes)
		{
			for (Node<T> *currNodeNeighbor : currNode->getNeighbors())
			{
				if (!(this->adjMatrix.isConnected(this->indexLookup[currNode],
						this->indexLookup[currNodeNeighbor])))
				{
					this->adjMatrix.connect(this->indexLookup[currNode],
							this->indexLookup[currNodeNeighbor]);
				}
			}
		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"no nodes present! cannot populate adj matrix");
	}

}

//NOTE: Must call before we try to create our adj matrix
template<class T>
inline void Graph<T>::populateLookups()
{
	if (this->allNodes.size() > 0)
	{
		this->nodeLookup.clear();
		this->indexLookup.clear();

		int currIndex = 0;
		for (Node<T> *currNode : this->allNodes)
		{

			this->nodeLookup.insert(
			{ currIndex, currNode });

			this->indexLookup.insert(
			{ currNode, currIndex });

			currIndex++;

		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Warning tried to populate lookups with no node list");
	}
}

template<class T>
inline std::vector<Node<T>*> Graph<T>::getReachableNodes(
		Node<T> *const&startingNode)
{
	std::unordered_set<Node<T>*> visitedNodes;
	// TODO: Please note that this current method does increase the size of our call stack a good bit due to the use of recursion.
	// 			Depending on how large of graphs we are dealing with this could become an issue and it may be a better
	// 			call to use a different method.

	std::vector<Node<T>*> reachableVecToReturn;

	this->getReachableHelper(startingNode, visitedNodes, reachableVecToReturn);
	return reachableVecToReturn;
}

//Should be correct. Passing pointer by reference
template<class T>
inline unsigned int Graph<T>::getIndexFromNode(Node<T> *const&queryNode)
{
	return this->indexLookup[queryNode];
}

template<class T>
inline Node<T>* Graph<T>::getNodeFromIndex(unsigned int const &queryIndex)
{
	return this->nodeLookup[queryIndex];
}

// TODO: Find a better way to remove all expired ptrs that will always work
// 			I am worried that just iterating through index will not work well
// 			in all cases. Could just do currIndex-- once we find an expired
// 			and remove it at the end of our current loop iteration tho.
//
template<class T>
inline void Graph<T>::lazyExpiredFixer()
{
	// Possibly a good way, need to run through some tests.
	unsigned int ogSize = this->allNodes.size();
	for (unsigned int currIndex = 0; currIndex < this->allNodes.size();
			currIndex++)
	{
		if (this->allNodes[currIndex].expired())
		{
			this->allNodes.erase(this->allNodes.begin() + currIndex);
			currIndex--;
		}
	}

	//	Extremely heavy way but def works
	//
	// unsigned int ogSize = this->allNodes.size();
	// std::vector<std::weak_ptr<Node<T>>> dustyList = this->allNodes;
	// this->allNodes.clear();
	// for (std::weak_ptr<Node<T>> currDusty : dustyList)
	// {
	// if (!(currDusty.expired()))
	// {
	// this->allNodes.push_back(currDusty);
	// }
	// }
	if (ogSize != this->allNodes.size())
	{
		this->populateLookups();
		this->populateAdjacencyMatrix();
	}
}

template<class T>
inline std::string Graph<T>::getGraphvizLink()
{
	std::string connectionArrow = "%20-%3E%20";
	std::string newLine = "%0A%09";
	std::string baseURL =
			"https://dreampuf.github.io/GraphvizOnline/#digraph%20G%20%7B%0A%09";
	std::string endBracket = "%0A%7D";

	//first make a collection of all of our node connections, just use set cause less chars lazy
	std::map<Node<T>*, std::set<Node<T>*>> nodeNeighs;
	for (Node<T> *currNode : this->getNodes())
	{
		for (Node<T> *currNeigh : currNode->getNeighbors())
		{
			//ensure that the current connection is not already present
			//	We know we do not have a specific connection if either
			// 		A) We do NOT have our neighbor as a key in the node neighs
			// 						OR
			// 		B) If we DO have our neighbor as a key in the node neighs, then we do NOT have
			// 				the currNode as a member
			if ((nodeNeighs.count(currNeigh) == 0)
					|| (nodeNeighs[currNeigh].count(currNode) == 0))
			{
				if (nodeNeighs[currNode].count(currNeigh) == 0)
				{
					nodeNeighs[currNode].insert(currNeigh);
				}
				else
				{
					badBehavior(__LINE__, __func__, "ARGH MATEY");
				}
			}
		} //end 4
	} //end 44

	std::string endURL;

	for (std::pair<Node<T>*, std::set<Node<T>*>> currPair : nodeNeighs)
	{
		for (Node<T> *currNeigh : currPair.second)
		{
			endURL += currPair.first->getName() + "->" + currNeigh->getName()
					+ newLine;
		}
	}
	endURL += endBracket;
	baseURL += endURL;
	return baseURL;
}
/*
 adjMatrix;
 std::vector<Node<T>*> allNodes;
 // TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
 std::unordered_map<unsigned int, Node<T>*> nodeLookup;
 std::unordered_map<Node<T>*, unsigned int> indexLookup;
 */

//copy constructor
template<class T>
inline Graph<T>::Graph(const Graph<T> &rhs) :
		adjMatrix(rhs.adjMatrix), nodeLookup(rhs.nodeLookup), indexLookup(
				rhs.indexLookup), allNodes(rhs.allNodes)
{
}

//move constructor
template<class T>
inline Graph<T>::Graph(Graph<T> &&rhs) :
		adjMatrix(rhs.adjMatrix), nodeLookup(rhs.nodeLookup), indexLookup(
				rhs.indexLookup), allNodes(rhs.allNodes)
{
}

//copy assignment
template<class T>
inline Graph<T>& Graph<T>::operator =(const Graph<T> &rhs)
{
	this->adjMatrix = rhs.adjMatrix;
	this->allNodes = rhs.allNodes;
	this->indexLookup = rhs.indexLookup;
	this->nodeLookup = rhs.nodeLookup;
	return *this;
}

//move assignment
template<class T>
inline Graph<T>& Graph<T>::operator =(Graph<T> &&rhs)
{
	this->adjMatrix = rhs.adjMatrix;
	this->allNodes = rhs.allNodes;
	this->indexLookup = rhs.indexLookup;
	this->nodeLookup = rhs.nodeLookup;
	return *this;
}

template<class T>
inline void Graph<T>::getReachableHelper(Node<T> *const&currentNode,
		std::unordered_set<Node<T>*> &visitedNodeSet,
		std::vector<Node<T>*> &reachableNodes)
{

	reachableNodes.push_back(currentNode);

	visitedNodeSet.insert(currentNode);
	for (Node<T> *currNeighbor : currentNode->getNeighbors())
	{
		if (!(visitedNodeSet.count(currNeighbor)))
		{
			this->getReachableHelper(currNeighbor, visitedNodeSet,
					reachableNodes);
		}
	}
}

}
#endif // TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP
