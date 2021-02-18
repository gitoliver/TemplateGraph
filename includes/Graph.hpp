#ifndef T_GRAPH_HPP
#define T_GRAPH_HPP

#include <algorithm> // std::unique,sort,reverse
#include "Edge.hpp"
#include "Algorithms/SubGraphMatching/SubGraph.hpp"

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
        unsigned int GetSize();
        std::string Print(std::string type = "label");
        
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetRoot(std::shared_ptr <Node<T>> node) {rootNode_ = node;}
        void ResetAllEdgesAndNodesToUnvisited();
        //inline void AddEdge(std::shared_ptr <Edge<T>> edge) {edges_.push_back(edge);}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        //std::vector<SubGraph<T>> SubGraphMatch(Graph<T> &queryGraph);

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

        //void RecurveSubGraphMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode, SubGraph<T> &matchingPartOfGraph);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        std::weak_ptr<Node<T>> rootNode_;
        //std::vector<std::weak_ptr<Edge<T>>> edges_;
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

// // Limit on exploration limited by query, not main. Can revisit places in main.
// // Need to account for matches with partial overlap...
// template <typename T> // requires at least one edge in subGraph
//     std::vector<SubGraph<T>> Graph<T>::SubGraphMatch(Graph<T> &queryGraph) 
//     {
//         //this->ResetAllEdgesAndNodesToUnvisited();
//         queryGraph.ResetAllEdgesAndNodesToUnvisited();
//         std::vector<SubGraph<T>> matchingSubGraphs; // Multiple parts of Graph can match subGraph. 
//         for(auto &node : this->GetNodes())
//         {
//             std::cout << node->GetLabel() << "\n";
//             if ( node->GetLabel() == queryGraph.GetRoot()->GetLabel() )
//             {
//                 std::cout << "Queryroot: " << queryGraph.GetRoot()->GetLabel() << " matches " << node->GetLabel() << "\n";
//                 SubGraph<T> matchingPartOfGraph;
//                 this->RecurveSubGraphMatch(node, queryGraph.GetRoot(), matchingPartOfGraph);
//                 this->ResetAllEdgesAndNodesToUnvisited();
//                 queryGraph.ResetAllEdgesAndNodesToUnvisited();
//                 std::cout << "Finished checking that path.\nQuery size: " << queryGraph.GetSize() << "\nMatch size: " << matchingPartOfGraph.GetSize() << "\n";
//                 if (matchingPartOfGraph.GetSize() == queryGraph.GetSize()) 
//                 { // After recursion, if they are the same size, they now match.
//                     matchingSubGraphs.push_back(matchingPartOfGraph); // A copy, but whatever man.
//                 }
//             }
//         }
//         return matchingSubGraphs;
//     }

// template <typename T>
//     void Graph<T>::RecurveSubGraphMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode, SubGraph<T> &matchingPartOfGraph)
//     {
//         //std::cout << "Match. EnteredRecurve. graphNode is " << graphNode->GetLabel() << ". queryGraphNode is " << queryGraphNode->GetLabel() << "\n"; 
//         for (auto &queryEdge: queryGraphNode->GetEdges())
//         {
//             if (!queryEdge->GetIsVisited())
//             {
//                 queryEdge->SetIsVisited(true);
//             //std::cout << "   " << queryEdge->GetLabel() << " of query, checking against edges:\n";
//                 for (auto &edge: graphNode->GetEdges())
//                 {
//                     //std::cout << "    " << edge->GetLabel() << " of main graph\n.";
//                     //std::cout << "    " << std::boolalpha << queryEdge->GetIsVisited() << " main is " << edge->GetIsVisited() << "\n";
//                     // if (!edge->GetIsVisited())
//                     // { // if edges are unvisited so far
//                     //     edge->SetIsVisited(true);
//                         if (queryEdge->CompareEdgeAndNodeLabels(edge))
//                         {
//                             matchingPartOfGraph.AddEdge(edge);
//                             std::cout << "   Main: " << edge->GetLabel() << " matches query: " << queryEdge->GetLabel() << "\n";
//                             RecurveSubGraphMatch(edge->GetTarget().lock(), queryEdge->GetTarget().lock(), matchingPartOfGraph);
//                         }
//                     // }
//                 //edge->SetIsVisited(false);    
//                 }
//             }
//             //queryEdge->SetIsVisited(false);
//         }
//         return;
//     }

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
                //std::cout << " " << outEdge->GetLabel() << " set to unvisited.\n";
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

template <typename T> 
    unsigned int Graph<T>::GetSize()
    {
        return this->GetNodes().size() + this->GetEdges().size();
    }

}
#endif // T_GRAPH_HPP