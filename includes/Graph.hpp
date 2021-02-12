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
        Graph(std::shared_ptr<Node<T>> rootNode) 
        {   
            rootNode_ = std::weak_ptr<Node<T>>(rootNode);
            ResetAllEdgesAndNodesToUnvisited();
        } // convert to weak_ptr
        // {
        //     this->InitializeGraph(std::shared_ptr<Node<T>> initialNode);
        // }
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
        std::string Print();
         void DetectCyclesInDFSGraph();
        // std::vector<Graph<T>> SubGraphMatch(Graph<T> &subGraph);

        // Go look in selection.cc in gmml for the types of functionality you need.
        // class DeadEndSeeker
        // {
        // public:
        //     DeadEndSeeker(AtomNode startNode, int startDepth, AtomNodeVector avoidList) : current_depth_ (startDepth), max_depth_ (startDepth), {path_.push_back(startNode); SetAvoidList(avoidList);} 
        //     inline AtomNodeVector Seek() {;}
        //     // inline void RemoveBond(Atom *otherAtom) {atomNodePtr_->RemoveEdge(otherAtom->GetNode());}
         
        // private:
        //     inline void SetAvoidList(AtomNodeVector avoidList) {for (auto &node : avoidList) node->SetIsVisited(true);} 
        //     int max_depth_;
        //     int current_depth_;
        //     AtomNodeVector path_;
        // };
        //////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        bool operator!= (const Node<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
        // std::ostream& operator<< (std::ostream &out, Graph<T> const& graph) 
        // {
        //     for (auto &edge : graph.GetEdges())
        //     {
        //         out << edge << '\n';                
        //     }
        //     return out;
        // }

	private:
        //////////////////////////////////////////////////////////
        //                       ACCESSOR                       //
        //////////////////////////////////////////////////////////
        std::vector<std::shared_ptr<Node<T>>> GetNodes();
        void RecurveFindNodes(std::vector<std::shared_ptr<Node<T>>> &foundNodes, std::shared_ptr<Node<T>> currentNode);
        std::vector<std::shared_ptr<Edge<T>>> GetEdges();
        std::shared_ptr <Node<T>> GetRoot();
        //////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

        //oid InitializeGraph(std::shared_ptr<Node<T>> initialNode);
		void ResetAllEdgesAndNodesToUnvisited();
        std::vector<std::shared_ptr<Node<T>>> GetCyclePoints();
        void FindPathsToSelf(std::shared_ptr<Node<T>> cycleStartNode, std::shared_ptr<Node<T>> currentNode, std::vector<std::shared_ptr<Node<T>>> currentPath);
        //void RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, Graph<T> &matchingPartOfGraph);

		//void DetermineCyclePointsRecurve(Node<T>* currentNode, Node<T>* previousNode, std::vector<Node<T>* > &nodePath);
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		// std::vector<Edge<T>*> edges_;
        std::vector<std::vector<std::shared_ptr<Node<T>>>> paths_;
  //       std::vector<std::string> labels_;
        //std::vector<std::weak_ptr<Edge<T>>> edges_;
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

// template <typename T> // requires at least one edge in subGraph
//     void Graph<T>::InitializeGraph(std::shared_ptr<Node<T>> initialNode)
//     {
//         for 
//     }


template <typename T> // requires at least one edge in subGraph
    std::shared_ptr <Node<T>> Graph<T>::GetRoot()
    {
        auto check = rootNode_.lock();
        if (!check)
        {
            std::cerr << "rootNode of graph no longer exists.\n";
            // Throw
        }
        return check;
    }

template <typename T> 
    std::string Graph<T>::Print()
    {
        std::string output;
        for (auto &edge : this->GetEdges())
        {
            output += edge->Print();
            output += "\n";
        }
        return output;
    }
// template <typename T> // requires at least one edge in subGraph
//     std::vector<Graph<T>> Graph<T>::SubGraphMatch(Graph<T> &subGraph) 
//     {
//         this->ResetAllEdgesAndNodesToUnvisited();
//         subGraph.ResetAllEdgesAndNodesToUnvisited();
//         std::vector<Graph<T>> matchingGraphs; // Multiple parts of Graph can match subGraph. 
//         std::cout << "subRoot label is " << subGraph.GetRoot()->GetLabel() << " and checking against:\n";
//         for(auto &node : this->GetNodes())
//         {
//             std::cout << node->GetLabel() << "\n";
//             if ( node->GetLabel() == subGraph.GetRoot()->GetLabel() )
//             {
//                 Graph<T> matchingPartOfGraph; 
//                 this->RecurveSubGraphMatch(node, subGraph.GetRoot(), matchingPartOfGraph);
//                 subGraph.ResetAllEdgesAndNodesToUnvisited();
//                 std::cout << "Finished checking that path.\nQuery size: " << subGraph.GetSize() << "\nMatch size: " << matchingPartOfGraph.GetSize() << "\n";
//                 if (matchingPartOfGraph.GetSize() == subGraph.GetSize()) 
//                 { // After recursion, if they are the same size, they now match.
//                     matchingGraphs.push_back(matchingPartOfGraph); // A copy, but whatever man.
//                 }
//             }
//         }
//         this->ResetAllEdgesAndNodesToUnvisited();
//         subGraph.ResetAllEdgesAndNodesToUnvisited();
//         return matchingGraphs;
//     }

// template <typename T>
//     void Graph<T>::RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, Graph<T> &matchingPartOfGraph)
//     {
//         for (auto &queryEdge: queryGraphNode->GetEdges())
//         {
//             std::cout << "   " << queryEdge->GetLabel() << queryEdge->GetTarget()->GetLabel() << " of query, checking against edges:\n";
//             for (auto &edge: graphNode->GetEdges())
//             {
//                 if (!queryEdge->GetIsVisited() && !edge->GetIsVisited())
//                 { // if edges are unvisited so far
//                     queryEdge->SetIsVisited(true);
//                     edge->SetIsVisited(true);
//                     std::cout << "   " << edge->GetLabel() << edge->GetTarget()->GetLabel() << "\n";
//                     if (queryEdge->CompareEdgeAndNodeLabels(edge))
//                     {
//                         matchingPartOfGraph.AddEdge(edge);
//                         std::cout << "   MATCH, now recurve with target node: " << queryEdge->GetTarget()->GetLabel() << "\n";
//                         RecurveSubGraphMatch(edge->GetTarget(), queryEdge->GetTarget(), matchingPartOfGraph);
//                     }
//                 }
//             }
//         }
//         return;
//     }
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
    std::vector<std::shared_ptr<Edge<T>>> Graph<T>::GetEdges() 
    { 
        std::vector<std::shared_ptr<Edge<T>>> edgesInGraph;
        for (auto &node : this->GetNodes())
        {
            //std::cout << "Outedges of " << node->GetLabel() << ":\n";
            for (auto &outEdge : node->GetOutEdges())
            {
                outEdge->SetIsVisited(false);
                edgesInGraph.push_back(outEdge);
                //std::cout << outEdge->GetLabel() << "\n";
            }
        }
        return edgesInGraph;
    } 

template <typename T>
    std::vector<std::shared_ptr<Node<T>>> Graph<T>::GetNodes() 
    { 
        std::vector<std::shared_ptr<Node<T>>> nodesInGraph;
        this->RecurveFindNodes(nodesInGraph, this->GetRoot());
        for(auto &node : nodesInGraph)
        {
            node->SetIsVisited(false);
        }
        return nodesInGraph;
    }

template <typename T>
    void Graph<T>::RecurveFindNodes(std::vector<std::shared_ptr<Node<T>>> &foundNodes, std::shared_ptr<Node<T>> currentNode)
    {
        currentNode->SetIsVisited(true);
        foundNodes.push_back(currentNode);
        for (auto &neighbor : currentNode->GetNeighbors())
        {
            if (!neighbor->GetIsVisited())
            {
                this->RecurveFindNodes(foundNodes, neighbor);
            }
        }
    }

// // Uses a Recurve function to find cycles.
template <typename T>
    void Graph<T>::DetectCyclesInDFSGraph()
    {
        this->ResetAllEdgesAndNodesToUnvisited();
        for (auto &cyclePoint : this->GetCyclePoints())
        {
            std::cout << "Cyclepoint is " << cyclePoint->GetIndex() << "\n";
            std::vector<std::shared_ptr<Node<T>>> currentPath;
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
    void Graph<T>::FindPathsToSelf(std::shared_ptr<Node<T>> cycleStartNode, std::shared_ptr<Node<T>> currentNode, std::vector<std::shared_ptr<Node<T>>> currentPath)
    {
        currentPath.push_back(currentNode);
        auto allNodeNeighbors = currentNode->GetNeighbors();
        auto cyclePointFoundCheck = std::find(allNodeNeighbors.begin(), allNodeNeighbors.end(), cycleStartNode);
        if (cyclePointFoundCheck != allNodeNeighbors.end()) // i.e. std::find found it in the neighbor list
        {
            currentPath.push_back(cycleStartNode);
            std::cout << "Found the cyclePoint!, current currentPath is: ";
            for (auto &node : currentPath)
            {
                std::cout << node->GetIndex() << ", ";
            }
            paths_.push_back(currentPath);
            //currentPath.pop_back();
            std::cout << std::endl;
        }
        else 
        {
            for(auto &inNeighbor : currentNode->GetIncomingEdgeNeighbors())
            {
                this->FindPathsToSelf(cycleStartNode, inNeighbor, currentPath);
            }
            currentPath.pop_back(); // remove currentNode if falling out of recursion
        }
        return;
    }

template <typename T>
    std::vector<std::shared_ptr<Node<T>>> Graph<T>::GetCyclePoints()
    {
        std::vector<std::shared_ptr<Node<T>>> cyclePoints;
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
        }
    }
}
#endif // T_GRAPH_HPP