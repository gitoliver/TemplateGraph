#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Node.hpp"
#include "HalfAdjacencyMatrix.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GenericObject/includes/GenericObject.hpp"

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

	~Graph();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	/* TODO: Finalize what we would like to pass. Using weak_ptr is nice because we can easily check
	 * 			if our node is even useful/alive still but this would get annoying to constantly pass
	 * 			weak_ptr<Node<T>*>. As of now, for our getNodes I will be using a raw ptr due to the fact
	 * 			that there should be no deletions etc. when we run our algos. I will do a check prior to
	 * 			returning the vector to ensure that all nodes in our node-list are still alive.
	 */
	std::vector<Node<T>*> getNodes() const;
	HalfAdjacencyMatrix<T> getAdjMatrix() const;

	unsigned int getIndexFromNode(Node<T> *const&queryNode);
	Node<T>* getNodeFromIndex(unsigned int const &queryIndex);
	/************************************************
	 *  MUTATORS
	 ***********************************************/

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	void chuckRottenTomatoes();

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	std::vector<std::weak_ptr<Node<T>>> tomatoGarden;

	HalfAdjacencyMatrix<T> adjMatrix;
	//TODO: Whenever we find a broken node then we can just run a refresh on all structures. Basically run bfs initializer again.
	std::unordered_set<Node<T>*> allNodes;
	// TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
	std::unordered_map<unsigned int, Node<T>*> nodeLookup;
	std::unordered_map<Node<T>*, unsigned int> indexLookup;

	/************************************************
	 *  HELPER FUNCTIONS
	 ***********************************************/
	void populateAdjacencyMatrix();
	void populateLookups();

	std::vector<Node<T>*> getReachableNodes(Node<T> *const&startingNode);
	// NOTE: To be used when we are passed solely a root node.
	void getReachableHelper(Node<T> *const&currentNode,
			std::unordered_set<Node<T>*> &visistedNodeSet,
			std::vector<Node<T>*> &reachableNodes);

};

template<class T>
Graph<T>::Graph()
{
	badBehavior(__LINE__, __func__,
			"Warning calling default graph constructor");
}

template<class T>
Graph<T>::Graph(Node<T> *const&initialNode)
{
	//NOTE: Verbose for now to prevent dupes && make sure we dont screw anything up
	std::vector<Node<T>*> tempNodeVec = this->getReachableNodes(initialNode);

	for (Node<T> *currentNode : tempNodeVec)
	{
		this->allNodes.insert(currentNode);
	}

	//populate our garden

	this->populateLookups();

	this->populateAdjacencyMatrix();

	for (unsigned int i = 0; i < this->allNodes.size(); i++)
	{
		std::cout << this->nodeLookup[i]->getName() + "\n\t";
		for (unsigned int j = 0; j < this->allNodes.size(); j++)
		{
			if (this->adjMatrix.isConnected(i, j))
			{
				std::cout << this->nodeLookup[j]->getName() + ", ";
			}
		}
		std::cout << "\n";
	}

}

template<class T>
Graph<T>::Graph(std::vector<Node<T>*> const &nodeList)
{
	if (nodeList.size() > 0)
	{
		for (std::weak_ptr<Node<T>> currNode : nodeList)
		{
			std::shared_ptr<Node<T>> lockedNode = currNode.lock();
			if (lockedNode)
			{
				if (!(this->allNodes.count(currNode)))
				{
					this->allNodes.insert(currNode);
				}
				else
				{
					badBehavior(__LINE__, __func__,
							"Tried to insert duplicate node named <"
									+ lockedNode->getName()
									+ "> into our all node list");
				}
			}
			else
			{
				badBehavior(__LINE__, __func__,
						"Was unable to lock our current node to place it in our set of nodes");
			}
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
Graph<T>::~Graph()
{
	lazyInfo(__LINE__, __func__, "Graph deleted");
}

template<class T>
std::vector<Node<T>*> Graph<T>::getNodes() const
{
	std::vector<Node<T>*> nodesToReturn;
	if (this->allNodes.size() > 0)
	{
		for (Node<T> *currNode : this->allNodes)
		{
			nodesToReturn.push_back(currNode);
		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Tried to get nodes but allNodes is empty");
	}
	return nodesToReturn;
}

template<class T>
HalfAdjacencyMatrix<T> Graph<T>::getAdjMatrix() const
{
	return this->adjMatrix;
}

template<class T>
void Graph<T>::populateAdjacencyMatrix()
{
	if ((this->allNodes.size() > 0) && (this->indexLookup.size()))
	{
		this->adjMatrix.initializeWorkaround(this->getNodes());
		for (Node<T> *currNode : this->allNodes)
		{
			for (std::weak_ptr<Node<T>> currNeighbor : currNode->getNeighbors())
			{
				std::shared_ptr<Node<T>> lockedNeighbor = currNeighbor.lock();
				if (lockedNeighbor)
				{
					if (!(this->adjMatrix.isConnected(
							this->indexLookup[currNode],
							this->indexLookup[lockedNeighbor.get()])))
					{
						this->adjMatrix.connect(this->indexLookup[currNode],
								this->indexLookup[lockedNeighbor.get()]);
					}
				}
				else
				{
					badBehavior(__LINE__, __func__,
							"Could not lock our neighbor");
				}
			}
		}

	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Warning tried to populate adjacency matrix with no node list");
	}
}

//NOTE: Must call before we try to create our adj matrix
template<class T>
void Graph<T>::populateLookups()
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
std::vector<Node<T>*> Graph<T>::getReachableNodes(Node<T> *const&startingNode)
{
	std::unordered_set<Node<T>*> visitedNodes;
	/* TODO: Please note that this current method does increase the size of our call stack a good bit due to the use of recursion.
	 * 			Depending on how large of graphs we are dealing with this could become an issue and it may be a better
	 * 			call to use a different method.
	 */

	std::vector<Node<T>*> reachableVecToReturn;
	this->getReachableHelper(startingNode, visitedNodes, reachableVecToReturn);
	return reachableVecToReturn;
}

template<class T>
void Graph<T>::chuckRottenTomatoes()
{
	unsigned int ogGardenSize = this->tomatoGarden.size();
	std::vector<std::weak_ptr<Node<T>>> garbageCan;

	for (unsigned int gardenIndex = 0; gardenIndex < ogGardenSize;
			gardenIndex++)
	{
		if (this->tomatoGarden[gardenIndex].expired())
		{
			this->tomatoGarden.erase(this->tomatoGarden.begin() + gardenIndex);
		}
	}

	//for (std::weak_ptr<Node<T>> currWeakNode : this->tomatoGarden)
	//{
	//	if (currWeakNode.expired())
	//	{
	/*
	 * ERROR HERE!!!
	 *
	 */

	//TODO: May break when actually used. We will see.
	//this->tomatoGarden.erase(
	//		std::remove(this->tomatoGarden.begin(),
	//				this->tomatoGarden.end(), currWeakNode),
	//		this->tomatoGarden.end());
	//	}
	//}
	if (ogGardenSize != this->tomatoGarden.size())
	{
		this->populateLookups();
		this->populateAdjacencyMatrix();
	}
}

//Should be correct. Passing pointer by reference
template<class T>
unsigned int Graph<T>::getIndexFromNode(Node<T> *const&queryNode)
{
	return this->indexLookup[queryNode];
}

template<class T>
Node<T>* Graph<T>::getNodeFromIndex(unsigned int const &queryIndex)
{
	return this->nodeLookup[queryIndex];
}

template<class T>
void Graph<T>::getReachableHelper(Node<T> *const&currentNode,
		std::unordered_set<Node<T>*> &visitedNodeSet,
		std::vector<Node<T>*> &reachableNodes)
{
	currentNode->getNeighbors();
	reachableNodes.push_back(currentNode);
	visitedNodeSet.insert(currentNode);
	for (std::weak_ptr<Node<T>> currNeighbor : currentNode->getNeighbors())
	{
		std::shared_ptr<Node<T>> lockedNeighbor = currNeighbor.lock();
		if (lockedNeighbor)
		{
			if (!(visitedNodeSet.count(lockedNeighbor.get())))
			{
				this->getReachableHelper(lockedNeighbor.get(), visitedNodeSet,
						reachableNodes);
			}
		}
		else
		{
			badBehavior(__LINE__, __func__,
					"Could not lock neighbor when trying to traverse");
		}
	}

}

}
#endif // GRAPH_HPP
