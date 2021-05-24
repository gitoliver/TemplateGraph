#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP

#include "./Edge.hpp"
#include "./GenericGraphObject.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"

#include <memory>
#include <unordered_set>

namespace TemplateGraph
{
template<class T>
class Node: public GenericGraphObject, public std::enable_shared_from_this<
		Node<T>>
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Node();
	Node(std::string inName, T *inObjectPtr);
	Node(std::string name, std::string label, T *inObjectPtr);

	//copy constructor
	Node(const Node<T> &rhs);

	//move constructor
	Node(Node<T> &&rhs);

	//copy assignment
	Node<T>& operator=(const Node<T> &rhs);

	//move assignment
	Node<T>& operator=(Node<T> &&rhs);

	~Node();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/

	T* getObjectPtr();

	std::vector<std::weak_ptr<Node<T>>> getNeighbors();
	//Eventually want to remove. Need to think about how to properly do this
	std::vector<Node<T>*> getRawNeighbors();

	std::vector<Edge<T>*> getEdges() const;
	std::vector<Edge<T>*> getOutEdges() const;
	std::vector<Edge<T>*> getInEdges() const;

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	void addEdge(Edge<T> *edgeToAdd);
	/* TODO: Finalize how we would like to add nodes to one another. The addNeighbor will just be a
	 * 			wrapper for our addChild. Please note how I avoided having an "addEdge" because from
	 * 			my understanding and what I am pretty sure our use will be every single edge is
	 * 			guaranteed to have both a source and sink node.
	 */
	void addNeighbor(std::string edgeName,
			std::shared_ptr<Node<T>> const &newNeighbor);

	void addChild(std::string edgeName,
			std::shared_ptr<Node<T>> const &childNode);
	void addParent(std::string edgeName,
			std::shared_ptr<Node<T>> const &parentNode);
	/* NOTE: We MUST remove the edge from out "child" node BEFORE deleting the edge by deleting the
	 * 			unique_ptr that owns it. This is handled in edge's destructor, all we have to worry
	 * 			about is deleting the unique ptr that owns our edge.
	 */
	void removeEdgeBetween(std::shared_ptr<Node<T>> const &otherNode);

	void removeInEdge(Edge<T> *edgeToRemove);
	void removeOutEdge(Edge<T> *edgeToRemove);

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool isNeighbor(std::shared_ptr<Node<T>> const &otherNode);
	Edge<T>* getConnectingEdge(std::shared_ptr<Node<T>> const &otherNode);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	/* TODO: Ensure that unique_ptr is our best option here, I am going to code it and then think more.
	 * 			For now going to use vectors, going to run tests & time, then going to figure if switch
	 * 			to set will help speed.
	 */
	std::vector<std::unique_ptr<Edge<T>>> outEdges;
	std::vector<Edge<T>*> inEdges;

	/* TODO: I disagree with past self regarding leaving out obj ptr, but I am still not 100% sure that
	 * 			including the object ptr that uses Node is considered good practice. I need to read more.
	 */

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	//only write it once
	void edgeConnectionUpdate();

	bool isChildOf(std::shared_ptr<Node<T>> const &possibleParent);
	bool isParentOf(std::shared_ptr<Node<T>> const &possibleChild);

	/* Separated these out incase we eventually want to worry about ownership. They could use weak_ptr instead
	 * 		but this allows me to hold off on checking locks when putting both into our neighbors vector. Casting
	 * 		is automatic.
	 *
	 */
	std::vector<std::shared_ptr<Node<T>>> getChildren();
	std::vector<std::shared_ptr<Node<T>>> getParents();

	/*	Needed to allow us to go from our node to the object we
	 * 		created the node with. Couldnt find an eloquent way
	 * 		around this. We should probably rename it.
	 */
	T *objectPtr;

	friend class Edge<T> ;
};

template<class T>
inline Node<T>::Node() :
		GenericGraphObject("INVALID NODE")
{
	this->objectPtr = NULL;
	badBehavior(__LINE__, __func__, "We called the default node constructor");
}

template<class T>
inline Node<T>::Node(std::string name, T *inObjectPtr) :
		GenericGraphObject(name), objectPtr(inObjectPtr)
{
	//lazyInfo(__LINE__, __func__,
	//		"Created node with name <" + this->getName() + ">");
}

template<class T>
inline Node<T>::Node(std::string name, std::string label, T *inObjectPtr) :
		GenericGraphObject(name, label), objectPtr(inObjectPtr)
{
	lazyInfo(__LINE__, __func__,
			"Created node with name <" + this->getName()
					+ ">\n\tAnd with label <" + this->getLabel() + ">");
}

template<class T>
inline Node<T>::~Node()
{


	std::vector<Edge<T>*> tempInEdge = this->inEdges;

	//go through and hit all our parents, i.e. the ones that own the incoming edge and delete them
	//TODO: Do this but not lazy
	for (Edge<T> *currInEdge : tempInEdge)
	{
		currInEdge->getSourceNode()->removeOutEdge(currInEdge);
	}
	lazyInfo(__LINE__, __func__,
					"Running destructor on node <" + this->getName() + ">");
	//lazyInfo(__LINE__, __func__,
	//		"Finishing destructor on node <" + this->getName() + ">");
}

//Copy constructor
template<class T>
inline Node<T>::Node(const Node<T> &rhs) :
		GenericGraphObject(rhs.getName(), rhs.getLabels(),
				rhs.getConnectivityTypeIdentifier()), objectPtr(rhs.objectPtr)
{
	lazyInfo(__LINE__, __func__, "Calling copy constructor");

	//copy our in edges
	/*
 		std::unique_ptr<Edge<T>> tempEdge(
				new Edge<T>(edgeName, this->shared_from_this(), childNode));

		childNode.get()->inEdges.push_back(tempEdge.get());

		this->outEdges.push_back(std::move(tempEdge));

		^goode code
	 */
	for (Edge<T> const *currInEdge : rhs.inEdges)
	{
		std::unique_ptr<Edge<T>> tempIn(new Edge<T>(*currInEdge));

		tempIn.get()->setTargetNode(this);

		this->inEdges.push_back(tempIn.get());
		tempIn.get()->getSourceNode()->outEdges.push_back(std::move(tempIn));
	}
	for (std::unique_ptr<Edge<T>> const &currOutEdge : rhs.outEdges)
	{
		std::unique_ptr<Edge<T>> tempOut(new Edge<T>(*currOutEdge.get()));

		tempOut.get()->setSourceNode(this);

		tempOut.get()->getTargetNode()->inEdges.push_back(tempOut.get());
		this->outEdges.push_back(std::move(tempOut));
	}

	this->edgeConnectionUpdate();

}

//move constructor
template<class T>
inline Node<T>::Node(Node<T> &&rhs) :
		GenericGraphObject(rhs.getName(), rhs.getLabels()), outEdges(
				std::move(rhs.outEdges)), inEdges(std::move(rhs.inEdges)), objectPtr(
				std::move(rhs.objectPtr))
{
	lazyInfo(__LINE__, __func__, "Calling move constructor");
	this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());
	this->edgeConnectionUpdate();
}

//copy assignment
template<class T>
inline Node<T>& Node<T>::operator =(const Node<T> &rhs)
{
	lazyInfo(__LINE__, __func__, "Calling copy assignment");
	this->setName(rhs.getName());
	this->setLabels(rhs.getLabels());
	this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());
	this->objectPtr = rhs.objectPtr;
	for (Edge<T> const *currEdge : rhs.getEdges())
	{
		//hopefully this will use proper copy constructor to make deep copies...
		//Since this is pushed onto vec here our node -> add edge that is called
		//	from the edge copy will cause us to throw the error
		Edge<T> lole(*currEdge);

		this->addEdge(lole);
	}
	this->edgeConnectionUpdate();

}

//move assignment
template<class T>
inline Node<T>& Node<T>::operator =(Node<T> &&rhs)
{
	lazyInfo(__LINE__, __func__, "Calling move assignment");
	this->setName(rhs.getName());
	this->setLabels(rhs.getLabels());
	this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());
	this->objectPtr = rhs.objectPtr;

	this->inEdges = std::move(rhs.inEdges);
	this->outEdges = std::move(rhs.outEdges);
}

template<class T>
inline std::vector<std::weak_ptr<Node<T>> > Node<T>::getNeighbors()
{
	std::vector<std::shared_ptr<Node<T>>> childrenVec = this->getChildren();
	std::vector<std::shared_ptr<Node<T>>> parentsVec = this->getParents();
	parentsVec.insert(parentsVec.end(), childrenVec.begin(), childrenVec.end());

	std::unordered_set<std::shared_ptr<Node<T>>> tempSet;

	//TODO: Actually prevent dupes instead of doing so lazily
	for (std::shared_ptr<Node<T>> cWP : parentsVec)
	{
		tempSet.insert(cWP);
	}

	std::vector<std::weak_ptr<Node<T>>> parentsVeclol;
	for (std::weak_ptr<Node<T>> cWP : tempSet)
	{
		parentsVeclol.push_back(cWP);
	}

	return parentsVeclol;
}

template<class T>
inline std::vector<Node<T>*> Node<T>::getRawNeighbors()
{
	std::vector<Node<T>*> rawNeighborVec;
	for (std::weak_ptr<Node<T>> currWeakNeigh : this->getNeighbors())
	{
		if (currWeakNeigh.expired())
		{
			badBehavior(__LINE__, __func__,
					"WARNING WE SOMEHOW HAVE AN EXPIRED NEIGHBOR");
		}
		else
		{
			rawNeighborVec.push_back(currWeakNeigh.lock().get());
		}
	}
	return rawNeighborVec;
}

template<class T>
inline std::vector<Edge<T>*> Node<T>::getEdges() const
{
	std::vector<Edge<T>*> outEdgesVec = this->getOutEdges();
	std::vector<Edge<T>*> inEdgesVec = this->getInEdges();
	outEdgesVec.insert(outEdgesVec.end(), inEdgesVec.begin(), inEdgesVec.end());
	return outEdgesVec;
}

template<class T>
inline std::vector<Edge<T>*> Node<T>::getOutEdges() const
{
	std::vector<Edge<T>*> outEdgeVecToReturn;
	//for (std::unique_ptr<Edge<T>*> const &currOutEdge : this->outEdges)
	for (auto const &currOutEdge : this->outEdges)
	{
		outEdgeVecToReturn.push_back(currOutEdge.get());
	}
	return outEdgeVecToReturn;
}

template<class T>
inline std::vector<Edge<T>*> Node<T>::getInEdges() const
{
	return this->inEdges;
}

template<class T>
inline void Node<T>::addNeighbor(std::string edgeName,
		std::shared_ptr<Node<T>> const &newNeighbor)
{
	this->addChild(edgeName, newNeighbor);
}

template<class T>
inline void Node<T>::addChild(std::string edgeName,
		std::shared_ptr<Node<T>> const &childNode)
{
	if (this->isNeighbor(childNode))
	{
		badBehavior(__LINE__, __func__,
				"Trying to make create an edge between two nodes that are already neighbors");
	}
	else if (this == childNode.get())
	{
		badBehavior(__LINE__, __func__,
				"Trying to add self as child, stop that!");
	}
	else
	{
		std::unique_ptr<Edge<T>> tempEdge(
				new Edge<T>(edgeName, this->shared_from_this(), childNode));

		childNode.get()->inEdges.push_back(tempEdge.get());

		this->outEdges.push_back(std::move(tempEdge));

	}
}

template<class T>
inline void Node<T>::addParent(std::string edgeName,
		const std::shared_ptr<Node<T> > &parentNode)
{
	parentNode.get()->addChild(edgeName, this);
}

template<class T>
inline void Node<T>::removeEdgeBetween(
		std::shared_ptr<Node<T>> const &otherNode)
{
	if (this->isNeighbor(otherNode))
	{
		if (this->isChildOf(otherNode))
		{
			Edge<T> *edgeToRemove = this->getConnectingEdge(otherNode);
			otherNode.get()->removeOutEdge(edgeToRemove);
		}
		else if (this->isParentOf(otherNode))
		{
			otherNode.get()->removeEdgeBetween(this->shared_from_this());
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
				"Tried to remove an edge between node <" + this->getName()
						+ "> and node <" + otherNode.get()->getName() + ">");
	}
}

template<class T>
inline bool Node<T>::isNeighbor(std::shared_ptr<Node<T>> const &otherNode)
{
	return this->isChildOf(otherNode) || this->isParentOf(otherNode);
}

template<class T>
bool Node<T>::isChildOf(std::shared_ptr<Node<T>> const &possibleParent)
{
	for (Edge<T> *currInEdge : this->inEdges)
	{
		if (currInEdge->getSourceNode() == possibleParent.get())
		{
			return true;
		}
	}
	return false;
}

template<class T>
inline bool Node<T>::isParentOf(std::shared_ptr<Node<T>> const &possibleChild)
{
	for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
	{
		if (currOutEdge.get()->getTargetNode() == possibleChild.get())
		{
			return true;
		}
	}
	return false;
}

template<class T>
inline std::vector<std::shared_ptr<Node<T>> > Node<T>::getChildren()
{
	std::vector<std::shared_ptr<Node<T>>> childrenVecToReturn;
	for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
	{
		childrenVecToReturn.push_back(
				currOutEdge.get()->getTargetNode()->shared_from_this());
		//lazyInfo(__LINE__, __func__,
		//		currOutEdge.get()->getTargetNode()->getName());
		//childrenVecToReturn.push_back(currOutEdge.get()->getTargetNode().shared_from_this());

	}
	return childrenVecToReturn;
}

template<class T>
inline T* Node<T>::getObjectPtr()
{
	return this->objectPtr;
}

template<class T>
inline void Node<T>::edgeConnectionUpdate()
{
	//So even tho we moved all the references to an edge, we have
	//	not updated said edges with their new vertex appropriately
	for (Edge<T> *currInEdge : this->inEdges)
	{
		currInEdge->setTargetNode(this);
	}
	for (std::unique_ptr<Edge<T>> &currOutEdge : this->outEdges)
	{
		currOutEdge.get()->setSourceNode(this);
	}
}

template<class T>
inline void Node<T>::addEdge(Edge<T> *edgeToAdd)
{
	//Note that we can only use this for edges that have not been claimed by a unique ptr. Below is just used as a
	bool borker = false;
	bool sanityBorker = false;
	bool lockable = false;

	//This is bad because our (should be) newly created edge that is not
	//present in either node should never be found
	for (std::unique_ptr<Edge<T>> &currOutEdge : edgeToAdd->getSourceNode()->outEdges)
	{
		if (currOutEdge.get() == edgeToAdd)
		{
			edgeToAdd->getTargetNode()->inEdges.push_back(currOutEdge.get());
			return;
		}
	}

	std::unique_ptr<Edge<T>> tempEdge(edgeToAdd);
//	tempEdge.get()->getTargetNode()->inEdges.push_back(tempEdge.get());

	tempEdge.get()->getSourceNode()->outEdges.push_back(std::move(tempEdge));

}

template<class T>
inline std::vector<std::shared_ptr<Node<T>> > Node<T>::getParents()
{
	std::vector<std::shared_ptr<Node<T>>> parentsVecToReturn;
	for (Edge<T> *currInEdge : this->inEdges)
	{
		parentsVecToReturn.push_back(
				currInEdge->getSourceNode()->shared_from_this());
	}
	return parentsVecToReturn;
}

template<class T>
inline void Node<T>::removeInEdge(Edge<T> *edgeToRemove)
{
	//lazyInfo(__LINE__, __func__,
	//		"removing in edge <" + edgeToRemove->getName()
	//				+ "> from node named <" + this->getName() + ">");
	this->inEdges.erase(
			std::remove(this->inEdges.begin(), this->inEdges.end(),
					edgeToRemove), this->inEdges.end());
}

template<class T>
inline void Node<T>::removeOutEdge(Edge<T> *edgeToRemove)
{
	//lazyInfo(__LINE__, __func__,
	//		"removing out edge <" + edgeToRemove->getName()
	//				+ "> from node named <" + this->getName() + ">");
	for (int outIndex = 0, ogSize = this->outEdges.size(); outIndex != ogSize;
			outIndex++)
	{
		if (this->outEdges[outIndex].get() == edgeToRemove)
		{
			this->outEdges.erase(this->outEdges.begin() + outIndex);
		}
	}
}

template<class T>
inline Edge<T>* TemplateGraph::Node<T>::getConnectingEdge(
		std::shared_ptr<Node<T>> const &otherNode)
{
	if (this->isNeighbor(otherNode))
	{
		if (this->isChildOf(otherNode))
		{
			for (Edge<T> *currInEdge : this->inEdges)
			{
				if (currInEdge->getSourceNode() == otherNode.get())
				{
					return currInEdge;
				}
			}
		}
		else if (this->isParentOf(otherNode))
		{
			for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
			{
				if (currOutEdge.get()->getTargetNode() == otherNode.get())
				{
					return currOutEdge.get();
				}
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
	return NULL;
}

}

#endif //end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP
