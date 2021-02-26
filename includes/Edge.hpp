#ifndef T_EDGE_HPP
#define T_EDGE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include "GenericObject.hpp"

namespace TemplateGraph
{
	template <class T> class Edge; // forward declare 
	template <class T>
	class Edge : public Abstract::GenericObject
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		Edge() : GenericObject {} {};
		Edge(std::weak_ptr<Node<T>> source, std::weak_ptr<Node<T>> target, std::string label = ""); // Only Node should call this. How to enforce?
		~Edge() { std::cout << "Edge labeled " << this->GetLabel() << ", with index " << this->GetIndex() << " destroyed\n";}
		//	Edge(Node *source, Node *target) : source_(source), target_(target){};
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		
		std::string GetId();
		inline std::weak_ptr<Node<T>> GetSource() {return source_;}
		inline std::weak_ptr<Node<T>> GetTarget() {return target_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		bool CompareEdgeAndNodeLabels(std::shared_ptr<Edge<T>> otherEdge);
		std::string Print(std::string type = "label");
		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== ( Edge<T>& rhs) { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= ( Edge<T>& rhs) { return (this->GetIndex() != rhs.GetIndex());}
        
	private:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetSource(std::weak_ptr<Node<T>> source) {source_ = source;}
		inline void SetTarget(std::weak_ptr<Node<T>> target) {target_ = target;}       		
    	//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        std::weak_ptr<Node<T>> source_;
		std::weak_ptr<Node<T>> target_;
		//////////////////////////////////////////////////////////
        //                       FRIENDS                        //
        //////////////////////////////////////////////////////////
		friend class Node<T>; // Allows Node to access private stuff like GetSource().
	};
	//////////////////////////////////////////////////////////
    //                       DEFINITIONS                    //
    //////////////////////////////////////////////////////////

	
template <typename T> 
	Edge<T>::Edge(std::weak_ptr<Node<T>> source, std::weak_ptr<Node<T>> target, std::string label)
	{
		this->SetSource(source);
		this->SetTarget(target);
		this->AddLabel(label);
		//this->GetSource()->AddEdge(this); Node will handle this.
		//this->GetTarget()->AddIncomingEdge(this); Node will handle this.
		//index_ = this->GenerateEdgeIndex();
		std::cout << "Edge labeled " << this->GetLabel() << ", with index " << this->GetIndex() << " constructed\n";
	}

template <typename T>  
	std::string Edge<T>::GetId()
	{
		std::stringstream ss;
		ss << this->GetSource().lock()->GetIndex() << "->" << this->GetTarget().lock()->GetIndex();
		return ss.str();
	}

template <typename T>  
	bool Edge<T>::CompareEdgeAndNodeLabels(std::shared_ptr<Edge<T>> otherEdge)
	{ // If any label here matches any in other label, return true
		if(this->CompareLabels(otherEdge->GetLabels()) 
			&& this->GetSource().lock()->CompareLabels(otherEdge->GetSource().lock()->GetLabels())
			&& this->GetTarget().lock()->CompareLabels(otherEdge->GetTarget().lock()->GetLabels()) )
				return true;
		return false;
	}

template <typename T>
	std::string Edge<T>::Print(std::string type)
	{
		std::stringstream ss;
		if (type == "label")
			ss << this->GetLabel();
		if (type == "ID")
			ss << this->GetId();
		return ss.str();
	}


}// TemplateGraph namespace
#endif // T_EDGE_HPP