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
	class Node : public std::enable_shared_from_this<Node<T>>
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
	//	Node() {} // Unless needed, no.
		Node(T *objectPtr) : objectPtr_ (objectPtr) {index_ = this->GenerateNodeIndex();}
		Node(T *objectPtr, std::string label) : objectPtr_ (objectPtr) {index_ = this->GenerateNodeIndex(); labels_.push_back(label);}
		// Node(T *objectPtr, unsigned long long index) : objectPtr_ (objectPtr), index_ (index){}
		~Node() 
		{
			for(auto& inNeighbor: this->GetIncomingEdgeNeighbors())
			{
			  	inNeighbor->UpdateOutEdges();
			}
			std::cout << "Node labelled " << this->GetLabel() << " destroyed\n";
		}
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline T* GetObjectPtr() {return objectPtr_;}
		inline unsigned long long GetIndex() {return index_;}
		bool GetIsVisitedBy(std::string visitor);
		inline std::vector<std::string> GetLabels() {return labels_;}
		std::string GetLabel();
		std::vector<std::shared_ptr<Edge<T>>> GetEdges();
		inline std::vector<std::shared_ptr<Edge<T>>> GetOutEdges() {return outEdges_;}
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetObjectPtr(T *objectPtr) {objectPtr_ = objectPtr;}
		inline void SetIsVisitedBy(std::string visitor = "") {visitors_.push_back(visitor);}
		void SetUnVisitedBy(std::string visitor);
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		void AddEdge(std::shared_ptr<Node<T>> targetNode, std::string label = "");
		void AddIncomingEdge(std::shared_ptr<Edge<T>> incomingEdge);
		//////////////////////////////////////////////////////////
        //                      FUNCTIONS                       //
        //////////////////////////////////////////////////////////
		std::vector<std::shared_ptr<Node<T>>> GetNeighbors();
		std::vector<T*> GetNodesNeighborsObjects();
		std::vector<std::shared_ptr<Node<T>>> GetIncomingEdgeNeighbors();
		bool CompareLabels(const std::shared_ptr<Node<T>> otherNode);
		void RemoveEdge(std::shared_ptr<Node<T>> otherNode);
		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Node<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
 		
	private:
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		std::vector<std::shared_ptr<Edge<T>>> GetInEdges();
		unsigned long long GenerateNodeIndex();
		void UpdateOutEdges();
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		T& const objectPtr_;
		std::vector<std::string> visitors_;
		std::string id_ = "";
		unsigned long long index_ ;
		std::vector<std::string> labels_;
		std::vector<std::shared_ptr<Edge<T>>> outEdges_;
		std::vector<std::weak_ptr<Edge<T>>> inEdges_;
		//std::vector<Edge<T>*> outEdges_;
	};

// Template classes are easier if it's all in one header file, so consider this next bit the equivalent to the cc file:
	//////////////////////////////////////////////////////////
    //                       DEFINITIONS                    //
    //////////////////////////////////////////////////////////

template <typename T>
	bool Node<T>::GetIsVisitedBy(std::string visitor)
	{
		if (std::find(visitors_.begin(), visitors_.end(), visitor ) != visitors_.end() )
			return true;
		return false;
	}

template <typename T>
	void Node<T>::SetUnVisitedBy(std::string visitor)
	{
		visitors_.erase(std::remove(visitors_.begin(), visitors_.end(), visitor), visitors_.end());
		return; 
	}

template <typename T>  // Need to alter type from weak to shared.
	std::vector<std::shared_ptr<Edge<T>>> Node<T>::GetInEdges()
	{
		std::vector<std::shared_ptr<Edge<T>>> returnEdges;
		for (auto &edge : inEdges_)
		{   // if able to lock, i.e. underlying thing still exists
			if(auto sharedEdge = edge.lock())
				returnEdges.push_back(sharedEdge);
		}
		return returnEdges;
	}

template <typename T>
	std::vector<std::shared_ptr<Edge<T>>> Node<T>::GetEdges()
	{
		std::vector<std::shared_ptr<Edge<T>>> allEdges = this->GetInEdges();
		allEdges.insert(allEdges.end(), outEdges_.begin(), outEdges_.end());
		// std::cout << "EDGES are: \n";
		// for (auto &edge: allEdges)
		// 	std::cout << edge->Print() << "\n";
		return allEdges;
	}

template <typename T>  
	void Node<T>::RemoveEdge(std::shared_ptr<Node<T>> otherNode)
	{
		for(auto &edge : this->GetOutEdges())
		{   // Can I be my own neighbor? Maybe!
			if (edge->GetTarget().lock() == otherNode)
			{ // Is this a bad idea in this loop?
				outEdges_.erase(std::remove(outEdges_.begin(), outEdges_.end(), edge), outEdges_.end());
			}
		} 
		// inEdges_ are weak, and them pointing at nothing is handled.
		// But need to go remove it from the otherNode's out
		for(auto &edge : this->GetInEdges())
		{   
			if (edge->GetSource().lock() == otherNode)
			{	
				otherNode->RemoveEdge(this->shared_from_this());
			}
		}
		return;
	}

template <typename T>  
	void Node<T>::UpdateOutEdges()
	{
		for(auto &edge : this->GetOutEdges())
		{   
			if (!edge->GetTarget().lock()) // If you cannot lock the weak pointer, get rid of this edge.
			{ 
				outEdges_.erase(std::remove(outEdges_.begin(), outEdges_.end(), edge), outEdges_.end());
			}
		} 
		return;
	}

template <typename T>  
	void Node<T>::AddEdge(std::shared_ptr<Node<T>> targetNode, std::string label)
	{ // assumes adding outEdge. Constructor of Edge calls targetNode.AddIncomingEdge
	    outEdges_.emplace_back(std::make_shared<Edge<T>>(this->shared_from_this(), targetNode, label));
	    targetNode->AddIncomingEdge(outEdges_.back());
	    return;
	} 

template <typename T>  
	void Node<T>::AddIncomingEdge(std::shared_ptr<Edge<T>> incomingEdge)
	{
	    inEdges_.push_back(std::weak_ptr<Edge<T>>(incomingEdge));
	    return;
	}

template <typename T>
	std::vector<std::shared_ptr<Node<T>>> Node<T>::GetNeighbors()
	{
		std::vector<std::shared_ptr<Node<T>>> neighbors;
		for(auto &edge : this->GetOutEdges())
		{   // Can I be my own neighbor? No!
			if(!edge->GetTarget().expired())
				neighbors.push_back(edge->GetTarget().lock());
		}
		for(auto &edge : this->GetInEdges())
		{   
			if(!edge->GetSource().expired())
				neighbors.push_back(edge->GetSource().lock());
		}
		return neighbors;
	}

template <typename T>
	std::vector<std::shared_ptr<Node<T>>> Node<T>::GetIncomingEdgeNeighbors()
	{
		std::vector<std::shared_ptr<Node<T>>> neighbors;
		for(auto &edge : this->GetInEdges())
		{   // Incoming Edges only
			neighbors.push_back(edge->GetSource().lock());
		}
		return neighbors;
	}



template <typename T>
	std::vector<T*> Node<T>::GetNodesNeighborsObjects()
	{
		std::vector<T*> neighborObjects;
		for(auto &node : this->GetNeighbors())
		{
			neighborObjects.push_back(node->GetObjectPtr());
		}
		return neighborObjects;
	}

template <typename T>  
	std::string Node<T>::GetLabel()
	{ // When requesting singular Label, likely want the latest one.
		if (labels_.empty())
			return "";
		return labels_.back();
	}

template <typename T>  
	bool Node<T>::CompareLabels(const std::shared_ptr<Node<T>> otherNode)
	{ // If any label here matches any in other label, return true
		for(auto &otherLabel : otherNode->GetLabels())
		{
			if (std::find(labels_.begin(), labels_.end(), otherLabel ) != labels_.end() )
			{
				//std::cout << "Node labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
				return true;
			}
			//std::cout << "Node labels DONT match for " << this->GetLabel() << " & " << otherLabel << "\n";
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