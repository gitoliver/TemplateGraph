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
	Graph(std::shared_ptr<Node<T>>const &initialNode);
	Graph(std::vector<Node<T>*> const &nodeList);

	~Graph();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	/* TODO: Finalize what we would like to pass. Using weak_ptr is nice because we can easily check
	 * 			if our node is even useful/alive still but this would get annoying to constantly pass
	 * 			weak_ptr<Node<T>>. As of now, for our getNodes I will be using a raw ptr due to the fact
	 * 			that there should be no deletions etc. when we run our algos. I will do a check prior to
	 * 			returning the vector to ensure that all nodes in our node-list are still alive.
	 */
	std::vector<Node<T>*> getRawNodes(); // const;
	HalfAdjacencyMatrix<T> getAdjMatrix() const;

	unsigned int getIndexFromNode(Node<T> *const&queryNode);
	Node<T>* getNodeFromIndex(unsigned int const &queryIndex);
	/************************************************
	 *  MUTATORS
	 ***********************************************/

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	HalfAdjacencyMatrix<T> adjMatrix;
	std::vector<std::weak_ptr<Node<T>>> allNodes;
	// TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
	std::unordered_map<unsigned int, Node<T>*> nodeLookup;
	std::unordered_map<Node<T>*, unsigned int> indexLookup;

	/************************************************
	 *  HELPER FUNCTIONS
	 ***********************************************/
	void populateAdjacencyMatrix();
	void populateLookups();
	void lazyExpiredFixer();

	std::vector<std::shared_ptr<Node<T>>> getReachableNodes(
			std::shared_ptr<Node<T>>const &startingNode);
	// NOTE: To be used when we are passed solely a root node.
	void getReachableHelper(std::shared_ptr<Node<T>> const &currentNode,
			std::unordered_set<Node<T>*> &visistedNodeSet,
			std::vector<std::shared_ptr<Node<T>>> &reachableNodes);

};

template<class T>
Graph<T>::Graph()
{
	badBehavior(__LINE__, __func__,
			"Warning calling default graph constructor");
}

template<class T>
Graph<T>::Graph(std::shared_ptr<Node<T>>const &initialNode)
{
	/* Lazy way to prevent dupes, cant use method used in get nodes
	 * due to weak_ptr being useless in our typical stl algo functions
	 */

	std::vector<std::shared_ptr<Node<T>>> tempNodeVec = this->getReachableNodes(
			initialNode);

	std::unordered_set<std::shared_ptr<Node<T>>> tempNodeSet(
			tempNodeVec.begin(), tempNodeVec.end());

	for (std::shared_ptr<Node<T>> currentNode : tempNodeSet)
	{
		this->allNodes.push_back(currentNode);
	}

	//populate our lookups
	this->populateLookups();

	this->populateAdjacencyMatrix();

}

template<class T>
Graph<T>::Graph(std::vector<Node<T>*> const &nodeList)
{
	if (nodeList.size() > 0)
	{

		//We dont need to do weak ptr here. Doesnt matter.
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
std::vector<Node<T>*> Graph<T>::getRawNodes() // const
{
	std::vector<Node<T>*> nodeVecToReturn;
	for (std::weak_ptr<Node<T>> currWeakNode : this->allNodes)
	{
		if (currWeakNode.expired())
		{
			//TODO: update everything
			badBehavior(__LINE__, __func__,
					"HIT EXPIRED NODE NEED TO UPDATE EVERYTHING");
			this->lazyExpiredFixer();
			return this->getRawNodes();
		}
		else
		{
			std::shared_ptr<Node<T>> lockedNode = currWeakNode.lock();
			if (lockedNode)
			{
				nodeVecToReturn.push_back(lockedNode.get());
			}
			else
			{
				badBehavior(__LINE__, __func__,
						"COULDNT LOCK OUR PTR TO PUT IN NODE VECTOR");
			}
		}
	}

	//ensure all unique listings
	std::sort(nodeVecToReturn.begin(), nodeVecToReturn.end());
	nodeVecToReturn.erase(
			std::unique(nodeVecToReturn.begin(), nodeVecToReturn.end()),
			nodeVecToReturn.end());

	return nodeVecToReturn;
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
		this->adjMatrix.initializeWorkaround(this->getRawNodes());
		for (std::weak_ptr<Node<T>> currWeakNode : this->allNodes)
		{
			if (currWeakNode.expired())
			{
				this->lazyExpiredFixer();
				goto calledExpiredFixer;
			}
			else
			{
				std::shared_ptr<Node<T>> currLocked = currWeakNode.lock();
				if (currLocked)
				{
					for (std::weak_ptr<Node<T>> currWeakNeighbor : currLocked.get()->getNeighbors())
					{
						if (currWeakNeighbor.expired())
						{
							this->lazyExpiredFixer();
							goto calledExpiredFixer;
						}
						else
						{
							std::shared_ptr<Node<T>> lockedNeighbor =
									currWeakNeighbor.lock();
							if (lockedNeighbor)
							{
								if (!(this->adjMatrix.isConnected(
										this->indexLookup[currLocked.get()],
										this->indexLookup[lockedNeighbor.get()])))
								{
									this->adjMatrix.connect(
											this->indexLookup[currLocked.get()],
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
				} // end if currLocked
				else
				{
					badBehavior(__LINE__, __func__,
							"Could not lock our curr node");
				}
			} //end else for not expired

		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Warning tried to populate adjacency matrix with no node list");
	}
	//just using the int to make compiler happy
	if (false)
	{
		calledExpiredFixer: lazyInfo(__LINE__, __func__,
				"We called our expired fixer");
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
		for (std::weak_ptr<Node<T>> currWeakNode : this->allNodes)
		{
			if (currWeakNode.expired())
			{
				badBehavior(__LINE__, __func__,
						"Warning, found an expired node");
				this->lazyExpiredFixer();
				break;
			}
			else
			{
				std::shared_ptr<Node<T>> currLockedNode = currWeakNode.lock();
				if (currLockedNode)
				{
					this->nodeLookup.insert(
					{ currIndex, currLockedNode.get() });

					this->indexLookup.insert(
					{ currLockedNode.get(), currIndex });

					currIndex++;
				}
				else
				{
					badBehavior(__LINE__, __func__,
							"Couldnt lock our current node");
					//TODO: How to handle correctly?
				}
			}

		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Warning tried to populate lookups with no node list");
	}
}

template<class T>
std::vector<std::shared_ptr<Node<T>>> Graph<T>::getReachableNodes(
		std::shared_ptr<Node<T>> const &startingNode)
{
	std::unordered_set<Node<T>*> visitedNodes;
	/* TODO: Please note that this current method does increase the size of our call stack a good bit due to the use of recursion.
	 * 			Depending on how large of graphs we are dealing with this could become an issue and it may be a better
	 * 			call to use a different method.
	 */

	std::vector<std::shared_ptr<Node<T>>> reachableVecToReturn;
	this->getReachableHelper(startingNode, visitedNodes, reachableVecToReturn);
	return reachableVecToReturn;
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

/* TODO: Find a better way to remove all expired ptrs that will always work
 * 			I am worried that just iterating through index will not work well
 * 			in all cases. Could just do currIndex-- once we find an expired
 * 			and remove it at the end of our current loop iteration tho.
 */

template<class T>
void Graph<T>::lazyExpiredFixer()
{

	/*
	 * Possibly a good way, need to run through some tests.
	 */
	unsigned int ogSize = this->allNodes.size();

	for (unsigned int currIndex = 0; currIndex < this->allNodes.size(); currIndex++)
	{
		if (this->allNodes[currIndex].expired())
		{
			this->allNodes.erase(this->allNodes.begin() + currIndex);
			currIndex--;
		}
	}

	/*	Extremely heavy way but def works
	 *
	 *
	 unsigned int ogSize = this->allNodes.size();

	 std::vector<std::weak_ptr<Node<T>>> dustyList = this->allNodes;

	 this->allNodes.clear();

	 for (std::weak_ptr<Node<T>> currDusty : dustyList)
	 {
	 if (!(currDusty.expired()))
	 {
	 this->allNodes.push_back(currDusty);
	 }
	 }
	 */
	if (ogSize != this->allNodes.size())
	{
		this->populateLookups();
		this->populateAdjacencyMatrix();
	}
}

template<class T>
void Graph<T>::getReachableHelper(std::shared_ptr<Node<T>> const &currentNode,
		std::unordered_set<Node<T>*> &visitedNodeSet,
		std::vector<std::shared_ptr<Node<T>>> &reachableNodes)
{
	currentNode->getNeighbors();
	reachableNodes.push_back(currentNode);
	visitedNodeSet.insert(currentNode.get());
	for (std::weak_ptr<Node<T>> currNeighbor : currentNode->getNeighbors())
	{
		std::shared_ptr<Node<T>> lockedNeighbor = currNeighbor.lock();
		if (lockedNeighbor)
		{
			if (!(visitedNodeSet.count(lockedNeighbor.get())))
			{
				this->getReachableHelper(lockedNeighbor, visitedNodeSet,
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
