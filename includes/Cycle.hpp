#ifndef T_CYCLE_HPP
#define T_CYCLE_HPP

#include <algorithm>    // std::reverse

namespace TemplateGraph
{
// 	template <class T> class Edge; // Forward declare Edge
	template <class T> class Node;
	template <class T>
	class Cycle
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		Cycle() {};
        Cycle(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes);

		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline std::vector<Node<T>*> GetNodes() {return nodes_;}
        inline Node<T>* GetRootNode() {return root_;}

		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
        inline void SetRootNode(Node<T>* root) {root_ = root;}
        inline void SetNodes(std::vector<Node<T>*> nodes) { nodes_ = nodes;}

        //////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////

	private:
		//////////////////////////////////////////////////////////
        //                  PRIVATE FUNCTIONS                   //
        //////////////////////////////////////////////////////////

        void TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes);

		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
        Node<T>* root_;
		std::vector<Node<T>*> nodes_;
	};

		//////////////////////////////////////////////////////////
        //                       DEFINITIONS                    //
        //////////////////////////////////////////////////////////



template <typename T>
    Cycle<T>::Cycle(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes)
	{
        this->SetRootNode(cycleRootNode);
        this->TrimPath(cycleRootNode, nodes);

    }

template <typename T>
    void Cycle<T>::TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> nodes)
    {
        //std::cout << "cycleRootNode is " << cycleRootNode->GetIndex() << "\n";
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
            nodes_.push_back(node);
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
        std::cout << "\nFucking Cycle Path: ";
        for (auto &node : nodes_)
        {
            std::cout << node->GetIndex() << ", ";
        }
        std::cout << std::endl;
    }
}
#endif // T_CYCLE_HPP