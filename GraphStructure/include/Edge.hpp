#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP

#include "../../LazyPrints/LazyPrinters.hpp"
#include "./GenericGraphObject.hpp"

#include <memory>

namespace temp_graph
{

  // TODO: Ensure that this is correct forward declare
  template<class T>
  class Node;

  template<class T>
  class Edge : public GenericGraphObject
  {
  public:
    /************************************************
     *  CONSTRUCTORS/DESTRUCTORS
     ***********************************************/
    Edge();
    Edge(std::string t_name, Node<T> *const &t_sourceNode, Node<T> *const &t_targetNode);
    Edge(std::string t_name, std::vector<std::string> t_labels, Node<T> *const &t_sourceNode,
         Node<T> *const &t_targetNode);

    // copy constructor
    Edge(const Edge<T> &rhs);

    // move constructor
    Edge(Edge<T> &&rhs);

    // copy assignment
    Edge<T> &operator=(const Edge<T> &rhs);

    // move assignment
    Edge<T> &operator=(Edge<T> &&rhs);

    virtual ~Edge();

    //	Return our weak to ensure alive.
    Node<T> *getTargetNode() const;
    Node<T> *getSourceNode() const;

    /************************************************
     *  GETTER/SETTER
     ***********************************************/
    // NOTE: Using shared pointer to get our source and sink in order to ensure
    // 			source and sink are good and alive.
    void setSourceNode(Node<T> *t_source);
    void setTargetNode(Node<T> *t_target);

  private:
    /************************************************
     *  ATTRIBUTES
     ***********************************************/
    // NOTE: Source node = the node that has a unique_ptr to this edge
    Node<T> *m_sourceNode;
    // NOTE: Sink node = the node that has a raw pointer to this edge
    Node<T> *m_targetNode;
  };

  template<class T>
  inline Edge<T>::Edge()
  {
    badBehavior(__LINE__, __func__, "Warning calling default constructor");
    this->m_targetNode = NULL;
    this->m_sourceNode = NULL;
  }

  template<class T>
  inline Edge<T>::Edge(std::string t_name, Node<T> *const &t_sourceNode, Node<T> *const &t_targetNode)
      : GenericGraphObject(t_name)
  {
    this->m_targetNode = t_targetNode;
    this->m_sourceNode = t_sourceNode;
  }

  template<class T>
  inline Edge<T>::Edge(std::string t_name, std::vector<std::string> t_labels, Node<T> *const &t_sourceNode,
                       Node<T> *const &t_targetNode)
      : GenericGraphObject(t_name, t_labels)
  {
    this->m_targetNode = t_targetNode;
    this->m_sourceNode = t_sourceNode;
  }

  template<class T>
  inline Edge<T>::~Edge()
  {
    // have our edge destructor remove itself from our inList then let die
    this->m_targetNode->removeInEdge(this);
    // lazyInfo(__LINE__, __func__,
    //			"Edge with name <" + this->getName() + "> deleted");
  }

  // copy constructor
  template<class T>
  inline Edge<T>::Edge(const Edge<T> &rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier()),
        m_sourceNode(rhs.getSourceNode()), m_targetNode(rhs.getTargetNode())
  {
    // lazyInfo(__LINE__, __func__,
    //		"Calling copy constructor on " + this->getName());
  }

  // move constructor
  template<class T>
  inline Edge<T>::Edge(Edge<T> &&rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier()),
        m_sourceNode(rhs.getSourceNode()), m_targetNode(rhs.getTargetNode())
  {
    // wanted data has been yoinked so we go ahead and delete this edge that we dont care about
    //	anymore. As stated in move assignment we dont care what state we leave our rhs in after a move
    // lazyInfo(__LINE__, __func__,
    //			"Calling move constructor on " + this->getName());

    rhs.getSourceNode()->removeOutEdge(rhs);
  }

  // copy assignment
  template<class T>
  inline Edge<T> &Edge<T>::operator=(const Edge<T> &rhs)
  {
    return *this = Edge<T>(rhs);
  }

  // move assignment
  template<class T>
  inline Edge<T> &Edge<T>::operator=(Edge<T> &&rhs)
  {
    // lazyInfo(__LINE__,__func__, "Edge move assignment");
    // Please note that in order to help prevent some bad behavior due to moving an edge
    //	causing bad connectivity to arise (i.e. multigraph creation, etc.) I am using
    //	the delete on move paradigm in order to help prevent this. Keep in mind move
    //	implies that we dont care about what happens to our rhs.
    this->m_sourceNode = rhs.m_sourceNode;
    this->m_targetNode = rhs.m_targetNode;
    this->setName(rhs.getName());
    this->setLabels(rhs.getLabels());

    // after we yoink data wanted from our rhs we go ahead and delete it
    rhs.getSourceNode()->removeOutEdge(rhs);

    return *this;
  }

  template<class T>
  inline void Edge<T>::setSourceNode(Node<T> *t_source)
  {
    this->m_sourceNode = t_source;
  }

  template<class T>
  inline void Edge<T>::setTargetNode(Node<T> *t_target)
  {
    this->m_targetNode = t_target;
  }

  template<class T>
  inline Node<T> *Edge<T>::getTargetNode() const
  {
    return this->m_targetNode;
  }

  template<class T>
  inline Node<T> *Edge<T>::getSourceNode() const
  {
    return this->m_sourceNode;
  }

} // namespace temp_graph
#endif // end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
