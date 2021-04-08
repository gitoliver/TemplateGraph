#ifndef SUBGRAPH_MATCHER_HPP
#define SUBGRAPH_MATCHER_HPP

#include "../../Graph.hpp"
#include "../../Node.hpp"
#include "../../Edge.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <map>
#include <set>
#include <list>

namespace TemplateGraph
{
template<class T>
class SubGraphMatcher
{
public:
	SubGraphMatcher();
	SubGraphMatcher(Graph<T> *mainGraph, Graph<T> *queryGraph);

	std::vector<std::unordered_set<Node<T>*>> findSubgraphs();

private:

	//all the other search functions are hit by what this calls
	void searchEntry(Graph<T> *subgraphToMatch,
			std::unordered_set<Node<T>*> &resultHolder,
			std::vector<std::vector<int>> mainAdjMatrix,
			std::vector<std::vector<int> > queryAdjMatrix);

	/* 0 = NONE
	 * 1 = LEAF
	 * 2 = TRAVERSE
	 */
	//maybe just return an unordered set for our current result how do we put it back into our total result
	int searchForPatterns(Graph<T> *subgraphToMatch, int counter,
			std::pair<std::string, std::set<std::string>> currPattern,
			std::map<std::string, std::set<std::string>> &patterns,
			std::unordered_set<Node<T>*> &resultHolder,
			std::vector<std::vector<int>> mainAdjMatrix);
	//	std::map<int, std::unordered_set<Node<T>*>> &resultHolder

	void searchMatches(Graph<T> *subgraphToMatch,
			std::vector<Node<T>*> nodeVector,
			std::map<std::string, std::set<std::string>> &patterns,
			std::unordered_set<Node<T>*> &resultHolder,
			std::vector<std::vector<int>> mainAdjMatrix, int counter);

	//will eventuall use some good comparator, gets patterns from the query graph
	std::map<std::string, std::set<std::string>> extractPaterns(
			std::vector<std::vector<int>> adjMat, bool isQuery);

	//this is our graph we are searching in
	Graph<T> *mainGraph;
	//this is our subgraph
	Graph<T> *queryGraph;

	/*std::vector<Node<T>*> subgraphPairDFSWalk(int subgraphNumber,
	 std::vector<int> &intMainHasVisited,
	 std::vector<int> &intQueryHasVisited, Node<T> *interestingMainNode,
	 Node<T> *interestingQueryNode);
	 */
	//lookup table stuff
	std::unordered_map<Node<T>*, int> mainIndexLookup;
	std::unordered_map<int, Node<T>*> mainNodeLookup;

	//lookup table stuff for query graph
	std::unordered_map<Node<T>*, int> queryIndexLookup;
	std::unordered_map<int, Node<T>*> queryNodeLookup;

	//because why not, make life easier for now to prove logic.
	std::vector<Node<T>*> mainAllNodes;
	std::vector<Node<T>*> queryAllNodes;

	//also populates our lists of all nodes.
	void populateLookups();

	void lookUpPrinter();

};

//idea, we traverse through the subgraph. Each step we even think about making we
//make sure that we can make the step in main graph.

/*	Build spanning tree on tiny graph, and keep track of the steps we took to get there
 * 		aka i.e. use a q. Then we run the same algo but trying to force the steps we took
 * 		to make our spanning tree. If we can build the exact same tree then we know we
 * 		at least have a POSSIBLE candidate.
 *
 * 		How do we verify? Run iterative DFS and use it to check against our subgraph.
 * 		Basically same as above but forced? Why not just get rid of above....
 *
 *
 *		************************************************************
 * 		Ngl why dont we just find a starting node on our input graph, try to go through our
 * 		input graph & try to force the same steps we take in our input to our main graph. If
 * 		we are unable to find a valid step in the big graph then we try again on another starting
 * 		node.
 * 		************************************************************
 *
 */

/* Current issue is we can be SUPER lazy. Since our nodes & strings all have
 * 	same name, why don't we just check and find all our edges & nodes and see if
 * 	they are in the graph itself. This is bad......
 *
 * 	p much regexing the labels themselves
 *
 */

template<class T>
SubGraphMatcher<T>::SubGraphMatcher()
{
	std::cout << "\n\nno\n\n";
	this->mainGraph = NULL;
	this->queryGraph = NULL;
}

template<class T>
SubGraphMatcher<T>::SubGraphMatcher(Graph<T> *mainGraph, Graph<T> *queryGraph)
{
	this->mainGraph = mainGraph;
	this->queryGraph = queryGraph;
	this->populateLookups();
	//this->lookUpPrinter();
	this->findSubgraphs();
}

template<class T>
std::vector<std::unordered_set<Node<T>*> > SubGraphMatcher<T>::findSubgraphs()
{
	std::vector<std::unordered_set<Node<T>*> > returnVec;
	/*first going to try using adj matricies to match. May need to go ahead and use
	 * Cordellas algo instead.... Need to test.....
	 *
	 * Worried that using half adj matrix here will cause issues. Need to understand algo better
	 */
	//please note our last row & column will be used for number of connections a node has
	std::vector<std::vector<int>> mainGraphAdj(this->mainAllNodes.size() + 1,
			std::vector<int>(this->mainAllNodes.size() + 1, 0));

	for (Node<T> *currentNode : this->mainAllNodes)
	{

		std::vector<Node<T>*> tempNeighbors = currentNode->GetNeighbors();
		std::unordered_set<Node<T>*> currentNodeNeighbors(tempNeighbors.begin(),
				tempNeighbors.end());
		for (Node<T> *currentNeighbor : currentNodeNeighbors)
		{
			mainGraphAdj[this->mainIndexLookup[currentNode]][this->mainIndexLookup[currentNeighbor]] =
					1;
			//recall we made this by using nodes size + 1 so using the og size works fine index at 1 lol
			//puts in our number of connections
			++mainGraphAdj[this->mainIndexLookup[currentNode]][this->mainAllNodes.size()];
			++mainGraphAdj[this->mainAllNodes.size()][this->mainIndexLookup[currentNeighbor]];
		}
	}

	std::vector<std::vector<int>> queryGraphAdj(this->queryAllNodes.size() + 1,
			std::vector<int>(this->queryAllNodes.size() + 1, 0));
	for (Node<T> *currentNode : this->queryAllNodes)
	{

		std::vector<Node<T>*> tempNeighbors = currentNode->GetNeighbors();
		std::unordered_set<Node<T>*> currentNodeNeighbors(tempNeighbors.begin(),
				tempNeighbors.end());
		for (Node<T> *currentNeighbor : currentNodeNeighbors)
		{
			queryGraphAdj[this->queryIndexLookup[currentNode]][this->queryIndexLookup[currentNeighbor]] =
					1;
			//recall we made this by using nodes size + 1 so using the og size works fine index at 1 lol
			//puts in our number of connections
			++queryGraphAdj[this->queryIndexLookup[currentNode]][this->queryAllNodes.size()];
			++queryGraphAdj[this->queryAllNodes.size()][this->queryIndexLookup[currentNeighbor]];
		}
	}
	std::unordered_set<Node<T>*> resultsLol;
	this->searchEntry(this->queryGraph, resultsLol, mainGraphAdj,
			queryGraphAdj);

	std::cout << "\n\n@@@@@@@@\n" + std::to_string(resultsLol.size()) + "\n";
	for (Node<T> *currNode : resultsLol)
	{
		std::cout << currNode->GetLabel() + ", ";
	}
	std::cout << "\n@@@@@@@@\n\n";
	//have built our adj mats
	/*
	 std::cout << "\n\n\n~~~OK~~~~\n\n\n";
	 for (int i = 0; i < queryGraphAdj.size(); i++)
	 {
	 //checking specific dimension sizes doesnt matter
	 for (int j = 0; j < queryGraphAdj[0].size(); j++)
	 {

	 std::cout << std::to_string(queryGraphAdj[i][j]) + " ";
	 }
	 std::cout << "\n";
	 }
	 */
	return returnVec;
}

template<class T>
void SubGraphMatcher<T>::searchEntry(Graph<T> *subgraphToMatch,
		std::unordered_set<Node<T>*> &resultHolder,
		std::vector<std::vector<int> > mainAdjMatrix,
		std::vector<std::vector<int> > queryAdjMatrix)
{
	std::map<Node<T>*, std::unordered_set<Node<T>*>> allResults;
	resultHolder.clear();
	std::map<std::string, std::set<std::string> > queryPatterns =
			this->extractPaterns(queryAdjMatrix, true);
	int i = 0;

	std::cout << "\n\n@@@@@@@@\n";
	for (auto const &currPair : queryPatterns)
	{
		std::cout << "Key value: " + currPair.first + "\n";
		for (std::string currString : currPair.second)
		{
			std::cout << currString + ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n@@@@@@@@\n\n";

	for (Node<T> *currNode : this->mainAllNodes)
	{
		//since our patterns are mutated when we run the algo, assuming we want to 
		//keep a clean bit of our patterns so when we run subgraph match on each specific node
		//it hits all
		//std::map<std::string, std::set<std::string> > queryPatterns = ogQueryPatterns;
		//dont think we need to worry about our query adj matrix. Unsure...

		this->searchForPatterns(subgraphToMatch, i, *queryPatterns.begin(),
				queryPatterns, resultHolder, mainAdjMatrix);
		i++;

		/*std::cout << "\n\n@@@@@@@@\n";
		 for (Node<T> *lolNode : resultHolder)
		 {
		 std::cout << lolNode->GetLabel() + ", ";
		 }
		 std::cout << "\n@@@@@@@@\n\n";*/

		//allResults.insert(std::pair<Node<T>*, std::unordered_set<Node<T>*>>(currNode, resultHolder));
		//resultHolder.clear();
	}
}

/* 0 = NONE
 * 1 = LEAF
 * 2 = TRAVERSE
 */
// Need to pass in specific patterns?
template<class T>
int SubGraphMatcher<T>::searchForPatterns(Graph<T> *subgraphToMatch,
		int counter, std::pair<std::string, std::set<std::string>> currPattern,
		std::map<std::string, std::set<std::string> > &patterns,
		std::unordered_set<Node<T>*> &resultHolder,
		std::vector<std::vector<int> > mainAdjMatrix)
{
	Node<T> *currNode = this->mainNodeLookup[counter];

	/* we want to eventually better design this. Possibly have a unique id for
	 * each node that will be used as a key and then we have a member value that
	 * denotes the element in our requirements?
	 */
	if (currPattern.first == currNode->GetLabel())
	{

		std::vector<Node<T>*> matchNodes;

		for (Node<T> *interestingNode : this->mainAllNodes)
		{
			if (!(resultHolder.count(interestingNode)))
			{

				if (mainAdjMatrix[this->mainIndexLookup[currNode]][this->mainIndexLookup[interestingNode]])
				{

					std::set<std::string> patternReqs =
							patterns[currNode->GetLabel()];
					if (currPattern.second.count(interestingNode->GetLabel()))
					{

						//					if (std::find(matchNodes.begin(), matchNodes.end(),
						//						interestingNode) != matchNodes.end())
						//			{
						//			std::cout
						//				<< "\n\n@@@@ FOUND INTERESTING NODE NAMED: <"
						//					+ interestingNode->GetLabel()
						//					+ "> IN OUR MATCHING NODE LIST AGAIN @@@@\n\n";
						//}
						matchNodes.push_back(interestingNode);
					}
				}
			}
		}

		if ((matchNodes.size() >= currPattern.second.size())
				&& (matchNodes.size() > 0))
		{
			// note that we end up destroying our patterns. Must be able to find all.
			// Keep track of what nodes not visited, then run algo begining on one of those nodes
			// keep going until we run algo on all nodes.

			//same as nulling out our key, change key to null
			std::set<std::string> tempPat = patterns[currPattern.first];
			patterns.erase(currNode->GetLabel());
			//patterns[currPattern.first] = "NULLED";

		/*	if (resultHolder.count(currNode))
			{
				std::cout
						<< "\n\n@@@@ FOUND CURRENT NODE NAMED: <"
								+ currNode->GetLabel()
								+ "> IN OUR RESULT NODE LIST AGAIN @@@@\n\n";
			}
			else
			{
				std::cout
						<< "\n\n@@@@ ADDING CURRENT NODE NAMED: <"
								+ currNode->GetLabel()
								+ "> TO OUR RESULT NODE LIST@@@@\n\n";*/
				resultHolder.insert(currNode);
		//	}
			this->searchMatches(subgraphToMatch, matchNodes, patterns,
					resultHolder, mainAdjMatrix, counter);
			// NOTE 2 = TRAVERSE
			return 2;
		}
		else if (matchNodes.size() == 0
				&& currPattern.second.size() == 0)
		{
			//same idea as nulling key
			currPattern.first = "";
			resultHolder.insert(currNode);

			//NOTE 1 = LEAF
			return 1;
		}

	}

	// done to satisfy our base case, we dont call any other funcs and just leave
	//NOTE 0 = NONE
	return 0;
}

template<class T>
void SubGraphMatcher<T>::searchMatches(Graph<T> *subgraphToMatch,
		std::vector<Node<T>*> nodeVector,
		std::map<std::string, std::set<std::string> > &patterns,
		std::unordered_set<Node<T>*> &resultHolder,
		std::vector<std::vector<int> > mainAdjMatrix, int counter)
{
	//note that we can do some logic shortcutting. For now just trying to implement algo
	// same way done in paper then if it works I will alter.
	for (Node<T> *currMatchNode : nodeVector)
	{
		for (std::pair<std::string, std::set<std::string>> currentPattern : patterns)
		{
			if (!(resultHolder.count(currMatchNode)))
			{
				if (currentPattern.first == currMatchNode->GetLabel())
				{
					//now do we need to pass in our specific found pattern? I dont think so due to current structure
					int searchState = this->searchForPatterns(subgraphToMatch,
							this->mainIndexLookup[currMatchNode],
							currentPattern, patterns, resultHolder,
							mainAdjMatrix);
					//if we are at a leaf now.
					if (searchState == 1)
					{
						break;
					}
				}
			}
		}
	}
}

template<class T>
std::map<std::string, std::set<std::string> > SubGraphMatcher<T>::extractPaterns(
		std::vector<std::vector<int>> adjMat, bool isQuery)
{
	std::map<std::string, std::set<std::string>> patternRet;
	if (isQuery)
	{
		for (int i = 0; i < this->queryAllNodes.size(); i++)
		{
			std::pair<std::string, std::set<std::string>> currPatternCreation;
			currPatternCreation.first = this->queryNodeLookup[i]->GetLabel();
			//keep in mind we are currently using label as our key
			//patternRet.insert(
			//{ this->queryNodeLookup[i]->GetLabel(), std::set<std::string>() });

			for (int j = 0; j < this->queryAllNodes.size(); j++)
			{
				if (adjMat[i][j] == 1)
				{
					currPatternCreation.second.insert(this->queryNodeLookup[j]->GetLabel());
				}
			}
			patternRet.insert(currPatternCreation);
		}
	}
	/*else
	 {
	 for (int i = 0; i < this->mainAllNodes.size(); i++)
	 {
	 //add our key
	 patternRet[this->mainNodeLookup[i]->GetLabel()];
	 for (int j = 0; j < this->mainAllNodes.size(); j++)
	 {
	 if (adjMat[i][j] == 1)
	 {

	 if (patternRet.find(this->mainNodeLookup[i]->GetLabel())
	 != patternRet.end())
	 {
	 std::set<std::string> tempSet =
	 patternRet[this->mainNodeLookup[i]->GetLabel()];
	 tempSet.insert(this->mainNodeLookup[j]->GetLabel());
	 patternRet[this->mainNodeLookup[i]->GetLabel()] =
	 tempSet;
	 }
	 else
	 {
	 patternRet.insert(this->mainNodeLookup[i]->GetLabel(),
	 this->mainNodeLookup[j]->GetLabel());
	 }
	 }
	 }
	 }
	 }*/

	std::cout << "\n\n@@@@\npatterns for graph\n@@@@\n";
	for (std::pair<std::string, std::set<std::string>> const &currPair : patternRet)
	{
		std::cout << "Key value: " + currPair.first + "\n";
		for (std::string currString : currPair.second)
		{
			std::cout << currString + ", ";
		}
		std::cout << "\n";
	}

	return patternRet;
}

//may need to switch to iterative dfs to make easier on brain
//template<class T>
//std::vector<std::unordered_set<Node<T>*> > SubGraphMatcher<T>::findSubgraphs()
//{
/* need our hasvisited for both our main and our query graph
 * Keep in mind we want to use our query graph as a "leader".
 * aka look ahead, see the step we want to take, check if current
 * node on main graph can take that step. If we can take that step, take it.
 *
 * Important cases:
 * 		- when we hit a cycle, we need to ensure that the cycle (back edge)
 * 			is also present in our main graph before we say we good and travel
 * 			back up our recursion tree
 */

//note our has visited contains what subgraph num
/*std::vector<int> queryHasVisited(this->queryAllNodes.size(), 0);

 //what our subgraphs will be stored in
 std::unordered_map<int, std::vector<Node<T>*>> matchingSubgraphs;

 int subgraphNum = 0;

 //recall, we want our query to lead our "steps"
 for (Node<T> *currentQueryNode : this->queryAllNodes)
 {
 std::vector<int> mainHasVisited(this->mainAllNodes.size(), 0);
 //now we want to get our matching node in our all nodes list
 Node<T> *matchingMainNode = NULL;
 for (Node<T> *possibleMatch : this->mainAllNodes)
 {
 //we have found a match. we are going to want to hold multiple of our matches.
 //	will eventually throw in set? Then we loop below.
 if (possibleMatch->GetLabel() == currentQueryNode->GetLabel())
 {
 matchingMainNode = possibleMatch;
 }
 }

 // I guess we are good enough to try and take a step
 bool stepImpossible = (matchingMainNode == NULL) ? true : false;
 if (!(stepImpossible)
 && (queryHasVisited[this->queryIndexLookup[currentQueryNode]]
 == 0)
 && (mainHasVisited[this->mainIndexLookup[matchingMainNode]] == 0))
 {
 matchingSubgraphs[queryHasVisited[this->queryIndexLookup[currentQueryNode]]] =
 this->subgraphPairDFSWalk(subgraphNum, mainHasVisited,
 queryHasVisited, matchingMainNode,
 currentQueryNode);
 }

 }

 }

 template<class T>
 std::vector<Node<T>*> SubGraphMatcher<T>::subgraphPairDFSWalk(
 int subgraphNumber, std::vector<int> &intMainHasVisited,
 std::vector<int> &intQueryHasVisited, Node<T> *interestingMainNode,
 Node<T> *interestingQueryNode)
 {
 //queue for dfs to keep track of our query and main graphs
 std::queue<Node<T>*> queryNodeQ;
 std::queue<Node<T>*> mainNodeQ;

 queryNodeQ.push(interestingQueryNode);
 mainNodeQ.push(interestingMainNode);

 std::vector<Node<T>* > subgraphNodes;
 while (!queryNodeQ.empty())
 {




 }




 }*/

template<class T>
void SubGraphMatcher<T>::populateLookups()
{
	//since populating these we want to first clear them out.
	this->queryIndexLookup.clear();
	this->queryNodeLookup.clear();

	this->mainIndexLookup.clear();
	this->mainNodeLookup.clear();

	//to handle all our query lookups, prevent dupes
	std::unordered_set<Node<T>*> tempNodes;
	for (Node<T> *currentQueryNode : this->queryGraph->GetNodes())
	{
		if (tempNodes.count(currentQueryNode))
		{
			std::cout << "Warning: dupe detected\n";
		}
		else
		{
			tempNodes.insert(currentQueryNode);
		}
	}
	unsigned int currIndexCount = 0;
	for (Node<T> *currNode : tempNodes)
	{
		this->queryAllNodes.push_back(currNode);
		this->queryIndexLookup.insert(
				std::pair<Node<T>*, int>(currNode, currIndexCount));
		this->queryNodeLookup.insert(
				std::pair<int, Node<T>*>(currIndexCount, currNode));
		currIndexCount++;
	}
	//done inserting our query lookups
	//now finish out main lookups
	tempNodes.clear();
	for (Node<T> *currentMainNode : this->mainGraph->GetNodes())
	{
		if (tempNodes.count(currentMainNode))
		{
			std::cout << "Warning: dupe detected main node\n";
		}
		else
		{
			tempNodes.insert(currentMainNode);
		}
	}
	currIndexCount = 0;
	for (Node<T> *currNode : tempNodes)
	{

		this->mainAllNodes.push_back(currNode);
		this->mainIndexLookup.insert(
				std::pair<Node<T>*, int>(currNode, currIndexCount));
		this->mainNodeLookup.insert(
				std::pair<int, Node<T>*>(currIndexCount, currNode));
		currIndexCount++;
	}
	//done inserting all into our lookups
}

template<class T>
void SubGraphMatcher<T>::lookUpPrinter()
{
	for (int i = 0; i < this->mainAllNodes.size(); i++)
	{
		std::cout
				<< "Index: " + std::to_string(i) + " relates to Node "
						+ this->mainNodeLookup[i]->GetLabel() + "\n";
	}

}

} // TemplateGraph namespace

#endif // SUBGRAPH_MATCHER_HPP
