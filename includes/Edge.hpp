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
	template <class T> class Edge; // forward declare 
	//template <class T> Edge();
	template <class T>
	class Edge // Always owned by a Node
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		Edge() {index_ = GenerateEdgeIndex();}
		Edge(std::weak_ptr<Node<T>> source, std::weak_ptr<Node<T>> target, std::string label = ""); // Only Node should call this. How to enforce?
		~Edge() { std::cout << "Edge labeled " << this->GetLabel() << ", with index " << this->GetIndex() << " destroyed\n";}
		//	Edge(Node *source, Node *target) : source_(source), target_(target){};
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		
		inline std::vector<std::string> GetLabels() {return labels_;}
		inline unsigned long long GetIndex() {return index_;}
		inline bool GetIsVisited() {return is_visited_;}
		std::string GetLabel();
		std::string GetId();

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		inline void SetIsVisited(bool status = false) {is_visited_ = status;}
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		bool CompareLabels(Edge<T>* otherEdge);
		bool CompareEdgeAndNodeLabels(Edge<T>* otherEdge);
		std::string Print(std::string type = "label");
		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Edge<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Edge<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
        // std::ostream& operator<< (std::ostream &out, Edge<T> const& edge) 
        // {
        //     out << 'edge: ' << edge.GetIndex() << ' : ';
        //     out << edge.GetLabel() << ' : ';
        //     out << edge.GetSource() << ' -> ' << edge.GetTarget(); 
        //     return out;
        // }
	private:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline std::weak_ptr<Node<T>> GetSource() {return source_;}
		inline std::weak_ptr<Node<T>> GetTarget() {return target_;}
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetSource(std::weak_ptr<Node<T>> source) {source_ = source;}
		inline void SetTarget(std::weak_ptr<Node<T>> target) {target_ = target;}       		
    	//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		unsigned long long GenerateEdgeIndex();
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        std::weak_ptr<Node<T>> source_;
		std::weak_ptr<Node<T>> target_;
		std::vector<std::string> labels_;
		unsigned long long index_ ;
		bool is_visited_ = false;
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
		index_ = this->GenerateEdgeIndex();
		std::cout << "Edge labeled " << this->GetLabel() << ", with index " << this->GetIndex() << " constructed\n";
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
		ss << this->GetSource().lock()->GetIndex() << "->" << this->GetTarget().lock()->GetIndex();
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
	std::string Edge<T>::Print(std::string type)
	{
		std::stringstream ss;
		if (type == "label")
			ss << this->GetLabel();
		if (type == "ID")
			ss << this->GetId();
		return ss.str();
	}
template <typename T>
	unsigned long long Edge<T>::GenerateEdgeIndex() 
	{
		static unsigned long long s_EdgeIndex = 0; // static keyword means it is created only once and persists beyond scope of code block.
		return s_EdgeIndex++; // makes copy of index, increments the real index, then returns the value in the copy
	} 

}// TemplateGraph namespace
#endif // T_EDGE_HPP