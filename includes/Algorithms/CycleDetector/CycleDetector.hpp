#ifndef CYCLE_DETECTOR_HPP
#define CYCLE_DETECTOR_HPP

#include "../../Graph.hpp"
#include "../../Node.hpp"
#include "../../Edge.hpp"
#include "../../Structure/HalfAdjMatrix.hpp"

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
//graph worked on below
//https://dreampuf.github.io/GraphvizOnline/#digraph%20G%20%7B%0A%09Bobie-%3ESteve%0A%09Steve-%3ERonne%0A%09Ronne-%3EBingo%0A%09Bingo-%3EMarsh%0A%09Marsh-%3EDelux%0A%09Steve-%3EFrank%0A%09Delux-%3EFrank%0A%09Ronne-%3EDelux%0A%09Ronne-%3EFrank%0A%09Delux-%3EBingo%0A%09Ronne-%3EBingo1%0A%09Bingo1-%3EMasrsh1%0A%09Bridge1%20-%3E%20cycle1%0A%09cycle1%20-%3E%20cycle2%0A%09cycle1%20-%3E%20cycle3%0A%09cycle2%20-%3E%20cycle3%0A%09cycle1%20-%3E%20Steve%0A%7D

/*just pass in a root node to this, store the cycles within to show how algos work.
 * Eventually we must not use a root node due to the fact that if we delete a bridge
 * can disconnect a part of the graph we may want to access. With current mem structure
 * it would be impossible to run algo on the two parts of the previously connected graph.
 *
 * NOTE: Once the algos are proven we will convert this class into a namespace that gets called. For sake of lines
 * not caring about getters or setters
 */

namespace TemplateGraph
{
template<class T>
class CycleDetector
{
public:
	CycleDetector();
	CycleDetector(std::string name, Graph<T> *graph);

	std::vector<std::unordered_set<Node<T>*>> getCyclesContainingNode(
			Node<T> *interestedNode);
	std::vector<std::unordered_set<Node<T>*>> getCyclesSpecificLength(
			unsigned int cycleLength);

	//should be able to edge and node at same time p easy
	void leafDetect();
	void bridgeDetect();

	void detectMegaCycles();

	/*named due to the method used, was unsure if we also wanted to
	 * incorporate a graph coloring algo instead. Still on the table, graph
	 * coloring is easier but much less verbose
	 */
	void xorCycleDetect();

private:

	//adj matrix representations
	HalfAdjMatrix<T> adjMatrix;
	std::vector<HalfAdjMatrix<T>> funCyclesAdj;
	std::vector<HalfAdjMatrix<T>> allCyclesAdj;

	//nodes that these correspond to. get Node <--> int by using the lookup tables
	std::vector<std::unordered_set<Node<T>*>> megaCycles;
	std::vector<std::unordered_set<Node<T>*>> funCycles;
	std::vector<std::unordered_set<Node<T>*>> allCycles;

	//collection of some structure signifiers
	//note that we consider an edge that goes to a leaf as a bridge edge
	std::unordered_set<Edge<T>*> bridgeEdges;
	std::unordered_set<Node<T>*> leafNodes;
	std::unordered_set<Node<T>*> bridgeNodes;

	//lookup table stuff
	std::unordered_map<Node<T>*, int> indexLookup;
	std::unordered_map<int, Node<T>*> nodeLookup;
	void populateIndexLookup();

	//a bunch of stupid prints to make life easier for now. Will remove
	void bridgePrinter();
	void leafPrinter();
	void megaCyclePrinter();
	void funCyclePrinter();
	void allCyclePrinter();
	void printAdjMatrix(HalfAdjMatrix<T> m);

	//helpers for our xor
	void computeFunCycles();

	//helpers to make life easier, can put somewhere else later
	int neighborCount(Node<T> *n);
	//get all edges between 2 nodes, helps due to multigraph
	std::vector<Edge<T>*> connectingEdges(Node<T> *nodeA, Node<T> *nodeB);

	void dfsBridgeHelper(Node<T> *nodeA, Node<T> *nodeB,
			std::vector<int> &preorderVisit, std::vector<int> &lowestTime,
			unsigned int &counter);

	std::vector<Node<T>*> dfsGetMegaReachable(int megaCycleNum,
			std::vector<int> &intHasVisited, Node<T> *interestingNode);

	void populateAdjMatrix();

	//just so we can refer to the graph we are running algos on.
	//This will eventually be  changed
	Graph<T> *graph;
	std::string name;
	std::vector<Node<T>*> allNodes;

	//to help us with our spanning tree
	struct TreeNode
	{
		int index;
		TreeNode *parent;
	};

	inline void unique_tree_path(TreeNode *pathNode,
			HalfAdjMatrix<T> &mutatingMat)
	{
		if (pathNode->parent != pathNode)
		{
			mutatingMat.connect(pathNode->index, pathNode->parent->index);
			unique_tree_path(pathNode->parent, mutatingMat);
		}
	}
};

template<class T>
CycleDetector<T>::CycleDetector()
{
	std::cout << "no" << std::endl;

	this->graph = NULL;

}

template<class T>
CycleDetector<T>::CycleDetector(std::string name, Graph<T> *graph)
{
	this->name = name;
	this->graph = graph;

	std::vector<Node<T>*> allNodes = this->graph->GetNodes();
	populateIndexLookup();

	//due to issues with construction
	this->adjMatrix.initializeWorkaround(allNodes);
	populateAdjMatrix();

	this->leafDetect();
	this->leafPrinter();

	this->bridgeDetect();
	this->bridgePrinter();

	this->detectMegaCycles();
	this->megaCyclePrinter();

	this->xorCycleDetect();
	allCyclePrinter();
	funCyclePrinter();

	//showing works for whatever
	for (Node<T> *currentNode : this->allNodes)
	{
		std::cout
				<< "Cycles containing node: " + currentNode->GetLabel() + "\n";
		int prettyCycleCounter = 0;
		for (std::unordered_set<Node<T>*> currentCycle : this->getCyclesContainingNode(
				currentNode))
		{
			std::cout
					<< "\tCycle #" + std::to_string(prettyCycleCounter)
							+ "\n\t\t";
			for (Node<T> *currentNodeInCycle : currentCycle)
			{
				std::cout << currentNodeInCycle->GetLabel() + ", ";
			}
			prettyCycleCounter++;
			std::cout << "\n";
		}
	}
	std::cout << "\n\n";
	for (unsigned int cycleLength = 3, cycleIterator = 0;
			cycleIterator < allCycles.size(); cycleIterator++, cycleLength++)
	{
		if (this->getCyclesSpecificLength(cycleLength).size() != 0)
			std::cout
					<< "Cycles of length: " + std::to_string(cycleLength)
							+ "\n";
		for (std::unordered_set<Node<T>*> currentCycle : this->getCyclesSpecificLength(
				cycleLength))
		{
			std::cout << "\tCycle #" + std::to_string(cycleLength) + "\n\t\t";
			for (Node<T> *currentNode : currentCycle)
			{
				std::cout << currentNode->GetLabel() + ", ";
			}
			std::cout << "\n";
		}

	}
}

//also detects some edge-bridges
template<class T>
void CycleDetector<T>::leafDetect()
{
	for (Node<T> *currentNode : allNodes)
	{
		//lazy check in case a leaf has multiple edges to the same node
		std::unordered_set<Node<T>*> tempNeighborCheck;
		std::vector<Node<T>*> tempNeighbors = currentNode->GetNeighbors();
		std::copy(tempNeighbors.begin(), tempNeighbors.end(),
				std::inserter(tempNeighborCheck, tempNeighborCheck.end()));
		if (tempNeighborCheck.size() == 1)
		{
			//found leaf, know all our connecting edges are bridges to our leaf also
			if (this->leafNodes.count(currentNode))
			{
				//already in set
				std::cout << "warning node already in set :"
						<< currentNode->GetLabel() << std::endl;
			}
			else
			{
				this->leafNodes.insert(currentNode);
				std::vector<Edge<T>*> nodeEdges = currentNode->GetEdges();
				std::copy(nodeEdges.begin(), nodeEdges.end(),
						std::inserter(this->bridgeEdges,
								this->bridgeEdges.end()));
			}
		}
	}
}

//need to make sure this works with multiple edges, i.e.as a bridge with the node
template<class T>
void CycleDetector<T>::bridgeDetect()
{
	//ALGO HELPING STUFF
	/*	Note that the only thing OG is meaning is original. Just wanting
	 * 		to keep track of structures used to track information. This
	 * 		was done when I pulled the tracking variables out of our class
	 * 		so they werent members
	 */
	//lowest time for certain node to be visited
	std::vector<int> lowestTimeOG(allNodes.size(), -1);
	//keep track of preorder visits
	std::vector<int> preorderVisitsOG(allNodes.size(), -1);
	unsigned int counterOG = 0;
	//2 edges connected to a node that is acting like a bridge should both be labeled
	for (Node<T> *currentNode : allNodes)
	{
		if (preorderVisitsOG[indexLookup[currentNode]] == -1)
		{
			dfsBridgeHelper(currentNode, currentNode, preorderVisitsOG,
					lowestTimeOG, counterOG);
		}
	}
	//now we know if all edges connected to a node that is not a leaf node it must be a bridge node
	for (Node<T> *currentNode : allNodes)
	{
		bool toInsert = false;
		for (Edge<T> *currentEdge : currentNode->GetEdges())
		{
			if (this->bridgeEdges.count(currentEdge))
			{
				toInsert = true;
			}
			else
			{
				toInsert = false;
				break;
			}
		}
		if (!this->leafNodes.count(currentNode) && toInsert)
			this->bridgeNodes.insert(currentNode);
	}
}

template<class T>
void CycleDetector<T>::dfsBridgeHelper(Node<T> *nodeA, Node<T> *nodeB,
		std::vector<int> &preorderVisit, std::vector<int> &lowestTime,
		unsigned int &counter)
{
	int nodeBIndex = indexLookup[nodeB];
	preorderVisit[nodeBIndex] = counter++;
	lowestTime[nodeBIndex] = preorderVisit[nodeBIndex];
	//run dfs on adjacent, aka neighbors
	for (Node<T> *bCurrentNeighbor : nodeB->GetNeighbors())
	{
		int wIndex = indexLookup[bCurrentNeighbor];
		if (preorderVisit[wIndex] == -1)
		{
			dfsBridgeHelper(nodeB, bCurrentNeighbor, preorderVisit, lowestTime,
					counter);
			lowestTime[nodeBIndex] = std::min(lowestTime[nodeBIndex],
					lowestTime[wIndex]);
			if (lowestTime[wIndex] == preorderVisit[wIndex])
			{
				//now get all edges between the two and keep track of them
				for (Edge<T> *edgeConnectingCurrentToB : this->connectingEdges(
						bCurrentNeighbor, nodeB))
				{
					if (!(this->bridgeEdges.count(edgeConnectingCurrentToB)))
					{
						this->bridgeEdges.insert(edgeConnectingCurrentToB);
					}
				}
			}
		}
		else if (wIndex != indexLookup[nodeA])
			lowestTime[nodeBIndex] = std::min(lowestTime[nodeBIndex],
					preorderVisit[wIndex]);
	}
}

template<class T>
void CycleDetector<T>::detectMegaCycles()
{
	//we want to first get our list of all reachable nodes from each specific node
	//then we want to get rid of all of our duplicates
	std::vector<int> hasVisisted(allNodes.size(), 0);
	//stores all reachable from node calling at
	std::unordered_map<int, std::vector<Node<T>*>> reachableNodes;
	//tracking what number megacycle we are on
	int megaCycleNum = 0;
	for (Node<T> *currentNode : allNodes)
	{
		//bridge nodes prevents us from traversing to any nodes that are a bridge.
		//this allows for beading. Important in full cycle detect. Same with leaf.
		bool badNode = (this->bridgeNodes.count(currentNode))
				|| (this->leafNodes.count(currentNode));
		if ((hasVisisted[indexLookup[currentNode]] == 0) && (!badNode))
		{
			megaCycleNum++;
			reachableNodes[hasVisisted[indexLookup[currentNode]]] =
					this->dfsGetMegaReachable(megaCycleNum, hasVisisted,
							currentNode);
		}
	}
	for (auto reachableNodePair : reachableNodes)
	{
		std::unordered_set<Node<T>*> tempMegaCycle;
		for (Node<T> *currentNode : reachableNodePair.second)
		{
			tempMegaCycle.insert(currentNode);
		}
		this->megaCycles.push_back(tempMegaCycle);
	}
}

template<class T>
std::vector<Node<T>*> CycleDetector<T>::dfsGetMegaReachable(int megaCycleNum,
		std::vector<int> &intHasVisited, Node<T> *interestingNode)
{
	//queue for our dfs to keep track
	std::queue<Node<T>*> nodeQ;
	nodeQ.push(interestingNode);
	intHasVisited[indexLookup[interestingNode]] = megaCycleNum;
	std::vector<Node<T>*> reachableNodes;
	while (!nodeQ.empty())
	{
		//node from front of our q, dunno if need better name
		Node<T> *frontNode = nodeQ.front();
		nodeQ.pop();
		//we only want to add the reachable node if it is NOT a leaf or a bridge.
		reachableNodes.push_back(frontNode);
		for (Node<T> *frontNodeNeighbor : frontNode->GetNeighbors())
		{
			//Check for making sure we are not traversing over a bridge. Take trip
			//is used to make sure that the connecting edges between two nodes is not a bridge.
			bool takeTrip = true;
			for (Edge<T> *edgeConnectingFrontToNeighbor : this->connectingEdges(
					frontNode, frontNodeNeighbor))
			{
				//if we hit a bridge edge then we stop right here. Bridges sep out mega cycles
				if (this->bridgeEdges.count(edgeConnectingFrontToNeighbor))
				{
					takeTrip = false;
					break;
				}
			}
			if (!intHasVisited[indexLookup[frontNodeNeighbor]] && takeTrip)
			{
				intHasVisited[indexLookup[frontNodeNeighbor]] = megaCycleNum;
				nodeQ.push(frontNodeNeighbor);
			}
		}
	}
	return reachableNodes;
}

template<class T>
void CycleDetector<T>::xorCycleDetect()
{
	//should be matching one another, just being verbose
	//can make this better, i.e.checking if our # nodes matches #nodes in matrix, etc.
	if (this->funCycles.size() == 0 || this->funCyclesAdj.size() == 0)
	{
		std::string msgInfo =
				(this->funCycles.size() == 0 && this->funCyclesAdj.size() == 0) ?
						" BOTH FUNCYCLES AND FUNCYCLES ADJ BEING ZERO" :
						"MISSMATCH OF FUNCYCLES AND FUNCYCLESADJ";
		std::cout
				<< "\n\n$$$$ AUTO REDOING FUN CYCLES DUE TO " + msgInfo
						+ " $$$$\n\n";
		this->computeFunCycles();
	}
	//since we are only dealing with the adj matrix in our algo
	//	I want to prevent all references to the map of nodes that create the actual cycles
	allCyclesAdj = funCyclesAdj;
	//the combinitorics vector allows us to run every needed permutation of our fundamental cycles.
	// it is used to create all our cycles and run the whole xor-ing part
	std::vector<bool> combinitoricsVector(funCyclesAdj.size());
	for (unsigned int currentFunCycleAdj = 2;
			currentFunCycleAdj <= funCyclesAdj.size(); currentFunCycleAdj++)
	{
		std::fill_n(combinitoricsVector.begin(), currentFunCycleAdj, 1);
		std::fill_n(combinitoricsVector.rbegin(),
				combinitoricsVector.size() - currentFunCycleAdj, 0);
		do
		{
			HalfAdjMatrix<T> mutatingMatrix(allNodes);
			int edgeCount = 0;
			for (unsigned int anotherFunCycleAdj = 0;
					anotherFunCycleAdj < funCyclesAdj.size();
					anotherFunCycleAdj++)
			{
				if (combinitoricsVector[anotherFunCycleAdj])
				{
					mutatingMatrix = mutatingMatrix
							^ funCyclesAdj[anotherFunCycleAdj];
					edgeCount += funCyclesAdj[anotherFunCycleAdj].getNumEdges();
				}
			}
			if (currentFunCycleAdj == 2)
			{
				if (edgeCount > mutatingMatrix.getNumEdges())
				{
					this->allCyclesAdj.push_back(mutatingMatrix);
				}
			}
			else
			{
				//fix this
				if (mutatingMatrix.validateCycleMatrix(mutatingMatrix))
				{
					if (edgeCount > mutatingMatrix.getNumEdges())
					{
						this->allCyclesAdj.push_back(mutatingMatrix);
					}
				}
			}
		} while (std::prev_permutation(combinitoricsVector.begin(),
				combinitoricsVector.end()));
	}

	//making sure our sets match one another. This is badly done. Could use and adj list or something. Unsure if needed
	for (HalfAdjMatrix<T> currentCycleAdj : this->allCyclesAdj)
	{
		std::unordered_set<Node<T>*> temporaryCycleSet;
		for (unsigned int nodeAIndex = 0; nodeAIndex < allNodes.size();
				nodeAIndex++)
		{
			for (unsigned int nodeBIndex = 0; (nodeBIndex < allNodes.size());
					nodeBIndex++)
			{
				if (currentCycleAdj.isConnected(nodeBIndex, nodeAIndex))
				{
					temporaryCycleSet.insert(nodeLookup[nodeAIndex]);
					temporaryCycleSet.insert(nodeLookup[nodeBIndex]);
				}
			}
		}
		if (temporaryCycleSet.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBAD CYCLESET\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		else
		{
			this->allCycles.push_back(temporaryCycleSet);
		}
	}
}

template<class T>
void CycleDetector<T>::computeFunCycles()
{

	this->funCycles.clear();
	this->funCyclesAdj.clear();

	//if we are clearing our cycle basis we must also clear our cycles
	//node that we will still get all correct cycles due to our cycle basis but this prevents dupes and is easy
	this->allCycles.clear();
	this->allCyclesAdj.clear();

	//to create our spanning tree
	std::unique_ptr<TreeNode[]> aTree(new TreeNode[allNodes.size()]);
	std::stack<unsigned int> nodeStack;

	// Start arbitrarily with the first Node!
	nodeStack.push(0);

	//copy our adj matrix because we are going to have to mutate it
	HalfAdjMatrix<T> mutatingAdjMatrix = adjMatrix;

//first have all tree nodes as parent, then make tree while algo progresses
	for (unsigned int currentNodeIndex = 0; currentNodeIndex < allNodes.size();
			++currentNodeIndex)
	{
		aTree[currentNodeIndex].parent = &aTree[currentNodeIndex];
		aTree[currentNodeIndex].index = currentNodeIndex;
	}

	while (nodeStack.size() > 0)
	{
		//node to look at index
		unsigned int currentNodeIndex = nodeStack.top();
		nodeStack.pop();
		TreeNode &currentTreeNode = aTree[currentNodeIndex];

		// Iterate though all edges connecting this node
		for (unsigned int anotherNodeIndex = 0;
				anotherNodeIndex < allNodes.size(); ++anotherNodeIndex)
		{
			// not connected?
			if (!mutatingAdjMatrix.isConnected(currentNodeIndex,
					anotherNodeIndex))
				continue;

			// Is the foreign node already in the tree?
			// This is the case, if the parent element of the TreeNode does not point to itself!
			if (aTree[anotherNodeIndex].parent != &aTree[anotherNodeIndex])
			{
				// Fundamental Cycle found!
				// Get unique paths from both nodes within the spanning tree!
				HalfAdjMatrix<T> currentNodePath(allNodes);
				HalfAdjMatrix<T> anotherNodePath(allNodes);
				unique_tree_path(&aTree[currentNodeIndex], currentNodePath);
				unique_tree_path(&aTree[anotherNodeIndex], anotherNodePath);

				// also the connection between currentNodeIndex and j has to be inserted
				// to ONE of the two paths (which one does not matter)
				currentNodePath.connect(currentNodeIndex, anotherNodeIndex);
				HalfAdjMatrix<T> cycleToInsert(
						currentNodePath ^ anotherNodePath);
				std::unordered_set<Node<T>*> tempFunCycle;
				//found a fundamental cycle so now need to make something that prevents dupes to store it
				for (unsigned int aTempNodeIndex = 0;
						aTempNodeIndex < allNodes.size(); aTempNodeIndex++)
				{
					for (unsigned int bTempNodeIndex = 0;
							bTempNodeIndex < allNodes.size(); bTempNodeIndex++)
					{
						if (cycleToInsert.isConnected(bTempNodeIndex,
								aTempNodeIndex))
						{
							tempFunCycle.insert(nodeLookup[aTempNodeIndex]);
							tempFunCycle.insert(nodeLookup[bTempNodeIndex]);
						}
					}
				}
				if (tempFunCycle.size() != 0)
				{

					this->funCycles.push_back(tempFunCycle);
					this->funCyclesAdj.push_back(cycleToInsert);
				}
				else
				{
					std::cout
							<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nFUN CYCLE SIZE 0\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
				}
			}
			else
			{
				//node is not in spanning tree thus we must add it
				aTree[anotherNodeIndex].parent = &currentTreeNode;
				// add the node to search stack
				nodeStack.push(anotherNodeIndex);
			}
			// Either way remove this connection!
			mutatingAdjMatrix.disconnect(currentNodeIndex, anotherNodeIndex);
		}
	}
}

template<class T>
std::vector<std::unordered_set<Node<T>*> > CycleDetector<T>::getCyclesContainingNode(
		Node<T> *interestedNode)
{
	std::vector<std::unordered_set<Node<T>*>> cyclesContainingSpecificNode;
	for (std::unordered_set<Node<T>*> currentCycleSet : allCycles)
	{
		if (currentCycleSet.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nSET OF 0 AH 0\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		if (currentCycleSet.count(interestedNode))
			cyclesContainingSpecificNode.push_back(currentCycleSet);
	}
	return cyclesContainingSpecificNode;
}

template<class T>
std::vector<std::unordered_set<Node<T>*> > CycleDetector<T>::getCyclesSpecificLength(
		unsigned int cycleLength)
{
	std::vector<std::unordered_set<Node<T>*>> cyclesOfSpecificLength;
	for (std::unordered_set<Node<T>*> currentCycleSet : allCycles)
	{
		//never hits
		if (currentCycleSet.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nSET OF 0 AH 0\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		if (currentCycleSet.size() == cycleLength)
			cyclesOfSpecificLength.push_back(currentCycleSet);
	}
	return cyclesOfSpecificLength;
}

//also populates all nodes list
template<class T>
void CycleDetector<T>::populateIndexLookup()
{
	//using an unordered map to enforce no dupes
	//need to redo both at same time to ensure they are matching
	this->indexLookup.clear();
	this->nodeLookup.clear();
	int currentIndexCount = 0;
	//use an unordered set so we can keep track of dupes
	std::unordered_set<Node<T>*> tempNodes;
	for (Node<T> *currentNode : this->graph->GetNodes())
	{
		if (tempNodes.count(currentNode))
		{
			std::cout << "Warning: dupe detected\n";
		}
		else
		{
			tempNodes.insert(currentNode);
		}
	}
	for (Node<T> *currentNode : tempNodes)
	{
		this->allNodes.push_back(currentNode);
		this->indexLookup.insert(
				std::pair<Node<T>*, int>(currentNode, currentIndexCount));
		this->nodeLookup.insert(
				std::pair<int, Node<T>*>(currentIndexCount, currentNode));
		currentIndexCount++;
	}

}

//groups all edges connecting nodes together. This allows our algos to
template<class T>
std::vector<Edge<T>*> CycleDetector<T>::connectingEdges(Node<T> *nodeA,
		Node<T> *nodeB)
{
	std::vector<Edge<T>*> connectingEdgesReturn;
	std::unordered_set<Edge<T>*> aEdges;
	for (Edge<T> *tempEdge : nodeA->GetEdges())
	{
		aEdges.insert(tempEdge);
	}
	std::unordered_set<Edge<T>*> bEdges;
	for (Edge<T> *tempEdge : nodeB->GetEdges())
	{
		bEdges.insert(tempEdge);
	}
	for (auto &aCurrentEdge : aEdges)
	{
		//found common edge between the 2
		if (bEdges.count(aCurrentEdge))
		{
			connectingEdgesReturn.push_back(aCurrentEdge);
		}
	}
	return connectingEdgesReturn;
}

template<class T>
void CycleDetector<T>::bridgePrinter()
{
	std::cout << "\n~~~~PRINTING BRIDGE NODES~~~~\n";
	for (Node<T> *currentBridgeNode : this->bridgeNodes)
	{
		std::cout << "\nBRIDGE NODE: " << currentBridgeNode->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING BRIDGE EDGES~~~~\n";
	for (Edge<T> *currentBridgeEdge : this->bridgeEdges)
	{
		std::cout << "\nBRIDGE EDGE: " << currentBridgeEdge->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
void CycleDetector<T>::leafPrinter()
{
	std::cout << "\n~~~~PRINTING LEAF NODES~~~~\n";
	for (Node<T> *currentLeafNode : this->leafNodes)
	{
		std::cout << "\nLEAF NODE: " << currentLeafNode->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
void CycleDetector<T>::megaCyclePrinter()
{
	std::cout << "\n~~~~PRINTING MEGA CYCLES~~~~\n";
	int prettyMegaCycleCounter = 0;
	for (std::unordered_set<Node<T>*> currentMegaCycle : this->megaCycles)
	{
		std::cout
				<< "Mega Cycle #" + std::to_string(prettyMegaCycleCounter)
						+ "\n\tNodes: ";
		for (Node<T> *currentNode : currentMegaCycle)
		{
			std::cout << currentNode->GetLabel() + ", ";
		}
		prettyMegaCycleCounter++;
		std::cout << "\n";
	}
	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
int CycleDetector<T>::neighborCount(Node<T> *n)
{
	std::unordered_set<Node<T>*> t;
	std::vector<Node<T>*> tempN = n->GetNeighbors();
	std::copy(tempN.begin(), tempN.end(), std::inserter(t, t.end()));
	return t.size();
}

template<class T>
void CycleDetector<T>::funCyclePrinter()
{
	int prettyFunCycleCounter = 0;
	for (std::unordered_set<Node<T>*> currentFunCycle : this->funCycles)
	{
		std::cout << "Fundamental cycle #" << prettyFunCycleCounter
				<< " elements: ";
		for (Node<T> *currentNode : currentFunCycle)
		{
			std::cout << currentNode->GetLabel() << ", ";
		}
		prettyFunCycleCounter++;
		std::cout << "\n";
	}
	std::cout << "\n";
}

template<class T>
inline void CycleDetector<T>::allCyclePrinter()
{
	int prettyCycleCounter = 0;
	for (std::unordered_set<Node<T>*> currentCycle : this->allCycles)
	{
		std::cout << "Cycle #" << prettyCycleCounter << " elements: ";
		for (Node<T> *currentNode : currentCycle)
		{
			std::cout << currentNode->GetLabel() << ", ";
		}
		prettyCycleCounter++;
		std::cout << "\n";
	}
	std::cout << "\n";
}

template<class T>
void CycleDetector<T>::populateAdjMatrix()
{
	for (Node<T> *currentNode : this->allNodes)
	{
		for (Node<T> *currentNeighbor : currentNode->GetNeighbors())
		{
			if (!(adjMatrix.isConnected(indexLookup[currentNode],
					indexLookup[currentNeighbor])))
			{
				this->adjMatrix.connect(indexLookup[currentNode],
						indexLookup[currentNeighbor]);
			}
		}
	}
}

template<class T>
inline void CycleDetector<T>::printAdjMatrix(HalfAdjMatrix<T> m)
{
	//just to print because for some reason some adj matricies are unpop
	for (unsigned int currentNodeIndex = 0; currentNodeIndex < allNodes.size();
			currentNodeIndex++)
	{
		std::cout
				<< "\n!!!! PRINTING ADJ of Node-> "
						+ nodeLookup[currentNodeIndex]->GetLabel()
						+ ", connections: \t";
		for (unsigned int otherNodeIndex = 0; otherNodeIndex < allNodes.size();
				otherNodeIndex++)
		{
			if (m.isConnected(otherNodeIndex, currentNodeIndex))
			{
				std::cout << nodeLookup[otherNodeIndex]->GetLabel() + ", ";
			}
		}

	}
	std::cout << "\n";
}

}
#endif // CYCLE_DETECTOR_HPP
