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

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
        std::shared_ptr <Node<T>> GetRoot();
        std::vector<std::shared_ptr<Node<T>>> GetNodes();

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetRoot(std::shared_ptr <Node<T>> node) {rootNode_ = node;}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        std::string Print(std::string type = "label");
        void ResetAllEdgesAndNodesToUnvisited();

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

	private:
        //////////////////////////////////////////////////////////
        //                       ACCESSOR                       //
        //////////////////////////////////////////////////////////
        void RecurveFindNodes(std::vector<std::shared_ptr<Node<T>>> &foundNodes, std::shared_ptr<Node<T>> currentNode);
        std::vector<std::shared_ptr<Edge<T>>> GetEdges();
        
        //////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

        //void RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, Graph<T> &matchingPartOfGraph);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        std::weak_ptr<Node<T>> rootNode_;
	};
		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////



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
    std::string Graph<T>::Print(std::string type)
    {
        std::string output;
        for (auto &edge : this->GetEdges())
        {
            output += edge->Print(type);
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
        //     std::cout << "Matches one of these ";
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