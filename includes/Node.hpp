#ifndef T_NODE_HPP
#define T_NODE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

namespace TemplateGraph
{
	 template <class T> class Edge; // Forward declare Edge
	 template <class T> class Node; // Forward declare Node
	// template<typename T>
	// std::ostream& operator << (std::ostream& out, const Node<T>& nodeObj);
	// template <class T> class Edge;
	template <class T>
	class Node
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
	//	Node() {} // Unless needed, no.
		Node(T *objectPtr) : objectPtr_ (objectPtr) {index_ = this->GenerateNodeIndex();}
		Node(T *objectPtr, std::string label) : objectPtr_ (objectPtr) {index_ = this->GenerateNodeIndex(); this->AddLabel(label);}
		Node(T *objectPtr, unsigned long long index) : objectPtr_ (objectPtr), index_ (index){}
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline std::vector<Edge<T>*> GetEdges() {return edges_;}
		inline T* GetObjectPtr() {return objectPtr_;}
		inline unsigned long long GetIndex() {return index_;}
		inline bool GetIsVisited() {return is_visited_;}
		inline std::vector<std::string> GetLabels() {return labels_;}
		std::string GetLabel();
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetObjectPtr(T *objectPtr) {objectPtr_ = objectPtr;}
		inline void AddEdge(Edge<T> *edgePtr) {edges_.push_back(edgePtr);}
		inline void SetIsVisited(bool status = false) {is_visited_ = status;}
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		std::vector<Node<T>*> GetNeighbors(); // Can't have a typedef for this to be NodePtrVector?
		std::vector<T*> GetNodesNeighborsObjects(); //
		std::vector<Node<T>*> GetIncomingEdgeNeighbors();
		bool CompareLabels(Node<T>* otherNode);

		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Node<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
	private:
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		unsigned long long GenerateNodeIndex();
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		T* objectPtr_;
		bool is_visited_ = false;
		std::string id_ = "";
		unsigned long long index_ ;
		std::vector<std::string> labels_;
		//std::vector<std::shared_ptr<Edge<T>>> edges_;
		std::vector<Edge<T>*> edges_;
	};

// Template classes are easier if it's all in one header file, so consider this next bit the equivalent to the cc file:
	//////////////////////////////////////////////////////////
    //                       DEFINITIONS                    //
    //////////////////////////////////////////////////////////

template <typename T>
	std::vector<Node<T>*> Node<T>::GetNeighbors()
	{
		std::vector<Node<T>*> neighbors;
		for(auto &edge : this->GetEdges())
		{   // Incoming Edges should be ignored
			if (edge->GetTarget() != this) 
			{
				neighbors.push_back(edge->GetTarget());
			}
		}
		return neighbors;
	}

template <typename T>
	std::vector<Node<T>*> Node<T>::GetIncomingEdgeNeighbors()
	{
		std::vector<Node<T>*> neighbors;
		//std::cout << "Incoming edge neighbors are: ";
		for(auto &edge : this->GetEdges())
		{   // Incoming Edges only
			if (edge->GetTarget() == this) 
			{
		//		std::cout << edge->GetSource()->GetIndex() << " ---> " << this->GetIndex() << ", "; 
				neighbors.push_back(edge->GetSource());
			}
		}
		//std::cout << std::endl;
		return neighbors;
	}

template <typename T>
	std::vector<T*> Node<T>::GetNodesNeighborsObjects()
	{
		std::vector<T*> neighborObjects;
		for(auto &edge : this->GetEdges())
		{
			neighborObjects.push_back(edge->GetTarget()->GetObjectPtr());
		}
		return neighborObjects;
	}

template <typename T>  
	std::string Node<T>::GetLabel()
	{
		if (labels_.empty())
			return "";
		else
			return labels_.back();
	}

template <typename T>  
	bool Node<T>::CompareLabels(Node<T>* otherNode)
	{ // If any label here matches any in other label, return true
		for(auto &otherLabel : otherNode->GetLabels())
		{
			if (std::find(labels_.begin(), labels_.end(), otherLabel ) != labels_.end() )
			{
				std::cout << "Node labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
				return true;
			}
			std::cout << "Node labels DONT match for " << this->GetLabel() << " & " << otherLabel << "\n";
		}
		return false;
	}

template <typename T>
	unsigned long long Node<T>::GenerateNodeIndex() 
	{
		static unsigned long long s_NodeIndex = 0; // static keyword means it is created only once and persists beyond scope of code block.
		return s_NodeIndex++; // makes copy of index, increments the real index, then returns the value in the copy
	} 

}
#endif // T_NODE_HPP