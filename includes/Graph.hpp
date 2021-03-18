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
        Graph(Node<T>* rootNode) : root_ (rootNode) {};
        
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
        std::vector<Node<T>*> GetNodes();
        unsigned int GetSize();
        std::string Print(std::string type = "label");
        
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetRoot(Node<T>* node) {root_ = node;}
        void ResetAllEdgesAndNodesToUnvisited(std::string visitorName = "");
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
        void RecurveFindNodes(std::vector<Node<T>*> &foundNodes, Node<T>* currentNode);
        std::vector<Edge<T>*> GetEdges();
        inline Node<T>* GetRoot() {return root_;}

        
        //////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

        //void RecurveSubGraphMatch(Node<T>* graphNode, Node<T>* queryGraphNode, SubGraph<T> &matchingPartOfGraph);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        Node<T>* root_;
        //std::vector<std::weak_ptr<Edge<T>>> edges_;
	};
		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////

template <typename T> 
    std::string Graph<T>::Print(std::string type)
    {
        std::string output;
        for (auto &edge : this->GetEdges())
        {
            output += edge->Print();
            output += "\n";
        }
        return output;
    }

template <typename T>
    std::vector<Edge<T>*> Graph<T>::GetEdges() 
    { 
        std::vector<Edge<T>*> edgesInGraph;
        for (auto &node : this->GetNodes())
        {
            //std::cout << "Outedges of " << node->GetLabel() << ":\n";
            for (auto &outEdge : node->GetOutEdges())
            {
                //outEdge->SetIsVisited(false);
                edgesInGraph.push_back(outEdge);
                //std::cout << " " << outEdge->GetLabel() << " set to unvisited.\n";
            }
        }
        return edgesInGraph;
    } 

template <typename T>
    std::vector<Node<T>*> Graph<T>::GetNodes() 
    { 
        std::vector<Node<T>*> nodesInGraph;
        this->RecurveFindNodes(nodesInGraph, this->GetRoot());
        for(auto &node : nodesInGraph)
        {
            node->RemoveVisitor("GraphRecurveFindNodes");
        }
        return nodesInGraph;
    }

template <typename T>
    void Graph<T>::RecurveFindNodes(std::vector<Node<T>*> &foundNodes, Node<T>* currentNode)
    {
        currentNode->AddVisitor("GraphRecurveFindNodes");
        foundNodes.push_back(currentNode);
        for (auto &neighbor : currentNode->GetNeighbors())
        {
            if (!neighbor->GetIsVisitedBy("GraphRecurveFindNodes"))
            {
                this->RecurveFindNodes(foundNodes, neighbor);
            }
        }
    }

template <typename T> 
    void Graph<T>::ResetAllEdgesAndNodesToUnvisited(std::string visitorName)
    {
        for(auto &edge : this->GetEdges())
        {
           edge->RemoveVisitor(visitorName);
        }
    }

template <typename T> 
    unsigned int Graph<T>::GetSize()
    {
        return this->GetNodes().size() + this->GetEdges().size();
    }

}
#endif // T_GRAPH_HPP