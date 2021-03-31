#ifndef CYCLE_DETECTOR_HPP
#define CYCLE_DETECTOR_HPP

#include "../../Graph.hpp"
#include "../../Node.hpp"
#include "../../Edge.hpp"

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

//this will be used to help run our algos on. Tried many different ways to rep this and even
//convert algo to not using a matrix but proved too difficult. Worry regarding space concerns later.
template<class T>
class HalfAdjMatrix
{
public:
	HalfAdjMatrix();
	//HalfAdjMatrix();
	HalfAdjMatrix(std::vector<Node<T>*> nList);

	HalfAdjMatrix(int sizeNodes);

	//copy construct
	HalfAdjMatrix(const HalfAdjMatrix<T> &m);

	//really this should be considered a "helper" veriable, i.e. we want it to just
	//be a representation of our nodes themselves
	std::vector<Node<T>*> getNodes();

	//og took nodes in but I want to try ints due to ease. Possibly
	bool isConnected(int a, int b);
	void connect(int a, int b);
	void disconnect(int a, int b);
	unsigned int getNumEdges() const;

	void fixUp(std::vector<Node<T>*> nList);
	void sizeAndClear(int i);

	//operator overloads

	inline HalfAdjMatrix<T> operator^(const HalfAdjMatrix<T> &rhs) const
	{
		HalfAdjMatrix<T> result(numNodes);
		if (this->numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL\n~~~~~~~~~~~~~\n\n";
			return result;
		}
		else
		{
			for (int i = 0; i < bitList.size(); i++)
			{
				if ((bitList[i] || rhs.bitList[i])
						&& (bitList[i] != rhs.bitList[i]))
				{
					result.bitList[i] = 1;
					++result.numEdges;
				}
			}
			return result;
		}
	}

	inline HalfAdjMatrix<T>& operator=(const HalfAdjMatrix<T> &rhs)
	{
		if (numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL\n~~~~~~~~~~~~~\n\n";
		}
		this->bitList = rhs.bitList;
		this->numEdges = rhs.numEdges;
		//this->indexindexLookup[n] = rhs.indexLookup;
		return *this;
	}

	inline HalfAdjMatrix<T>& operator^=(const HalfAdjMatrix<T> &rhs)
	{
		if (this->numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL\n~~~~~~~~~~~~~\n\n";

		}
		numEdges = 0;
		for (int i = 0; i < bitList.size(); i++)
		{
			if ((bitList[i] || rhs.bitList[i])
					&& (bitList[i] != rhs.bitList[i]))
			{
				bitList[i] = 1;
				numEdges++;
			}
			else
				bitList[i] = 0;
		}
		return *this;
	}

private:
	//proper index Lookup b/c dealing with half of a adj matrix
	int index(const int i, const int j);

	//actual connections, think of this as our 1d adj matrix and not this helper class
	std::vector<bool> bitList;
	//possibly convert for 64/32 bit system independence
	unsigned int numEdges;
	//same as below, Lookup table for handing our indices
	//std::unordered_map<Node<T>*, int> indexLookup;
	unsigned int numNodes;
	long long indexFactor;
};

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix()
{
	this->numEdges = 0;
	this->numNodes = 0;
	this->indexFactor = 0;
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(std::vector<Node<T>*> nList)
{
	int listSize = nList.size();
	this->bitList.assign(((listSize * (listSize - 1)) / 2), 0);
	this->numNodes = listSize;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (listSize - 2));
}

template<class T>
void HalfAdjMatrix<T>::fixUp(std::vector<Node<T>*> nList)
{
	int listSize = nList.size();
	this->bitList.assign(((listSize * (listSize - 1)) / 2), 0);
	this->numNodes = listSize;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (listSize - 2));

}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(int sizeNodes)
{
	this->bitList.assign(((sizeNodes * (sizeNodes - 1)) / 2), 0);
	this->numNodes = sizeNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (sizeNodes - 2));
}

template<class T>
void HalfAdjMatrix<T>::sizeAndClear(int i)
{
	this->numNodes = i;
	this->numEdges = 0;
	this->bitList.assign(((i * (i - 1)) / 2), 0);
	this->indexFactor = (1 + 2 * (i - 2));
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(const HalfAdjMatrix<T> &m)
{
	this->bitList = m.bitList;
	this->numEdges = m.numEdges;
	this->numNodes = m.numNodes;
	this->indexFactor = m.indexFactor;
	//this->indexLookup = m.indexLookup;
}

//not needed
template<class T>
inline std::vector<Node<T>*> HalfAdjMatrix<T>::getNodes()
{
	std::vector<Node<T>*> tempN;
	//for (auto const &mapItem : this->indexLookup)
	//{
	//	tempN.push_back(mapItem.fist);
	//}
	return tempN;
}

template<class T>
bool HalfAdjMatrix<T>::isConnected(int nodeA, int nodeB)
{
	if (nodeA == nodeB)
		return false;
	return this->bitList[index(nodeA, nodeB)];
}

template<class T>
void HalfAdjMatrix<T>::connect(int nodeA, int nodeB)
{
	if (bitList[index(nodeA, nodeB)])
	{
		std::cout << "Trying to add a connection that was already there\n";
	}
	else
	{
		bitList[index(nodeA, nodeB)] = true;
		numEdges++;
	}
}

template<class T>
void HalfAdjMatrix<T>::disconnect(int nodeA, int nodeB)
{

	if (!bitList[index(nodeA, nodeB)])
	{
		std::cout << "Trying to remove a connection that was not there\n";
	}
	else
	{
		bitList[index(nodeA, nodeB)] = false;
		numEdges--;
	}
}

template<class T>
inline unsigned int HalfAdjMatrix<T>::getNumEdges() const
{
	return this->numEdges;
}

template<class T>
int HalfAdjMatrix<T>::index(const int i, const int j)
{
	if (i < numNodes && j < numNodes && i != j)
	{
		long long li = i, lj = j;
		if (i < j)
			return (lj - li * (li - indexFactor) / 2) - 1;
		else
			return (li - lj * (lj - indexFactor) / 2) - 1;
	}
	else
	{
		std::cout << "ERROR due to index picks\n";
		return -1;
	}
}

template<class T>
class CycleDetector
{
public:
	CycleDetector();
	CycleDetector(std::string name, Graph<T> *graph);

	//should be able to edge and node at same time p easy
	void leafDetect();
	void bridgeDetect();

	void detectMegaCycles();

	void xorCycleDetect();

private:

	bool validateCycleMatrix(HalfAdjMatrix<T> &m);
	void validateCycleMatrixRec(HalfAdjMatrix<T> &m, int &pathLength,
			const int i, int previousNode, std::set<int> &aVisited);

	//helpers for our xor
	void computeFunCycles();

	//a simple way for us to not have to worry about correct node indexing now.
	std::unordered_map<Node<T>*, int> indexLookup;
	std::unordered_map<int, Node<T>*> nodeLookup;
	void populateIndexLookup();

	void bridgePrinter();
	void leafPrinter();
	//megas are beads
	void megaCyclePrinter();

	//now we want to do the whole half adj matrix. This is used because it makes our actual algo
	//that will be run to find cycles much easier. For now the algo will be implemented
	//on the whole graph. Eventually we will want to create adj matrix for each of our
	//mega cycles and then run it on them. Should be a pretty plug and play switch
	//very very unsure of how to do this....

	//helpers to make life easier, can put somewhere else later
	int neighborCount(Node<T> *n);
	//get all edges between 2 nodes, helps due to multigraph
	std::vector<Edge<T>*> connectingEdges(Node<T> *a, Node<T> *b);

	void dfsBridgeHelper(Node<T> *nNode, Node<T> *pNode);

	std::vector<Node<T>*> dfsGetReachable(int compNum, Node<T> *s);

	//just so we can refer to the graph we are running algos on.
	//This will eventually be  changed
	Graph<T> *graph;
	Node<T> *rootNode;
	//to prevent dupes

	std::string name;

	//for our bridge finding algo, we can figure out where/how we want to structure this later
	int counter;
	//lowest time for certain node to be visited
	std::vector<int> low;
	//know when node is 1st visited
	std::vector<int> disc;
	//keep track of visited
	//std::vector<bool> visited;
	//keep track of what visiited
	std::vector<int> intVisited;
	//keep track of parent
	std::vector<int> pre;

	/* PLEASE NOTE THIS IS EXTREMELY NON-MEM SAFE PRACTICES.
	 * ONCE FINAL STRUCTURE IS AGREED UPON THIS WILL DELTA.
	 * ONLY DONE TO NOT HAVE TO CHANGE OTHER CODE AND TO PROVE HOW ALGOS WORK.
	 */

	//all of our cycles
	std::vector<std::unordered_set<Node<T>*>> allCycles;
	std::vector<HalfAdjMatrix<T>> allCyclesAdj;
	//just our fundemental so we know what they are and dont dirty them
	std::vector<std::unordered_set<Node<T>*>> funCycles;
	std::vector<HalfAdjMatrix<T>> funCyclesAdj;

	//this is what will be decomposed into all of our cycles
	std::vector<std::unordered_set<Node<T>*>> megaCycles;

	//most likely only need bridge edges
	std::unordered_set<Edge<T>*> bridgeEdges;
	//Just going to use unordered set of edge * for now to keep track of our bridges and leafs
	std::unordered_set<Node<T>*> leafNodes;
	//most likely only need bridge edges
	std::unordered_set<Node<T>*> bridgeNodes;

	std::vector<Node<T>*> allNodes;

	void populateAdjMatrix();

	HalfAdjMatrix<T> adjMatrix;
	void printAdjMatrix(HalfAdjMatrix<T> m);

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
	this->counter = 0;
	this->graph = NULL;
	this->rootNode = NULL;
}

template<class T>
CycleDetector<T>::CycleDetector(std::string name, Graph<T> *graph)
{
	this->name = name;
	this->graph = graph;
	this->counter = 0;
	std::vector<Node<T>*> tem = this->allNodes;

	//this->adjMatrix = HalfAdjMatrix(tem);
	populateIndexLookup();
	populateAdjMatrix();
	this->xorCycleDetect();

	//find our bridges
	//
	//find our leafs
	this->leafDetect();

	this->bridgeDetect();
	this->bridgePrinter();
	this->detectMegaCycles();
	int i = 0;
	/*
	for (HalfAdjMatrix<T> uS : this->allCyclesAdj)
	{
		std::cout << "all cycle #" << i << " elements: ";
		printAdjMatrix(uS);
		std::cout << "\n";
		i++;
	}
	*/

	for (std::unordered_set<Node<T>*> l : this->allCycles)
	{
		std::cout << "all cycle #" << i << " elements: ";
		for (Node<T>* n : l)
		{
			std::cout << n->GetLabel() <<", ";
		}
		i++;
		std::cout << "\n";
	}

	this->megaCyclePrinter();
	//now go through and find all our mega cycles
	//this->detectMegaCycles();
}

//also detects SOME of our edge bridges

//warning always adds our first added node!
template<class T>
void CycleDetector<T>::leafDetect()
{
	std::cout << "\n\n~~~~RUNNING LEAF DETECT~~~~\n\n";
	std::vector<Node<T>*> n = this->graph->GetNodes();
	for (Node<T> *cn : n)
	{
		//lazy check in case a leaf has multiple edges to the same node
		std::unordered_set<Node<T>*> tempCheck;
		std::vector<Node<T>*> tempN = cn->GetNeighbors();
		std::copy(tempN.begin(), tempN.end(),
				std::inserter(tempCheck, tempCheck.end()));
		if (tempN.size() == 1)
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
	this->leafPrinter();
}

//need to make sure this works with multiple edges, i.e.
//2 edges connected to a node that is acting like a bridge should both be labeled as a bridge with the node
template<class T>
void CycleDetector<T>::bridgeDetect()
{

	std::vector<Node<T>*> allNodes = this->graph->GetNodes();
	int numN = allNodes.size();
	this->counter = 0;
	this->low.clear();
	this->low.resize(numN);
	this->pre.clear();
	this->pre.resize(numN);
	for (int i = 0; i < numN; i++)
	{
		low[i] = -1;
		pre[i] = -1;
	}

	for (Node<T> *n : allNodes)
	{
		if (pre[indexLookup[n]] == -1)
			dfsBridgeHelper(n, n);
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

//nnode = next, p node = parent
template<class T>
void CycleDetector<T>::dfsBridgeHelper(Node<T> *u, Node<T> *v)
{
	int vIndex = indexLookup[v];
	int uIndex = indexLookup[u];
	pre[vIndex] = counter++;
	low[vIndex] = pre[vIndex];
	//run dfs on adj aka neighbors
	for (Node<T> *w : v->GetNeighbors())
	{
		int wIndex = indexLookup[w];
		if (pre[wIndex] == -1)
		{
			dfsBridgeHelper(v, w);
			low[vIndex] = std::min(low[vIndex], low[wIndex]);
			if (low[wIndex] == pre[wIndex])
			{
				//now get all edges between the two and keep track of them
				for (Edge<T> *e : this->connectingEdges(w, v))
				{
					if (this->bridgeEdges.count(e))
					{
						std::cout
								<< "Warning: Edge " + e->GetLabel()
										+ "  already present";
					}
					else
					{
						this->bridgeEdges.insert(e);
					}
				}
			}
		}
		else if (wIndex != uIndex)
			low[vIndex] = std::min(low[vIndex], pre[wIndex]);
	}
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

template<class T>
void CycleDetector<T>::detectMegaCycles()
{
	//we want to first get our list of all reachable nodes from each specific node
	//then we want to get rid of all of our duplicates
	std::vector<Node<T>*> allNodes = this->graph->GetNodes();
	intVisited.clear();
	intVisited.resize(allNodes.size());
	for (unsigned int i = 0; i < allNodes.size(); i++)
	{
		intVisited[i] = 0;
	}
	//stores all reachable from node calling at
	std::unordered_map<int, std::vector<Node<T>*>> reachableNodes;
	//tracking what graph "component" node is part of.
	int componentNum = 0;
	for (Node<T> *n : allNodes)
	{
		int nIndex = indexLookup[n];
		//bridge nodes prevents us from traversing to any nodes that are a bridge.
		//this allows for beading. Important in full cycle detect. Same with leaf.
		bool badNode = (this->bridgeNodes.count(n))
				|| (this->leafNodes.count(n));
		if ((intVisited[nIndex] == 0) && (!badNode))
		{
			componentNum++;
			reachableNodes[intVisited[nIndex]] = this->dfsGetReachable(
					componentNum, n);
		}
	}
	for (auto nL : reachableNodes)
	{
		std::unordered_set<Node<T>*> tempN;
		for (Node<T> *n : nL.second)
		{
			//std::cout
			//		<< "Comp number: " + std::to_string(nL.first) + " ~~Node: "
			//				+ n->GetLabel() +"\n";
			tempN.insert(n);
		}
		this->megaCycles.push_back(tempN);
	}
}

template<class T>
void CycleDetector<T>::xorCycleDetect()
{
	std::cout << "\n\nSTARTING XOR\n\n";
	if (this->funCycles.size() == 0)
		this->computeFunCycles();
	allCycles = funCycles;
	for (HalfAdjMatrix<T> hA : funCyclesAdj)
	{
		this->allCyclesAdj.push_back(hA);
	}

	std::cout
			<< "\n\n\n\n@@@@@@@@@@@@@@@@@@@@@@@@\n"
					+ std::to_string(funCyclesAdj.size());
	std::vector<bool> combiV(funCycles.size());
	for (int l = 2; l <= funCycles.size(); l++)
	{
		std::fill_n(combiV.begin(), l, 1);
		std::fill_n(combiV.rbegin(), combiV.size() - l, 0);

		do
		{
			HalfAdjMatrix<T> M(allNodes);
			int edCount = 0;

			for (int i = 0; i < funCyclesAdj.size(); i++)
			{
				//std::cout << "\n\n\n\n@@@@@@@@@@@@@@@@@@@@@@@@\n";
				//this->printAdjMatrix(funCyclesAdj[i]);
				if (combiV[i])
				{
					//std::cout << "\n\n\n\n@@@@@@@@@@@@@@@@@@@@@@@@\n";
					//this->printAdjMatrix(funCyclesAdj[i]);
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

				//printAdjMatrix(M);
				if (validateCycleMatrix(M))
				{
					if (edCount > M.getNumEdges())
					{
						this->allCyclesAdj.push_back(M);
					}
				}
			}

		} while (std::prev_permutation(combiV.begin(), combiV.end()));

	}

	for (HalfAdjMatrix<T> hA : this->allCyclesAdj)
	{
		std::unordered_set<Node<T>*> cycSet;
		for (int i = 0; i < allNodes.size(); i++)
		{`
			for (int j = 0; j < allNodes.size(); j++)
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
			std::cout<<"\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBAD CYCLESET\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		}
		else
			this->allCycles.push_back(cycSet);
	}
}

template<class T>
void CycleDetector<T>::computeFunCycles()
{

	if (!funCycles.empty())
		return;

	std::unique_ptr<TreeNode[]> aTree(new TreeNode[allNodes.size()]);
	std::stack<size_t> nodeStack;

	// Start arbitrarily with the first Node!
	nodeStack.push(0);

	// Copy the adjacency matrix as it will be necessary to remove edges!
	HalfAdjMatrix<T> adjMat = adjMatrix;

	// At the beginning, all tree nodes point to itself as parent!
	// The tree is built on the fly
	for (size_t i = 0; i < allNodes.size(); ++i)
	{
		aTree[i].parent = &aTree[i];
		aTree[i].index = i;
	}

	// Loop until all nodes are removed from the stack!
	while (nodeStack.size() > 0)
	{
		// Next node index:
		size_t currentNodeIndex = nodeStack.top();
		nodeStack.pop();
		TreeNode &currentTreeNode = aTree[currentNodeIndex];

		// Iterate though all edges connecting this node:
		for (size_t j = 0; j < allNodes.size(); ++j)
		{
			// not connected?
			if (!adjMat.isConnected(currentNodeIndex, j))
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
				for (int i = 0; i < allNodes.size(); i++)
				{
					for (int j = 0; j < allNodes.size(); j++)
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
					//printAdjMatrix(allCyclesAdj.back());
				}
				//printAdjMatrix(pi ^ pj);
				// combine the two matrices with XOR (^) to obtain the fundamental cycle.
				//something wrong with xor
				//	std::cout << "++++printing path pi\n";
				//	printAdjMatrix(pi);
				//	std::cout << "++++printing path\n";
				//	printAdjMatrix(pj);
			}
			else
			{
				// The foreign node is not contained in the tree yet; add it now!
				aTree[j].parent = &currentTreeNode;
				// add the node to the search stack!
				nodeStack.push(j);
			}
			// Either way remove this connection!
			adjMat.disconnect(currentNodeIndex, j);
		}
	}
}

template<class T>
std::vector<Node<T>*> CycleDetector<T>::dfsGetReachable(int compNum, Node<T> *s)
{
	//queue for our dfs to keep track
	std::queue<Node<T>*> nQ;
	nQ.push(s);
	intVisited[indexLookup[s]] = compNum;
	std::vector<Node<T>*> reachableNodes;
	//hopefully will work after adding in extra constraints
	while (!nQ.empty())
	{
		Node<T> *u = nQ.front();
		nQ.pop();
		//we only want to add the reachable node if it is NOT a leaf or a bridge.
		reachableNodes.push_back(u);

		//same as hitting our adjacency list, keep in mind this does not prevent dupes
		for (Node<T> *nN : u->GetNeighbors())
		{
			//NOTE THAT THIS IS WHERE WE WANT TO ALSO WORRY ABOUT OUR UNWANTED CASES

			//Check for making sure we are not traversing over a bridge. Take trip
			//is used to make sure that the connecting edges between two nodes is not a bridge.
			bool takeTrip = true;
			for (Edge<T> *eD : this->connectingEdges(u, nN))
			{
				//should not have to do this for all, being verbose
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

//groups all edges connecting nodes together. This allows our algos to
template<class T>
std::vector<Edge<T>*> CycleDetector<T>::connectingEdges(Node<T> *a, Node<T> *b)
{

	std::vector<Edge<T>*> eR;
	std::vector<Edge<T>*> aeTemp = a->GetEdges();
	std::vector<Edge<T>*> beTemp = b->GetEdges();
	std::unordered_set<Edge<T>*> aE;

	for (Edge<T> *tempE : aeTemp)
	{
		aE.insert(tempE);
	}
	std::unordered_set<Edge<T>*> bE;
	for (Edge<T> *tempE : beTemp)
	{
		bE.insert(tempE);
	}

	for (auto &aEC : aE)
	{
		//found common edge between the 2
		if (bE.count(aEC))
			eR.push_back(aEC);
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
void CycleDetector<T>::populateAdjMatrix()
{
	this->adjMatrix.fixUp(allNodes);
	for (Node<T> *n : this->allNodes)
	{
		std::cout
				<< "\nNode: " + n->GetLabel() + " index="
						+ std::to_string(indexLookup[n]) + " neigh: ";
		std::vector<Node<T>*> tempNeigh = n->GetNeighbors();
		for (Node<T> *cN : tempNeigh)
		{
			std::cout << cN->GetLabel() + ", ";
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
	for (int i = 0; i < allNodes.size(); i++)
	{

		std::cout
				<< "\n!!!!!!!!PRINTING ADJ - Node: " + nodeLookup[i]->GetLabel()
						+ "\t~~connections: ";

		for (int j = 0; j < allNodes.size(); j++)
		{
			if (m.isConnected(j, i))
			{
				std::cout << nodeLookup[j]->GetLabel() + ",";
			}
		}

	}
	std::cout << "\n";
}

template<class T>
bool CycleDetector<T>::validateCycleMatrix(HalfAdjMatrix<T> &m)
{

	int pathL = 0;
	for (int i = 0; i < allNodes.size(); i++)
	{
		for (int j = 0; j < allNodes.size(); j++)
		{
			if (m.isConnected(j, i))
			{
				++pathL;
				std::set<int> isVisited;
				isVisited.insert(i);
				validateCycleMatrixRec(m, pathL, j, i, isVisited);

				return (pathL + 1) == m.getNumEdges();
			}
		}
	}
	std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: NO EDGES\n\n";
}

template<class T>
void CycleDetector<T>::validateCycleMatrixRec(HalfAdjMatrix<T> &m, int &pathL,
		const int i, int prevNode, std::set<int> &aVisited)
{
	if (pathL > 750)
	{
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: TOO LONG PATH\n\n";
	}
	else
	{
		for (int j = 0; j < allNodes.size(); j++)
		{
			if (m.isConnected(i, j) && j != prevNode)
			{
				auto ppVisited = aVisited.find(j);
				if (ppVisited != aVisited.end())
				{
					return;
				}
				++pathL;
				aVisited.insert(i);
				validateCycleMatrixRec(m, pathL, j, i, aVisited);
				return;
			}
		}
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: DEAD END\n\n";
	}
}
}
#endif // CYCLE_DETECTOR_HPP
