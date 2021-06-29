#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP

#include "../../LazyPrints/LazyPrinters.hpp"
#include "./Edge.hpp"
#include "./GenericGraphObject.hpp"

#include <memory>
#include <unordered_set>

namespace temp_graph
{
  template<class T>
  class Node : public GenericGraphObject, public std::enable_shared_from_this<Node<T>>
  {
  public:
    /************************************************
     *  CONSTRUCTORS/DESTRUCTORS
     ***********************************************/
    Node();
    Node(std::string t_name);
    Node(std::string t_name, std::string t_label);

    // copy constructor
    Node(const Node<T> &rhs);

    // move constructor
    Node(Node<T> &&rhs);

    // copy assignment
    Node<T> &operator=(const Node<T> &rhs);

    // move assignment
    Node<T> &operator=(Node<T> &&rhs);

    virtual ~Node();

    /************************************************
     *  GETTER/SETTER
     ***********************************************/

    inline T *getDeriviedClass()
    {
      auto derived = static_cast<T *>(this);
      return derived;
    }

    std::vector<Node<T> *> getNeighbors();

    std::vector<Edge<T> *> getEdges() const;
    std::vector<Edge<T> *> getOutEdges() const;
    std::vector<Edge<T> *> getInEdges() const;

    /************************************************
     *  MUTATORS
     ***********************************************/
    void addEdge(Edge<T> *t_edgeToAdd);
    /* TODO: Finalize how we would like to add nodes to one another. The addNeighbor will just be a
     * 			wrapper for our addChild. Please note how I avoided having an "addEdge" because from
     * 			my understanding and what I am pretty sure our use will be every single edge is
     * 			guaranteed to have both a source and sink node.
     */
    void addNeighbor(std::string t_edgeName, Node<T> *const &t_newNeighbor);

    void addChild(std::string t_edgeName, Node<T> *const &t_childNode);

    void addParent(std::string t_edgeName, Node<T> *const &t_parentNode);
    /* NOTE: We MUST remove the edge from out "child" node BEFORE deleting the edge by deleting the
     * 			unique_ptr that owns it. This is handled in edge's destructor, all we have to worry
     * 			about is deleting the unique ptr that owns our edge.
     */
    void removeEdgeBetween(Node<T> *const &t_otherNode);

    void removeInEdge(Edge<T> *t_edgeToRemove);
    void removeOutEdge(Edge<T> *t_edgeToRemove);

    /************************************************
     *  FUNCTIONS
     ***********************************************/
    bool     isNeighbor(Node<T> *const &t_otherNode);
    Edge<T> *getConnectingEdge(Node<T> *const &t_otherNode);

  private:
    /************************************************
     *  ATTRIBUTES
     ***********************************************/
    std::vector<std::unique_ptr<Edge<T>>> m_outEdges;
    std::vector<Edge<T> *>                m_inEdges;

    /************************************************
     *  FUNCTIONS
     ***********************************************/
    void edgeConnectionUpdate();

    bool isChildOf(Node<T> *const &t_possibleParent);
    bool isParentOf(Node<T> *const &t_possibleChild);

    std::vector<Node<T> *> getChildren();
    std::vector<Node<T> *> getParents();

    friend class Edge<T>;
  };

  template<class T>
  inline Node<T>::Node() : GenericGraphObject("INVALID NODE")
  {
    badBehavior(__LINE__, __func__, "We called the default node constructor");
  }

  template<class T>
  inline Node<T>::Node(std::string name) : GenericGraphObject(name)
  {
    // lazyInfo(__LINE__, __func__,
    //		"Created node with name <" + this->getName() + ">");
  }

  template<class T>
  inline Node<T>::Node(std::string t_name, std::string t_label) : GenericGraphObject(t_name, t_label)
  {
    // lazyInfo(__LINE__, __func__,
    //		"Created node with name <" + this->getName()
    //				+ ">\n\tAnd with label <" + this->getLabel() + ">");
  }

  template<class T>
  inline Node<T>::~Node()
  {
    std::vector<Edge<T> *> tempInEdge = this->m_inEdges;
    lazyInfo(__LINE__, __func__, "Destroying Node: " + this->getName());
    std::cout << "\tMem Addr: " << this << "\n\n";
    // go through and hit all our parents, i.e. the ones that own the incoming edge and delete them
    // TODO: Do this but not lazy
    this->m_outEdges.clear();
    for (Edge<T> *currInEdge : tempInEdge)
      {
        currInEdge->getSourceNode()->removeOutEdge(currInEdge);
      }
    tempInEdge.clear();
  }

  // Copy constructor
  template<class T>
  inline Node<T>::Node(const Node<T> &rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier())
  {
    // std::cout << "\n\tGiven object ptr: " << rhs.objectPtr << "\n\n";
    // lazyInfo(__LINE__, __func__, "Calling copy constructor");
    for (Edge<T> const *currInEdge : rhs.m_inEdges)
      {
        std::unique_ptr<Edge<T>> tempIn(new Edge<T>(*currInEdge));

        tempIn.get()->setTargetNode(this);

        this->m_inEdges.push_back(tempIn.get());
        tempIn.get()->getSourceNode()->m_outEdges.push_back(std::move(tempIn));
      }
    for (std::unique_ptr<Edge<T>> const &currOutEdge : rhs.m_outEdges)
      {
        std::unique_ptr<Edge<T>> tempOut(new Edge<T>(*currOutEdge.get()));

        tempOut.get()->setSourceNode(this);

        tempOut.get()->getTargetNode()->m_inEdges.push_back(tempOut.get());
        this->m_outEdges.push_back(std::move(tempOut));
      }
  }

  // move constructor
  template<class T>
  inline Node<T>::Node(Node<T> &&rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier()),
        m_outEdges(std::move(rhs.m_outEdges)), m_inEdges(std::move(rhs.m_inEdges))
  {
    // lazyInfo(__LINE__, __func__, "Calling node move constructor");
    this->edgeConnectionUpdate();
  }

  // copy assignment
  template<class T>
  inline Node<T> &Node<T>::operator=(const Node<T> &rhs)
  {
    return *this = Node<T>(rhs);
  }

  // move assignment
  template<class T>
  inline Node<T> &Node<T>::operator=(Node<T> &&rhs)
  {
    // lazyInfo(__LINE__, __func__, "Calling node move assignment");
    this->setName(rhs.getName());
    this->setLabels(rhs.getLabels());
    this->setConnectivityTypeIdentifier(rhs.getConnectivityTypeIdentifier());

    this->m_inEdges  = std::move(rhs.m_inEdges);
    this->m_outEdges = std::move(rhs.m_outEdges);
    this->edgeConnectionUpdate();

    return *this;
  }

  template<class T>
  inline std::vector<Node<T> *> Node<T>::getNeighbors()
  {
    std::vector<Node<T> *> childrenVec = this->getChildren();
    std::vector<Node<T> *> parentsVec  = this->getParents();
    parentsVec.insert(parentsVec.end(), childrenVec.begin(), childrenVec.end());

    std::unordered_set<Node<T> *> tempSet;

    // TODO: Actually prevent dupes instead of doing so lazily
    for (Node<T> *cWP : parentsVec)
      {
        tempSet.insert(cWP);
      }

    std::vector<Node<T> *> parentsVeclol;
    for (Node<T> *cWP : tempSet)
      {
        parentsVeclol.push_back(cWP);
      }
    return parentsVeclol;
  }

  template<class T>
  inline std::vector<Edge<T> *> Node<T>::getEdges() const
  {
    std::vector<Edge<T> *> outEdgesVec = this->getOutEdges();
    std::vector<Edge<T> *> inEdgesVec  = this->getInEdges();
    outEdgesVec.insert(outEdgesVec.end(), inEdgesVec.begin(), inEdgesVec.end());
    return outEdgesVec;
  }

  template<class T>
  inline std::vector<Edge<T> *> Node<T>::getOutEdges() const
  {
    std::vector<Edge<T> *> outEdgeVecToReturn;
    for (std::unique_ptr<Edge<T>> const &currOutEdge : this->m_outEdges)
      {
        outEdgeVecToReturn.push_back(currOutEdge.get());
      }
    return outEdgeVecToReturn;
  }

  template<class T>
  inline std::vector<Edge<T> *> Node<T>::getInEdges() const
  {
    return this->m_inEdges;
  }

  template<class T>
  inline void Node<T>::addNeighbor(std::string t_edgeName, Node<T> *const &t_newNeighbor)
  {
    this->addChild(t_edgeName, t_newNeighbor);
  }

  template<class T>
  inline void Node<T>::addChild(std::string t_edgeName, Node<T> *const &t_childNode)
  {
    if (this->isNeighbor(t_childNode))
      {
        badBehavior(__LINE__, __func__, "Trying to make create an edge between two nodes that are already neighbors");
      }
    else if (this == t_childNode)
      {
        badBehavior(__LINE__, __func__, "Trying to add self as child, stop that!");
      }
    else
      {
        std::unique_ptr<Edge<T>> tempEdge(new Edge<T>(t_edgeName, this, t_childNode));

        t_childNode->m_inEdges.push_back(tempEdge.get());

        this->m_outEdges.push_back(std::move(tempEdge));
      }
  }

  template<class T>
  inline void Node<T>::addParent(std::string t_edgeName, Node<T> *const &t_parentNode)
  {
    t_parentNode->addChild(t_edgeName, this);
  }

  template<class T>
  inline void Node<T>::removeEdgeBetween(Node<T> *const &t_otherNode)
  {
    if (this->isNeighbor(t_otherNode))
      {
        if (this->isChildOf(t_otherNode))
          {
            Edge<T> *edgeToRemove = this->getConnectingEdge(t_otherNode);
            t_otherNode->removeOutEdge(edgeToRemove);
          }
        else if (this->isParentOf(t_otherNode))
          {
            t_otherNode->removeEdgeBetween(this);
          }
        else
          {
            badBehavior(__LINE__, __func__, "Tried to remove an edge for some reason it isnt a parent or child");
          }
      }
    else
      {
        badBehavior(__LINE__, __func__,
                    "Tried to remove an edge between node <" + this->getName() + "> and node <" +
                        t_otherNode->getName() + ">");
      }
  }

  template<class T>
  inline bool Node<T>::isNeighbor(Node<T> *const &t_otherNode)
  {
    return this->isChildOf(t_otherNode) || this->isParentOf(t_otherNode);
  }

  template<class T>
  bool Node<T>::isChildOf(Node<T> *const &t_possibleParent)
  {
    for (Edge<T> *currInEdge : this->m_inEdges)
      {
        if (currInEdge->getSourceNode() == t_possibleParent)
          {
            return true;
          }
      }
    return false;
  }

  template<class T>
  inline bool Node<T>::isParentOf(Node<T> *const &t_possibleChild)
  {
    for (std::unique_ptr<Edge<T>> const &currOutEdge : this->m_outEdges)
      {
        if (currOutEdge.get()->getTargetNode() == t_possibleChild)
          {
            return true;
          }
      }
    return false;
  }

  template<class T>
  inline std::vector<Node<T> *> Node<T>::getChildren()
  {
    std::vector<Node<T> *> childrenVecToReturn;
    for (std::unique_ptr<Edge<T>> const &currOutEdge : this->m_outEdges)
      {
        childrenVecToReturn.push_back(currOutEdge.get()->getTargetNode());
        // lazyInfo(__LINE__, __func__,
        //		currOutEdge.get()->getTargetNode()->getName());
        // childrenVecToReturn.push_back(currOutEdge.get()->getTargetNode().shared_from_this());
      }
    return childrenVecToReturn;
  }

  template<class T>
  inline void Node<T>::edgeConnectionUpdate()
  {
    // So even tho we moved all the references to an edge, we have
    //	not updated said edges with their new vertex appropriately
    for (Edge<T> *currInEdge : this->m_inEdges)
      {
        currInEdge->setTargetNode(this);
      }
    for (std::unique_ptr<Edge<T>> &currOutEdge : this->m_outEdges)
      {
        currOutEdge.get()->setSourceNode(this);
      }
  }

  template<class T>
  inline std::vector<Node<T> *> Node<T>::getParents()
  {
    std::vector<Node<T> *> parentsVecToReturn;
    for (Edge<T> *currInEdge : this->m_inEdges)
      {
        parentsVecToReturn.push_back(currInEdge->getSourceNode());
      }
    return parentsVecToReturn;
  }

  template<class T>
  inline void Node<T>::removeInEdge(Edge<T> *t_edgeToRemove)
  {
    // lazyInfo(__LINE__, __func__,
    //		"removing in edge <" + edgeToRemove->getName()
    //				+ "> from node named <" + this->getName() + ">");
    this->m_inEdges.erase(std::remove(this->m_inEdges.begin(), this->m_inEdges.end(), t_edgeToRemove),
                          this->m_inEdges.end());
  }

  template<class T>
  inline void Node<T>::removeOutEdge(Edge<T> *t_edgeToRemove)
  {
    // lazyInfo(__LINE__, __func__,
    //			"removing out edge <" + edgeToRemove->getName()
    //	 				+ "> from node named <" + this->getName() + ">");
    for (unsigned int outIndex = 0; outIndex <= this->m_outEdges.size(); outIndex++)
      {
        if (this->m_outEdges[outIndex].get() == t_edgeToRemove)
          {
            this->m_outEdges.erase(this->m_outEdges.begin() + outIndex);
          }
      }
  }

  template<class T>
  inline Edge<T> *temp_graph::Node<T>::getConnectingEdge(Node<T> *const &t_otherNode)
  {
    if (this->isNeighbor(t_otherNode))
      {
        if (this->isChildOf(t_otherNode))
          {
            for (Edge<T> *currInEdge : this->m_inEdges)
              {
                if (currInEdge->getSourceNode() == t_otherNode)
                  {
                    return currInEdge;
                  }
              }
          }
        else if (this->isParentOf(t_otherNode))
          {
            for (std::unique_ptr<Edge<T>> const &currOutEdge : this->m_outEdges)
              {
                if (currOutEdge.get()->getTargetNode() == t_otherNode)
                  {
                    return currOutEdge.get();
                  }
              }
          }

        // NOTE: Should never hit here
        badBehavior(__LINE__, __func__, "Found that two nodes were neihgbors but could not find the connecting edge");
      }
    else
      {
        badBehavior(__LINE__, __func__, "Tried to get a connecting edge whene the two nodes are not connected");
        return NULL;
      }
    return NULL;
  }

} // namespace temp_graph

#endif // end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_NODE_HPP
