#ifndef NODE_HPP
#define NODE_HPP

#include "./Edge.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GenericObject/includes/GenericObject.hpp"

#include <memory>

namespace TemplateGraph
{
template<class T>
class Node: public Abstract::GenericObject, public std::enable_shared_from_this<
		Node<T>>

{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	//TODO: Check if we need the default constructors. I feel that they should never be called
	Node();
	Node(std::string inName) :
			GenericObject{ inName }
	{
	}
	;
	Node(std::string name, std::string label);

	~Node();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	std::vector<std::weak_ptr<Node<T>>> getNeighbors();

	/* TODO: Maybe we WANT to use a shared_ptr for our edges, I am worried regarding having an easy
	 * 			check for if returned are actually decent/usable edges. I will leave it up to divine
	 * 			intervention to know if we must worry about this.
	 */
	std::vector<Edge<T>*> getEdges();
	std::vector<Edge<T>*> getOutEdges();
	std::vector<Edge<T>*> getInEdges();

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	/* TODO: Finalize how we would like to add nodes to one another. The addNeighbor will just be a wrapper for our addChild. Please
	 * 			note how I avoided having an "addEdge" because from my understanding and what I am pretty sure our use will be every
	 * 			single edge is guaranteed to have both a source and sink node.
	 *
	 */
	void addNeighbor(std::string edgeName,
			std::shared_ptr<Node<T>> newNeighbor);

	//add on check to make sure we are only adding 1 connection between our nodes. Do we want to worry about adding a parent?
	void addChild(std::string edgeName, std::shared_ptr<Node<T>> childNode);
	//NOTE: We MUST remove the edge from out "child" node BEFORE deleting the edge by deleting the unique_ptr that owns it.
	void removeEdgeBetween(std::shared_ptr<Node<T>> otherNode);

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool isNeighbor(std::shared_ptr<Node<T>> otherNode);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	/* TODO: Ensure that unique_ptr is our best option here, I am going to code it and then think more.
	 * 			Also we may want to switch to unordered_set or something since order does not matter and
	 * 			lookup times are faster. For now I will leave as vector but this must be discussed.
	 */
	std::vector<std::unique_ptr<Edge<T>*>> outEdges;
	std::vector<Edge<T>*> inEdges;

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool isChildOf(std::shared_ptr<Node<T>> possibleParent);
	bool isParentOf(std::shared_ptr<Node<T>> possibleChild);

	/* Separated these out incase we eventually want to worry about ownership. They could use weak_ptr instead
	 * 		but this allows me to hold off on checking locks when putting both into our neighbors vector. Casting
	 * 		is automatic.
	 *
	 */
	std::vector<std::shared_ptr<Node<T>>> getChildren();
	std::vector<std::shared_ptr<Node<T>>> getParents();

	Edge<T>* getConnectingEdge(std::shared_ptr<Node<T>> otherNode);

	friend class Edge<T> ;

};

template<class T>
Node<T>::Node()
{
	this->setName("INVALID NODE");
	badBehavior(__LINE__, __func__, "We called the default node constructor");
}

template<class T>
Node<T>::Node(std::string name, std::string label)
{
	this->setName(name);
	this->setLabels(label);
}

template<class T>
Node<T>::~Node()
{
	lazyInfo(__LINE__, __func__, "Taking care of raw ptrs");
	for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
	{
		lazyInfo(__LINE__, __func__, "Okay lol");
		//currOutEdge->getSinkNode();
		//currOutEdge.get()->getSinkNode();
		//std::weak_ptr<Node<T>> *childNode = currOutEdge.get()->getSinkNode();
		//childNode->inEdges.erase(
		//		std::remove(childNode->inEdges.begin(),
		//				childNode->inEdges.end(), currOutEdge.get()));
	}
	lazyInfo(__LINE__, __func__,
			"Done with raw ptrs, now just let naturally destruct");
}

template<class T>
std::vector<std::weak_ptr<Node<T>> > Node<T>::getNeighbors()
{
	std::vector<std::weak_ptr<Node<T>>> childrenVec;
	std::vector<std::weak_ptr<Node<T>>> parentsVec;
	parentsVec.insert(parentsVec.end(), childrenVec.begin(), childrenVec.end());
	return parentsVec;
}

template<class T>
std::vector<Edge<T>*> Node<T>::getEdges()
{
	std::vector<Edge<T>*> outEdgesVec = this->getOutEdges();
	std::vector<Edge<T>*> inEdgesVec = this->getInEdges();
	outEdgesVec.insert(outEdgesVec.end(), inEdgesVec.begin(), inEdgesVec.end());
	return outEdgesVec;
}

template<class T>
std::vector<Edge<T>*> Node<T>::getOutEdges()
{
	std::vector<Edge<T>*> outEdgeVecToReturn;
	for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
	{
		outEdgeVecToReturn.push_back(currOutEdge.get());
	}
	return outEdgeVecToReturn;
}

template<class T>
std::vector<Edge<T>*> Node<T>::getInEdges()
{
	std::vector<Edge<T>*> inEdgeVecToReturn;
	for (Edge<T> *currInEdge : this->inEdges)
	{
		inEdgeVecToReturn.push_back(currInEdge);
	}
	return inEdgeVecToReturn;
}

template<class T>
void Node<T>::addNeighbor(std::string edgeName,
		std::shared_ptr<Node<T>> newNeighbor)
{
	this->addChild(edgeName, newNeighbor);
}

template<class T>
void Node<T>::addChild(std::string edgeName,
		std::shared_ptr<Node<T>> childNode)
{
	if (this->isNeighbor(childNode))
	{
		badBehavior(__LINE__, __func__,
				"Trying to make create an edge between two nodes that are already neighbors");
	}
	else
	{
		this->outEdges.push_back(
				std::make_unique<Edge<T>*>(edgeName, this->shared_from_this(),
						childNode));
		childNode.get()->inEdges.push_back(this->outEdges.back().get());
	}
}

template<class T>
void Node<T>::removeEdgeBetween(std::shared_ptr<Node<T>> otherNode)
{
	if (this->isNeighbor(otherNode))
	{
		if (this->isChildOf(otherNode))
		{
			Edge<T> *edgeToRemove = this->getConnectingEdge(otherNode);
			this->inEdges.erase(
					std::remove(this->inEdges.begin(), this->inEdges.end(),
							edgeToRemove), this->inEdges.end());
			//TODO: Decide if we would like to switch over to sets...Timing tests are comming.
			unsigned int currOutIndex = 0;
			for (std::unique_ptr<Edge<T>*> const &currOtherOutEdge : otherNode.get()->outEdges)
			{
				if (currOtherOutEdge.get() == edgeToRemove)
					otherNode.get()->outEdges.erase(
							otherNode.get()->outEdges.erase.begin
									+ currOutIndex);
				currOutIndex++;
			}
		}
		else if (this->isParentOf(otherNode))
		{
			otherNode.get()->removeEdge(this);
		}
		else
		{
			badBehavior(__LINE__, __func__,
					"Tried to remove an edge for some reason it isnt a parent or child");
		}
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Tried to remove an edge between two nodes that are not connected");
	}
}

template<class T>
bool Node<T>::isNeighbor(std::shared_ptr<Node<T>> otherNode)
{
	return (this->isChildOf(otherNode) || this->isParentOf(otherNode));
}

template<class T>
bool Node<T>::isChildOf(std::shared_ptr<Node<T>> possibleParent)
{
	for (Edge<T> *currInEdge : this->inEdges)
	{
		if (currInEdge->sourceNode.get() == possibleParent.get())
		{
			return true;
		}
	}
	return false;
}

template<class T>
bool Node<T>::isParentOf(std::shared_ptr<Node<T>> possibleChild)
{
	for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
	{
		if (currOutEdge.get()->sinkNode.get() == possibleChild.get())
			return true;
	}
	return false;
}

template<class T>
std::vector<std::shared_ptr<Node<T>> > Node<T>::getChildren()
{
	std::vector<std::shared_ptr<Node<T>>> childrenVecToReturn;
	for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
	{
		childrenVecToReturn.insert(currOutEdge.get()->sinkNode);
	}
	return childrenVecToReturn;
}

template<class T>
std::vector<std::shared_ptr<Node<T>> > Node<T>::getParents()
{
	std::vector<std::shared_ptr<Node<T>>> parentsVecToReturn;
	for (Edge<T> *currInEdge : this->inEdges)
	{
		parentsVecToReturn.insert(currInEdge->sourceNode);
	}
	return parentsVecToReturn;
}

template<class T>
Edge<T>* TemplateGraph::Node<T>::getConnectingEdge(
		std::shared_ptr<Node<T>> otherNode)
{
	if (this->isNeighbor(otherNode))
	{
		if (this->isChildOf(otherNode))
		{
			for (Edge<T> *currEdge : this->inEdges)
			{
				if (currEdge->sourceNode.get() == otherNode.get())
					return currEdge;
			}
		}
		else
		{
			for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
			{
				if (currOutEdge.get()->sinkNode.get() == otherNode.get())
					return currOutEdge.get();
			}
		}
		// NOTE: Should never hit here
		badBehavior(__LINE__, __func__,
				"Found that two nodes were neihgbors but could not find the connecting edge");
	}
	else
	{
		badBehavior(__LINE__, __func__,
				"Tried to get a connecting edge whene the two nodes are not connected");
		return NULL;
	}
}

}

#endif //NODE_HPP
