#ifndef T_CYCLEPOINT_HPP
#define T_CYCLEPOINT_HPP

#include <algorithm>    // std::reverse

namespace TemplateGraph
{
// 	template <class T> class Edge; // Forward declare Edge
	template <class T> class Node;
	template <class T>
	class CyclePoint
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		CyclePoint() {};
        CyclePoint(Node<T>* startNode);

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline Node<T>* GetRootNode() {return graphRoot_;}
        inline Node<T>* GetCycleStartNode() {return startNode_;}
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetRootNode(Node<T>* root) {graphRoot_ = root;}
        inline void SetCycleStartNode(Node<T>* startNode) {startNode_ = startNode;}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
        void FindPathsToSelf(Node<T>* currentNode, std::vector<Node<T>*> currentPath);

	private:
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

        //void TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        Node<T>* graphRoot_;
        Node<T>* startNode_;
        std::vector<std::vector<Node<T>*>> paths_; 
	};

		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////

template <typename T>
    void CyclePoint<T>::FindPathsToSelf(Node<T>* currentNode, std::vector<Node<T>*> currentPath)
    {
        currentPath.push_back(currentNode);
        auto allNodeNeighbors = currentNode->GetNeighbors();
        auto findCyclePoint = std::find(allNodeNeighbors.begin(), allNodeNeighbors.end(), this->GetCycleStartNode());
        if (findCyclePoint != allNodeNeighbors.end()) // i.e. std::find found it in the neighbor list
        {
            currentPath.push_back(this->GetCycleStartNode());
            std::cout << "Found it!, current path is: ";
            for (auto &node : currentPath)
            {
                std::cout << node->GetIndex() << ", ";
            }
            currentPath.pop_back();
            std::cout << std::endl;
        }
        for(auto &inNeighbor : currentNode->GetIncomingEdgeNeighbors())
        {
            this->FindPathsToSelf(inNeighbor, currentPath);
        }
        currentPath.pop_back(); // remove currentNode if falling out of recursion
        return;
    }

template <typename T>
    CyclePoint<T>::CyclePoint(Node<T>* startNode)
	{
         this->SetCycleStartNode(startNode);
        // this->TrimPath(cycleRootNode, nodes);
    }

// template <typename T>
//     void CyclePoint<T>::TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes)
//     {
//         //std::cout << "cycleRootNode is " << cycleRootNode->GetIndex() << "\n";
//         std::reverse(nodes.begin(), nodes.end());
//         // path will be A B C D E F D, where D is cycleRootNode. Need to trim off A B C.
//         bool foundFirst = false;
//         for (auto &node : nodes)
//         {
//             nodes_.push_back(node);
//             if (node->GetIndex() == cycleRootNode->GetIndex())
//             {
//                 if (foundFirst == true)
//                 {
//                     break;
//                 }
//                 foundFirst = true;
//             }
//         }
//     }
}
#endif // T_CYCLEPOINT_HPP