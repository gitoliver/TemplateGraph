#ifndef T_GRAPH_HPP
#define T_GRAPH_HPP

#include "Edge.hpp"
#include "Cycle.hpp"

namespace TemplateGraph
{
 	template <class T> class Edge; // Forward declare Edge
	template <class T> class Node;
	template <class T> class Cycle;
	template <class T>
	class Graph
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		Graph() {};
		Graph(std::vector<Edge<T>*> edges);

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline std::vector<Edge<T>*> GetEdges() {return edges_;}
	//	inline bool GetIfCyclic() {return isCyclic_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetEdges(std::vector<Edge<T>*> edges) {edges_ = edges;}
        inline void AddEdge(Edge<T>* edge) {edges_.push_back(edge);}
        inline void AddEdge(Node<T>* node1, Node<T>* node2) {edges_.emplace_back(new Edge<T>(node1, node2));}
        inline void AddEdge(T* object1, T* object2) {this->AddEdge(new Node<T>(object1) , new Node<T>(object2));}
        inline void AddCycle(Node<T>* root, std::vector<Node<T>*> nodePath) {cycles_.emplace_back(new Cycle<T>(root, nodePath));}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        bool DetectCycles(Node<T>* currentNode = nullptr);

        // void AddEdge(Node<T>* node1, Node<T>* node2);
	private:
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

		void ResetAllNodesToUnvisited();
		bool DetectCyclesRecurve(Node<T>* currentNode, std::vector<Node<T>* > &nodePath);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		std::vector<Edge<T>*> edges_;
		std::vector<Cycle<T>*> cycles_;
	};

		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////
// template <typename T> 
// 	Graph<T>::Graph(std::vector< Edge<T>* > edges)
// 	{
// 		this->SetEdges(edges);
// 	}

// template <typename T> 
// 	std::vector<Edge<T>*> Graph<T>::GetEdges()
// 	{
// 		return edges_;
// 	}

// template <typename T> 
// 	void Graph<T>::SetEdges(std::vector< Edge<T>* > edges)
// 	{
// 		edges_ = edges;
// 	}

// template <typename T> 
// 	void Graph<T>::AddEdge(Edge<T>* edge)
// 	{
// 		edges_.push_back(edge);
// 	}

// template <typename T> 
// 	void Graph<T>::AddEdge(Node<T>* node1, Node<T>* node2)
// 	{
// 		edges_.emplace_back(new Edge<T>(node1, node2));
// 	}

// template <typename T> 
// 	void Graph<T>::AddEdge(T* object1, T* object2)
// 	{
// 		this->AddEdge(new Node<T>(object1) , new Node<T>(object2));
// 	}

// Uses a Recurve function to find cycles.
template <typename T>
    bool Graph<T>::DetectCycles(Node<T>* currentNode)
	{
    	this->ResetAllNodesToUnvisited();
       	if (!currentNode) // Just started and no Node supplied.
       	{
       		currentNode = this->GetEdges().at(0)->GetSource();
       	}
       	currentNode->SetIsVisited(true);
       	std::vector<Node<T>* > nodePath = {currentNode};
       	for (auto &neighbor : currentNode->GetNeighbors())
       	{
       		if (this->DetectCyclesRecurve(neighbor, nodePath))
       		{
       			this->ResetAllNodesToUnvisited();
       			return true;
       		}
       	}
       	this->ResetAllNodesToUnvisited();
       	return false;
    }

template <typename T>
    bool Graph<T>::DetectCyclesRecurve(Node<T>* currentNode, std::vector<Node<T>* > &nodePath)
    {
    	if (currentNode->IsVisited())
    	{
    		this->AddCycle(currentNode, nodePath);
    	}
    	currentNode->SetIsVisited(true);
    	for (auto &neighbor : currentNode->GetNeighbors())
       	{
       		if (this->DetectCyclesRecurve(neighbor, nodePath))
       		{
       			this->ResetAllNodesToUnvisited();
       			return true;
       		}
       	}
    	return false;
    }

template <typename T> 
	void Graph<T>::ResetAllNodesToUnvisited()
	{
		for(auto &edge : this->GetEdges())
		{
			edge->GetSource()->SetIsVisited(false);
			edge->GetTarget()->SetIsVisited(false);
		}
	}


// template <typename T> 
// 	void Graph<T>::AddEdge(Node<T>* node1, Node<T>* node2)
// 	{
// 		edges_.emplace_back(new Edge<T>(node1, node2));
// 	}
}
#endif // T_GRAPH_HPP