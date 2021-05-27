#ifndef TEMPLATEGRAPH_ALGORITHMS_INCLUDE_TOTALCYCLEDECOMPOSITION_HPP
#define TEMPLATEGRAPH_ALGORITHMS_INCLUDE_TOTALCYCLEDECOMPOSITION_HPP

#include <set>
#include <stack>
#include <vector>
#include <memory>
#include <unordered_set>

//since we are going to have to make adj matricies for running algos
//include needed

#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GraphStructure/include/Graph.hpp"
#include "../../GraphStructure/include/Node.hpp"
#include "../../GraphStructure/include/HalfAdjacencyMatrix.hpp"

namespace
{

//struct to help create our spanning tree
struct TreeNode
{
	int index;
	TreeNode *parent;
};

//to help us validate that our tree path is actually unique, allows
//finding of all the different needed cycles
template<class T>
void unique_tree_path(TreeNode *pathNode,
		TemplateGraph::HalfAdjacencyMatrix<T> &mutatingMatrix)
{
	if (pathNode->parent != pathNode)
	{
		mutatingMatrix.connect(pathNode->index, pathNode->parent->index);
		unique_tree_path(pathNode->parent, mutatingMatrix);
	}
}

// to compute all of our fundamental cycles and return them as a vec
template<class T>
std::pair<std::vector<std::unordered_set<TemplateGraph::Node<T>*>>,
		std::vector<TemplateGraph::HalfAdjacencyMatrix<T>>> computeFundamentalCycles(
		TemplateGraph::Graph<T> &interestingGraph)
{
	std::vector<std::unordered_set<TemplateGraph::Node<T>*>> funCycleSet;

	std::vector<TemplateGraph::HalfAdjacencyMatrix<T>> funCycleAdjMatrixSet;

	//to construct our actual spanning tree
	std::unique_ptr<TreeNode[]> aTree(
			new TreeNode[interestingGraph.getNodes().size()]);

	std::stack<unsigned int> nodeStack;
	//start randomly with our 0 node
	nodeStack.push(0);

	//copy over our matrix we are gonna be mutating. Dont want to screw
	//	up our actual structure
	TemplateGraph::HalfAdjacencyMatrix<T> mutatingAdjMatrix(
			interestingGraph.getAdjMatrix());

	//initially have all treenodes as their own parent, will create our spanning
	// tree while running algo
	for (unsigned int currIndex = 0;
			currIndex < interestingGraph.getNodes().size(); ++currIndex)
	{
		aTree[currIndex].parent = &aTree[currIndex];
		aTree[currIndex].index = currIndex;
	}

	while (nodeStack.size() > 0)
	{
		//grab most recent node
		unsigned int currNodeIndex = nodeStack.top();
		nodeStack.pop();
		TreeNode &currTreeNode = aTree[currNodeIndex];

//hit all edges connecting to this node
		for (unsigned int anotherNodeIndex = 0;
				anotherNodeIndex < interestingGraph.getNodes().size();
				anotherNodeIndex++)
		{
			//not connected we skip current iteration
			if (!(mutatingAdjMatrix.isConnected(currNodeIndex, anotherNodeIndex)))
			{
				continue;
			}
			//is our anotherNode already in tree? True if parent element of treenode
			// doesnt point to self
			if (aTree[anotherNodeIndex].parent != &aTree[anotherNodeIndex])
			{
				TemplateGraph::HalfAdjacencyMatrix<T> currNodePath(
						interestingGraph.getNodes());
				TemplateGraph::HalfAdjacencyMatrix<T> anotherNodePath(
						interestingGraph.getNodes());

				//to get our path from the current node
				unique_tree_path(&aTree[currNodeIndex], currNodePath);
				//to get our path from the other node
				unique_tree_path(&aTree[anotherNodeIndex], anotherNodePath);

				//we go ahead and actually connect the 2 nodes to show our path
				currNodePath.connect(currNodeIndex, anotherNodeIndex);

				//xor our 2 matriciies to get our fundamental cycle
				TemplateGraph::HalfAdjacencyMatrix<T> funCycleAdjMatrix(
						currNodePath ^ anotherNodePath);

				std::unordered_set<TemplateGraph::Node<T>*> funCycleNodeSet;

				//TODO: Make this legitimate, slow...
				for (unsigned int aNodeIndex = 0;
						aNodeIndex < interestingGraph.getNodes().size();
						aNodeIndex++)
				{
					for (unsigned int bNodeIndex = 0;
							bNodeIndex < interestingGraph.getNodes().size();
							bNodeIndex++)
					{
						if (funCycleAdjMatrix.isConnected(aNodeIndex,
								bNodeIndex))
						{

							funCycleNodeSet.insert(
									interestingGraph.getNodeFromIndex(
											aNodeIndex));

							funCycleNodeSet.insert(
									interestingGraph.getNodeFromIndex(
											bNodeIndex));
						}
					}
				}			// end out garbage node finder
				if (funCycleNodeSet.size() > 0)
				{
					funCycleSet.push_back(funCycleNodeSet);
					funCycleAdjMatrixSet.push_back(funCycleAdjMatrix);
				}
				else
				{
					badBehavior(__LINE__, __func__, "Fun cycle size of 0");
				}

			}
			else
			{
				//node is not in spanning tree thus we add it
				aTree[anotherNodeIndex].parent = &currTreeNode;
				nodeStack.push(anotherNodeIndex);
			}
			//either way remove this connection since we already hit it
			mutatingAdjMatrix.disconnect(currNodeIndex, anotherNodeIndex);
		}
	}
	std::pair<std::vector<std::unordered_set<TemplateGraph::Node<T>*>>,
			std::vector<TemplateGraph::HalfAdjacencyMatrix<T>>> funCycleInfo(
			funCycleSet, funCycleAdjMatrixSet);

	return funCycleInfo;
}			// end compute fundamental cycles

template<class T>
void validateCycleMatrixRecursive(
		TemplateGraph::HalfAdjacencyMatrix<T> &matrixToValidate,
		unsigned int &currPathLength, const int interestingNodeIndex,
		unsigned int prevNodeIndex, std::set<unsigned int> &visitedTracker)
{
	//just makes sure our call stack isnt stupid large, we can mutate this to our needs
	if (currPathLength > 750)
	{
		badBehavior(__LINE__, __func__, "Our path is too long");
	}
	else
	{
		for (unsigned int curiousIndex = 0;
				curiousIndex < matrixToValidate.getNumNodes(); curiousIndex++)
		{
			if ((matrixToValidate.isConnected(interestingNodeIndex,
					curiousIndex)) && (curiousIndex != prevNodeIndex))
			{
				auto possVisited = visitedTracker.find(curiousIndex);
				if (possVisited != visitedTracker.end())
				{
					//if we have visited and not at end we leave
					return;
				}
				++currPathLength;
				visitedTracker.insert(interestingNodeIndex);
				validateCycleMatrixRecursive(matrixToValidate, currPathLength,
						curiousIndex, interestingNodeIndex, visitedTracker);
				return;
			}
		}
		badBehavior(__LINE__, __func__,
				"Dead end when checking our cycle validation");
	}
}			//end validate recursion

template<class T>
bool validateCycleMatrix(TemplateGraph::HalfAdjacencyMatrix<T> &matrixToCheck)
{
	unsigned int pathLength = 0;
	for (unsigned int aNodeIndex = 0; aNodeIndex < matrixToCheck.getNumNodes();
			aNodeIndex++)
	{
		for (unsigned int bNodeIndex = 0;
				bNodeIndex < matrixToCheck.getNumNodes(); bNodeIndex++)
		{
			if (matrixToCheck.isConnected(aNodeIndex, bNodeIndex))
			{
				//when we are connected we check our cycle matrix
				++pathLength;
				std::set<unsigned int> isVisited;
				isVisited.insert(aNodeIndex);
				validateCycleMatrixRecursive(matrixToCheck, pathLength,
						bNodeIndex, aNodeIndex, isVisited);
				return ((pathLength + 1) == matrixToCheck.getNumEdges());
			}
		}
	}
	badBehavior(__LINE__, __func__, "No edges");
	return false;
}			// end validate cycle matrix

} // end our anon namespace

namespace cycleDetector
{

// This is our main function that actually returns all the decomposed cycles.
//		We return a vector that contains pairs of the nodes within a cycle
//		and the edges so we know our exact connectivity. The reasoning for this
//		is explained a little bit down.
template<typename T>
std::vector<
		std::pair<std::unordered_set<TemplateGraph::Node<T>*>,
				std::unordered_set<TemplateGraph::Edge<T>*>>> totalCycleDetect(
		TemplateGraph::Graph<T> &inputGraph)
{

	//	The following is used to rip out all of our fundamental cycles which
	// 		are used to compute all cycles. This is currently inneficient due
	// 		to my use of double storing (i.e. storing our nodes sets & adj sets
	// 		in 2 different stl containers. Will keep for now to increase readability
	//
	// 		TODO: Need to not do above.

	std::pair<std::vector<std::unordered_set<TemplateGraph::Node<T>*>>,
			std::vector<TemplateGraph::HalfAdjacencyMatrix<T>>> funCycleInfo =
			computeFundamentalCycles(inputGraph);

	std::vector<TemplateGraph::HalfAdjacencyMatrix<T>> funCycleAdj =
			funCycleInfo.second;

	//	This is kind of a doozy but this way each cycle we have contains both out nodes
	// 		and edges. A "pair" contains a first and second member, pretty self explanatory.
	//		Now if we insert them into any stl that uses a key-value pair type relation
	// 		the first member will be utilized as the key and the second will be used as
	// 		the value.
	std::vector<
			std::pair<std::unordered_set<TemplateGraph::Node<T>*>,
					std::unordered_set<TemplateGraph::Edge<T>*>>> allCycleEdgesNodes;

	//	All of our fundamental cycles are still cycles so we throw them over
	// 		to our  whole adj list.
	std::vector<TemplateGraph::HalfAdjacencyMatrix<T>> allCyclesAdj(
			funCycleAdj);

	std::vector<bool> combinitoricsVector(funCycleInfo.second.size());

	for (unsigned int currFunAdj = 2; currFunAdj <= funCycleAdj.size();
			currFunAdj++)
	{
		std::fill_n(combinitoricsVector.begin(), currFunAdj, 1);
		std::fill_n(combinitoricsVector.rbegin(),
				combinitoricsVector.size() - currFunAdj, 0);

		do
		{
			TemplateGraph::HalfAdjacencyMatrix<T> mutatingMatrix(
					inputGraph.getNodes());

			unsigned int edgeCount = 0;

			for (unsigned int anotherFunAdj = 0;
					anotherFunAdj < funCycleAdj.size(); anotherFunAdj++)
			{
				// pretty much running every combo of our fundamental cycles
				// 		since our fundamental cycles form a total cycle basis
				// 		if we hit every combo of them we know we will produce
				// 		every cycle present in the graph
				//
				if (combinitoricsVector[anotherFunAdj])
				{
					mutatingMatrix = mutatingMatrix
							^ funCycleAdj[anotherFunAdj];
					edgeCount += funCycleAdj[anotherFunAdj].getNumEdges();
				}
			}
			//our base case
			if (currFunAdj == 2)
			{
				if (edgeCount > mutatingMatrix.getNumEdges())
				{
					allCyclesAdj.push_back(mutatingMatrix);
				}
			}
			else
			{
				if (validateCycleMatrix(mutatingMatrix))
				{
					if (edgeCount > mutatingMatrix.getNumEdges())
					{
						allCyclesAdj.push_back(mutatingMatrix);
					}
				}
			}

		} while (std::prev_permutation(combinitoricsVector.begin(),
				combinitoricsVector.end()));
	} //end our for loop

	// Just transfering our cycles to the node ptrs and edge ptrs.Up until here we
	//		were running our algo only using adj lists. Now we convert to our (hopefully)
	//		more useful data.
	for (TemplateGraph::HalfAdjacencyMatrix<T> currentCycleAdj : allCyclesAdj)
	{
		std::unordered_set<TemplateGraph::Node<T>*> temporaryNodeCycleSet;
		std::unordered_set<TemplateGraph::Edge<T>*> temporaryEdgeCycleSet;
		for (unsigned int aNodeIndex = 0;
				aNodeIndex < inputGraph.getNodes().size(); aNodeIndex++)
		{
			for (unsigned int bNodeIndex = 0;
					bNodeIndex < inputGraph.getNodes().size(); bNodeIndex++)
			{
				if (currentCycleAdj.isConnected(bNodeIndex, aNodeIndex))
				{
					temporaryNodeCycleSet.insert(
							inputGraph.getNodeFromIndex(aNodeIndex));
					temporaryNodeCycleSet.insert(
							inputGraph.getNodeFromIndex(bNodeIndex));

					// Now to insert our specifc edges in order to ensure we completely destroy any
					// 		possibility of being able to produce an induced cycle from what is returned.
					//		This method is easiest, if we try to run checks after getting our node list
					//		returned it will cause a large increase in complexity. Tried to run checks
					//		but ended up being a total waste of time, would be easier to just add edges
					//		to our returned data.
					//
					// 		TODO: Silence worries regarding using "shared_from_this()" where I grab
					//				the edge. Our edge is returned from everything within
					// 				the parenthesis of "insert". Also we could just pop
					//				The previous 2 Nodes out of our nodeCycle set since
					//				it is now switched to handle order. Was done for this reason.
					//
					//		NOTE: Turns out I was being dumb with the "set" stl preserving
					//				order. It turns out that the elements are ordered within
//									the set by use of a comparator. This means ordering is not
					//				based on when we inserted. Switched back to unordered, I
					//				tested using the set stl and it did return the correct
					//				order (i.e. order we traversed everything) BUT this was due
					//				to memory following the comparator.
					TemplateGraph::Edge<T> *tempLoleEdge =
							inputGraph.getNodeFromIndex(aNodeIndex)->getConnectingEdge(
									inputGraph.getNodeFromIndex(bNodeIndex));
					temporaryEdgeCycleSet.insert(tempLoleEdge);
				}
			}
		}
		if (temporaryNodeCycleSet.size() == 0)
		{
			badBehavior(__LINE__, __func__, "Our found cycle is empty");
		}
		else
		{
			// We now have our set of edges and set of nodes that create a cycle
			//		so we must put them into our vector as a pair.
			allCycleEdgesNodes.push_back(
			{ temporaryNodeCycleSet, temporaryEdgeCycleSet });

		}
	}
	return allCycleEdgesNodes;
} // end total cycle detect

}

#endif //end TEMPLATEGRAPH_ALGORITHMS_INCLUDE_TOTALCYCLEDECOMPOSITION_HPP
