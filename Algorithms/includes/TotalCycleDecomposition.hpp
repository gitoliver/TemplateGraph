#ifndef TOTALCYCLEDECOMPOSITION_HPP
#define TOTALCYCLEDECOMPOSITION_HPP

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

	std::unique_ptr<TreeNode[]> aTree(
			new TreeNode[interestingGraph.getRawNodes().size()]);

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
			currIndex < interestingGraph.getRawNodes().size(); ++currIndex)
	{
		aTree[currIndex].parent = &aTree[currIndex];
		aTree[currIndex].index = currIndex;
	}

	while (nodeStack.size() > 0)
	{
		unsigned int currNodeIndex = nodeStack.top();
		nodeStack.pop();
		TreeNode &currTreeNode = aTree[currNodeIndex];

//hit all edges connecting to this node
		for (unsigned int anotherNodeIndex = 0;
				anotherNodeIndex < interestingGraph.getRawNodes().size();
				anotherNodeIndex++)
		{
			//not connected we skip iteration
			if (!(mutatingAdjMatrix.isConnected(currNodeIndex, anotherNodeIndex)))
			{
				continue;
			}
			//is our anotherNode already in tree? True if parent element of treenode
			// doesnt point to self
			if (aTree[anotherNodeIndex].parent != &aTree[anotherNodeIndex])
			{
				TemplateGraph::HalfAdjacencyMatrix<T> currNodePath(
						interestingGraph.getRawNodes());
				TemplateGraph::HalfAdjacencyMatrix<T> anotherNodePath(
						interestingGraph.getRawNodes());

				unique_tree_path(&aTree[currNodeIndex], currNodePath);
				unique_tree_path(&aTree[anotherNodeIndex], anotherNodePath);

				currNodePath.connect(currNodeIndex, anotherNodeIndex);

				TemplateGraph::HalfAdjacencyMatrix<T> funCycleAdjMatrix(
						currNodePath ^ anotherNodePath);

				std::unordered_set<TemplateGraph::Node<T>*> funCycleNodeSet;

				//TODO: Make this legitimate, slow...
				for (unsigned int aNodeIndex = 0;
						aNodeIndex < interestingGraph.getRawNodes().size();
						aNodeIndex++)
				{
					for (unsigned int bNodeIndex = 0;
							bNodeIndex < interestingGraph.getRawNodes().size();
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
//switch to weak_ptr so we can correctly observe, now just go
template<typename T>
std::vector<std::unordered_set<TemplateGraph::Node<T>*>> totalCycleDetect(
		TemplateGraph::Graph<T> &inputGraph)
{

	std::pair<std::vector<std::unordered_set<TemplateGraph::Node<T>*>>,
			std::vector<TemplateGraph::HalfAdjacencyMatrix<T>>> funCycleInfo =
			computeFundamentalCycles(inputGraph);

	std::vector<std::unordered_set<TemplateGraph::Node<T>*>> funCycleNodeSets =
			funCycleInfo.first;
	std::vector<TemplateGraph::HalfAdjacencyMatrix<T>> funCycleAdj =
			funCycleInfo.second;

	std::vector<std::unordered_set<TemplateGraph::Node<T>*>> allCyclesNodeList;

	std::vector<TemplateGraph::HalfAdjacencyMatrix<T>> allCyclesAdj(
			funCycleAdj);

	std::vector<bool> combinitoricsVector(funCycleAdj.size());

	for (unsigned int currFunAdj = 2; currFunAdj <= funCycleAdj.size();
			currFunAdj++)
	{
		std::fill_n(combinitoricsVector.begin(), currFunAdj, 1);
		std::fill_n(combinitoricsVector.rbegin(),
				combinitoricsVector.size() - currFunAdj, 0);

		do
		{
			TemplateGraph::HalfAdjacencyMatrix<T> mutatingMatrix(
					inputGraph.getRawNodes());

			unsigned int edgeCount = 0;

			for (unsigned int anotherFunAdj = 0;
					anotherFunAdj < funCycleAdj.size(); anotherFunAdj++)
			{
				if (combinitoricsVector[anotherFunAdj])
				{
					mutatingMatrix = mutatingMatrix
							^ funCycleAdj[anotherFunAdj];
					edgeCount += funCycleAdj[anotherFunAdj].getNumEdges();
				}
			}
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

	for (TemplateGraph::HalfAdjacencyMatrix<T> currentCycleAdj : allCyclesAdj)
	{
		std::unordered_set<TemplateGraph::Node<T>*> temporaryCycleSet;
		for (unsigned int aNodeIndex = 0;
				aNodeIndex < inputGraph.getRawNodes().size(); aNodeIndex++)
		{
			for (unsigned int bNodeIndex = 0;
					bNodeIndex < inputGraph.getRawNodes().size(); bNodeIndex++)
			{
				if (currentCycleAdj.isConnected(bNodeIndex, aNodeIndex))
				{
					temporaryCycleSet.insert(
							inputGraph.getNodeFromIndex(aNodeIndex));
					temporaryCycleSet.insert(
							inputGraph.getNodeFromIndex(bNodeIndex));
				}
			}
		}
		if (temporaryCycleSet.size() == 0)
		{
			badBehavior(__LINE__, __func__, "Our found cycle is empty");
		}
		else
		{
			allCyclesNodeList.push_back(temporaryCycleSet);
		}
	}
	return allCyclesNodeList;
}// end total cycle detect

}

#endif //TOTALCYCLEDECOMPOSITION_HPP
