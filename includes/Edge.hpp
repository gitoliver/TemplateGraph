#ifndef T_EDGE_HPP
#define T_EDGE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
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
		inline std::vector<std::string> GetLabels() {return labels_;}
		std::string GetLabel();
		
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetSource(Node<T>* source) {source_ = source;}
		inline void SetTarget(Node<T>* target) {target_ = target;}
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}

		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		std::string GetId();

	private:
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		Node<T> *source_ = nullptr;
		Node<T> *target_ = nullptr;
		std::vector<std::string> labels_;
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

// Should I throw an exception if the user asks for a label and none are set? Probably.
// If user wants a label and there are none, give them "", otherwise give them the last element added.
// I assume they want whatever they've added with AddLabel() when they are requesting just one with GetLabel().
template <typename T>  
	std::string Edge<T>::GetLabel()
	{
		if (labels_.empty())
			return "";
		else
			return labels_.back();
	}

template <typename T>  
	std::string Edge<T>::GetId()
	{
		std::stringstream ss;
		ss << this->GetSource()->GetIndex() << "-->" << this->GetTarget()->GetIndex();
		return ss.str();
	}
}// TemplateGraph namespace
#endif // T_EDGE_HPP