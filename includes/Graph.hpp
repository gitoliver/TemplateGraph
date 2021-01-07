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
        Graph(std::shared_ptr<Node<T>> rootNode) : rootNode_ (rootNode) {;}

		//Graph(std::vector<std::weak_ptr<Edge<T>>> edges);

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		// inline std::vector<std::shared_ptr<Edge<T>>> GetEdges() {return edges_;}
  //       inline Node<T>* GetRoot() {return this->GetNodes().at(0);}
  //       inline std::vector<std::string> GetLabels() {return labels_;}
  //       inline size_t GetSize() {return this->GetEdges().size();}
       // std::string GetLabel();
	//	inline bool GetIfCyclic() {return isCyclic_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        // inline void SetEdges(std::vector<Edge<T>*> edges) {edges_ = edges;}
        // inline void AddEdge(Edge<T>* edge) {edges_.push_back(edge);}
        // inline void AddEdge(Node<T>* node1, Node<T>* node2) {edges_.emplace_back(new Edge<T>(node1, node2));}
        // inline void AddEdge(T* object1, T* object2) {this->AddEdge(new Node<T>(object1) , new Node<T>(object2));}
        // inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
        // inline void AddLabel(std::string label) {labels_.push_back(label);}
        // //inline void AddCycle(Node<T>* rootNode, std::vector<Node<T>*> nodePath) {cycles_.emplace_back(new Cycle<T>(rootNode, nodePath));}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        void DetectCyclesInDFSGraph();
        std::vector<Graph<T>> SubGraphMatch(Graph<T> &subGraph);

	private:
        //////////////////////////////////////////////////////////
        //                       ACCESSOR                       //
        //////////////////////////////////////////////////////////
        std::vector<std::shared_ptr<Node<T>>> GetNodes();
        inline std::vector<std::shared_ptr<Node<T>>> GetRoot() {return rootNode_;}
        //////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

		void ResetAllEdgesAndNodesToUnvisited();
        std::vector<Node<T>*> GetCyclePoints();
        void FindPathsToSelf(Node<T>* cycleStartNode, Node<T>* currentNode, std::vector<Node<T>*> currentPath);
        void RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, Graph<T> &matchingPartOfGraph);

		//void DetermineCyclePointsRecurve(Node<T>* currentNode, Node<T>* previousNode, std::vector<Node<T>* > &nodePath);
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		std::vector<Edge<T>*> edges_;
        std::vector<std::vector<Node<T>*>> paths_;
        std::vector<std::string> labels_;
        std::weak_ptr<Node<T>> rootNode_;
	};
		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////

// template <typename T>  
//     std::string Graph<T>::GetLabel()
//     {
//         if (labels_.empty())
//             return "";
//         else
//             return labels_.back();
//     }



template <typename T> // requires at least one edge in subGraph
    std::vector<Graph<T>> Graph<T>::SubGraphMatch(Graph<T> &subGraph) 
    {
        this->ResetAllEdgesAndNodesToUnvisited();
        subGraph.ResetAllEdgesAndNodesToUnvisited();
        std::vector<Graph<T>> matchingGraphs; // Multiple parts of Graph can match subGraph. 
        std::cout << "subRoot label is " << subGraph.GetRoot()->GetLabel() << " and checking against:\n";
        for(auto &node : this->GetNodes())
        {
            std::cout << node->GetLabel() << "\n";
            if ( node->GetLabel() == subGraph.GetRoot()->GetLabel() )
            {
                Graph<T> matchingPartOfGraph; 
                this->RecurveSubGraphMatch(node, subGraph.GetRoot(), matchingPartOfGraph);
                subGraph.ResetAllEdgesAndNodesToUnvisited();
                std::cout << "Finished checking that path.\nQuery size: " << subGraph.GetSize() << "\nMatch size: " << matchingPartOfGraph.GetSize() << "\n";
                if (matchingPartOfGraph.GetSize() == subGraph.GetSize()) 
                { // After recursion, if they are the same size, they now match.
                    matchingGraphs.push_back(matchingPartOfGraph); // A copy, but whatever man.
                }
            }
        }
        this->ResetAllEdgesAndNodesToUnvisited();
        subGraph.ResetAllEdgesAndNodesToUnvisited();
        return matchingGraphs;
    }

template <typename T>
    void Graph<T>::RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, Graph<T> &matchingPartOfGraph)
    {
        for (auto &queryEdge: queryGraphNode->GetEdges())
        {
            std::cout << "   " << queryEdge->GetLabel() << queryEdge->GetTarget()->GetLabel() << " of query, checking against edges:\n";
            for (auto &edge: graphNode->GetEdges())
            {
                if (!queryEdge->GetIsVisited() && !edge->GetIsVisited())
                { // if edges are unvisited so far
                    queryEdge->SetIsVisited(true);
                    edge->SetIsVisited(true);
                    std::cout << "   " << edge->GetLabel() << edge->GetTarget()->GetLabel() << "\n";
                    if (queryEdge->CompareEdgeAndNodeLabels(edge))
                    {
                        matchingPartOfGraph.AddEdge(edge);
                        std::cout << "   MATCH, now recurve with target node: " << queryEdge->GetTarget()->GetLabel() << "\n";
                        RecurveSubGraphMatch(edge->GetTarget(), queryEdge->GetTarget(), matchingPartOfGraph);
                    }
                }
            }
        }
        return;
    }
        // if(std::any_of(nodes.cbegin(), nodes.cend(), [rootNode](Node<T>* a) {return a->GetLabel() == rootNode->GetLabel();} ) )
        // {
        //     std::cout << "Matches one of these fucks";
        // }
        // auto nodeMatch = (std::find_if(nodes.cbegin(), nodes.cend(), [rootNode](Node<T>* a) {return a->GetLabel() == rootNode->GetLabel();} ) );+

        //     for(auto &node : this->GetNodes())
        //     {
        //         if(node->GetLabel() == subGraph.GetRoot()->GetLabel())
        //         {
        //             for(auto &queryEdge)
        //         }
        // }

        // for (auto &neighbor : subGraph.GetRoot()->GetNeighbors());

        //     for(auto &queryEdge : rootNode.GetEdges())
        //     {

        //         for(auto &edge : this->GetEdges())
        //     {   // Can pass a function to std::if_any instead of a string to match.
        //         std::vector<std::string> edgeLabels = edge->GetLabels();
        //         if (std::find(edgeLabels.begin(), edgeLabels.end(), queryEdge->GetLabel() ) != edgeLabels.end()) 
        //         {   // If label in subgraph edge matches any of labels in graph
        //             std::cout << "Edges " << queryEdge->GetId() << " & " << edge->GetId() << " matcheted!\n";
        //         }
        //     }
        // }

template <typename T>
    std::vector<std::shared_ptr<Node<T>>> Graph<T>::GetNodes() 
    { // Why not store nodes_ you asK? Imaginary scenarios where I don't have the NodeList when constructing. Feck.
        std::vector<std::shared_ptr<Node<T>>> nodes;
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
        this->ResetAllEdgesAndNodesToUnvisited();
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
        this->ResetAllEdgesAndNodesToUnvisited();
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
    void Graph<T>::ResetAllEdgesAndNodesToUnvisited()
    {
        for(auto &edge : this->GetEdges())
        {
           edge->SetIsVisited(false);
           edge->GetSource()->SetIsVisited(false);
           edge->GetTarget()->SetIsVisited(false);
        }
    }
}
#endif // T_GRAPH_HPP