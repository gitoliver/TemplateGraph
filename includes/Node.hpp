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
		inline std::vector<Edge<T>*> GetEdges() {return edges_;}
		inline T* GetObjectPtr() {return objectPtr_;}
		inline unsigned long long GetIndex() {return index_;}
		inline bool IsVisited() {return is_visited_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetObjectPtr(T *objectPtr) {objectPtr_ = objectPtr;}
		inline void AddEdge(Edge<T> *edgePtr) {edges_.push_back(edgePtr);}
		inline void SetIsVisited(bool status = false) {is_visited_ = status;}

		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		std::vector<Node<T>*> GetNeighbors(); // Can't have a typedef for this to be NodePtrVector?
		std::vector<T*> GetNodesNeighborsObjects(); //
		std::vector<Node<T>*> GetIncomingEdgeNeighbors();

		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Node<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}

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
		for(auto &edge : this->GetEdges())
		{   // Incoming Edges only
			if (edge->GetTarget() == this) 
			{
				std::cout << edge->GetSource()->GetIndex() << " ---> " << this->GetIndex() << "\n"; 
				neighbors.push_back(edge->GetSource());
			}
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

}
#endif // T_NODE_HPP