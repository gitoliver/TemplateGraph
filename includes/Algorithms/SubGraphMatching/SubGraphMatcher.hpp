#ifndef SUBGRAPH_MATCHER_HPP
#define SUBGRAPH_MATCHER_HPP

#include "../../Graph.hpp"
#include "../../Node.hpp"
#include "../../Edge.hpp"
#include "../../Structure/HalfAdjMatrix.hpp"

#include <unordered_set>
#include <unordered_map>
#include <map>

namespace TemplateGraph
{
template<class T>
class SubgraphMatcher
{
public:
	SubgraphMatcher();
	SubgraphMatcher(Graph<T> *mainGraph, Graph<T> *queryGraph);

private:
	//our graphs themselves
	Graph<T> *mainGraph;
	Graph<T> *queryGraph;

	//all of our main graph relationships
	std::unordered_map<Node<T>*, int> mainIndexLookup;
	std::unordered_map<int, Node<T>*> mainNodeLookup;
	std::vector<Node<T>*> mainAllNodes;
	HalfAdjMatrix<T> mainAdjMatrix;

	//all of our query graph relationships
	std::unordered_map<Node<T>*, int> queryIndexLookup;
	std::unordered_map<int, Node<T>*> queryNodeLookup;
	std::vector<Node<T>*> queryAllNodes;
	HalfAdjMatrix<T> queryAdjMatrix;

	//as of now we just popualte all our lookups at the same time. Eventually worry about seping this out
	void populateLookups();

	//our entry for our search function
	void searchEntry(std::unordered_map<Node<T>*, std::vector<Node<T>*>> &searchResults);

	/* To prevent having to do a custom enum or something because lazy just keeping track with ints
	 *
	 * 0 = NONE
	 * 1 = LEAF
	 * 2 = TRAVERSE
	 */
	int searchForPatterns(int counter,
			std::map<std::string, std::vector<std::string>> patterns,
			std::vector<Node<T>*> &results,
			std::unordered_set<Node<T>*> &visitedKeys);

	//search matches
	void searchMatches(std::vector<Node<T>*> matches,
			std::map<std::string, std::vector<std::string>> patterns,
			std::vector<Node<T>*> &results,
			std::unordered_set<Node<T>*> &visitedKeys);

	//to find our pattern set, note that we will have to change return types depending on what we want for key and our value signifier
	std::map<std::string, std::vector<std::string>> patternExtraction(
			bool forQuery);

	//some printing stuff to help us out
	void lookupPrinter();
	void printAdjMatrix();

};

template<class T>
SubgraphMatcher<T>::SubgraphMatcher()
{
	this->mainGraph = NULL;
	this->queryGraph = NULL;
	std::cout << "\n\nSTOP NO\n\n";
}

template<class T>
SubgraphMatcher<T>::SubgraphMatcher(Graph<T> *mainGraph, Graph<T> *queryGraph)
{
	std::cout
			<< "\n@@@@@@@@@@@@\nENTERING SUBGRAPH MATCHER CONSTRUCTOR\n@@@@@@@@@@@@\n";
	this->mainGraph = mainGraph;
	this->queryGraph = queryGraph;
	this->populateLookups();
	this->printAdjMatrix();
	this->lookupPrinter();

	std::unordered_map<Node<T>*, std::vector<Node<T>*>> resultsLol;
	this->searchEntry(resultsLol);

	std::cout
				<< "\n@@@@@@@@@@@@\nPLEASE NOTE THAT IF WE OBSERVE OUR MEMORY OR SOMETHING THAT WILL ACTUALLY SHOW OUR UNIQUE NODE WE CAN SEE ALL ARE DIFFERENT.\n@@@@@@@@@@@@\n";
	for (std::pair<Node<T>*, std::vector<Node<T>*>> currPair : resultsLol)
	{
		std::cout << "Subgraphs (Node Collections) that match query with Node <" + currPair.first->GetLabel() + "> \n";
		for (Node<T>* currNode : currPair.second)
		{
			std::cout << "<" + currNode->GetLabel() + "> ";
		}
		std::cout << "\n";
	}
}

//this will eventually be used within the final graph class, dupe code of whats in cyc detect
template<class T>
void SubgraphMatcher<T>::populateLookups()
{
	//clear all query info
	this->queryIndexLookup.clear();
	this->queryNodeLookup.clear();
	this->queryAdjMatrix.initializeWorkaround(this->queryAdjMatrix);
	this->queryAllNodes.clear();

	//something to hold all our nodes in & prevent dupes
	std::unordered_set<Node<T>*> tempNodes;
	//something to keep track for filling in our maps
	int indexCounter = 0;

	for (Node<T> *currNode : this->queryGraph->GetNodes())
	{
		if (tempNodes.count(currNode))
		{
			std::cout << "Warning: dupe detected\n";
		}
		else
		{
			tempNodes.insert(currNode);
		}
	}

	for (Node<T> *currNode : tempNodes)
	{
		this->queryAllNodes.push_back(currNode);

		this->queryIndexLookup.insert(
		{ currNode, indexCounter });

		this->queryNodeLookup.insert(
		{ indexCounter, currNode });

		indexCounter++;
	}

	//after using both tempnodes and counter reset them
	tempNodes.clear();
	indexCounter = 0;

	//now handle our adj matrix stuff for query
	this->queryAdjMatrix.initializeWorkaround(this->queryAllNodes);

	for (Node<T> *currNode : this->queryAllNodes)
	{
		for (Node<T> *currNeighbor : currNode->GetNeighbors())
		{
			if (!(this->queryAdjMatrix.isConnected(
					this->queryIndexLookup[currNode],
					this->queryIndexLookup[currNeighbor])))
			{
				this->queryAdjMatrix.connect(this->queryIndexLookup[currNode],
						this->queryIndexLookup[currNeighbor]);
			}
		}
	}

	//do same as above but for main info

	//clear all main info
	this->mainIndexLookup.clear();
	this->mainNodeLookup.clear();
	this->mainAdjMatrix.initializeWorkaround(this->mainAllNodes);
	this->mainAllNodes.clear();

	for (Node<T> *currNode : this->mainGraph->GetNodes())
	{
		if (tempNodes.count(currNode))
		{
			std::cout << "Warning: dupe detected\n";
		}
		else
		{
			tempNodes.insert(currNode);
		}
	}

	for (Node<T> *currNode : tempNodes)
	{
		this->mainAllNodes.push_back(currNode);
		this->mainIndexLookup.insert(
		{ currNode, indexCounter });

		this->mainNodeLookup.insert(
		{ indexCounter, currNode });

		indexCounter++;
	}

	//now handle our adj matrix stuff for main
	this->mainAdjMatrix.initializeWorkaround(this->mainAllNodes);
	for (Node<T> *currNode : this->mainAllNodes)
	{
		for (Node<T> *currNeighbor : currNode->GetNeighbors())
		{
			if (!(this->mainAdjMatrix.isConnected(
					this->mainIndexLookup[currNode],
					this->mainIndexLookup[currNeighbor])))
			{
				this->mainAdjMatrix.connect(this->mainIndexLookup[currNode],
						this->mainIndexLookup[currNeighbor]);
			}
		}
	}

}

//TODO: Need to figure out a way to sep out multiple subgraphs that are kinda close to being same
template<class T>
void SubgraphMatcher<T>::searchEntry(std::unordered_map<Node<T>*, std::vector<Node<T>*>> &results)
{
	results.clear();
	std::map<std::string, std::vector<std::string> > ogPatterns =
				this->patternExtraction(1);

	std::map<std::string, std::vector<std::string> > subgraphPatterns =
			this->patternExtraction(1);
	std::unordered_set<Node<T>*> keyVisitTracker;
	//std::unordered_map<Node<T>*, std::vector<Node<T>*>> trueResults;
	std::vector<Node<T>*> currentNodeResults;

	for (unsigned int i = 0; i < this->mainAllNodes.size(); i++)
	{
		//probably need to pass results by ref but right now dont care
		this->searchForPatterns(i, subgraphPatterns, currentNodeResults, keyVisitTracker);

		if (currentNodeResults.size() != 0)
		{
			/*std::cout
					<< "\n\n11111111111111111111111\n RESULTS FOR I VALUE OF: <"
							+ std::to_string(i) + "> aka Node:"
							+ this->mainNodeLookup[i]->GetLabel() + "\n";
			for (Node<T>* cN : currentNodeResults)
			{
				std::cout << cN->GetLabel() + ", ";
			}
			std::cout << "\n11111111111111111111111\n\n";*/
			results.insert({this->mainNodeLookup[i], currentNodeResults});
			currentNodeResults.clear();
		}
		keyVisitTracker.clear();

	}
}

template<class T>
int SubgraphMatcher<T>::searchForPatterns(int counter,
		std::map<std::string, std::vector<std::string> > patterns,
		std::vector<Node<T>*> &results,
		std::unordered_set<Node<T>*> &visitedKeys)
{
	Node<T> *nodeA = this->mainNodeLookup[counter];
	//our key matches our current node, aka there is a pattern which means its in querygraph also
	if (patterns.count(nodeA->GetLabel()) && !visitedKeys.count(nodeA))
	{
		visitedKeys.insert(nodeA);
		std::vector<Node<T>*> foundMatches;
		for (Node<T> *interestingNode : this->mainAllNodes)
		{
			bool isInterestingInResults =
					(std::find(results.begin(), results.end(),
							interestingNode) != results.end());
			//we do NOT currently have our interesting node in our results and have not used it as a
			//entry point to check our algos yet so we want to check it and see if we are in the needed pattern
			//if (!isInterestingInResults )
			//if (!(visitedKeys.count(interestingNode)) &&  !isInterestingInResults)
			if (!(visitedKeys.count(interestingNode) && !isInterestingInResults))
			{
				int aNodeIndex = this->mainIndexLookup[nodeA];
				int interestingNodeIndex =
						this->mainIndexLookup[interestingNode];

				if (this->mainAdjMatrix.isConnected(aNodeIndex,
						interestingNodeIndex))
				{
					//now lets check that our interesting node is a pattern requiremnt for nodeA seperating out code because want to make verbose
					std::vector<std::string> tempPatternReqs =
							patterns[nodeA->GetLabel()];
					bool isInterestingInReqs = (std::find(
							tempPatternReqs.begin(), tempPatternReqs.end(),
							interestingNode->GetLabel())
							!= tempPatternReqs.end());
					if (isInterestingInReqs)
					{
						foundMatches.push_back(interestingNode);
					}
				}
			}
		} //end for loop

		//to make algo readability easier
		int aNodeReqLength = patterns[nodeA->GetLabel()].size();

		if ((foundMatches.size() >= aNodeReqLength)
				&& (foundMatches.size() > 0))
		{
			patterns.erase(nodeA->GetLabel());
			results.push_back(nodeA);
			this->searchMatches(foundMatches, patterns, results, visitedKeys);
			return 2;
		}
		else if (foundMatches.size() == 0 && aNodeReqLength == 0)
		{
			patterns.erase(nodeA->GetLabel());
			results.push_back(nodeA);
			//return leaf
			return 1;
		}

	}
	//return none, done to break recursion
	return 0;
}

template<class T>
void SubgraphMatcher<T>::searchMatches(std::vector<Node<T>*> matches,
		std::map<std::string, std::vector<std::string> > patterns,
		std::vector<Node<T>*> &results,
		std::unordered_set<Node<T>*> &visitedKeys)
{
	for (Node<T> *currMatch : matches)
	{
		bool isMatchInResults = (std::find(results.begin(), results.end(),
				currMatch) != results.end());
		//if (!(isMatchInResults))
		if (!(visitedKeys.count(currMatch)) && !isMatchInResults)
		{
			int searchState = this->searchForPatterns(
					this->mainIndexLookup[currMatch], patterns, results,
					visitedKeys);
			//if leaf
			if (searchState == 1)
			{
				break;
			}

		}
	}
}

template<class T>
std::map<std::string, std::vector<std::string> > SubgraphMatcher<T>::patternExtraction(
		bool forQuery)
{
	std::map<std::string, std::vector<std::string> > foundPatterns;
	if (forQuery)
	{
		for (unsigned int i = 0; i < this->queryAllNodes.size(); i++)
		{
			std::pair<std::string, std::vector<std::string>> growingPattern;
			//set our key as the label of node <---> i
			growingPattern.first = this->queryNodeLookup[i]->GetLabel();

			for (unsigned int j = 0; j < this->queryAllNodes.size(); j++)
			{
				if (this->queryAdjMatrix.isConnected(j, i))
				{
					//add in our requirement, as of now this is our nodes label, we could eventually do a element signifier or something that we check
					growingPattern.second.push_back(
							this->queryNodeLookup[j]->GetLabel());
				}
			}
			foundPatterns.insert(growingPattern);
		}
	}
	else
	{
		std::cout << "\n@@@@@@@@@@@@\nHey now we dont need to find patterns for our main graph but in our alpha version of finalized structure this will not matter.\n\tThis whole function will work for any graph we pass in\n@@@@@@@@@@@@\n";
	}
	return foundPatterns;
}

template<class T>
void SubgraphMatcher<T>::lookupPrinter()
{
	std::cout << "\n@@@@@@@@@@@@\nPrinting lookups of query\n@@@@@@@@@@@@\n";
	for (Node<T> *currNode : this->queryAllNodes)
	{
		int indexTemp = this->queryIndexLookup[currNode];
		std::cout
				<< "Using node <" + currNode->GetLabel()
						+ "> as key, int value <" + std::to_string(indexTemp)
						+ ">\n";
		std::cout
				<< "Using index <" + std::to_string(indexTemp)
						+ "> as key, node value <"
						+ this->queryNodeLookup[indexTemp]->GetLabel() + ">\n";
	}
	std::cout
			<< "@@@@@@@@@@@@\nQuery lookups printing complete\n@@@@@@@@@@@@\n";
	std::cout << "\n@@@@@@@@@@@@\nPrinting lookups of main\n@@@@@@@@@@@@\n";
	for (Node<T> *currNode : this->mainAllNodes)
	{
		int indexTemp = this->mainIndexLookup[currNode];
		std::cout
				<< "Using node <" + currNode->GetLabel()
						+ "> as key, int value <" + std::to_string(indexTemp)
						+ ">\n";
		std::cout
				<< "Using index <" + std::to_string(indexTemp)
						+ "> as key, node value <"
						+ this->mainNodeLookup[indexTemp]->GetLabel() + ">\n";
	}
	std::cout
			<< "@@@@@@@@@@@@\nMain lookups printing complete\n@@@@@@@@@@@@\n\n";

}

template<class T>
void SubgraphMatcher<T>::printAdjMatrix()
{
	std::cout << "\n\n@@@@@@@@@@@@\nPrinting Adj list of query\n@@@@@@@@@@@@";
	for (unsigned int currentNodeIndex = 0;
			currentNodeIndex < queryAllNodes.size(); currentNodeIndex++)
	{
		std::cout
				<< "\n!!!! PRINTING ADJ of Node-> "
						+ queryNodeLookup[currentNodeIndex]->GetLabel()
						+ ", connections: \t";
		for (unsigned int otherNodeIndex = 0;
				otherNodeIndex < queryAllNodes.size(); otherNodeIndex++)
		{
			if (this->queryAdjMatrix.isConnected(otherNodeIndex,
					currentNodeIndex))
			{
				std::cout << queryNodeLookup[otherNodeIndex]->GetLabel() + ", ";
			}
		}

	}
	std::cout
			<< "\n@@@@@@@@@@@@\nQuery adj list printing complete\n@@@@@@@@@@@@\n";
	std::cout << "\n@@@@@@@@@@@@\nPrinting Adj list of main\n@@@@@@@@@@@@";
	for (unsigned int currentNodeIndex = 0;
			currentNodeIndex < mainAllNodes.size(); currentNodeIndex++)
	{
		std::cout
				<< "\n!!!! PRINTING ADJ of Node-> "
						+ mainNodeLookup[currentNodeIndex]->GetLabel()
						+ ", connections: \t";
		for (unsigned int otherNodeIndex = 0;
				otherNodeIndex < mainAllNodes.size(); otherNodeIndex++)
		{
			if (this->mainAdjMatrix.isConnected(otherNodeIndex,
					currentNodeIndex))
			{
				std::cout << mainNodeLookup[otherNodeIndex]->GetLabel() + ", ";
			}
		}

	}
	std::cout
			<< "\n@@@@@@@@@@@@\nMain adj list printing complete\n@@@@@@@@@@@@\n\n";
}

} // TemplateGraph namespace

#endif // SUBGRAPH_MATCHER_HPP
