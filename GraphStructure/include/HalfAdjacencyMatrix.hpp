#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_HALF_ADJACENCY_MATRIX_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_HALF_ADJACENCY_MATRIX_HPP

#include <vector>

#include "../../LazyPrints/LazyPrinters.hpp"

namespace temp_graph
{
  // TODO:Remove forward declare, why throwing errors?
  template<class T>
  class Node;

  template<class T>
  class HalfAdjacencyMatrix
  {
  public:
    /************************************************
     *  CONSTRUCTORS/DESTRUCTORS
     ***********************************************/
    HalfAdjacencyMatrix();
    HalfAdjacencyMatrix(std::vector<Node<T> *> const &t_nodeList);
    // copy constructor
    HalfAdjacencyMatrix(const HalfAdjacencyMatrix<T> &m);

    ~HalfAdjacencyMatrix() {};

    /************************************************
     *  GETTER/SETTER
     ***********************************************/
    unsigned int getNumEdges();
    unsigned int getNumNodes();

    /************************************************
     *  MUTATORS
     ***********************************************/
    void connect(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex);
    void disconnect(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex);

    /************************************************
     *  FUNCTIONS
     ***********************************************/
    bool isConnected(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex);

    // TODO: Actually fix our constructors so we do not have to use this initialize workaround stuff
    void initializeWorkaround(const HalfAdjacencyMatrix<T> &m);
    void initializeWorkaround(std::vector<Node<T> *> const &t_nodeList);
    void emptyInitializeWorkaround(const HalfAdjacencyMatrix<T> &m);

    /************************************************
     *  OPERATOR OVERLOADS
     ***********************************************/
    inline bool operator()(unsigned int t_nodeAIndex, unsigned int t_nodeBIndex)
    {
      return isConnected(t_nodeAIndex, t_nodeBIndex);
    }

    // our XOR overload
    inline HalfAdjacencyMatrix<T> operator^(const HalfAdjacencyMatrix<T> &rhs) const
    {
      // TODO: Figure this out, issue is not passing in our
      //			type. Tries to pass in the int.
      // HalfAdjacencyMatrix<T> result(this->numNodes);
      HalfAdjacencyMatrix<T> result(rhs);
      result.emptyInitializeWorkaround(rhs);
      if (this->m_numNodes != rhs.m_numNodes)
        {
          badBehavior(__LINE__, __func__, "WARNING NUMBER OF NODES NOT EQUAL XOR");
          return result;
        }
      else
        {
          for (unsigned int bitListIndex = 0; bitListIndex < this->m_bitList.size(); bitListIndex++)
            {
              if ((this->m_bitList[bitListIndex] || rhs.m_bitList[bitListIndex]) &&
                  (this->m_bitList[bitListIndex] != rhs.m_bitList[bitListIndex]))
                {
                  result.m_bitList[bitListIndex] = 1;
                  ++result.m_numEdges;
                }
            }
          return result;
        }
    }

    // our XOR-EQUALS overload
    inline HalfAdjacencyMatrix<T> &operator^=(const HalfAdjacencyMatrix<T> &rhs)
    {
      if (this->m_numNodes != rhs.m_numNodes)
        {
          badBehavior(__LINE__, __func__, "WARNING NUMBER OF NODES NOT EQUAL XOREQUALS");
        }
      m_numEdges = 0;
      for (unsigned int bitListIndex = 0; bitListIndex < this->m_bitList.size(); bitListIndex++)
        {
          if ((this->m_bitList[bitListIndex] || rhs.m_bitList[bitListIndex]) &&
              (this->m_bitList[bitListIndex] != rhs.m_bitList[bitListIndex]))
            {
              this->m_bitList[bitListIndex] = 1;
              m_numEdges++;
            }
          else
            this->m_bitList[bitListIndex] = 0;
        }
      return *this;
    }

    // our equals operator, well assignment equals
    inline HalfAdjacencyMatrix<T> &operator=(const HalfAdjacencyMatrix<T> &rhs)
    {
      if (this->m_numNodes != rhs.m_numNodes)
        {
          badBehavior(__LINE__, __func__, "WARNING NUMBER OF NODES NOT EQUAL EQUAL");
        }
      this->m_bitList  = rhs.m_bitList;
      this->m_numEdges = rhs.m_numEdges;
      return *this;
    }

  private:
    // Allows for proper index lookup. We use a half adjacency matrix because
    // 	our data is mirrored on the diagonal and we would like to save space
    unsigned int index(const unsigned int t_aNodeIndex, const unsigned int t_bNodeIndex);
    // Our actual connections/Adj matrix
    std::vector<bool> m_bitList;
    unsigned int      m_numEdges;
    unsigned int      m_numNodes;
    long long         m_indexFactor;
  };

  template<class T>
  HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix()
  {
    badBehavior(__LINE__, __func__, "Warning default constructor called");
    this->m_numEdges    = 0;
    this->m_numNodes    = 0;
    this->m_indexFactor = 0;
  }

  template<class T>
  HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix(std::vector<Node<T> *> const &t_nodeList)
  {
    unsigned int numNodes = t_nodeList.size();
    this->m_bitList.assign(((numNodes * (numNodes - 1)) / 2), 0);
    this->m_numNodes    = numNodes;
    this->m_numEdges    = 0;
    this->m_indexFactor = (1 + 2 * (numNodes - 2));
  }

  template<class T>
  HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix(const HalfAdjacencyMatrix<T> &m)
  {
    this->m_bitList     = m.m_bitList;
    this->m_numEdges    = m.m_numEdges;
    this->m_numNodes    = m.m_numNodes;
    this->m_indexFactor = m.m_indexFactor;
  }

  template<class T>
  void HalfAdjacencyMatrix<T>::initializeWorkaround(const HalfAdjacencyMatrix<T> &m)
  {
    this->m_bitList     = m.m_bitList;
    this->m_numEdges    = m.m_numEdges;
    this->m_numNodes    = m.m_numNodes;
    this->m_indexFactor = m.m_indexFactor;
  }

  template<class T>
  void HalfAdjacencyMatrix<T>::initializeWorkaround(std::vector<Node<T> *> const &t_nodeList)
  {
    unsigned int numNodes = t_nodeList.size();
    this->m_bitList.assign(((numNodes * (numNodes - 1)) / 2), 0);
    this->m_numNodes    = numNodes;
    this->m_numEdges    = 0;
    this->m_indexFactor = (1 + 2 * (numNodes - 2));
  }

  template<class T>
  unsigned int HalfAdjacencyMatrix<T>::getNumEdges()
  {
    return this->m_numEdges;
  }

  template<class T>
  void HalfAdjacencyMatrix<T>::connect(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex)
  {
    if (this->m_bitList[index(t_aNodeIndex, t_bNodeIndex)])
      {
        badBehavior(__LINE__, __func__, "TRYING TO ADD A CONNECTION THAT WAS ALREADY THERE");
      }
    else
      {
        this->m_bitList[index(t_aNodeIndex, t_bNodeIndex)] = true;
        this->m_numEdges++;
      }
  }

  template<class T>
  void HalfAdjacencyMatrix<T>::disconnect(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex)
  {
    if (!(this->m_bitList[index(t_aNodeIndex, t_bNodeIndex)]))
      {
        badBehavior(__LINE__, __func__, "TRYING TO REMOVE A CONNECTION THAT WAS NOT THERE");
      }
    else
      {
        this->m_bitList[index(t_aNodeIndex, t_bNodeIndex)] = false;
        this->m_numEdges--;
      }
  }

  template<class T>
  bool HalfAdjacencyMatrix<T>::isConnected(unsigned int t_aNodeIndex, unsigned int t_bNodeIndex)
  {
    if (t_aNodeIndex == t_bNodeIndex)
      return false;
    return this->m_bitList[index(t_aNodeIndex, t_bNodeIndex)];
  }

  template<class T>
  unsigned int HalfAdjacencyMatrix<T>::getNumNodes()
  {
    return this->m_numNodes;
  }

  template<class T>
  void HalfAdjacencyMatrix<T>::emptyInitializeWorkaround(const HalfAdjacencyMatrix<T> &m)
  {
    unsigned int numNodes = m.m_numNodes;
    this->m_bitList.assign(((numNodes * (numNodes - 1)) / 2), 0);
    this->m_numNodes    = numNodes;
    this->m_numEdges    = 0;
    this->m_indexFactor = (1 + 2 * (numNodes - 2));
  }

  template<class T>
  unsigned int HalfAdjacencyMatrix<T>::index(const unsigned int t_aNodeIndex, const unsigned int t_bNodeIndex)
  {
    if ((t_aNodeIndex < this->m_numNodes) && (t_bNodeIndex < this->m_numNodes) && (t_aNodeIndex != t_bNodeIndex))
      {
        long long aLongIndex = t_aNodeIndex;
        long long bLongIndex = t_bNodeIndex;
        if (t_aNodeIndex < t_bNodeIndex)
          {
            return (bLongIndex - aLongIndex * (aLongIndex - m_indexFactor) / 2) - 1;
          }
        else
          {
            return (aLongIndex - bLongIndex * (bLongIndex - m_indexFactor) / 2) - 1;
          }
      }
    else
      {
        badBehavior(__LINE__, __func__, "ERROR DUE TO OUR INDEX PICKS");
        return -1;
      }
  }

} // namespace TemplateGraph
#endif // end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_HALF_ADJACENCY_MATRIX_HPP
