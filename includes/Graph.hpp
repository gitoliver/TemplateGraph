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
        inline void AddCycle(Node<T>* rootNode, std::vector<Node<T>*> nodePath) {cycles_.emplace_back(new Cycle<T>(rootNode, nodePath));}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        void DetectCycles(Node<T>* currentNode = nullptr);
        std::vector<Node<T>*> GetNodes();

	private:
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

		void ResetAllNodesToUnvisited();
        std::vector<Node<T>*> GetCyclePoints();

		//void DetermineCyclePointsRecurve(Node<T>* currentNode, Node<T>* previousNode, std::vector<Node<T>* > &nodePath);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		std::vector<Edge<T>*> edges_;
		std::vector<Cycle<T>*> cycles_;
	};

		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////
template <typename T>
    std::vector<Node<T>*> Graph<T>::GetNodes() 
    { // Why not store nodes_ you asK? Imaginary scenarios where I don't have the NodeList when constructing. Feck.
        std::vector<Node<T>*> nodes;
        for(auto &edge : this->GetEdges())
        {
           nodes.push_back(edge->GetSource());
           nodes.push_back(edge->GetTarget());
        }
        // nodes has duplicates. Want to get only unique nodes. std::unique requires a sorted vector.
        std::sort(nodes.begin(), nodes.end()); // uses the overloaded == operator in Nodes class.
        auto newEnd = std::unique(nodes.begin(), nodes.end()); // After removing duplicates, it returns the position of the new end.
        nodes.resize( std::distance(nodes.begin(), newEnd) ); // Must reize nodes now.
        return nodes;
    }

// // Uses a Recurve function to find cycles.
template <typename T>
     void Graph<T>::DetectCycles(Node<T>* currentNode)
    {
        this->ResetAllNodesToUnvisited();
        for (auto &cyclePoint : this->GetCyclePoints())
        {
            std::cout << "Cyclepoint is " << cyclePoint->GetIndex() << "\n";
        }
        // if (!currentNode) // Just started and no Node supplied.
        // {
        //     currentNode = this->GetEdges().at(0)->GetSource();
       	//     std::cout << "Starting with node " << currentNode->GetIndex() << "\n";
        // }
        // currentNode->SetIsVisited(true);
        // std::vector<Node<T>* > nodePath = {currentNode};
        // for (auto &neighbor : currentNode->GetNeighbors())
        // {
        //     neighbor->SetIsVisited(true);
        //     nodePath.push_back(neighbor);
        //     std::cout << "A neighbor is " << neighbor->GetIndex() << "\n"; 
        // }
        // for (auto &neighbor : currentNode->GetNeighbors())
        // {
        // 	std::cout << "Start neighbor is " << neighbor->GetIndex() << " status is " << neighbor->IsVisited() << "\n";
        // 	this->DetermineCyclePointsRecurve(neighbor, currentNode, nodePath);       		
        // }
        this->ResetAllNodesToUnvisited();
        return;
    }


template <typename T>
    std::vector<Node<T>*> Graph<T>::GetCyclePoints()
    {
        std::vector<Node<T>*> cyclePoints;
        for(auto &node : this->GetNodes())
        {   // For each incoming edge, add node to cyclePoints. Can appear twice.
            std::cout << "Node is " << node->GetIndex() << "\n";
            int numberOfIncoming = 0;
            for(auto &incoming : node->GetIncomingEdgeNeighbors())
            {
                numberOfIncoming++;
                if(numberOfIncoming > 1)
                {
                    cyclePoints.push_back(node);
                }
            }
        }
        return cyclePoints;
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

}
#endif // T_GRAPH_HPP