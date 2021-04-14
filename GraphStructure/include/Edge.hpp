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
	//TODO: Check if we actually need these default constructors
	Edge();
	Edge(std::string name, std::shared_ptr<Node<T>> sourceNode,
			std::shared_ptr<Node<T>> sinkNode);
	Edge(std::string name, std::vector<std::string> labels,
			std::shared_ptr<Node<T>> sourceNode,
			std::shared_ptr<Node<T>> sinkNode);

	~Edge();

	std::weak_ptr<Node<T>> getSinkNode();
	std::weak_ptr<Node<T>> getSourceNode();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	/* TODO: Check if we want to use std::shared_ptr<Node<T>> instead. Unsure
	 * 			if this would impact speed much etc. Must think/read/discuss.
	 */
	void setSourceNode(std::shared_ptr<Node<T>> source);
	void setSinkNode(std::shared_ptr<Node<T>> sink);
	// NOTE: Not creating getters because it will end up (from my understanding) breaking our encapsulation

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
	//NOTE: Source node = the node that has a unique_ptr to this edge
	std::shared_ptr<Node<T>> sourceNode;
	//NOTE: Sink node = the node that has a raw pointer to this edge
	std::shared_ptr<Node<T>> sinkNode;

	/************************************************
	 *  FRIENDS
	 ***********************************************/
	/* TODO: Ensure this is correct usage, it seems correct but we must argue to make sure.
	 * 			Since we are using a friend relationship we do not want a getSource/getSink from our edge.
	 * 			From my understanding, this would assist in breaking encapsulation.
	 */

};

template<class T>
Edge<T>::Edge()
{
	badBehavior(__LINE__, __func__, "Warning calling default constructor");
}

template<class T>
Edge<T>::Edge(std::string name, std::shared_ptr<Node<T>> sourceNode,
		std::shared_ptr<Node<T>> sinkNode)
{
	this->setName(name);
	this->setSourceNode(sourceNode);
	this->setSinkNode(sinkNode);
}

template<class T>
Edge<T>::Edge(std::string name, std::vector<std::string> labels,
		std::shared_ptr<Node<T>> sourceNode,
		std::shared_ptr<Node<T>> sinkNode)
{
	this->setName(name);
	this->setLabels(labels);
	this->setSourceNode(sourceNode);
	this->setSinkNode(sinkNode);
}

template<class T>
Edge<T>::~Edge()
{
	lazyInfo(__LINE__, __func__,
			"Edge with name <" + this->getName() + "> deleted");
}

template<class T>
void Edge<T>::setSourceNode(std::shared_ptr<Node<T>> source)
{
	this->sourceNode = source;
}

template<class T>
void Edge<T>::setSinkNode(std::shared_ptr<Node<T>> sink)
{
	this->sinkNode = sink;
}

template<class T>
std::weak_ptr<Node<T>> Edge<T>::getSinkNode()
{
	return this->sinkNode;
}

template<class T>
std::weak_ptr<Node<T>> Edge<T>::getSourceNode()
{
	return this->sourceNode;
}

}
#endif // EDGE_HPP
