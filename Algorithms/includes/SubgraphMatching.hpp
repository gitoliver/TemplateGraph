#ifndef SUBGRAPHMATCHING_HPP
#define SUBGRAPHMATCHING_HPP

#include <map>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../../LazyPrints/LazyPrinters.hpp"
#include "../../GraphStructure/include/Graph.hpp"
#include "../../GraphStructure/include/HalfAdjacencyMatrix.hpp"


namespace
{

/* As of now our patterns are created by matching the "name" of the
 * 	observed node. We may want to consider this in order to allow for
 * 	both efficient and proper matching along all our cases
 */
template<class T>
std::map<std::string, std::vector<std::string>> patternExtractor(
		TemplateGraph::Graph<T> &graphToHunt)
{
	std::map<std::string, std::vector<std::string>> foundPatterns;
	for (unsigned int indexA = 0; indexA < graphToHunt.getNodes().size();
			indexA++)
	{
		std::pair<std::string, std::vector<std::string>> currPattern;
		currPattern.first = graphToHunt.getNodeFromIndex(indexA)->getName();
		for (unsigned int indexB; indexB < graphToHunt.getNodes().size();
				indexB++)
		{
			if (graphToHunt.getAdjMatrix().isConnected(indexA, indexB))
			{
				currPattern.second.push_back(
						graphToHunt.getNodeFromIndex(indexB)->getName());
			}
		}
		foundPatterns.insert(currPattern);
	}
	return foundPatterns;
} //end pattern extraction

//forward declare cause computers dumb
template<class T>
void searchMatches(std::vector<TemplateGraph::Node<T>*> matches,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> const &graphSearch);


/* As with said above regarding how our pattern extraction uses the name member
 * 		we would have to alter our search function to reflect any changes with
 * 		our patterns.
 */
template<class T>
int searchForPatterns(unsigned int givenIndex,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> &graphSearch)
{
	TemplateGraph::Node<T> *givenNode = graphSearch.getNodeFromIndex(
			givenIndex);

	if (patterns.count(givenNode->getLabel())
			&& !(visitedKeys.count(givenNode)))
	{
		visitedKeys.insert(givenNode);

		std::vector<TemplateGraph::Node<T>*> foundMatches;

		for (TemplateGraph::Node<T> *interestingNode : graphSearch.getNodes())
		{
			bool interestingInResults = (std::find(results.begin(),
					results.end(), interestingNode) != results.end());

			/*	We want to check if our current node is in the results and ALSO
			 * 		want to check if we have even visited this node yet. The reason is
			 * 		that we must continue pattern matching only if we have hit through
			 * 		this node and its children, childrens children, etc. and check their
			 * 		patterns
			 */
			if (!(visitedKeys.count(interestingNode)) && !interestingInResults)
			{

				unsigned int interestingIndex = graphSearch.getIndexFromNode(
						interestingNode);

				if (graphSearch.getAdjMatrix().isConnected(givenIndex,
						interestingIndex))
				{
					/* now peep if we care about interesting node is in
					 * 		our pattern required from our given node
					 */
					std::vector<std::string> tempPatternReqs =
							patterns[givenNode->getName()];

					bool interestingInReqs = (std::find(tempPatternReqs.begin(),
							tempPatternReqs.end(), interestingNode->getLabel())
							!= tempPatternReqs.end());

					if (interestingInReqs)
					{
						foundMatches.push_back(interestingNode);
					}
				}

			}

		} // end our for loop

		unsigned int givenNodeReqLength = patterns[givenNode->getName()].size();

		if ((foundMatches.size() >= givenNodeReqLength)
				&& (foundMatches.size() > 0))
		{
			patterns.erase(givenNode->getLabel());
			results.push_back(givenNode);

			searchMatches(foundMatches, patterns, results, visitedKeys,
					graphSearch);

			// Return traverse
			return 2;
		}
		else if (foundMatches.size() == 0 && givenNodeReqLength == 0)
		{
			patterns.erase(givenNode->getName());
			results.push_back(givenNode);
			//return leaf
			return 1;
		}

	} //end our big if

	//return none, break recursion.
	return 0;
} //end search for patterns

template<class T>
void searchMatches(std::vector<TemplateGraph::Node<T>*> matches,
		std::map<std::string, std::vector<std::string>> patterns,
		std::vector<TemplateGraph::Node<T>*> &results,
		std::unordered_set<TemplateGraph::Node<T>*> &visitedKeys,
		TemplateGraph::Graph<T> &graphSearch)
{
	for (TemplateGraph::Node<T> *currMatchNode : matches)
	{
		bool matchInResults = (std::find(results.begin(), results.end(),
				currMatchNode) != results.end());
		if (!(visitedKeys.count(currMatchNode)) && !matchInResults)
		{
			int searchState = searchForPatterns(
					graphSearch.getIndexFromNode(currMatchNode), patterns,
					results, visitedKeys, graphSearch);
			//if we hit leaf
			if (searchState == 1)
			{
				break;
			}
		}
	}
} // end search matches

}//end anon namespace

namespace subgraphMatcher
{

template<class T>
std::unordered_map<TemplateGraph::Node<T>*, std::vector<TemplateGraph::Node<T>*>> findSubgraphs(
		TemplateGraph::Graph<T> &mainGraph,
		TemplateGraph::Graph<T> &queryGraph)
{

	std::unordered_map<TemplateGraph::Node<T>*, std::vector<TemplateGraph::Node<T>*>> resultsVecToReturn;

	mainGraph.chuckRottenTomatoes();
	queryGraph.chuckRottenTomatoes();

	std::map<std::string, std::vector<std::string>> queryGraphPatterns =
			patternExtractor(queryGraph);

	std::unordered_set<TemplateGraph::Node<T>*> visitedKeys;

	std::vector<TemplateGraph::Node<T>*> currNodeResults;

	for (unsigned int currSearchIndex = 0;
			currSearchIndex < mainGraph.getNodes().size(); currSearchIndex++)
	{
		searchForPatterns(currSearchIndex, queryGraphPatterns, currNodeResults,
				visitedKeys, queryGraph);

		visitedKeys.clear();

	}
	return resultsVecToReturn;
} // end subgraph matching

}


#endif //SUBGRAPHMATCHING_HPP
