#ifndef STRUCTUALIDENTIFIER_HPP
#define STRUCTUALIDENTIFIER_HPP

#include <vector>
#include <memory>

#include "../../GraphStructure/include/Node.hpp"
#include "../../GraphStructure/include/Graph.hpp"

#include "../../LazyPrints/LazyPrinters.hpp"

/*	Note: Due to me being dumb, we will just use ints to set our structural id. They are as
 * 			follows:
 *
 * 			0 = UNKNOWN
 * 			1 = BRIDGE
 * 			2 = LEAF
 * 			3 = INCYCLE
 *
 *			I ranted in the leafDetect function about this.
 */

namespace
{

}

namespace structualIdentifier
{

/* This is easy to do, we know that a node is a leaf if it has 1 neighbor. Thus we label our node
 * 	that is being checked as a leaf and the edge associated with it as a bridge edge.
 *
 */
template<class T>
void leafDetect(TemplateGraph::Graph<T> &graphToLeaf)
{
	for (TemplateGraph::Node<T>* currNode : graphToLeaf.getRawNodes())
	{
		//we are at leaf
		if (currNode->getNeighbors().size() == 1)
		{
			/* TODO: Tried fiddling around and want to eventuall use something like:
			 * 			currNode->setStructuralTypeIdentifier(LEAF);
			 * 		 	but for some reason this gets the compiler angry. As of now
			 * 		 	I will be using ints to help us just get the code going.
			 * 		 	Very bummed about this right now and was hunting for ages
			 * 		 	of whats wrong. Will eventually find.
			 *
			 *	Set node as leaf
			 */
			currNode->setStructuralTypeIdentifier(2);
			int errorEdgeCounter = 0;
			for (TemplateGraph::Edge<T>* currEdge : currNode->getEdges())
			{
				if (errorEdgeCounter > 1)
				{
					badBehavior(__LINE__, __func__, "WE FOUND A NODE THAT HAS MORE THAN ONE EDGE BUT ONLY HAS ONE NEIGHBOR");
					break;
				}
				else
				{
					//set edge as bridge
					currEdge->setStructuralTypeIdentifier(1);
				}
				errorEdgeCounter++;
			}// end currEdge for
		}

	}//end all nodes for


}//end leaf detect


}


#endif // end STRUCTUALIDENTIFIER_HPP
