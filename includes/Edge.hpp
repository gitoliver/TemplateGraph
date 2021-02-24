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
		bool GetIsVisitedBy(std::string visitor);
		std::string GetLabel();
		std::string GetId();
		inline std::weak_ptr<Node<T>> GetSource() {return source_;}
		inline std::weak_ptr<Node<T>> GetTarget() {return target_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		inline void SetIsVisitedBy(std::string visitor = "") {visitors_.push_back(visitor);}
		void SetUnVisitedBy(std::string visitor);
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		bool CompareLabels(std::shared_ptr<Edge<T>> otherEdge);
		bool CompareEdgeAndNodeLabels(std::shared_ptr<Edge<T>> otherEdge);
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
		std::vector<std::string> visitors_;
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


template <typename T>
	bool Edge<T>::GetIsVisitedBy(std::string visitor)
	{
		if (std::find(visitors_.begin(), visitors_.end(), visitor ) != visitors_.end() )
			return true;
		std::cout << "Not visited.\n";
		return false;
	}

template <typename T>
	void Edge<T>::SetUnVisitedBy(std::string visitor)
	{
		visitors_.erase(std::remove(visitors_.begin(), visitors_.end(), visitor), visitors_.end());
		return; 
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
	bool Edge<T>::CompareLabels(std::shared_ptr<Edge<T>> otherEdge)
	{ // If any label here matches any in other label, return true
		for(auto &otherLabel : otherEdge->GetLabels())
		{
			std::string tempOtherLabel = otherLabel;
			if (std::find(labels_.begin(), labels_.end(), tempOtherLabel ) != labels_.end() )
			{
				//std::cout << "Edge labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
				return true;
			}
			//std::cout << "Edge labels DONT match for " << this->GetLabel() << " & " << otherLabel << "\n";
		}
		return false;
	}

template <typename T>  
	bool Edge<T>::CompareEdgeAndNodeLabels(std::shared_ptr<Edge<T>> otherEdge)
	{ // If any label here matches any in other label, return true
		if(this->CompareLabels(otherEdge) 
			&& this->GetSource().lock()->CompareLabels(otherEdge->GetSource().lock())
			&& this->GetTarget().lock()->CompareLabels(otherEdge->GetTarget().lock()) )
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