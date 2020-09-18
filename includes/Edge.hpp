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

		Edge() {}
		Edge(Node<T> *source, Node<T> *target);
		//	Edge(Node *source, Node *target) : source_(source), target_(target){};

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline Node<T>* GetSource() {return source_;}
		inline Node<T>* GetTarget() {return target_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetSource(Node<T>* source) {source_ = source;}
		inline void SetTarget(Node<T>* target) {target_ = target;}

	private:
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		Node<T> *source_ = nullptr;
		Node<T> *target_ = nullptr;
	};

	//////////////////////////////////////////////////////////
    //                       DEFINITIONS                    //
    //////////////////////////////////////////////////////////


template <typename T> 
	Edge<T>::Edge(Node<T> *source, Node<T> *target)
	{
		this->SetSource(source);
		this->SetTarget(target);
		this->GetSource()->AddEdge(this);
		this->GetTarget()->AddEdge(this); // Incoming Edge
		//std::cout << "Constructed with " << *source_ << " source and " << *target_ << "target." << std::endl;
	}
}// TemplateGraph namespace
#endif // T_EDGE_HPP