#ifndef T_GRAPH_HPP
#define T_GRAPH_HPP

#include <algorithm> // std::unique,sort,reverse
#include "Edge.hpp"

namespace TemplateGraph
{
 	template <class T> class Edge; // Forward declare Edge
	template <class T> class Node;
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
        //inline void AddCycle(Node<T>* rootNode, std::vector<Node<T>*> nodePath) {cycles_.emplace_back(new Cycle<T>(rootNode, nodePath));}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        void DetectCyclesInDFSGraph();
        bool SubGraphMatch(Graph<T> &subGraph); // Need someway to specify what will match
        std::vector<Node<T>*> GetNodes();

	private:
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

		void ResetAllNodesToUnvisited();
        std::vector<Node<T>*> GetCyclePoints();
        void FindPathsToSelf(Node<T>* cycleStartNode, Node<T>* currentNode, std::vector<Node<T>*> currentPath);

		//void DetermineCyclePointsRecurve(Node<T>* currentNode, Node<T>* previousNode, std::vector<Node<T>* > &nodePath);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		std::vector<Edge<T>*> edges_;
        std::vector<std::vector<Node<T>*>> paths_;
	};

		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////

template <typename T>
    bool Graph<T>::SubGraphMatch(Graph<T> &subGraph) 
    {
        bool returnMatch = false;
        for(auto &subEdge : subGraph.GetEdges())
        {
            for(auto &edge : this->GetEdges())
            {       // Can pass a function here instead of a string to match.
                // If label in subgraph edge matches any of labels in graph
                std::vector<std::string> edgeLabels = edge->GetLabels();
                 if (std::find(edgeLabels.begin(), edgeLabels.end(), subEdge->GetLabel() ) != edgeLabels.end()) 
                // {
                     std::cout << "Edges " << subEdge->GetId() << " & " << edge->GetId() << " matched! Beer time\n";
                // }
            }
        }
        return returnMatch;
    }

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
    void Graph<T>::DetectCyclesInDFSGraph()
    {
        this->ResetAllNodesToUnvisited();
        for (auto &cyclePoint : this->GetCyclePoints())
        {
            std::cout << "Cyclepoint is " << cyclePoint->GetIndex() << "\n";
            std::vector<Node<T>*> currentPath;
            currentPath.push_back(cyclePoint);
            for (auto &neighbor : cyclePoint->GetIncomingEdgeNeighbors())
            {
                currentPath.push_back(neighbor);
                for (auto &secondNeighbor : neighbor->GetIncomingEdgeNeighbors())
                {
                    if(secondNeighbor != cyclePoint)
                    {
                        this->FindPathsToSelf(cyclePoint, secondNeighbor, currentPath);
                    }
                }
                currentPath.pop_back();
            }           
        }
        std::cout << "Found these paths:\n";
        for (auto &currentPath : paths_)
        {
            for (auto &node : currentPath)
            {
                std::cout << node->GetIndex() << ", ";
            }
            std::cout << "\n";
        }
        this->ResetAllNodesToUnvisited();
        return;
    }

template <typename T>
    void Graph<T>::FindPathsToSelf(Node<T>* cycleStartNode, Node<T>* currentNode, std::vector<Node<T>*> currentPath)
    {
        currentPath.push_back(currentNode);
        auto allNodeNeighbors = currentNode->GetNeighbors();
        auto cyclePointFoundCheck = std::find(allNodeNeighbors.begin(), allNodeNeighbors.end(), cycleStartNode);
        if (cyclePointFoundCheck != allNodeNeighbors.end()) // i.e. std::find found it in the neighbor list
        {
            currentPath.push_back(cycleStartNode);
            std::cout << "Found it!, current currentPath is: ";
            for (auto &node : currentPath)
            {
                std::cout << node->GetIndex() << ", ";
            }
            paths_.push_back(currentPath);
            currentPath.pop_back();
            std::cout << std::endl;
        }
        for(auto &inNeighbor : currentNode->GetIncomingEdgeNeighbors())
        {
            this->FindPathsToSelf(cycleStartNode, inNeighbor, currentPath);
        }
        currentPath.pop_back(); // remove currentNode if falling out of recursion
        return;
    }


template <typename T>
    std::vector<Node<T>*> Graph<T>::GetCyclePoints()
    {
        std::vector<Node<T>*> cyclePoints;
        for(auto &node : this->GetNodes())
        {   // For each incoming edge, add node to cyclePoints. Can appear twice.
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