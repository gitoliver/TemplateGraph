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
	Edge(std::string name, Node<T> *const&sourceNode,
			Node<T> *const&targetNode);
	Edge(std::string name, std::vector<std::string> labels,
			Node<T> *const&sourceNode, Node<T> *const&targetNode);

	//copy constructor
	Edge(const Edge<T> &rhs);

	//move constructor
	Edge(Edge<T> &&rhs);

	//copy assignment
	Edge<T>& operator=(const Edge<T> &rhs);

	//move assignment
	Edge<T>& operator=(Edge<T> &&rhs);

	virtual ~Edge();

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
inline Edge<T>::Edge(std::string name, Node<T> *const&sourceNode,
		Node<T> *const&targetNode) :
		GenericGraphObject(name)
{
	this->targetNode = targetNode;
	this->sourceNode = sourceNode;
}

template<class T>
inline Edge<T>::Edge(std::string name, std::vector<std::string> labels,
		Node<T> *const&sourceNode, Node<T> *const&targetNode) :
		GenericGraphObject(name, labels)
{
	this->targetNode = targetNode;
	this->sourceNode = sourceNode;
}

template<class T>
inline Edge<T>::~Edge()
{
	//have our edge destructor remove itself from our inList then let die
	this->targetNode->removeInEdge(this);
	//lazyInfo(__LINE__, __func__,
	//			"Edge with name <" + this->getName() + "> deleted");
}

//copy constructor
template<class T>
inline Edge<T>::Edge(const Edge<T> &rhs) :
		GenericGraphObject(rhs.getName(), rhs.getLabels(),
				rhs.getConnectivityTypeIdentifier()), sourceNode(
				rhs.getSourceNode()), targetNode(rhs.getTargetNode())
{
	//lazyInfo(__LINE__, __func__,
	//		"Calling copy constructor on " + this->getName());
}

//move constructor
template<class T>
inline Edge<T>::Edge(Edge<T> &&rhs) :
		GenericGraphObject(rhs.getName(), rhs.getLabels(),
				rhs.getConnectivityTypeIdentifier()), sourceNode(
				rhs.getSourceNode()), targetNode(rhs.getTargetNode())
{
	//wanted data has been yoinked so we go ahead and delete this edge that we dont care about
	//	anymore. As stated in move assignment we dont care what state we leave our rhs in after a move
	//lazyInfo(__LINE__, __func__,
	//			"Calling move constructor on " + this->getName());

	rhs.getSourceNode()->removeOutEdge(rhs);
}

//copy assignment
template<class T>
inline Edge<T>& Edge<T>::operator =(const Edge<T> &rhs)
{
	return *this = Edge<T>(rhs);
}

//move assignment
template<class T>
inline Edge<T>& Edge<T>::operator =(Edge<T> &&rhs)
{
	//lazyInfo(__LINE__,__func__, "Edge move assignment");
	//Please note that in order to help prevent some bad behavior due to moving an edge
	//	causing bad connectivity to arise (i.e. multigraph creation, etc.) I am using
	//	the delete on move paradigm in order to help prevent this. Keep in mind move
	//	implies that we dont care about what happens to our rhs.
	this->sourceNode = rhs.sourceNode;
	this->targetNode = rhs.targetNode;
	this->setName(rhs.getName());
	this->setLabels(rhs.getLabels());

	//after we yoink data wanted from our rhs we go ahead and delete it
	rhs.getSourceNode()->removeOutEdge(rhs);

	return *this;
}

template<class T>
inline void Edge<T>::setSourceNode(Node<T> *source)
{
	this->sourceNode = source;
}

template<class T>
inline void Edge<T>::setTargetNode(Node<T> *target)
{
	this->targetNode = target;
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
