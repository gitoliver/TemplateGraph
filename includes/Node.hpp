#ifndef T_NODE_HPP
#define T_NODE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "GenericObject.hpp"

namespace TemplateGraph
{
	 template <class T> class Edge; // Forward declare Edge
	 template <class T> class Node; // Forward declare Node
	// template<typename T>
	// std::ostream& operator << (std::ostream& out, const Node<T>& nodeObj);
	// template <class T> class Edge;
	template <class T>
	class Node : public Abstract::GenericObject
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		//Node() {std::cout << "Ownerless Node created.\n";} // Unless needed, no.
		Node(T *objectPtr) : objectPtr_ (objectPtr) {};
		Node(T *objectPtr, std::string label) : GenericObject {label}, objectPtr_ (objectPtr) {};
		// Node(T *objectPtr, unsigned long long index) : objectPtr_ (objectPtr), index_ (index){}
		~Node() 
		{
			for(auto& inNeighbor: this->GetIncomingEdgeNeighbors())
			{
			  	inNeighbor->RemoveOutEdgeToNode(this);
			}
			std::cout << "Node labelled " << this->GetLabel() << " destroyed\n";
		}
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline T* GetObjectPtr() {return objectPtr_;}
		std::vector<Edge<T>*> GetEdges();
		std::vector<Edge<T>*> GetOutEdges();
		std::vector<Edge<T>*> GetInEdges();
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		//inline void SetObjectPtr(T *objectPtr) {objectPtr_ = objectPtr;}
		void AddEdge(Node<T>* targetNode, std::string label = "");
		void AddIncomingEdge(std::shared_ptr<Edge<T>> incomingEdge);
		//////////////////////////////////////////////////////////
        //                      FUNCTIONS                       //
        //////////////////////////////////////////////////////////
		std::vector<Node<T>*> GetNeighbors();
		std::vector<T*> GetNodesNeighborsObjects();
		std::vector<Node<T>*> GetIncomingEdgeNeighbors();
		void RemoveEdge(Node<T>* otherNode);
	private:
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        inline std::vector<std::shared_ptr<Edge<T>>> GetSharedOutEdges() {return outEdges_;}
        inline std::vector<std::weak_ptr<Edge<T>>> GetWeakInEdges() {return inEdges_;}
		void RemoveOutEdgeToNode(Node<T>* otherNode);
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		T* objectPtr_;
		std::vector<std::shared_ptr<Edge<T>>> outEdges_;
		std::vector<std::weak_ptr<Edge<T>>> inEdges_;
	};

// Template classes are easier if it's all in one header file, so consider this next bit the equivalent to the cc file:
	//////////////////////////////////////////////////////////
    //                       DEFINITIONS                    //
    //////////////////////////////////////////////////////////


template <typename T>  // Need to alter type from weak to shared.
	std::vector<Edge<T>*> Node<T>::GetInEdges()
	{
		std::vector<Edge<T>*> returnEdges;
		for (auto &edge : this->GetWeakInEdges())
		{   // if able to lock, i.e. underlying thing still exists
			if(auto sharedEdge = edge.lock()) // is this unnecessary given ownership rules?
				returnEdges.push_back(sharedEdge.get());
		}
		return returnEdges;
	}

template <typename T>  // Need to alter type from weak to shared.
	std::vector<Edge<T>*> Node<T>::GetOutEdges()
	{
		std::vector<Edge<T>*> returnEdges;
		for (auto &edge : this->GetSharedOutEdges())
		{   
			returnEdges.push_back(edge.get());
		}
		return returnEdges;
	}

template <typename T>
	std::vector<Edge<T>*> Node<T>::GetEdges()
	{
		auto allEdges = this->GetInEdges();
		allEdges.insert(allEdges.end(), outEdges_.begin(), outEdges_.end());
		// std::cout << "EDGES are: \n";
		// for (auto &edge: allEdges)
		// 	std::cout << edge->Print() << "\n";
		return allEdges;
	}

template <typename T>  
	void Node<T>::RemoveEdge(Node<T>* otherNode)
	{
		for(auto &edge : this->GetSharedOutEdges())
		{   // Can I be my own neighbor? Maybe!
			if (edge->GetTarget() == otherNode)
			{ 
				outEdges_.erase(std::remove(outEdges_.begin(), outEdges_.end(), edge), outEdges_.end());
			}
		} 
		// inEdges_ are weak, and them pointing at nothing is handled.
		// But need to go remove it from the otherNode's out
		for(auto &edge : this->GetInEdges())
		{   
			if (edge->GetSource() == otherNode)
			{	
				otherNode->RemoveEdge(this);
			}
		}
		return;
	}

template <typename T>  
	void Node<T>::RemoveOutEdgeToNode(Node<T>* otherNode)
	{
		for(auto &edge : this->GetSharedOutEdges())
		{   
			if (edge->GetTarget() == otherNode) 
			{ 
				outEdges_.erase(std::remove(outEdges_.begin(), outEdges_.end(), edge), outEdges_.end());
			}
		} 
		return;
	}

template <typename T>  
	void Node<T>::AddEdge(Node<T>* targetNode, std::string label)
	{ // assumes adding outEdge. Constructor of Edge calls targetNode.AddIncomingEdge
	    outEdges_.push_back(std::make_shared<Edge<T>>(this, targetNode, label));
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
	std::vector<Node<T>*> Node<T>::GetNeighbors()
	{
		std::vector<Node<T>*> neighbors;
		for(auto &edge : this->GetOutEdges())
		{   // Can I be my own neighbor? No!
			neighbors.push_back(edge->GetTarget());
		}
		for(auto &edge : this->GetInEdges())
		{   
			neighbors.push_back(edge->GetSource());
		}
		return neighbors;
	}

template <typename T>
	std::vector<Node<T>*> Node<T>::GetIncomingEdgeNeighbors()
	{
		std::vector<Node<T>*> neighbors;
		for(auto &edge : this->GetInEdges())
		{   // Incoming Edges only
			neighbors.push_back(edge->GetSource());
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

}
#endif // T_NODE_HPP