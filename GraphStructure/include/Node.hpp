#ifndef NODE_HPP
#define NODE_HPP

#include "./Edge.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GenericObject/includes/GenericObject.hpp"

#include <memory>
#include <unordered_set>


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
	Node();
	Node(std::string inName);
	Node(std::string name, std::string label);

	~Node();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	std::vector<std::weak_ptr<Node<T>>> getNeighbors();

	std::vector<Edge<T>*> getEdges();
	std::vector<Edge<T>*> getOutEdges();
	std::vector<Edge<T>*> getInEdges();

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	/* TODO: Finalize how we would like to add nodes to one another. The addNeighbor will just be a
	 * 			wrapper for our addChild. Please note how I avoided having an "addEdge" because from
	 * 			my understanding and what I am pretty sure our use will be every single edge is
	 * 			guaranteed to have both a source and sink node.
	 */
	void addNeighbor(std::string edgeName,
			std::shared_ptr<Node<T>> const &newNeighbor);

	//Do we want to worry about adding a parent?
	void addChild(std::string edgeName, std::shared_ptr<Node<T>> const &childNode);
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
	bool isChildOf(std::shared_ptr<Node<T>> const &possibleParent);
	bool isParentOf(std::shared_ptr<Node<T>> const &possibleChild);

	/* Separated these out incase we eventually want to worry about ownership. They could use weak_ptr instead
	 * 		but this allows me to hold off on checking locks when putting both into our neighbors vector. Casting
	 * 		is automatic.
	 *
	 */
	std::vector<std::shared_ptr<Node<T>>> getChildren();
	std::vector<std::shared_ptr<Node<T>>> getParents();

	Edge<T>* getConnectingEdge(std::shared_ptr<Node<T>> const &otherNode);

	friend class Edge<T> ;
};

template<class T>
Node<T>::Node()
{
	this->setName("INVALID NODE");
	badBehavior(__LINE__, __func__, "We called the default node constructor");
}

template<class T>
Node<T>::Node(std::string name)
{
	this->setName(name);
	//lazyInfo(__LINE__, __func__,
	//		"Created node with name <" + this->getName() + ">");
}

template<class T>
Node<T>::Node(std::string name, std::string label)
{
	this->setName(name);
	this->setLabels(label);

	lazyInfo(__LINE__, __func__,
			"Created node with name <" + this->getName()
					+ ">\n\tAnd with label <" + this->getLabel() + ">");
}

template<class T>
Node<T>::~Node()
{

	//lazyInfo(__LINE__, __func__,
	//		"Running destructor on node <" + this->getName() + ">");
	std::vector<Edge<T>*> tempInEdge = this->inEdges;
	//go through and hit all our parents, i.e. the ones that own the incoming edge and delete them
	//TODO: Do this but not lazy
	for (Edge<T> *currInEdge : tempInEdge)
	{
		std::shared_ptr<Node<T>> sourceLock =
				currInEdge->getSourceNode().lock();
		if (sourceLock)
		{
			sourceLock.get()->removeOutEdge(currInEdge);
		}
		else
		{
			badBehavior(__LINE__, __func__, "Could not lock our source node");
		}
	}
	//lazyInfo(__LINE__, __func__,
	//		"Finishing destructor on node <" + this->getName() + ">");
}

template<class T>
std::vector<std::weak_ptr<Node<T>> > Node<T>::getNeighbors()
{
	std::vector<std::shared_ptr<Node<T>>> childrenVec = this->getChildren();
	std::vector<std::shared_ptr<Node<T>>> parentsVec = this->getParents();
	parentsVec.insert(parentsVec.end(), childrenVec.begin(), childrenVec.end());

	std::unordered_set<std::shared_ptr<Node<T>>> tempSet;

	//TODO: Actually prevent dupes instead of doing so lazily
	for(std::shared_ptr<Node<T>> cWP : parentsVec)
	{
		tempSet.insert(cWP);
	}

	std::vector<std::weak_ptr<Node<T>>> parentsVeclol;
	for(std::weak_ptr<Node<T>> cWP : tempSet)
	{
		parentsVeclol.push_back(cWP);
	}

	return parentsVeclol;
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
		std::shared_ptr<Node<T>> const &newNeighbor)
{
	this->addChild(edgeName, newNeighbor);
}

template<class T>
void Node<T>::addChild(std::string edgeName, std::shared_ptr<Node<T>> const &childNode)
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
		this->outEdges.push_back(
				std::make_unique<Edge<T>>(edgeName, this->shared_from_this(),
						childNode));
		childNode.get()->inEdges.push_back(this->outEdges.back().get());
	}
}

template<class T>
void Node<T>::removeEdgeBetween(std::shared_ptr<Node<T>> const &otherNode)
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
bool Node<T>::isNeighbor(std::shared_ptr<Node<T>> const &otherNode)
{
	return (this->isChildOf(otherNode) || this->isParentOf(otherNode));
}

template<class T>
bool Node<T>::isChildOf(std::shared_ptr<Node<T>> const &possibleParent)
{
	for (Edge<T> *currInEdge : this->inEdges)
	{
		std::shared_ptr<Node<T>> sourceNodeLock =
				currInEdge->getSourceNode().lock();
		if (sourceNodeLock)
		{
			if (sourceNodeLock.get() == possibleParent.get())
			{
				return true;
			}
		}
		else
		{
			badBehavior(__LINE__, __func__, "Could not lock our parent");
		}
	}
	return false;
}

template<class T>
bool Node<T>::isParentOf(std::shared_ptr<Node<T>> const &possibleChild)
{
	for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
	{
		std::shared_ptr<Node<T>> sinkNodeLock =
				currOutEdge.get()->getSinkNode().lock();
		if (sinkNodeLock)
		{
			if (sinkNodeLock.get() == possibleChild.get())
			{
				return true;
			}
		}
		else
		{
			badBehavior(__LINE__, __func__, "Could not lock our child");
		}
	}
	return false;
}

template<class T>
std::vector<std::shared_ptr<Node<T>> > Node<T>::getChildren()
{
	std::vector<std::shared_ptr<Node<T>>> childrenVecToReturn;
	for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
	{
		std::shared_ptr<Node<T>> lockedSink =
				currOutEdge.get()->getSinkNode().lock();
		if (lockedSink)
		{
			childrenVecToReturn.push_back(lockedSink);
		}
		else{
			badBehavior(__LINE__, __func__, "Couldnt lock our sink");
		}
	}
	return childrenVecToReturn;
}

template<class T>
std::vector<std::shared_ptr<Node<T>> > Node<T>::getParents()
{
	std::vector<std::shared_ptr<Node<T>>> parentsVecToReturn;
	for (Edge<T> *currInEdge : this->inEdges)
	{
		std::shared_ptr<Node<T>> lockedSource =
				currInEdge->getSourceNode().lock();
		if (lockedSource)
		{
			parentsVecToReturn.push_back(lockedSource);
		}
		else
		{
			badBehavior(__LINE__, __func__, "Couldnt lock our source node");
		}
	}
	return parentsVecToReturn;
}

template<class T>
void Node<T>::removeInEdge(Edge<T> *edgeToRemove)
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
Edge<T>* TemplateGraph::Node<T>::getConnectingEdge(
		std::shared_ptr<Node<T>> const &otherNode)
{
	if (this->isNeighbor(otherNode))
	{
		if (this->isChildOf(otherNode))
		{
			for (Edge<T> *currInEdge : this->inEdges)
			{

				std::shared_ptr<Node<T>> sourceNodeLock =
						currInEdge->getSourceNode().lock();
				if (sourceNodeLock)
				{
					if (sourceNodeLock.get() == otherNode.get())
					{
						return currInEdge;
					}
				}
				else
				{
					badBehavior(__LINE__, __func__,
							"Could not lock our source node");
				}
			}
		}
		else
		{
			for (std::unique_ptr<Edge<T>> const &currOutEdge : this->outEdges)
			{
				std::shared_ptr<Node<T>> sinkNodeLock =
						currOutEdge->getSinkNode().lock();
				if (sinkNodeLock)
				{
					if (sinkNodeLock.get() == otherNode.get())
					{
						return currOutEdge.get();
					}
				}
				else
				{
					badBehavior(__LINE__, __func__,
							"Could not lock our sink node");
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

#endif //NODE_HPP
