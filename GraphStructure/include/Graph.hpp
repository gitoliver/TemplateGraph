#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP

#include "../../LazyPrints/LazyPrinters.hpp"
#include "HalfAdjacencyMatrix.hpp"
#include "Node.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace temp_graph
{

  template<class T>
  class Graph
  {
  public:
    /************************************************
     *  CONSTRUCTORS/DESTRUCTORS
     ***********************************************/
    Graph();
    // TODO: Ensure we would like this functionality, current idea is pass root node then get all traversable from this
    // node and store in our set
    Graph(Node<T> *const &t_initialNode);
    Graph(std::vector<Node<T> *> const &t_nodeList);

    // copy constructor
    Graph(const Graph<T> &rhs);

    // move constructor
    Graph(Graph<T> &&rhs);

    // copy assignment
    Graph<T> &operator=(const Graph<T> &rhs);

    // move assignment
    Graph<T> &operator=(Graph<T> &&rhs);

    virtual ~Graph();

    /************************************************
     *  GETTER/SETTER
     ***********************************************/
    // TODO: Finalize what we would like to pass. Using weak_ptr is nice because we can easily check
    // 			if our node is even useful/alive still but this would get annoying to constantly pass
    // 			weak_ptr<Node<T>>. As of now, for our getNodes I will be using a raw ptr due to the fact
    // 			that there should be no deletions etc. when we run our algos. I will do a check prior to
    // 			returning the vector to ensure that all nodes in our node-list are still alive.
    //
    std::vector<Node<T> *> getNodes();
    HalfAdjacencyMatrix<T> getAdjMatrix() const;

    unsigned int getIndexFromNode(Node<T> *const &t_queryNode);
    Node<T> *    getNodeFromIndex(unsigned int const &t_queryIndex);
    /************************************************
     *  MUTATORS
     ***********************************************/

    /************************************************
     *  FUNCTIONS
     ***********************************************/
    std::string getGraphvizLink();

  private:
    /************************************************
     *  ATTRIBUTES
     ***********************************************/
    HalfAdjacencyMatrix<T> m_adjMatrix;
    std::vector<Node<T> *> m_allNodes;
    // TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
    std::unordered_map<unsigned int, Node<T> *> m_nodeLookup;
    std::unordered_map<Node<T> *, unsigned int> m_indexLookup;

    /************************************************
     *  HELPER FUNCTIONS
     ***********************************************/
    void populateAdjacencyMatrix();
    void populateLookups();
    void lazyExpiredFixer();

    std::vector<Node<T> *> getReachableNodes(Node<T> *const &t_startingNode);
    // NOTE: To be used when we are passed solely a root node.
    void getReachableHelper(Node<T> *const &t_currentNode, std::unordered_set<Node<T> *> &t_visistedNodeSet,
                            std::vector<Node<T> *> &t_reachableNodes);
  }; // end graph class

  template<class T>
  inline Graph<T>::Graph()
  {
    badBehavior(__LINE__, __func__, "Warning calling default graph constructor");
  }

  template<class T>
  inline Graph<T>::Graph(Node<T> *const &t_initialNode)
  {

    // Lazy way to prevent dupes, cant use method used in get nodes
    // due to weak_ptr being useless in our typical stl algo functions

    std::vector<Node<T> *> tempNodeVec = this->getReachableNodes(t_initialNode);

    std::unordered_set<Node<T> *> tempNodeSet(tempNodeVec.begin(), tempNodeVec.end());

    for (Node<T> *currentNode : tempNodeSet)
      {
        this->m_allNodes.push_back(currentNode);
      }

    // populate our lookups
    this->populateLookups();
    this->populateAdjacencyMatrix();
  }

  template<class T>
  inline Graph<T>::Graph(std::vector<Node<T> *> const &t_nodeList)
  {
    if (t_nodeList.size() > 0)
      {
        // Lazy way to prevent dupes, again need to come up with
        // a more efficient way to actually prevent our dupes
        std::unordered_set<Node<T> *> tempNodeSet(t_nodeList.begin(), t_nodeList.end());
        for (Node<T> *currNode : tempNodeSet)
          {
            this->m_allNodes.push_back(currNode);
          }

        this->populateLookups();
        this->populateAdjacencyMatrix();
      }
    else
      {
        badBehavior(__LINE__, __func__, "Was passed a nodelist of size 0");
      }
  }

  template<class T>
  inline Graph<T>::~Graph()
  {
    lazyInfo(__LINE__, __func__, "Graph deleted");
  }

  template<class T>
  inline std::vector<Node<T> *> Graph<T>::getNodes()
  {
    return this->m_allNodes;
  }

  template<class T>
  inline HalfAdjacencyMatrix<T> Graph<T>::getAdjMatrix() const
  {
    return this->m_adjMatrix;
  }

  template<class T>
  inline void Graph<T>::populateAdjacencyMatrix()
  {
    if ((this->m_allNodes.size() > 0) && (this->m_indexLookup.size()))
      {
        this->m_adjMatrix.initializeWorkaround(this->getNodes());
        for (Node<T> *currNode : this->m_allNodes)
          {
            for (Node<T> *currNodeNeighbor : currNode->getNeighbors())
              {
                if (!(this->m_adjMatrix.isConnected(this->m_indexLookup[currNode],
                                                    this->m_indexLookup[currNodeNeighbor])))
                  {
                    this->m_adjMatrix.connect(this->m_indexLookup[currNode], this->m_indexLookup[currNodeNeighbor]);
                  }
              }
          }
      }
    else
      {
        badBehavior(__LINE__, __func__, "no nodes present! cannot populate adj matrix");
      }
  }

  // NOTE: Must call before we try to create our adj matrix
  template<class T>
  inline void Graph<T>::populateLookups()
  {
    if (this->m_allNodes.size() > 0)
      {
        this->m_nodeLookup.clear();
        this->m_indexLookup.clear();

        int currIndex = 0;
        for (Node<T> *currNode : this->m_allNodes)
          {

            this->m_nodeLookup.insert({currIndex, currNode});

            this->m_indexLookup.insert({currNode, currIndex});

            currIndex++;
          }
      }
    else
      {
        badBehavior(__LINE__, __func__, "Warning tried to populate lookups with no node list");
      }
  }

  template<class T>
  inline std::vector<Node<T> *> Graph<T>::getReachableNodes(Node<T> *const &t_startingNode)
  {
    std::unordered_set<Node<T> *> visitedNodes;
    // TODO: Please note that this current method does increase the size of our call stack a good bit due to the use of
    // recursion. 			Depending on how large of graphs we are dealing with this could become an issue and it may be
    // a better 			call to use a different method.

    std::vector<Node<T> *> reachableVecToReturn;

    this->getReachableHelper(t_startingNode, visitedNodes, reachableVecToReturn);
    return reachableVecToReturn;
  }

  // Should be correct. Passing pointer by reference
  template<class T>
  inline unsigned int Graph<T>::getIndexFromNode(Node<T> *const &t_queryNode)
  {
    return this->m_indexLookup[t_queryNode];
  }

  template<class T>
  inline Node<T> *Graph<T>::getNodeFromIndex(unsigned int const &t_queryIndex)
  {
    return this->m_nodeLookup[t_queryIndex];
  }

  // TODO: Find a better way to remove all expired ptrs that will always work
  // 			I am worried that just iterating through index will not work well
  // 			in all cases. Could just do currIndex-- once we find an expired
  // 			and remove it at the end of our current loop iteration tho.
  //
  template<class T>
  inline void Graph<T>::lazyExpiredFixer()
  {
    // Possibly a good way, need to run through some tests.
    unsigned int ogSize = this->m_allNodes.size();
    for (unsigned int currIndex = 0; currIndex < this->m_allNodes.size(); currIndex++)
      {
        if (this->m_allNodes[currIndex].expired())
          {
            this->m_allNodes.erase(this->m_allNodes.begin() + currIndex);
            currIndex--;
          }
      }

    //	Extremely heavy way but def works
    //
    // unsigned int ogSize = this->allNodes.size();
    // std::vector<std::weak_ptr<Node<T>>> dustyList = this->allNodes;
    // this->allNodes.clear();
    // for (std::weak_ptr<Node<T>> currDusty : dustyList)
    // {
    // if (!(currDusty.expired()))
    // {
    // this->m_allNodes.push_back(currDusty);
    // }
    // }
    if (ogSize != this->m_allNodes.size())
      {
        this->populateLookups();
        this->populateAdjacencyMatrix();
      }
  }

  template<class T>
  inline std::string Graph<T>::getGraphvizLink()
  {
    std::string connectionArrow = "%20-%3E%20";
    std::string newLine         = "%0A%09";
    std::string baseURL         = "https://dreampuf.github.io/GraphvizOnline/#digraph%20G%20%7B%0A%09";
    std::string endBracket      = "%0A%7D";

    // first make a collection of all of our node connections, just use set cause less chars lazy
    std::map<Node<T> *, std::set<Node<T> *>> nodeNeighs;
    for (Node<T> *currNode : this->getNodes())
      {
        for (Node<T> *currNeigh : currNode->getNeighbors())
          {
            // ensure that the current connection is not already present
            //	We know we do not have a specific connection if either
            // 		A) We do NOT have our neighbor as a key in the node neighs
            // 						OR
            // 		B) If we DO have our neighbor as a key in the node neighs, then we do NOT have
            // 				the currNode as a member
            if ((nodeNeighs.count(currNeigh) == 0) || (nodeNeighs[currNeigh].count(currNode) == 0))
              {
                if (nodeNeighs[currNode].count(currNeigh) == 0)
                  {
                    nodeNeighs[currNode].insert(currNeigh);
                  }
                else
                  {
                    badBehavior(__LINE__, __func__, "ARGH MATEY");
                  }
              }
          } // end 4
      }     // end 44

    std::string endURL;

    for (std::pair<Node<T> *, std::set<Node<T> *>> currPair : nodeNeighs)
      {
        for (Node<T> *currNeigh : currPair.second)
          {
            endURL += currPair.first->getName() + "->" + currNeigh->getName() + newLine;
          }
      }
    endURL += endBracket;
    baseURL += endURL;
    return baseURL;
  }
  /*
   adjMatrix;
   std::vector<Node<T>*> allNodes;
   // TODO: Ensure the correct hashing function is being used. Must be 100% sure, am only somewhat sure.
   std::unordered_map<unsigned int, Node<T>*> nodeLookup;
   std::unordered_map<Node<T>*, unsigned int> m_indexLookup;
   */

  // copy constructor
  template<class T>
  inline Graph<T>::Graph(const Graph<T> &rhs)
      : m_adjMatrix(rhs.m_adjMatrix), m_nodeLookup(rhs.m_nodeLookup), m_indexLookup(rhs.m_indexLookup),
        m_allNodes(rhs.m_allNodes)
  {
  }

  // move constructor
  template<class T>
  inline Graph<T>::Graph(Graph<T> &&rhs)
      : m_adjMatrix(rhs.m_adjMatrix), m_nodeLookup(rhs.m_nodeLookup), m_indexLookup(rhs.m_indexLookup),
        m_allNodes(rhs.m_allNodes)
  {
  }

  // copy assignment
  template<class T>
  inline Graph<T> &Graph<T>::operator=(const Graph<T> &rhs)
  {
    this->m_adjMatrix   = rhs.m_adjMatrix;
    this->m_allNodes    = rhs.m_allNodes;
    this->m_indexLookup = rhs.m_indexLookup;
    this->m_nodeLookup  = rhs.m_nodeLookup;
    return *this;
  }

  // move assignment
  template<class T>
  inline Graph<T> &Graph<T>::operator=(Graph<T> &&rhs)
  {
    this->m_adjMatrix   = rhs.m_adjMatrix;
    this->m_allNodes    = rhs.m_allNodes;
    this->m_indexLookup = rhs.m_indexLookup;
    this->m_nodeLookup  = rhs.m_nodeLookup;
    return *this;
  }

  template<class T>
  inline void Graph<T>::getReachableHelper(Node<T> *const &t_currentNode, std::unordered_set<Node<T> *> &visitedNodeSet,
                                           std::vector<Node<T> *> &t_reachableNodes)
  {

    t_reachableNodes.push_back(t_currentNode);

    visitedNodeSet.insert(t_currentNode);
    for (Node<T> *currNeighbor : t_currentNode->getNeighbors())
      {
        if (!(visitedNodeSet.count(currNeighbor)))
          {
            this->getReachableHelper(currNeighbor, visitedNodeSet, t_reachableNodes);
          }
      }
  }

} // namespace temp_graph
#endif // TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GRAPH_HPP
