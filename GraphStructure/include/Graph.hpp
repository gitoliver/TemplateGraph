#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Node.hpp"
#include "HalfAdjacencyMatrix.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GenericObject/includes/GenericObject.hpp"

#include <unordered_map>
#include <unordered_set>

namespace TemplateGraph
{

//TODO: Do we want to have this be a generic object also so we can name it, etc.? May have to switch all keys to Node<T>* instead of shared, smh
template<class T>
class Graph
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Graph();
	//TODO: Ensure we would like this functionality, current idea is pass root node then get all traversable from this node and store in our set
	Graph(std::weak_ptr<Node<T>*> initialNode);
	Graph(std::vector<std::weak_ptr<Node<T>*>> nodeList);

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
	//TODO: Whenever we find a broken node then we can just run a refresh on all structures. Basically run bfs initializer again.
	std::unordered_set<std::weak_ptr<Node<T>*>> allNodes;
	// TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
	std::unordered_map<unsigned int, std::weak_ptr<Node<T>*>> nodeLookup;
	std::unordered_map<std::weak_ptr<Node<T>*>, unsigned int> indexLookup;

	/************************************************
	 *  HELPER FUNCTIONS
	 ***********************************************/
	void populateAdjacencyMatrix();
	void populateLookups();

	std::vector<std::weak_ptr<Node<T>*>> getReachableNodes(
			std::weak_ptr<Node<T>*> startingNode);
	// NOTE: To be used when we are passed solely a root node.
	void getReachableHelper(std::weak_ptr<Node<T>*> currentNode,
			std::unordered_set<std::shared_ptr<Node<T>*>> &visistedNodeSet,
			std::vector<std::weak_ptr<Node<T>*>> &reachableNodes);

};

template<class T>
Graph<T>::Graph()
{
	badBehavior(__LINE__, __func__,
			"Warning calling default graph constructor");
}

template<class T>
Graph<T>::Graph(std::weak_ptr<Node<T>*> initialNode)
{

}

template<class T>
Graph<T>::Graph(std::vector<std::weak_ptr<Node<T>*> > nodeList)
{
	if (nodeList.size() > 0)
	{
		for (std::weak_ptr<Node<T>*> currNode : nodeList)
		{
			std::shared_ptr<Node<T>*> lockedNode = currNode.lock();
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
	if (this->allNodes.size > 0)
	{

		for (std::weak_ptr<Node<T>*> currNode : this->allNodes)
		{
			std::shared_ptr<Node<T>*> lockedNode = currNode.lock();
			if (lockedNode)
			{
				nodesToReturn.insert(lockedNode);
			}
			else
			{
				badBehavior(__LINE__, __func__,
						"Was unable to lock our current node while trying to place into return vec");
			}
		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Tried to get nodes but allNodes is empty");
	}
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
		for (Node<T> *currNode : this->getNodes())
		{
			for (Node<T> *currNeighbor : currNode->getNeighbors())
			{
				if (!(this->adjMatrix.isConnected(this->indexLookup[currNode],
						this->indexLookup[currNeighbor])))
				{
					this->adjMatrix.connect(this->indexLookup[currNode],
							this->indexLookup[currNeighbor]);
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

template<class T>
void Graph<T>::populateLookups()
{
	if (this->allNodes.size() > 0)
	{
		this->nodeLookup.clear();
		this->indexLookup.clear();
		int currIndex = 0;
		for (std::weak_ptr<Node<T>*> currNode : this->allNodes)
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
std::vector<std::weak_ptr<Node<T>*> > Graph<T>::getReachableNodes(
		std::weak_ptr<Node<T>*> startingNode)
{
	std::unordered_set<std::shared_ptr<Node<T>*>> visitedNodes;
	/* TODO: Please note that this current method does increase the size of our call stack a good bit due to the use of recursion.
	 * 			Depending on how large of graphs we are dealing with this could become an issue and it may be a better
	 * 			call to use a different method.
	 */
	std::vector<std::weak_ptr<Node<T>*>> reachableVecToReturn;
	this->getReachableHelper(startingNode, visitedNodes, reachableVecToReturn);
	return reachableVecToReturn;
}

template<class T>
void Graph<T>::getReachableHelper(std::weak_ptr<Node<T>*> currentNode,
		std::unordered_set<std::shared_ptr<Node<T>*>> &visitedNodeSet,
		std::vector<std::weak_ptr<Node<T>*>> &reachableNodes)
{
	std::shared_ptr<Node<T>*> lockedNode = currentNode.lock();
	if (lockedNode)
	{
		reachableNodes.push_back(lockedNode);
		visitedNodeSet.insert(lockedNode);
		for (std::weak_ptr<Node<T>*> currNeighbor : lockedNode->getNeighbors())
		{
			std::shared_ptr<Node<T>*> lockedNeighbor = currNeighbor.lock();
			if (lockedNeighbor)
			{
				if (!(visitedNodeSet.count(lockedNeighbor)))
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
	else
	{
		badBehavior(__LINE__, __func__,
				"Could not lock node when we were traversing all nodes");
	}
}

}
#endif // GRAPH_HPP
