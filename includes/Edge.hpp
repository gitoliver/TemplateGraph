#ifndef T_EDGE_HPP
#define T_EDGE_HPP

#include <string>
#include <iostream>
#include <vector>
#include "Node.hpp"

namespace TemplateGraph
{
	template <class T>
	class Edge
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////

		Edge();
		Edge(Node<T> *source, Node<T> *target);
		//	Edge(Node *source, Node *target) : source_(source), target_(target){};

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////


		Node<T>* GetSource();
		Node<T>* GetTarget();

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		void SetSource(Node<T>* source);
		void SetTarget(Node<T>* target);

	private:
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		Node<T> *source_;
		Node<T> *target_;
	};

template <typename T> 
	Edge<T>::Edge()
	{
		source_ = nullptr;
		target_ = nullptr;
	}

template <typename T> 
	Edge<T>::Edge(Node<T> *source, Node<T> *target)
	{
		this->SetSource(source);
		this->SetTarget(target);
		this->GetSource()->AddEdge(this);
		this->GetTarget()->AddEdge(this);
		// //std::cout << "Constructed with " << *source_ << " source and " << *target_ << "target." << std::endl;
	}
	
template <typename T> 
	Node<T>* Edge<T>::GetTarget()
	{
		return target_;
	}
	
template <typename T> 
	Node<T>* Edge<T>::GetSource()
	{
		return source_;
	}

template <typename T> 
	void Edge<T>::SetTarget(Node<T>* target)
	{
		target_ = target;
	}
	
template <typename T> 
	void Edge<T>::SetSource(Node<T>* source)
	{
		source_ = source;
	}

}// TemplateGraph namespace
#endif // T_EDGE_HPP