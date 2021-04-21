#ifndef SUBGRAPHMATCHING_HPP
#define SUBGRAPHMATCHING_HPP

#include <map>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GraphStructure/include/Node.hpp"
#include "../../GraphStructure/include/Graph.hpp"
#include "../../GraphStructure/include/HalfAdjacencyMatrix.hpp"

namespace
{
/*	This is used to extract the patterns of our query graph. A pattern is defined by a key and a set of node signifiers.
 * 		This is used to "step through" what our query graph is and see if the steps relate to our actual graph. As of now,
 * 		our key and node signifiers are the names of the nodes. This can be changed but please note it will require us to
 * 		cascade changes throughout our algo.
 */
template<class T>
std::map<std::string, std::vector<std::string>> patternExtractor(
		TemplateGraph::Graph<T> &patternGraph)
{
	std::map<std::string, std::vector<std::string>> foundPatterns;

	for (unsigned int indexA = 0; indexA < patternGraph.getRawNodes().size();
			indexA++)
	{
		//our "A-node"'s name will be our key for patterns
		std::pair<std::string, std::vector<std::string>> currentPattern;
		//we now place current nodes name as key for our current pattern
		currentPattern.first = patternGraph.getNodeFromIndex(indexA)->getName();
		//now we hit all other nodes and if the 2 nodes are connected we throw the
		//	secondary node signifier in our pattern
		for (unsigned int indexB = 0; indexB < patternGraph.getRawNodes().size();
				indexB++)
		{
			//now we must check if the two nodes are connected. We can do this faster.
			//Fix up later
			if (patternGraph.getAdjMatrix().isConnected(indexA, indexB))
			{
				currentPattern.second.push_back(
						patternGraph.getNodeFromIndex(indexB)->getName());
			}
		}
		//once we populate all our patterns to the current key, we throw the pair
		//into our total pattern set, then go to next node.
		foundPatterns.insert(currentPattern);
	}
	return foundPatterns;
}		//end found patterns

//forward declare because dumb
template<class T>
void searchMatches(std::vector<TemplateGraph::Node<T>*> matches,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> &graphSearch);

template<class T>
int searchForPatterns(unsigned int currNodeIndex,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> &graphSearch)
{
	TemplateGraph::Node<T> *currNode = graphSearch.getNodeFromIndex(
			currNodeIndex);

	//we check if we have a corresponding pattern to this node
	if (patterns.count(currNode->getName()) && !visitedKeys.count(currNode))
	{
		visitedKeys.insert(currNode);

		/* When we are searching and we hit a match in our graphSearch (i.e. a parital
		 * 	match onto our query graph) we want to put them in our matched nodes, then
		 * 	we want to check search our patterns for each match.
		 */
		std::vector<TemplateGraph::Node<T>*> foundMatches;

		for (TemplateGraph::Node<T> *interestingNode : graphSearch.getRawNodes())
		{
			//for now we just want to check if we have our current node
			//we are checking out in our results vector.
			bool isInterestingInResults = (std::find(results.begin(),
					results.end(), interestingNode) != results.end());
			//if we dont have current node in our results we want to check it out
			if (!(visitedKeys.count(interestingNode) && !isInterestingInResults))
			{

				//now ensure the two nodes are actually connected
				int interestingNodeIndex = graphSearch.getIndexFromNode(
						interestingNode);

				if (graphSearch.getAdjMatrix().isConnected(currNodeIndex,
						interestingNodeIndex))
				{
					//after confirmed connect we check if our interesting node
					//is within the pattern requirements of our "current" node
					//recall our keys for our pattern matching is the name of the node
					std::vector<std::string> tempPatternReqs =
							patterns[currNode->getName()];

					bool isInterestingInReqs =
							(std::find(tempPatternReqs.begin(),
									tempPatternReqs.end(),
									interestingNode->getName())
									!= tempPatternReqs.end());

					if (isInterestingInReqs)
					{
						/* We now know our interesting node is not in our results,
						 * 		is connected to the current node we are checking out,
						 * 		and that it is a pattern requirement for the current
						 * 		node we are checking out. Thus we put it on our matches
						 * 		that we are going to have to search through in this same
						 * 		manner.
						 *
						 */
						foundMatches.push_back(interestingNode);
					}
				} // end the if for if teh 2 nodes are connected
			}
		} //end our for loop

		unsigned int currNodeReqsLength = patterns[currNode->getName()].size();
		/* now we want to make sure that we have AT LEAST the same amount of matches
		 * 	as we do for the current node requirements. Keep in mind we need to hit
		 * 	all of each nodes pattern requirements to continue with a valid/matching
		 * 	subgraph. Also we need to ensure our matches size is larger than 0 because
		 * 	if we have 0 matches and 0 req length we are at a leaf
		 */
		if ((foundMatches.size() >= currNodeReqsLength)
				&& (foundMatches.size() > 0))
		{
			//we now prune our matches in order to make the rest of our resursive journey
			//easier and faster. Once we match a pattern who cares about it anymore for this current run
			patterns.erase(currNode->getName());
			//add the current node we are checking out to our results since we are good so far
			results.push_back(currNode);

			searchMatches(foundMatches, patterns, results, visitedKeys,
					graphSearch);

			//return 2 to designate we are continuing our traversal
			return 2;
		}
		else if ((foundMatches.size() == 0) && (currNodeReqsLength == 0))
		{
			//we have hit a leaf, update our patterns
			patterns.erase(currNode->getName());
			results.push_back(currNode);

			//return 1 for our leaf case
			return 1;
		}
	}
	//return 0 since we dont hit a match
	return 0;
}			//end search patterns

/* This is used to search all the matches we previously found. Works by calling our
 * 	search patterns function for each matched node, then we find THOSE matches
 * 	then hit this function again.
 */
template<class T>
void searchMatches(std::vector<TemplateGraph::Node<T>*> matches,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> &graphSearch)
{
	for (TemplateGraph::Node<T> *currMatch : matches)
	{
		//as before we need to check if our node we are checking out is
		//already in our results i.e. already been run through
		bool isMatchInResults = (std::find(results.begin(), results.end(),
				currMatch) != results.end());
		/* We want to make sure we havent used the current node as an "entry point"
		 * 		for our search pattern function AND we want to make sure she doesnt
		 * 		exist in our results yet.
		 */
		if (!isMatchInResults && !(visitedKeys.count(currMatch)))
		{
			int searchState = searchForPatterns(
					graphSearch.getIndexFromNode(currMatch), patterns, results,
					visitedKeys, graphSearch);

			//if we hit a leaf we get out
			if (searchState == 1)
				break;
		}
	}
} //end search matches

}
//end anon namespace

namespace subgraphMatcher
{
template<class T>
std::unordered_map<TemplateGraph::Node<T>*, std::vector<TemplateGraph::Node<T>*>> findSubgraphs(
		TemplateGraph::Graph<T> &mainGraph, TemplateGraph::Graph<T> &queryGraph)
{
//first we want to grab all our patterns that we will use to match on our main graph
	std::map<std::string, std::vector<std::string>> patternsToMatch =
			patternExtractor(queryGraph);

	std::vector<TemplateGraph::Node<T>*> currResults;

	std::unordered_set<TemplateGraph::Node<T>*> keyVisitTracker;

	std::unordered_map<TemplateGraph::Node<T>*,
			std::vector<TemplateGraph::Node<T>*>> finalResults;

	//now we run a search for each node
	for (unsigned int searchStartNodeIndex = 0;
			searchStartNodeIndex < mainGraph.getRawNodes().size();
			searchStartNodeIndex++)
	{
		searchForPatterns(searchStartNodeIndex, patternsToMatch, currResults,
				keyVisitTracker, mainGraph);

		if (currResults.size() != 0)
		{
			finalResults.insert(
			{ mainGraph.getNodeFromIndex(searchStartNodeIndex), currResults });
			//after we recursively hit all patterns we want to go ahead and clear out
			//our results so we hit all again
			currResults.clear();
		}
		keyVisitTracker.clear();
	}
	return finalResults;
}

} //end subgraph matcher namespace

#endif //SUBGRAPHMATCHING_HPP
