#ifndef EDGE_HPP
#define EDGE_HPP

#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GenericObject/includes/GenericObject.hpp"

#include <memory>

namespace TemplateGraph
{

//TODO: Ensure that this is correct forward declare
template<class T> class Node;

template<class T>
class Edge: public Abstract::GenericObject
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Edge();
	Edge(std::string name, std::shared_ptr<Node<T>> sourceNode,
			std::shared_ptr<Node<T>> sinkNode);
	Edge(std::string name, std::vector<std::string> labels,
			std::shared_ptr<Node<T>> sourceNode,
			std::shared_ptr<Node<T>> sinkNode);

	~Edge();

	/*	Return our weak to ensure alive.
	 *
	 */
	std::weak_ptr<Node<T>> getSinkNode();
	std::weak_ptr<Node<T>> getSourceNode();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	/* NOTE: Using shared pointer to get our source and sink in order to ensure
	 * 			source and sink are good and alive.
	 */
	void setSourceNode(std::shared_ptr<Node<T>> source);
	void setSinkNode(std::shared_ptr<Node<T>> sink);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	//NOTE: Source node = the node that has a unique_ptr to this edge
	Node<T> *sourceNode;
	//NOTE: Sink node = the node that has a raw pointer to this edge
	Node<T> *sinkNode;
};

template<class T>
Edge<T>::Edge()
{
	badBehavior(__LINE__, __func__, "Warning calling default constructor");
	this->sinkNode = NULL;
	this->sourceNode = NULL;
}

template<class T>
Edge<T>::Edge(std::string name, std::shared_ptr<Node<T>> sourceNode,
		std::shared_ptr<Node<T>> sinkNode)
{
	this->setName(name);
	this->sinkNode = sinkNode.get();
	this->sourceNode = sourceNode.get();
	//lazyInfo(__LINE__, __func__,
	//		"Created edge with source node <" + this->sourceNode->getName()
	//				+ "> with sink node <" + this->sinkNode->getName()
	//				+ ">\n\tWith edge name <" + this->getName() + ">");
}

template<class T>
Edge<T>::Edge(std::string name, std::vector<std::string> labels,
		std::shared_ptr<Node<T>> sourceNode, std::shared_ptr<Node<T>> sinkNode)
{
	this->setName(name);
	this->setLabels(labels);
	this->sinkNode = sinkNode.get();
	this->sourceNode = sourceNode.get();
}

template<class T>
Edge<T>::~Edge()
{
	this->sinkNode->removeInEdge(this);
	//have our edge destructor remove itself from our inList then let die
	//lazyInfo(__LINE__, __func__,
	//		"Edge with name <" + this->getName() + "> deleted");
}

template<class T>
void Edge<T>::setSourceNode(std::shared_ptr<Node<T>> source)
{
	this->sourceNode = source.get();
}

template<class T>
void Edge<T>::setSinkNode(std::shared_ptr<Node<T>> sink)
{
	this->sinkNode = sink.get();
}

template<class T>
std::weak_ptr<Node<T>> Edge<T>::getSinkNode()
{
	return this->sinkNode->shared_from_this();
}

template<class T>
std::weak_ptr<Node<T>> Edge<T>::getSourceNode()
{
	return this->sourceNode->shared_from_this();
}

}
#endif // EDGE_HPP
