#ifndef T_EDGE_HPP
#define T_EDGE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
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
		Edge() {index_ = GenerateEdgeIndex();}
		Edge(Node<T> *source, Node<T> *target);
		//	Edge(Node *source, Node *target) : source_(source), target_(target){};
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline Node<T>* GetSource() {return source_;}
		inline Node<T>* GetTarget() {return target_;}
		inline std::vector<std::string> GetLabels() {return labels_;}
		inline unsigned long long GetIndex() {return index_;}
		inline bool GetIsVisited() {return is_visited_;}
		std::string GetLabel();
		std::string GetId();

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetSource(Node<T>* source) {source_ = source;}
		inline void SetTarget(Node<T>* target) {target_ = target;}
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		inline void SetIsVisited(bool status = false) {is_visited_ = status;}
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		bool CompareLabels(Edge<T>* otherEdge);
		bool CompareEdgeAndNodeLabels(Edge<T>* otherEdge);
		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Edge<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Edge<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
	private:
    	//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		unsigned long long GenerateEdgeIndex();
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		Node<T> *source_ = nullptr;
		Node<T> *target_ = nullptr;
		std::vector<std::string> labels_;
		unsigned long long index_ ;
		bool is_visited_ = false;
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
		index_ = this->GenerateEdgeIndex();
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

template <typename T>  
	bool Edge<T>::CompareLabels(Edge<T>* otherEdge)
	{ // If any label here matches any in other label, return true
		for(auto &otherLabel : otherEdge->GetLabels())
		{
			std::string tempOtherLabel = otherLabel;
			if (std::find(labels_.begin(), labels_.end(), tempOtherLabel ) != labels_.end() )
			{
				std::cout << "Edge labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
				return true;
			}
			std::cout << "Edge DONT labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
		}
		return false;
	}

template <typename T>  
	bool Edge<T>::CompareEdgeAndNodeLabels(Edge<T>* otherEdge)
	{ // If any label here matches any in other label, return true
		if(this->CompareLabels(otherEdge) 
			&& this->GetSource()->CompareLabels(otherEdge->GetSource())
			&& this->GetTarget()->CompareLabels(otherEdge->GetTarget()) )
				return true;
		return false;
	}

template <typename T>
	unsigned long long Edge<T>::GenerateEdgeIndex() 
	{
		static unsigned long long s_EdgeIndex = 0; // static keyword means it is created only once and persists beyond scope of code block.
		return s_EdgeIndex++; // makes copy of index, increments the real index, then returns the value in the copy
	} 

}// TemplateGraph namespace
#endif // T_EDGE_HPP