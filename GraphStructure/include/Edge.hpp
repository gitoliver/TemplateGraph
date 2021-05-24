#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP

#include "./GenericGraphObject.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"

#include <memory>

namespace TemplateGraph
{

//TODO: Ensure that this is correct forward declare
template<class T> class Node;

template<class T>
class Edge: public GenericGraphObject
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Edge();
	Edge(std::string name, std::shared_ptr<Node<T>> const &sourceNode,
			std::shared_ptr<Node<T>> const &targetNode);
	Edge(std::string name, std::vector<std::string> labels,
			std::shared_ptr<Node<T>> const &sourceNode,
			std::shared_ptr<Node<T>> const &targetNode);

	//copy constructor
	Edge(const Edge<T> &rhs);

	//move constructor
	Edge(Edge<T> &&rhs);

	//copy assignment
	Edge<T>& operator=(const Edge<T> &rhs);

	//move assignment
	Edge<T>& operator=(Edge<T> &&rhs);

	~Edge();

	//	Return our weak to ensure alive.
	Node<T>* getTargetNode() const;
	Node<T>* getSourceNode() const;

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	// NOTE: Using shared pointer to get our source and sink in order to ensure
	// 			source and sink are good and alive.
	void setSourceNode(Node<T> *source);
	void setTargetNode(Node<T> *target);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	//NOTE: Source node = the node that has a unique_ptr to this edge
	Node<T> *sourceNode;
	//NOTE: Sink node = the node that has a raw pointer to this edge
	Node<T> *targetNode;
};

template<class T>
inline Edge<T>::Edge()
{
	badBehavior(__LINE__, __func__, "Warning calling default constructor");
	this->targetNode = NULL;
	this->sourceNode = NULL;
}

template<class T>
inline Edge<T>::Edge(std::string name,
		std::shared_ptr<Node<T>> const &sourceNode,
		std::shared_ptr<Node<T>> const &targetNode) :
		GenericGraphObject(name)
{
	this->targetNode = targetNode.get();
	this->sourceNode = sourceNode.get();
}

template<class T>
inline Edge<T>::Edge(std::string name, std::vector<std::string> labels,
		std::shared_ptr<Node<T>> const &sourceNode,
		std::shared_ptr<Node<T>> const &targetNode) :	GenericGraphObject(name, labels)
{
	this->targetNode = targetNode.get();
	this->sourceNode = sourceNode.get();
}

template<class T>
inline Edge<T>::~Edge()
{
	this->targetNode->removeInEdge(this);

	//have our edge destructor remove itself from our inList then let die
	//lazyInfo(__LINE__, __func__,
	//		"Edge with name <" + this->getName() + "> deleted");
}

template<class T>
inline void Edge<T>::setSourceNode(Node<T> *source)
{
	this->sourceNode = source;
}

//copy constructor
template<class T>
inline Edge<T>::Edge(const Edge<T> &rhs) :
		GenericGraphObject(rhs.getName(), rhs.getLabels(),
				rhs.getConnectivityTypeIdentifier()), sourceNode(
				rhs.getSourceNode()), targetNode(rhs.getTargetNode())
{
	lazyInfo(__LINE__, __func__,
			"Calling copy constructor on " + this->getName());
}

//move constructor
template<class T>
inline Edge<T>::Edge(Edge<T> &&rhs) :
		sourceNode(rhs.getSourceNode().lock().get()), targetNode(
				rhs.getTargetNode().lock().get()),
				GenericGraphObject(rhs.getName(), rhs.getLabels(),
								rhs.getConnectivityTypeIdentifier())
{
	lazyInfo(__LINE__, __func__, "Calling move constructor");
	this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());
	//well since we dont really care about our rhs for move and I want to stress
	//	that we dont care about it due to unique ptr gonna go ahead and delete the og
	rhs.sourceNode->removeOutEdge(rhs);

	//granted we could add before removeing the previous but meh.
	this->sourceNode->addEdge(this);
}

//copy assignment
template<class T>
inline Edge<T>& Edge<T>::operator =(const Edge<T> &rhs)
{
	lazyInfo(__LINE__, __func__, "Calling copy assignment");

	//PLEASE NOTE THAT IF USED THIS WILL SCREW UP OUR GRAPH STRUCTURE
	//	IF INVOKED IT WILL CAUSE A MULTIGRAPH TO SPAWN, ALGOS ARE NOT
	//	FOR A MULTIGRAPH BUT I CAN MAKE THEM FOR A MULTIGRAPH
	this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());
	bool bork = true;
	bool borkbork = true;

	if (rhs.getTargetNode().lock())
	{
		bork = false;
		this->setTargetNode(rhs.getTargetNode());
	}
	if (rhs.getSourceNode().lock())
	{
		borkbork = false;
		this->setSourceNode(rhs.getSourceNode());
	}

	this->sourceNode->addEdge(this);
	return *this;
}

//move assignment
template<class T>
inline Edge<T>& Edge<T>::operator =(Edge<T> &&rhs)
{

	lazyInfo(__LINE__, __func__, "Calling move assignment");

}

template<class T>
inline void Edge<T>::setTargetNode(Node<T> *sink)
{
	this->targetNode = sink;
}

template<class T>
inline Node<T>* Edge<T>::getTargetNode() const
{
	return this->targetNode;
}

template<class T>
inline Node<T>* Edge<T>::getSourceNode() const
{
	return this->sourceNode;
}

}
#endif //end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
