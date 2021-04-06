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
			unsigned int cycL);

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
	std::vector<Edge<T>*> connectingEdges(Node<T> *a, Node<T> *b);

	void dfsBridgeHelper(Node<T> *u, Node<T> *v, std::vector<int> &pre,
			std::vector<int> &low, unsigned int &counter);

	std::vector<Node<T>*> dfsGetMegaReachable(int compNum,
			std::vector<int> &intVisited, Node<T> *s);

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

	inline void unique_tree_path(TreeNode *pNode, HalfAdjMatrix<T> &muMat)
	{
		if (pNode->parent != pNode)
		{
			muMat.connect(pNode->index, pNode->parent->index);
			unique_tree_path(pNode->parent, muMat);
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
	for (Node<T> *n : this->allNodes)
	{
		std::cout << "Cycles containing node: " + n->GetLabel() + "\n";
		int i = 0;
		for (std::unordered_set<Node<T>*> uS : this->getCyclesContainingNode(n))
		{
			std::cout << "\tCycle #" + std::to_string(i) + "\n\t\t";
			for (Node<T> *cN : uS)
			{
				std::cout << cN->GetLabel() + ", ";
			}
			i++;
			std::cout << "\n";
		}
	}
	std::cout << "\n";
	std::cout << "\n";
	//warning noticed how i have more keys than values in all cyles. i.e.
	for (unsigned int i = 3, j = 0; j < allCycles.size(); j++, i++)
	{
		if (this->getCyclesSpecificLength(i).size() != 0)
			std::cout << "Cycles of length: " + std::to_string(i) + "\n";
		for (std::unordered_set<Node<T>*> uS : this->getCyclesSpecificLength(i))
		{
			std::cout << "\tCycle #" + std::to_string(i) + "\n\t\t";
			for (Node<T> *cN : uS)
			{
				std::cout << cN->GetLabel() + ", ";
			}
			std::cout << "\n";
		}

	}
}

//also detects some edge-bridges
template<class T>
void CycleDetector<T>::leafDetect()
{
	for (Node<T> *cn : allNodes)
	{
		//lazy check in case a leaf has multiple edges to the same node
		std::unordered_set<Node<T>*> tempCheck;
		std::vector<Node<T>*> tempN = cn->GetNeighbors();
		std::copy(tempN.begin(), tempN.end(),
				std::inserter(tempCheck, tempCheck.end()));
		if (tempCheck.size() == 1)
		{
			//found leaf, know all our connecting edges are bridges to our leaf also
			if (this->leafNodes.count(cn))
			{
				//already in set
				std::cout << "warning node already in set :" << cn->GetLabel()
						<< std::endl;
			}
			else
			{
				this->leafNodes.insert(cn);
				std::vector<Edge<T>*> e = cn->GetEdges();
				std::copy(e.begin(), e.end(),
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
	//lowest time for certain node to be visited
	std::vector<int> lowOG(allNodes.size(), -1);
	//keep track of preorder visits
	std::vector<int> preOG(allNodes.size(), -1);
	unsigned int counterOG = 0;
	//2 edges connected to a node that is acting like a bridge should both be labeled
	for (Node<T> *n : allNodes)
	{
		if (preOG[indexLookup[n]] == -1)
		{
			dfsBridgeHelper(n, n, preOG, lowOG, counterOG);
		}
	}
	//now we know if all edges connected to a node that is not a leaf node it must be a bridge node
	for (Node<T> *n : allNodes)
	{
		bool toInsert = false;
		for (Edge<T> *e : n->GetEdges())
		{
			if (this->bridgeEdges.count(e))
			{
				toInsert = true;
			}
			else
			{
				toInsert = false;
				break;
			}
		}
		if (!this->leafNodes.count(n) && toInsert)
			this->bridgeNodes.insert(n);
	}
}

template<class T>
void CycleDetector<T>::dfsBridgeHelper(Node<T> *u, Node<T> *v,
		std::vector<int> &pre, std::vector<int> &low, unsigned int &counter)
{
	int vIndex = indexLookup[v];
	pre[vIndex] = counter++;
	low[vIndex] = pre[vIndex];
	//run dfs on adj, aka neighbors
	for (Node<T> *w : v->GetNeighbors())
	{
		int wIndex = indexLookup[w];
		if (pre[wIndex] == -1)
		{
			dfsBridgeHelper(v, w, pre, low, counter);
			low[vIndex] = std::min(low[vIndex], low[wIndex]);
			if (low[wIndex] == pre[wIndex])
			{
				//now get all edges between the two and keep track of them
				for (Edge<T> *e : this->connectingEdges(w, v))
				{
					if (!(this->bridgeEdges.count(e)))
					{
						this->bridgeEdges.insert(e);
					}
				}
			}
		}
		else if (wIndex != indexLookup[u])
			low[vIndex] = std::min(low[vIndex], pre[wIndex]);
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
	//tracking what graph "component" node is part of.
	int componentNum = 0;
	for (Node<T> *n : allNodes)
	{
		//bridge nodes prevents us from traversing to any nodes that are a bridge.
		//this allows for beading. Important in full cycle detect. Same with leaf.
		bool badNode = (this->bridgeNodes.count(n))
				|| (this->leafNodes.count(n));
		if ((hasVisisted[indexLookup[n]] == 0) && (!badNode))
		{
			componentNum++;
			reachableNodes[hasVisisted[indexLookup[n]]] =
					this->dfsGetMegaReachable(componentNum, hasVisisted, n);
		}
	}
	for (auto nL : reachableNodes)
	{
		std::unordered_set<Node<T>*> tempN;
		for (Node<T> *n : nL.second)
		{
			tempN.insert(n);
		}
		this->megaCycles.push_back(tempN);
	}
}

template<class T>
std::vector<Node<T>*> CycleDetector<T>::dfsGetMegaReachable(int compNum,
		std::vector<int> &intVisited, Node<T> *s)
{
	//queue for our dfs to keep track
	std::queue<Node<T>*> nQ;
	nQ.push(s);
	intVisited[indexLookup[s]] = compNum;
	std::vector<Node<T>*> reachableNodes;
	while (!nQ.empty())
	{
		Node<T> *u = nQ.front();
		nQ.pop();
		//we only want to add the reachable node if it is NOT a leaf or a bridge.
		reachableNodes.push_back(u);
		for (Node<T> *nN : u->GetNeighbors())
		{
			//Check for making sure we are not traversing over a bridge. Take trip
			//is used to make sure that the connecting edges between two nodes is not a bridge.
			bool takeTrip = true;
			for (Edge<T> *eD : this->connectingEdges(u, nN))
			{
				//turns out I do need this
				if (this->bridgeEdges.count(eD))
				{
					takeTrip = false;
					break;
				}
			}
			if (!intVisited[indexLookup[nN]] && takeTrip)
			{
				intVisited[indexLookup[nN]] = compNum;
				nQ.push(nN);
			}
		}
	}
	return reachableNodes;
}

template<class T>
void CycleDetector<T>::xorCycleDetect()
{
	//should be matching one another, just being verbose
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
	std::vector<bool> combiV(funCyclesAdj.size());
	for (unsigned int l = 2; l <= funCyclesAdj.size(); l++)
	{
		std::fill_n(combiV.begin(), l, 1);
		std::fill_n(combiV.rbegin(), combiV.size() - l, 0);

		do
		{
			HalfAdjMatrix<T> M(allNodes);
			int edCount = 0;
			for (unsigned int i = 0; i < funCyclesAdj.size(); i++)
			{
				if (combiV[i])
				{
					M = M ^ funCyclesAdj[i];
					edCount += funCyclesAdj[i].getNumEdges();
				}
			}
			if (l == 2)
			{
				if (edCount > M.getNumEdges())
				{
					this->allCyclesAdj.push_back(M);
				}
			}
			else
			{
				if (M.validateCycleMatrix(M))
				{
					if (edCount > M.getNumEdges())
					{
						this->allCyclesAdj.push_back(M);
					}
				}
			}
		} while (std::prev_permutation(combiV.begin(), combiV.end()));
	}

	//making sure our sets match one another. This is badly done. Could use and adj list or something. Unsure if needed
	for (HalfAdjMatrix<T> hA : this->allCyclesAdj)
	{
		std::unordered_set<Node<T>*> cycSet;
		for (unsigned int i = 0; i < allNodes.size(); i++)
		{
			for (unsigned int j = 0; (j < allNodes.size()); j++)
			{
				if (hA.isConnected(j, i))
				{
					cycSet.insert(nodeLookup[i]);
					cycSet.insert(nodeLookup[j]);
				}
			}
		}
		if (cycSet.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBAD CYCLESET\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		else
		{
			this->allCycles.push_back(cycSet);
		}
	}
}

template<class T>
void CycleDetector<T>::computeFunCycles()
{

	this->funCycles.clear();
	this->funCyclesAdj.clear();

	//if we are clearing our cycle basis we must also clear our cycles
	this->allCycles.clear();
	this->allCyclesAdj.clear();

	//to create our spanning tree
	std::unique_ptr<TreeNode[]> aTree(new TreeNode[allNodes.size()]);
	std::stack<unsigned int> nodeStack;

	// Start arbitrarily with the first Node!
	nodeStack.push(0);

	//copy our adj matrix because we are going to have to mutate it
	HalfAdjMatrix<T> adjMutMat = adjMatrix;

//first have all tree nodes as parent, then make tree while algo progresses
	for (size_t i = 0; i < allNodes.size(); ++i)
	{
		aTree[i].parent = &aTree[i];
		aTree[i].index = i;
	}

	while (nodeStack.size() > 0)
	{
		//node to look at index
		size_t currentNodeIndex = nodeStack.top();
		nodeStack.pop();
		TreeNode &currentTreeNode = aTree[currentNodeIndex];

		// Iterate though all edges connecting this node:
		for (size_t j = 0; j < allNodes.size(); ++j)
		{
			// not connected?
			if (!adjMutMat.isConnected(currentNodeIndex, j))
				continue;

			// Is the foreign node already in the tree?
			// This is the case, if the parent element of the TreeNode does not point to itself!
			if (aTree[j].parent != &aTree[j])
			{
				// Fundamental Cycle found!
				// Get unique paths from both nodes within the spanning tree!
				HalfAdjMatrix<T> pi(allNodes), pj(allNodes);
				unique_tree_path(&aTree[currentNodeIndex], pi);
				unique_tree_path(&aTree[j], pj);

				// also the connection between currentNodeIndex and j has to be inserted
				// to ONE of the two paths (which one does not matter)
				pi.connect(currentNodeIndex, j);
				HalfAdjMatrix<T> cycleToInsert(pi ^ pj);
				std::unordered_set<Node<T>*> tempC;
				for (unsigned int i = 0; i < allNodes.size(); i++)
				{
					for (unsigned int j = 0; j < allNodes.size(); j++)
					{
						if (cycleToInsert.isConnected(j, i))
						{
							Node<T> *tNI = nodeLookup[i];
							Node<T> *tNJ = nodeLookup[j];
							tempC.insert(tNI);
							tempC.insert(tNJ);
						}
					}
				}
				if (tempC.size() != 0)
				{

					this->funCycles.push_back(tempC);
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
				aTree[j].parent = &currentTreeNode;
				// add the node to search stack
				nodeStack.push(j);
			}
			// Either way remove this connection!
			adjMutMat.disconnect(currentNodeIndex, j);
		}
	}
}

template<class T>
std::vector<std::unordered_set<Node<T>*> > CycleDetector<T>::getCyclesContainingNode(
		Node<T> *interestedNode)
{
	std::vector<std::unordered_set<Node<T>*>> retVec;
	for (std::unordered_set<Node<T>*> cU : allCycles)
	{
		if (cU.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nSET OF 0 AH 0\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		if (cU.count(interestedNode))
			retVec.push_back(cU);
	}
	return retVec;
}

template<class T>
std::vector<std::unordered_set<Node<T>*> > CycleDetector<T>::getCyclesSpecificLength(
		unsigned int cycL)
{
	std::vector<std::unordered_set<Node<T>*>> retVec;
	for (std::unordered_set<Node<T>*> cU : allCycles)
	{
		if (cU.size() == 0)
		{
			std::cout
					<< "\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nSET OF 0 AH 0\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		if (cU.size() == cycL)
			retVec.push_back(cU);
	}
	return retVec;
}

//also populates all nodes list
template<class T>
void CycleDetector<T>::populateIndexLookup()
{
	//using an unordered map to enforce no dupes
	this->indexLookup.clear();
	int i = 0;
	std::unordered_set<Node<T>*> tN;
	for (Node<T> *n : this->graph->GetNodes())
	{
		if (tN.count(n))
		{
			std::cout << "Warning: dupe detected\n";
		}
		else
		{
			tN.insert(n);
		}
	}
	for (Node<T> *n : tN)
	{
		this->allNodes.push_back(n);
		this->indexLookup.insert(std::pair<Node<T>*, int>(n, i));
		this->nodeLookup.insert(std::pair<int, Node<T>*>(i, n));
		i++;
	}

}

//groups all edges connecting nodes together. This allows our algos to
template<class T>
std::vector<Edge<T>*> CycleDetector<T>::connectingEdges(Node<T> *a, Node<T> *b)
{
	std::vector<Edge<T>*> eR;
	std::unordered_set<Edge<T>*> aE;
	for (Edge<T> *tempE : a->GetEdges())
	{
		aE.insert(tempE);
	}
	std::unordered_set<Edge<T>*> bE;
	for (Edge<T> *tempE : b->GetEdges())
	{
		bE.insert(tempE);
	}
	for (auto &aEC : aE)
	{
		//found common edge between the 2
		if (bE.count(aEC))
		{
			eR.push_back(aEC);
		}
	}
	return eR;
}

template<class T>
void CycleDetector<T>::bridgePrinter()
{
	std::cout << "\n~~~~PRINTING BRIDGE NODES~~~~\n";
	for (Node<T> *i : this->bridgeNodes)
	{
		std::cout << "\nBRIDGE NODE: " << i->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING BRIDGE EDGES~~~~\n";
	for (Edge<T> *e : this->bridgeEdges)
	{
		std::cout << "\nBRIDGE EDGE: " << e->GetLabel() + "\n";
	}

	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
void CycleDetector<T>::leafPrinter()
{
	std::cout << "\n~~~~PRINTING LEAF NODES~~~~\n";
	for (Node<T> *i : this->leafNodes)
	{
		std::cout << "\nLEAF NODE: " << i->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
void CycleDetector<T>::megaCyclePrinter()
{
	std::cout << "\n~~~~PRINTING MEGA CYCLES~~~~\n";
	int i = 0;
	for (std::unordered_set<Node<T>*> tU : this->megaCycles)
	{
		std::cout << "Mega Cycle #" + std::to_string(i) + "\n\tNodes: ";
		for (Node<T> *n : tU)
		{
			std::cout << n->GetLabel() + ", ";
		}
		i++;
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
	int i = 0;
	for (std::unordered_set<Node<T>*> l : this->funCycles)
	{
		std::cout << "Fundamental cycle #" << i << " elements: ";
		for (Node<T> *n : l)
		{
			std::cout << n->GetLabel() << ", ";
		}
		i++;
		std::cout << "\n";
	}
	std::cout << "\n";
}

template<class T>
inline void CycleDetector<T>::allCyclePrinter()
{
	int i = 0;
	for (std::unordered_set<Node<T>*> l : this->allCycles)
	{
		std::cout << "Cycle #" << i << " elements: ";
		for (Node<T> *n : l)
		{
			std::cout << n->GetLabel() << ", ";
		}
		i++;
		std::cout << "\n";
	}
	std::cout << "\n";
}

template<class T>
void CycleDetector<T>::populateAdjMatrix()
{
	for (Node<T> *n : this->allNodes)
	{
		std::vector<Node<T>*> tempNeigh = n->GetNeighbors();
		for (Node<T> *cN : tempNeigh)
		{
			if (!(adjMatrix.isConnected(indexLookup[n], indexLookup[cN])))
			{
				this->adjMatrix.connect(indexLookup[n], indexLookup[cN]);
			}
		}
	}
}

template<class T>
inline void CycleDetector<T>::printAdjMatrix(HalfAdjMatrix<T> m)
{
	//just to print because for some reason some adj matricies are unpop
	for (unsigned int i = 0; i < allNodes.size(); i++)
	{
		std::cout
				<< "\n!!!! PRINTING ADJ of Node-> " + nodeLookup[i]->GetLabel()
						+ ", connections: \t";
		for (unsigned int j = 0; j < allNodes.size(); j++)
		{
			if (m.isConnected(j, i))
			{
				std::cout << nodeLookup[j]->GetLabel() + ", ";
			}
		}

	}
	std::cout << "\n";
}

}
#endif // CYCLE_DETECTOR_HPP
