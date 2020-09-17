#ifndef T_CYCLE_HPP
#define T_CYCLE_HPP


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

        void TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> &nodes);

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

    }


template <typename T>
    void Cycle<T>::TrimPath(Node<T>* cycleRootNode, std::vector<Node<T>*> &nodes)
    {
            
    }
}
#endif // T_CYCLE_HPP