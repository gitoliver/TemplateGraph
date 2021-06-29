#ifndef TEMPLATEGRAPH_ALGORITHMS_INCLUDE_SUBGRAPHMATCHING_HPP
#define TEMPLATEGRAPH_ALGORITHMS_INCLUDE_SUBGRAPHMATCHING_HPP

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../GraphStructure/include/Graph.hpp"
#include "../../GraphStructure/include/HalfAdjacencyMatrix.hpp"
#include "../../GraphStructure/include/Node.hpp"
#include "../../LazyPrints/LazyPrinters.hpp"

namespace
{
  //	This is used to extract the patterns of our query graph. A pattern is defined by a key and a set of node
  // signifiers.
  // This is used to "step through" what our query graph is and see if the steps relate to our actual graph. As of
  // now, our key and node signifiers are the names of the nodes. This can be changed but please note it will require us
  // to cascade changes throughout our algo.
  template<class T>
  std::map<std::string, std::vector<std::string>> patternExtractor(temp_graph::Graph<T> &t_patternGraph)
  {
    std::map<std::string, std::vector<std::string>> foundPatterns;

    for (unsigned int indexA = 0; indexA < t_patternGraph.getNodes().size(); indexA++)
      {
        // our "A-node"'s name will be our key for patterns
        std::pair<std::string, std::vector<std::string>> currentPattern;
        // we now place current nodes name as key for our current pattern
        currentPattern.first = t_patternGraph.getNodeFromIndex(indexA)->getName();
        // now we hit all other nodes and if the 2 nodes are connected we throw the
        //	secondary node signifier in our pattern
        for (unsigned int indexB = 0; indexB < t_patternGraph.getNodes().size(); indexB++)
          {
            // now we must check if the two nodes are connected. We can do this faster.
            // Fix up later
            if (t_patternGraph.getAdjMatrix().isConnected(indexA, indexB))
              {
                currentPattern.second.push_back(t_patternGraph.getNodeFromIndex(indexB)->getName());
              }
          }
        // once we populate all our patterns to the current key, we throw the pair
        // into our total pattern set, then go to next node.
        foundPatterns.insert(currentPattern);
      }
    return foundPatterns;
  } // end found patterns

  // forward declare because dumb
  template<class T>
  void searchMatches(std::vector<temp_graph::Node<T> *>                                                 t_matches,
                     std::map<std::string, std::vector<std::string>>                                    t_patterns,
                     std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>> &t_resultsPair,
                     std::unordered_set<temp_graph::Node<T> *> &t_visitedKeys, temp_graph::Node<T> *t_previousNode,
                     temp_graph::Graph<T> &t_graphSearch);

  template<class T>
  int searchForPatterns(
      unsigned int t_currNodeIndex, std::map<std::string, std::vector<std::string>> t_patterns,
      std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>> &t_resultsPair,
      std::unordered_set<temp_graph::Node<T> *> &t_visitedKeys, temp_graph::Node<T> *t_previousNode,
      temp_graph::Graph<T> &t_graphSearch)
  {
    temp_graph::Node<T> *currNode = t_graphSearch.getNodeFromIndex(t_currNodeIndex);

    // we check if we have a corresponding pattern to this node
    if (t_patterns.count(currNode->getName()) && !t_visitedKeys.count(currNode))
      {
        t_visitedKeys.insert(currNode);
        // When we are searching and we hit a match in our graphSearch (i.e. a parital
        // 	match onto our query graph) we want to put them in our matched nodes, then
        // 	we want to check search our patterns for each match.
        std::vector<temp_graph::Node<T> *> foundMatches;

        for (temp_graph::Node<T> *interestingNode : t_graphSearch.getNodes())
          {
            // for now we just want to check if we have our current node
            // we are checking out in our results vector.
            bool isInterestingInResults = (std::find(t_resultsPair.first.begin(), t_resultsPair.first.end(),
                                                     interestingNode) != t_resultsPair.first.end());
            // if we dont have current node in our results we want to check it out
            if (!(t_visitedKeys.count(interestingNode) && !isInterestingInResults))
              {
                // now ensure the two nodes are actually connected
                int interestingNodeIndex = t_graphSearch.getIndexFromNode(interestingNode);

                if (t_graphSearch.getAdjMatrix().isConnected(t_currNodeIndex, interestingNodeIndex))
                  {
                    // after confirmed connect we check if our interesting node
                    // is within the pattern requirements of our "current" node
                    // recall our keys for our pattern matching is the name of the node
                    std::vector<std::string> tempPatternReqs = t_patterns[currNode->getName()];

                    bool isInterestingInReqs = (std::find(tempPatternReqs.begin(), tempPatternReqs.end(),
                                                          interestingNode->getName()) != tempPatternReqs.end());

                    if (isInterestingInReqs)
                      {
                        // We now know our interesting node is not in our results,
                        // 		is connected to the current node we are checking out,
                        //		and that it is a pattern requirement for the current
                        // 		node we are checking out. Thus we put it on our matches
                        // 		that we are going to have to search through in this same
                        // 		manner.
                        foundMatches.push_back(interestingNode);
                      }
                  } // end the if for if teh 2 nodes are connected
              }
          }
        // end our for loop, this finds all matches we want to check and possibly throw in our
        // pairResults

        unsigned int currNodeReqsLength = t_patterns[currNode->getName()].size();
        // now we want to make sure that we have AT LEAST the same amount of matches
        // 	as we do for the current node requirements. Keep in mind we need to hit
        // 	all of each nodes pattern requirements to continue with a valid/matching
        // 	subgraph. Also we need to ensure our matches size is larger than 0 because
        // 	if we have 0 matches and 0 req length we are at a leaf
        if ((foundMatches.size() >= currNodeReqsLength) && (foundMatches.size() > 0))
          {
            // we now prune our matches in order to make the rest of our resursive journey
            // easier and faster. Once we match a pattern who cares about it anymore for this current run
            t_patterns.erase(currNode->getName());

            //	Now we get the edge between the previous node and this node to insert.
            //
            //  NOTE: For some odd reason when I take out this size check I end up
            //			trying to get a connecting edge between 2 nodes (many times) when
            //			they do NOT have an edge between them. Was due to never updating the
            //			"firstNode" value which would stay the same value as the node returned
            //			by checking key "0" from our graph.
            // if (resultsPair.first.size() > 0)
            //{
            // Since I have a small brain and need to check whats getting algo mad I wanna
            //	see if my idea is correct. It was, we were trying to get an edge between
            //	our node and itself (a "loop" edge).
            // if (!(currNode == resultsPair.first.back()))
            if (!(t_previousNode == currNode))
              {
                t_resultsPair.second.push_back(currNode->getConnectingEdge(t_previousNode));
              }
            //}				//end our bit that inserts edge.

            // add the current node we are checking out to our results since we are good so far
            t_resultsPair.first.push_back(currNode);
            t_previousNode = currNode;
            searchMatches(foundMatches, t_patterns, t_resultsPair, t_visitedKeys, t_previousNode, t_graphSearch);

            // return 2 to designate we are continuing our traversal
            return 2;
          }
        else if ((foundMatches.size() == 0) && (currNodeReqsLength == 0))
          {
            // we have hit a leaf, update our patterns
            t_patterns.erase(currNode->getName());

            if (t_resultsPair.first.size() > 0)
              {
                // Since I have a small brain and need to check whats getting algo mad I wanna
                //	see if my idea is correct. It was, we were trying to get an edge between
                //	our node and itself (a "loop" edge).
                if (!(currNode == t_resultsPair.first.back()))
                  {
                    t_resultsPair.second.push_back(currNode->getConnectingEdge(t_resultsPair.first.back()));
                  }
              } // end our bit that inserts edge.

            t_resultsPair.first.push_back(currNode);

            // return 1 for our leaf case
            return 1;
          }
      }
    // return 0 since we dont hit a match
    return 0;
  } // end search patterns

  // This is used to search all the matches we previously found. Works by calling our
  // 	search patterns function for each matched node, then we find THOSE matches
  // 	then hit this function again.
  //
  template<class T>
  void searchMatches(std::vector<temp_graph::Node<T> *>                                                 t_matches,
                     std::map<std::string, std::vector<std::string>>                                    t_patterns,
                     std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>> &t_resultsPair,
                     std::unordered_set<temp_graph::Node<T> *> &t_visitedKeys, temp_graph::Node<T> *t_previousNode,
                     temp_graph::Graph<T> &t_graphSearch)
  {
    for (temp_graph::Node<T> *currMatch : t_matches)
      {
        // as before we need to check if our node we are checking out is
        // already in our results i.e. already been run through
        bool isMatchInResults =
            (std::find(t_resultsPair.first.begin(), t_resultsPair.first.end(), currMatch) != t_resultsPair.first.end());
        // We want to make sure we havent used the current node as an "entry point"
        // 		for our search pattern function AND we want to make sure she doesnt
        // 		exist in our results yet.
        //
        if (!isMatchInResults && !(t_visitedKeys.count(currMatch)))
          {
            int searchState = searchForPatterns(t_graphSearch.getIndexFromNode(currMatch), t_patterns, t_resultsPair,
                                                t_visitedKeys, t_previousNode, t_graphSearch);
            // if we hit a leaf we get out
            if (searchState == 1)
              break;
          }
      }
  } // end search matches

} // namespace
// end anon namespace

//	As with the issue brought up within the cycle decomposition algo, the same issue of
//		us accidently get an induced structure arises here. Granted it may be more
//		difficult to accidently get an induced subgraph but it is totally possible.
//		This would be extremely annoying because we would have to completely hunt down
//		that it is this algo itself and the way we traverse the data is causing us to
//		return bad data.
//
//	TODO: Check if this works for cycles and we correctly return the needed data,
//			for instance how the cycle decomp returns all cycles and all edges. I am worried
//			this will end up missing an edge.
namespace subgraph_match
{
  template<class T>
  std::unordered_map<temp_graph::Node<T> *,
                     std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>>>
  findSubgraphs(temp_graph::Graph<T> &t_mainGraph, temp_graph::Graph<T> &t_queryGraph)
  {
    // This will be what is returned, we be slowly built using what is right below it.
    std::unordered_map<temp_graph::Node<T> *,
                       std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>>>
        subgraphEdgeNodeResults;

    std::pair<std::vector<temp_graph::Node<T> *>, std::vector<temp_graph::Edge<T> *>> pairedResult;

    // first we want to grab all our patterns that we will use to match on our main graph
    std::map<std::string, std::vector<std::string>> patternsToMatch = patternExtractor(t_queryGraph);

    std::vector<temp_graph::Node<T> *> currResults;

    std::unordered_set<temp_graph::Node<T> *> keyVisitTracker;

    std::unordered_map<temp_graph::Node<T> *, std::vector<temp_graph::Node<T> *>> finalResults;

    // now we run a search for each node
    for (unsigned int searchStartNodeIndex = 0; searchStartNodeIndex < t_mainGraph.getNodes().size();
         searchStartNodeIndex++)
      {
        temp_graph::Node<T> *firstNode = t_mainGraph.getNodeFromIndex(searchStartNodeIndex);
        searchForPatterns(searchStartNodeIndex, patternsToMatch, pairedResult, keyVisitTracker, firstNode, t_mainGraph);

        if (pairedResult.first.size() != 0)
          {
            subgraphEdgeNodeResults.insert({t_mainGraph.getNodeFromIndex(searchStartNodeIndex), pairedResult});
            // after we recursively hit all patterns we want to go ahead and clear out
            //	our results so we hit all again. From my understanding this is, in our case,
            //	the best way to clear our pairedResult pair.
            pairedResult = {};
          }
        keyVisitTracker.clear();
      }
    return subgraphEdgeNodeResults;
  }

} // namespace subgraph_match

#endif // END TEMPLATEGRAPH_ALGORITHMS_INCLUDE_SUBGRAPHMATCHING_HPP
