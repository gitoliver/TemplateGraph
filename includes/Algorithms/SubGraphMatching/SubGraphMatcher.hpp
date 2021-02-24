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
        inline void SaveCurrentSubGraph() {matchingSubGraphs_.push_back(currentSubGraph_);}
        inline std::vector<SubGraph<T>> GetMatches() {return matchingSubGraphs_;}
        inline std::string GetFunctionName() {return functionName_;}

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
        void RecurveSimpleMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode);
        inline unsigned int GetCurrentSubGraphSize() {return currentSubGraph_.GetSize();}
        inline void AddEdgeToSubGraph(std::shared_ptr <Edge<T>> edge) {currentSubGraph_.AddEdge(edge);}
        inline void RemoveEdgeFromCurrentSubGraph(std::shared_ptr <Edge<T>> edge) {currentSubGraph_.RemoveEdge(edge);}
        inline void PopEdgeFromCurrentSubGraph() {currentSubGraph_.PopEdge();}
        //////////////////////////////////////////////////////////
        //                  PRIVATE VARIABLES                   //
        //////////////////////////////////////////////////////////
 
        Graph<T> &mainGraph_;
        Graph<T> &queryGraph_;
        SubGraph<T> currentSubGraph_;
        std::vector<SubGraph<T>> matchingSubGraphs_;
        std::string functionName_ = "SGMSimpleMatch";

    };

// Limit on exploration limited by query, not main. Can revisit places in main.
// Need to account for matches with partial overlap...
template <typename T> // requires at least one edge in subGraph
    void SubGraphMatcher<T>::SimpleMatch() 
    {
        //this->ResetAllEdgesAndNodesToUnvisited();
        //queryGraph_.ResetAllEdgesAndNodesToUnvisited();
        //std::vector<SubGraph<T>> matchingSubGraphs; // Multiple parts of Graph can match subGraph. 
        for(auto &node : mainGraph_.GetNodes())
        {
            std::cout << node->GetLabel() << "\n";
            if ( node->GetLabel() == queryGraph_.GetRoot()->GetLabel() )
            {
                std::cout << "Queryroot: " << queryGraph_.GetRoot()->GetLabel() << " matches " << node->GetLabel() << "\n";
                //SubGraph<T> matchingPartOfGraph;
                this->RecurveSimpleMatch(node, queryGraph_.GetRoot());
                //mainGraph_.ResetAllEdgesAndNodesToUnvisited("SGMSimpleMatch");
                queryGraph_.ResetAllEdgesAndNodesToUnvisited(this->GetFunctionName());
                std::cout << "Finished checking that path.\nQuery size: " << queryGraph_.GetSize() << "\nMatch size: " << this->GetCurrentSubGraphSize() << "\n";
                // if (matchingPartOfGraph.GetSize() == queryGraph_.GetSize()) 
                // { // After recursion, if they are the same size, they now match.
                //     //matchingSubGraphs.push_back(matchingPartOfGraph); // A copy, but whatever man.
                //     AddMatchingSubGraph(matchingPartOfGraph);
                // }
            }
        }
        return ;
    }

template <typename T>
    void SubGraphMatcher<T>::RecurveSimpleMatch(std::shared_ptr<Node<T>> graphNode, std::shared_ptr<Node<T>> queryGraphNode)
    {
        //std::cout << "Match. EnteredRecurve. graphNode is " << graphNode->GetLabel() << ". queryGraphNode is " << queryGraphNode->GetLabel() << "\n"; 
        for (auto &queryEdge: queryGraphNode->GetEdges())
        {
            if (!queryEdge->GetIsVisitedBy(this->GetFunctionName()))
            {
                queryEdge->SetIsVisitedBy(this->GetFunctionName());
            //std::cout << "   " << queryEdge->GetLabel() << " of query, checking against edges:\n";
                for (auto &edge: graphNode->GetEdges())
                {
                    std::cout << "    Checking" << edge->GetLabel() << " of main graph\n.";
                    //std::cout << "    " << std::boolalpha << queryEdge->GetIsVisited() << " main is " << edge->GetIsVisited() << "\n";
                    // if (!edge->GetIsVisited())
                    // { // if edges are unvisited so far
                    //     edge->SetIsVisited(true);
                    if (queryEdge->CompareEdgeAndNodeLabels(edge))
                    {
                        this->AddEdgeToSubGraph(edge);
                        std::cout << "   Main: " << edge->GetLabel() << " matches query: " << queryEdge->GetLabel() << "\n";
                        std::cout << "   MatchSize: " << this->GetCurrentSubGraphSize() << " query: " << queryGraph_.GetSize() << "\n";
                        if (this->GetCurrentSubGraphSize() == queryGraph_.GetSize()) 
                        { //if they are the same size, they now match.
                            std::cout << "Saving matching graph\n";
                            this->SaveCurrentSubGraph();
                        }   
                        RecurveSimpleMatch(edge->GetTarget().lock(), queryEdge->GetTarget().lock());
                        //this->PopEdgeFromCurrentSubGraph();
                        
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