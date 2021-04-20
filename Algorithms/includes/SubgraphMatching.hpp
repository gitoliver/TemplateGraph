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
		TemplateGraph::Graph<T> patternGraph)
{
	std::map<std::string, std::vector<std::string>> foundPatterns;

	for (unsigned int indexA = 0; indexA < patternGraph.getNodes().size();
			indexA++)
	{
		//our "A node"'s name will be our key for patterns
		std::pair<std::string, std::vector<std::string>> currentPattern;
		//we now place current nodes name as key for our current pattern
		currentPattern.first = patternGraph.getNodeFromIndex(indexA)->getName();
		//now we hit all other nodes and if the 2 nodes are connected we throw the
		//	secondary node signifier in our pattern
		for (unsigned int indexB = 0; indexB < patternGraph.getNodes().size();
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
}

} //end anon namespace

namespace subgraphMatcher
{
template<class T>
void findSubgraphs(TemplateGraph::Graph<T> mainGraph,
		TemplateGraph::Graph<T> queryGraph)
{
	patternExtractor(queryGraph);
}
} //end subgraph matcher namespace

#endif //SUBGRAPHMATCHING_HPP
