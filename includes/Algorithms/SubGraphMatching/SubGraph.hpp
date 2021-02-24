#ifndef T_SUBGRAPH_HPP
#define T_SUBGRAPH_HPP

#include <algorithm> // std::unique,sort,reverse
#include "Edge.hpp"

namespace TemplateGraph
{
 	template <class T>
	class SubGraph
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		SubGraph() {};
        
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
        std::vector<std::shared_ptr<Edge<T>>> GetEdges(); 
        std::vector<std::shared_ptr<Node<T>>> GetNodes();
        
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void AddEdge(std::shared_ptr <Edge<T>> edge) {edges_.push_back(edge);}
        inline void PopEdge() {edges_.pop_back();}
        void RemoveEdge(std::shared_ptr <Edge<T>> edge);
        void ResetAllEdgesAndNodesToUnvisited();

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        inline unsigned int GetSize() {return this->GetNodes().size() + this->GetEdges().size();}
        std::string Print(std::string type = "label");

	private:
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        std::vector<std::weak_ptr<Edge<T>>> edges_;
        // edge pairs between main and query?
	};
		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////

template <typename T>
    std::vector<std::shared_ptr<Edge<T>>> SubGraph<T>::GetEdges()
    {
        std::vector<std::shared_ptr<Edge<T>>> returnEdges;
        for (auto &weakEdge : edges_)
        {   // if able to lock, i.e. underlying thing still exists
            if(auto sharedEdge = weakEdge.lock())
                returnEdges.push_back(sharedEdge);
        }
        return returnEdges;
    } 

template <typename T>
    std::vector<std::shared_ptr<Node<T>>> SubGraph<T>::GetNodes() 
    { 
        std::vector<std::shared_ptr<Node<T>>> returnNodes;
        for(auto &edge : this->GetEdges())
        {
            std::shared_ptr source = edge->GetSource().lock();
            if (std::find(returnNodes.begin(), returnNodes.end(), source) == returnNodes.end())
                returnNodes.push_back(source);
            std::shared_ptr target = edge->GetTarget().lock();
            if (std::find(returnNodes.begin(), returnNodes.end(), target) == returnNodes.end())
                returnNodes.push_back(target);
        }
        return returnNodes;
    }

template <typename T>
    void SubGraph<T>::RemoveEdge(std::shared_ptr <Edge<T>> edge)
    {
        edges_.erase(std::remove(edges_.begin(), edges_.end(), edge), edges_.end());
        return;
    }

template <typename T> 
    std::string SubGraph<T>::Print(std::string type)
    {
        std::string output;
        for (auto &edge : this->GetEdges())
        {
            output += edge->Print(type);
            output += "\n";
        }
        return output;
    }

template <typename T> 
    void SubGraph<T>::ResetAllEdgesAndNodesToUnvisited()
    {
        for(auto &edge : this->GetEdges())
        {
           edge->SetIsVisited(false);
        }
    }

}
#endif // T_SUBGRAPH_HPP