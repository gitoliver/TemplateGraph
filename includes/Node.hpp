#ifndef T_NODE_HPP
#define T_NODE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <memory>

typedef std::vector<std::string> StringVector;

namespace TemplateGraph
{
	
	 template <class T> class Edge; // Forward declare Edge
	 template <class T> class Node; // Forward declare Node


	// template<typename T>
	// std::ostream& operator << (std::ostream& out, const Node<T>& nodeObj);
	// template <class T> class Edge;
	// typedef std::vector<Edge*> EdgePtrVector;
	template <class T>
	class Node
	{

    //typedef std::vector<Edge<T>*> EdgePtrVector;

	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
	//	Node() {} // Unless needed, no.
		Node(T *objectPtr) : objectPtr_ {objectPtr} {index_ = GenerateNodeIndex();}
		Node(T *objectPtr, unsigned long long index) : objectPtr_ (objectPtr), index_ (index){}
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		std::vector<Edge<T>*> GetEdges();
		std::vector<Node<T>*> GetNeighbors(); // Can't have a typedef for this to be NodePtrVector?
		std::vector<T*> GetNodesNeighborsObjects(); //
		T* GetObjectPtr();
		unsigned long long GetIndex();
		inline bool IsVisited() {return is_visited_;}
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		void SetObjectPtr(T *objectPtr);
		void AddEdge(Edge<T> *edgePtr);
		inline void SetIsVisited(bool status = false) {is_visited_ = status;}

		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}

	private:
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		T* objectPtr_;
		bool is_visited_ = false;
		std::string id_ = "";
		unsigned long long index_ ;
		StringVector tags_;
		//std::vector<std::shared_ptr<Edge<T>>> edges_;
		std::vector<Edge<T>*> edges_;
		unsigned long long GenerateNodeIndex();
	};

// Template classes are easier if it's all in one header file, so consider this next bit the equivalent to the cc file:

template <typename T>
	T* Node<T>::GetObjectPtr()
	{
		return objectPtr_;
	}

template <typename T>
	unsigned long long Node<T>::GetIndex()
	{
		return index_;
	}

template <typename T>
	void Node<T>::SetObjectPtr(T *objectPtr)
	{
		objectPtr_ = objectPtr;
	}

template <typename T>
	void Node<T>::AddEdge(Edge<T> *edge)
	{
		edges_.push_back(edge);
	}

template < typename T> 
	std::vector<Edge<T>*> Node<T>::GetEdges()
	{
		return edges_;
	}

template <typename T>
	std::vector<Node<T>*> Node<T>::GetNeighbors()
	{
		std::vector<Node<T>*> neighbors;
		for(auto &edge : this->GetEdges())
		{
			neighbors.push_back(edge->GetTarget());
		}
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
	unsigned long long Node<T>::GenerateNodeIndex() 
	{
		static unsigned long long s_NodeIndex = 0; // static keyword means it is created only once and persists beyond scope of code block.
		std::cout << "Generated index: " << s_NodeIndex << std::endl;
		return s_NodeIndex++; // makes copy of s_AtomIndex, increments the real s_AtomIndex, then returns the value in the copy
	} // end generateAtomIndex

	// std::ostream& operator << <>(std::ostream &out, const Node<T> &nodeObj)
	// {
	// 	out << "Node:\n    Id: " << nodeObj.id_ << "\n    Index: " << nodeObj.index_ << "\n    Visted?:" << nodeObj.is_visited_ << "\n";
	// 	return out;
	// }



}
#endif // T_NODE_HPP