#ifndef SUBGRAPH_MATCHER_HPP
#define SUBGRAPH_MATCHER_HPP

#include "../../Graph.hpp"

namespace TemplateGraph
{
template <class T>

	class SubGraphMatcher 
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		SubGraphMatcher(Graph<T> &mainGraph, Graph<T> &queryGraph) : mainGraph_ (mainGraph), queryGraph_ (queryGraph) {this->SimpleMatch();}
        
        //////////////////////////////////////////////////////////
        //                       ACCESSOR                       //
        //////////////////////////////////////////////////////////
        inline void AddMatchingSubGraph(SubGraph<T> match) {matchingSubGraphs_.push_back(match);}
        inline std::vector<SubGraph<T>> GetMatches() {return matchingSubGraphs_;}

        //////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////
        //                     FUNCTIONS                       //
        //////////////////////////////////////////////////////////

        void SimpleMatch();
    

    private:
    	//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////
        void RecurveSimpleMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode, SubGraph<T> &matchingPartOfGraph);

        //////////////////////////////////////////////////////////
        //                  PRIVATE VARIABLES                   //
        //////////////////////////////////////////////////////////
 
        Graph<T> &mainGraph_;
        Graph<T> &queryGraph_;
        std::vector<SubGraph<T>> matchingSubGraphs_;

    };

// Limit on exploration limited by query, not main. Can revisit places in main.
// Need to account for matches with partial overlap...
template <typename T> // requires at least one edge in subGraph
    void SubGraphMatcher<T>::SimpleMatch() 
    {
        //this->ResetAllEdgesAndNodesToUnvisited();
        queryGraph_.ResetAllEdgesAndNodesToUnvisited();
        //std::vector<SubGraph<T>> matchingSubGraphs; // Multiple parts of Graph can match subGraph. 
        for(auto &node : mainGraph_.GetNodes())
        {
            std::cout << node->GetLabel() << "\n";
            if ( node->GetLabel() == queryGraph_.GetRoot()->GetLabel() )
            {
                std::cout << "Queryroot: " << queryGraph_.GetRoot()->GetLabel() << " matches " << node->GetLabel() << "\n";
                SubGraph<T> matchingPartOfGraph;
                this->RecurveSimpleMatch(node, queryGraph_.GetRoot(), matchingPartOfGraph);
                mainGraph_.ResetAllEdgesAndNodesToUnvisited();
                queryGraph_.ResetAllEdgesAndNodesToUnvisited();
                std::cout << "Finished checking that path.\nQuery size: " << queryGraph_.GetSize() << "\nMatch size: " << matchingPartOfGraph.GetSize() << "\n";
                if (matchingPartOfGraph.GetSize() == queryGraph_.GetSize()) 
                { // After recursion, if they are the same size, they now match.
                    //matchingSubGraphs.push_back(matchingPartOfGraph); // A copy, but whatever man.
                    AddMatchingSubGraph(matchingPartOfGraph);
                }
            }
        }
        return ;
    }

template <typename T>
    void SubGraphMatcher<T>::RecurveSimpleMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode, SubGraph<T> &matchingPartOfGraph)
    {
        //std::cout << "Match. EnteredRecurve. graphNode is " << graphNode->GetLabel() << ". queryGraphNode is " << queryGraphNode->GetLabel() << "\n"; 
        for (auto &queryEdge: queryGraphNode->GetEdges())
        {
            if (!queryEdge->GetIsVisited())
            {
                queryEdge->SetIsVisited(true);
            //std::cout << "   " << queryEdge->GetLabel() << " of query, checking against edges:\n";
                for (auto &edge: graphNode->GetEdges())
                {
                    //std::cout << "    " << edge->GetLabel() << " of main graph\n.";
                    //std::cout << "    " << std::boolalpha << queryEdge->GetIsVisited() << " main is " << edge->GetIsVisited() << "\n";
                    // if (!edge->GetIsVisited())
                    // { // if edges are unvisited so far
                    //     edge->SetIsVisited(true);
                        if (queryEdge->CompareEdgeAndNodeLabels(edge))
                        {
                            matchingPartOfGraph.AddEdge(edge);
                            std::cout << "   Main: " << edge->GetLabel() << " matches query: " << queryEdge->GetLabel() << "\n";
                            RecurveSimpleMatch(edge->GetTarget().lock(), queryEdge->GetTarget().lock(), matchingPartOfGraph);
                        }
                    // }
                //edge->SetIsVisited(false);    
                }
            }
            //queryEdge->SetIsVisited(false);
        }
        return;
    }


}// TemplateGraph namespace
#endif // SUBGRAPH_MATCHER_HPP