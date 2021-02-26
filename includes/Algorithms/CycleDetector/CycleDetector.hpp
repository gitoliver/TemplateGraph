#ifndef CYCLE_DETECTOR_HPP
#define CYCLE_DETECTOR_HPP

#include "../../Graph.hpp"

namespace TemplateGraph
{
template <class T>

	class CycleDetector : public Graph<T>
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		CycleDetector() {};
        CycleDetector(std::shared_ptr<Node<T>> rootNode) 
        {   
            this->SetRoot(rootNode);
            this->ResetAllEdgesAndNodesToUnvisited();
        }
        //////////////////////////////////////////////////////////
        //                     FUNCTIONS                       //
        //////////////////////////////////////////////////////////
        void DetectCyclesInDFSGraph();
    private:
    	//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////
        inline std::vector<std::vector<std::weak_ptr<Node<T>>>> GetPaths() {return paths_;}
    	std::vector<std::shared_ptr<Node<T>>> GetCyclePoints();
        void FindPathsToSelf(std::shared_ptr<Node<T>> cycleStartNode, std::shared_ptr<Node<T>> currentNode, std::vector<std::weak_ptr<Node<T>>> currentPath);
        void TrimCyclePath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes);
		//////////////////////////////////////////////////////////
        //                  PRIVATE VARIABLES                   //
        //////////////////////////////////////////////////////////
 
        std::vector<std::vector<std::weak_ptr<Node<T>>>> paths_;

        //////////////////////////////////////////////////////////
        //                       FRIENDS                        //
        //////////////////////////////////////////////////////////
    };

// Uses a Recurve function to find cycles.
template <typename T>
    void CycleDetector<T>::DetectCyclesInDFSGraph()
    {
        for (auto &cyclePoint : this->GetCyclePoints())
        {
            std::cout << "Cyclepoint is " << cyclePoint->GetIndex() << "\n";
            std::vector<std::weak_ptr<Node<T>>> currentPath;
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
        for (auto &currentPath : this->GetPaths())
        {
            for (auto &node : currentPath)
            {
                std::cout << node.lock()->GetIndex() << ", ";
            }
            std::cout << "\n";
        }
        return;
    }


template <typename T>
    std::vector<std::shared_ptr<Node<T>>> CycleDetector<T>::GetCyclePoints()
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
    void CycleDetector<T>::FindPathsToSelf(std::shared_ptr<Node<T>> cycleStartNode, std::shared_ptr<Node<T>> currentNode, std::vector<std::weak_ptr<Node<T>>> currentPath)
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
                std::cout << node.lock()->GetIndex() << ", ";
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
    void CycleDetector<T>::TrimCyclePath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes)
    {
        //std::cout << "cycleRootNode is " << cycleRootNode->GetIndex() << "\n";
        std::vector<Node<T>*> trimmedPath;
        std::reverse(nodes.begin(), nodes.end());
      //  std::cout << "Reversed Nodes: ";
        // for (auto &node : nodes)
        // {
        //     std::cout << node->GetIndex() << ", ";
        // }
        
        // path will be A B C D E F D, where D is cycleRootNode. Need to trim off A B C.
        bool foundFirst = false;
        for (auto &node : nodes)
        {
            trimmedPath.push_back(node);
            if (node->GetIndex() == cycleRootNode->GetIndex())
            {
                if (foundFirst == true)
                {
                    break;
                }
                foundFirst = true;
            }
        }
       //auto position = TemplateGraph::find(nodes, cycleRootNode);
        // nodes.erase(nodes.begin(), position - 1);
        std::cout << "\nCycle Path: ";
        for (auto &node : trimmedPath)
        {
            std::cout << node->GetIndex() << ", ";
        }
        std::cout << std::endl;
    }

}// TemplateGraph namespace
#endif // CYCLE_DETECTOR_HPP