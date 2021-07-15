#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP

//#include "../../LazyPrints/LazyPrinters.hpp"
#include "./GenericGraphObject.hpp"

#include <memory>

namespace glygraph
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
    Edge(std::string name_t, Node<T> *const &sourceNode_t, Node<T> *const &targetNode_t);
    Edge(std::string name_t, std::vector<std::string> labels_t, Node<T> *const &sourceNode_t,
         Node<T> *const &targetNode_t);

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
    void setSourceNode(Node<T> *source_t);
    void setTargetNode(Node<T> *target_t);

  private:
    /************************************************
     *  ATTRIBUTES
     ***********************************************/
    // NOTE: Source node = the node that has a unique_ptr to this edge
    Node<T> *sourceNode_m;
    // NOTE: Sink node = the node that has a raw pointer to this edge
    Node<T> *targetNode_m;
  };

  template<class T>
  inline Edge<T>::Edge()
  {
    //badBehavior(__LINE__, __func__, "Warning calling default constructor");
    this->targetNode_m = NULL;
    this->sourceNode_m = NULL;
  }

  template<class T>
  inline Edge<T>::Edge(std::string name_t, Node<T> *const &sourceNode_t, Node<T> *const &targetNode_t)
      : GenericGraphObject(name_t)
  {
    this->targetNode_m = targetNode_t;
    this->sourceNode_m = sourceNode_t;
  }

  template<class T>
  inline Edge<T>::Edge(std::string name_t, std::vector<std::string> labels_t, Node<T> *const &sourceNode_t,
                       Node<T> *const &targetNode_t)
      : GenericGraphObject(name_t, labels_t)
  {
    this->targetNode_m = targetNode_t;
    this->sourceNode_m = sourceNode_t;
  }

  template<class T>
  inline Edge<T>::~Edge()
  {
    // have our edge destructor remove itself from our inList then let die
    this->targetNode_m->removeInEdge(this);
    // lazyInfo(__LINE__, __func__,
    //			"Edge with name <" + this->getName() + "> deleted");
  }

  // copy constructor
  template<class T>
  inline Edge<T>::Edge(const Edge<T> &rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier()),
        sourceNode_m(rhs.getSourceNode()), targetNode_m(rhs.getTargetNode())
  {
    // lazyInfo(__LINE__, __func__,
    //		"Calling copy constructor on " + this->getName());
  }

  // move constructor
  template<class T>
  inline Edge<T>::Edge(Edge<T> &&rhs)
      : GenericGraphObject(rhs.getName(), rhs.getLabels(), rhs.getConnectivityTypeIdentifier()),
        sourceNode_m(rhs.getSourceNode()), targetNode_m(rhs.getTargetNode())
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
    this->sourceNode_m = rhs.sourceNode_m;
    this->targetNode_m = rhs.targetNode_m;
    this->setName(rhs.getName());
    this->setLabels(rhs.getLabels());

    // after we yoink data wanted from our rhs we go ahead and delete it
    rhs.getSourceNode()->removeOutEdge(rhs);

    return *this;
  }

  template<class T>
  inline void Edge<T>::setSourceNode(Node<T> *source_t)
  {
    this->sourceNode_m = source_t;
  }

  template<class T>
  inline void Edge<T>::setTargetNode(Node<T> *target_t)
  {
    this->targetNode_m = target_t;
  }

  template<class T>
  inline Node<T> *Edge<T>::getTargetNode() const
  {
    return this->targetNode_m;
  }

  template<class T>
  inline Node<T> *Edge<T>::getSourceNode() const
  {
    return this->sourceNode_m;
  }

} // namespace temp_graph
#endif // end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_EDGE_HPP
