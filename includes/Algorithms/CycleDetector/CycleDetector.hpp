#ifndef CYCLE_DETECTOR_HPP
#define CYCLE_DETECTOR_HPP

#include "../../Graph.hpp"
#include "../../Node.hpp"
#include "../../Edge.hpp"

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>

/*just pass in a root node to this, store the cycles within to show how algos work.
 * Eventually we must not use a root node due to the fact that if we delete a bridge
 * can disconnect a part of the graph we may want to access. With current mem structure
 * it would be impossible to run algo on the two parts of the previously connected graph.
 *
 * NOTE: Once the algos are proven we will convert this class into a namespace that gets called. For sake of lines
 * not caring about getters or setters
 */
namespace TemplateGraph {

template<class T>
class CycleDetector {
public:
	CycleDetector();
	CycleDetector(std::string name, Graph<T> *graph);

	//should be able to edge and node at same time p easy
	void leafDetect();
	void bridgeDetect();

	void detectMegaCycles();

private:

	//a simple way for us to not have to worry about correct node indexing now.
	std::unordered_map<Node<T>*, int> indexLookUp;
	void populateIndexLookUp();

	void bridgePrinter();
	void leafPrinter();

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
	//just our fundemental so we know what they are and dont dirty them
	std::vector<std::unordered_set<Node<T>*>> funCycles;
	//this is what will be decomposed into all of our cycles
	std::vector<std::unordered_set<Node<T>*>> megaCycles;

	//most likely only need bridge edges
	std::unordered_set<Edge<T>*> bridgeEdges;
	//Just going to use unordered set of edge * for now to keep track of our bridges and leafs
	std::unordered_set<Node<T>*> leafNodes;
	//most likely only need bridge edges
	std::unordered_set<Node<T>*> bridgeNodes;

};

template<class T>
CycleDetector<T>::CycleDetector() {
	std::cout << "no" << std::endl;
	this->counter = 0;
	this->graph = NULL;
	this->rootNode = NULL;
}

template<class T>
CycleDetector<T>::CycleDetector(std::string name, Graph<T> *graph) {
	this->name = name;
	this->graph = graph;
	//std::cout<<"\n\nCycle Detector Graph Ptr: " << this->graph << "\n\n";
	//std::cout<<"\n\nCycle Detector Graph ROOT Ptr: " << this->graph->PoobGetRoot() << "\n\n";
	this->counter = 0;
	populateIndexLookUp();
	//find our bridges
	//
	//find our leafs
	this->leafDetect();

	this->bridgeDetect();
	this->bridgePrinter();
	this->detectMegaCycles();
	//now go through and find all our mega cycles
	//this->detectMegaCycles();
}

//also detects SOME of our edge bridges
template<class T>
void CycleDetector<T>::leafDetect() {
	std::cout << "\n\n~~~~RUNNING BRIDGE DETECT~~~~\n\n";
	std::vector<Node<T>*> n = this->graph->GetNodes();
	for (Node<T> *cn : n) {
		//lazy check in case a leaf has multiple edges to the same node
		std::unordered_set<Node<T>*> tempCheck;
		std::vector<Node<T>*> tempN = cn->GetNeighbors();
		std::copy(tempN.begin(), tempN.end(),
				std::inserter(tempCheck, tempCheck.end()));
		if (tempN.size() == 1) {
			//found leaf, know all our connecting edges are bridges to our leaf also
			if (this->leafNodes.count(cn)) {
				//already in set
				std::cout << "warning node already in set :" << cn->GetLabel()
						<< std::endl;
			} else {
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
void CycleDetector<T>::bridgeDetect() {

	std::vector<Node<T>*> allNodes = this->graph->GetNodes();
	int numN = allNodes.size();
	this->counter = 0;
	this->low.clear();
	this->low.resize(numN);
	this->pre.clear();
	this->pre.resize(numN);
	for (int i = 0; i < numN; i++) {
		low[i] = -1;
		pre[i] = -1;
	}

	for (Node<T> *n : allNodes) {
		if (pre[indexLookUp[n]] == -1)
			dfsBridgeHelper(n, n);
	}
	//now we know if all edges connected to a node that is not a leaf node it must be a bridge node
	for (Node<T> *n : allNodes) {
		bool toInsert = false;
		for (Edge<T> *e : n->GetEdges()) {
			if (this->bridgeEdges.count(e)) {
				toInsert = true;
			} else {
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
void CycleDetector<T>::dfsBridgeHelper(Node<T> *u, Node<T> *v) {
	int vIndex = indexLookUp[v];
	int uIndex = indexLookUp[u];
	pre[vIndex] = counter++;
	low[vIndex] = pre[vIndex];
	//run dfs on adj aka neighbors
	for (Node<T> *w : v->GetNeighbors()) {
		int wIndex = indexLookUp[w];
		if (pre[wIndex] == -1) {
			dfsBridgeHelper(v, w);
			low[vIndex] = std::min(low[vIndex], low[wIndex]);
			if (low[wIndex] == pre[wIndex]) {
				//now get all edges between the two and keep track of them
				for (Edge<T> *e : this->connectingEdges(w, v)) {
					if (this->bridgeEdges.count(e)) {
						std::cout
								<< "Warning: Edge " + e->GetLabel()
										+ "  already present";
					} else {
						this->bridgeEdges.insert(e);
					}
				}
			}
		} else if (wIndex != uIndex)
			low[vIndex] = std::min(low[vIndex], pre[wIndex]);
	}
}

template<class T>
void CycleDetector<T>::populateIndexLookUp() {
	//using an unordered map to enforce no dupes
	this->indexLookUp.clear();
	int i = 0;
	std::unordered_set<Node<T>*> tN;
	for (Node<T> *n : this->graph->GetNodes()) {
		if (tN.count(n)) {
			std::cout << "Warning: dupe detected\n";
		} else
			tN.insert(n);
	}
	for (Node<T> *n : tN) {
		this->indexLookUp.insert(std::pair<Node<T>*, int>(n, i));
		i++;
	}
}

template<class T>
void CycleDetector<T>::detectMegaCycles() {
	std::cout << "\nTO NEXT NODE\n";

	//we want to first get our list of all reachable nodes from each specific node
	//then we want to get rid of all of our duplicates
	std::vector<Node<T>*> allNodes = this->graph->GetNodes();
	intVisited.clear();
	intVisited.resize(allNodes.size());
	for (int i = 0; i < allNodes.size(); i++) {
		intVisited[i] = 0;
	}
	//stores all reachable from node calling at
	std::unordered_map<int, std::vector<Node<T>*>> reachableNodes;

	//tracking what graph "component" node is part of. Eventually use to circumvent dirty logic
	int componentNum = 0;

	for (Node<T> *n : allNodes) {
		int nIndex = indexLookUp[n];

		if (intVisited[nIndex] == 0) {
			std::cout << "\nTO NEXT NODE\n";
			componentNum++;
			reachableNodes[intVisited[nIndex]] = this->dfsGetReachable(
					componentNum, n);
		}
	}
	for (auto nL : reachableNodes)
	{
		for (Node<T>* n : nL.second)
		{
			std::cout << "~~Node: " + n->GetLabel();
		}
		std::cout << "\nTO NEXT NODE\n";
	}
}

template<class T>
std::vector<Node<T>*> TemplateGraph::CycleDetector<T>::dfsGetReachable(
		int compNum, Node<T> *s) {
	//queue for our dfs to keep track
	std::queue<Node<T>*> nQ;
	nQ.push(s);
	intVisited[indexLookUp[s]] = compNum;
	std::vector<Node<T>*> reachableNodes;
	//hopefully will work after adding in extra constraints
	while (!nQ.empty()) {
		Node<T> *u = nQ.front();
		nQ.pop();
		reachableNodes.push_back(u);
		//same as hitting our adjacency list, keep in mind this does not prevent dupes
		for (Node<T> *nN : u->GetNeighbors()) {
			if (!intVisited[indexLookUp[nN]]) {
				intVisited[indexLookUp[nN]] = compNum;
				nQ.push(nN);
			}
		}
	}
	return reachableNodes;
}

//groups all edges connecting nodes together. This allows our algos to
template<class T>
std::vector<Edge<T>*> CycleDetector<T>::connectingEdges(Node<T> *a,
		Node<T> *b) {

	std::vector<Edge<T>*> eR;

	std::vector<Edge<T>*> aeTemp = a->GetEdges();
	std::vector<Edge<T>*> beTemp = b->GetEdges();

	std::unordered_set<Edge<T>*> aE;

	for (Edge<T> *tempE : aeTemp) {
		aE.insert(tempE);
	}
	std::unordered_set<Edge<T>*> bE;
	for (Edge<T> *tempE : beTemp) {
		bE.insert(tempE);
	}

	for (auto &aEC : aE) {
		//found common edge between the 2
		if (bE.count(aEC))
			eR.push_back(aEC);
	}
	return eR;
}

template<class T>
void CycleDetector<T>::bridgePrinter() {
	std::cout << "\n~~~~PRINTING BRIDGE NODES~~~~\n";
	for (Node<T> *i : this->bridgeNodes) {
		std::cout << "\nBRIDGE NODE: " << i->GetLabel() + "\n";
	}
	for (Edge<T> *e : this->bridgeEdges) {
		std::cout << "\nBRIDGE EDGE: " << e->GetLabel() + "\n";
	}

	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
void CycleDetector<T>::leafPrinter() {
	std::cout << "\n~~~~PRINTING LEAF NODES~~~~\n";
	for (Node<T> *i : this->leafNodes) {
		std::cout << "\nLEAF NODE: " << i->GetLabel() + "\n";
	}
	std::cout << "\n~~~~PRINTING COMPLETE~~~~\n";
}

template<class T>
int CycleDetector<T>::neighborCount(Node<T> *n) {
	std::unordered_set<Node<T>*> t;
	std::vector<Node<T>*> tempN = n->GetNeighbors();
	std::copy(tempN.begin(), tempN.end(), std::inserter(t, t.end()));
	return t.size();
}
}

#endif // CYCLE_DETECTOR_HPP
