#ifndef HALF_ADJACENCY_MATRIX_HPP
#define HALF_ADJACENCY_MATRIX_HPP

#include <vector>

#include "../../LazyPrints/LazyPrinters.hpp"

namespace TemplateGraph
{
//TODO:Remove forward declare, why throwing errors?
template<class T> class Node;

template<class T>
class HalfAdjacencyMatrix
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	HalfAdjacencyMatrix();
	HalfAdjacencyMatrix(std::vector<Node<T>*> nodeList);
	//copy constructor
	HalfAdjacencyMatrix(const HalfAdjacencyMatrix<T> &m);

	~HalfAdjacencyMatrix(){};

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	unsigned int getNumEdges();

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	void connect(unsigned int aNodeIndex, unsigned int bNodeIndex);
	void disconnect(unsigned int aNodeIndex, unsigned int bNodeIndex);

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool isConnected(unsigned int aNodeIndex, unsigned int bNodeIndex);

	//TODO: Actually fix our constructors so we do not have to use this initialize workaround stuff
	void initializeWorkaround(const HalfAdjacencyMatrix<T> &m);
	void initializeWorkaround(std::vector<Node<T>*> nodeList);

	/************************************************
	 *  OPERATOR OVERLOADS
	 ***********************************************/
	inline bool operator()(unsigned int nodeAIndex, unsigned int nodeBIndex)
	{
		return isConnected(nodeAIndex, nodeBIndex);
	}

	//our XOR overload
	inline HalfAdjacencyMatrix<T> operator^(
			const HalfAdjacencyMatrix<T> &rhs) const
	{
		HalfAdjacencyMatrix<T> result(numNodes);
		if (this->numNodes != rhs.numNodes)
		{
			badBehavior(__LINE__, __func__,
					"WARNING NUMBER OF NODES NOT EQUAL XOR");
			return result;
		}
		else
		{
			for (unsigned int bitListIndex = 0; bitListIndex < bitList.size();
					bitListIndex++)
			{
				if ((bitList[bitListIndex] || rhs.bitList[bitListIndex])
						&& (bitList[bitListIndex] != rhs.bitList[bitListIndex]))
				{
					result.bitList[bitListIndex] = 1;
					++result.numEdges;
				}
			}
			return result;
		}
	}

	//our XOR-EQUALS overload
	inline HalfAdjacencyMatrix<T>& operator^=(const HalfAdjacencyMatrix<T> &rhs)
	{
		if (this->numNodes != rhs.numNodes)
		{
			badBehavior(__LINE__, __func__,
					"WARNING NUMBER OF NODES NOT EQUAL XOREQUALS");
		}
		numEdges = 0;
		for (unsigned int bitListIndex = 0; bitListIndex < bitList.size();
				bitListIndex++)
		{
			if ((bitList[bitListIndex] || rhs.bitList[bitListIndex])
					&& (bitList[bitListIndex] != rhs.bitList[bitListIndex]))
			{
				bitList[bitListIndex] = 1;
				numEdges++;
			}
			else
				bitList[bitListIndex] = 0;
		}
		return *this;
	}

	//our equals operator, well assignment equals
	inline HalfAdjacencyMatrix<T>& operator=(const HalfAdjacencyMatrix<T> &rhs)
	{
		if (this->numNodes != rhs.numNodes)
		{
			badBehavior(__LINE__, __func__,
					"WARNING NUMBER OF NODES NOT EQUAL EQUAL");
		}
		this->bitList = rhs.bitList;
		this->numEdges = rhs.numEdges;
		return *this;
	}

private:
	/* Allows for proper index lookup. We use a half adjacency matrix because
	 * 	our data is mirrored on the diagonal and we would like to save space
	 */
	unsigned int index(const unsigned int aNodeIndex,
			const unsigned int bNodeIndex);
	//Our actual connections/Adj matrix
	std::vector<bool> bitList;
	unsigned int numEdges;
	unsigned int numNodes;
	long long indexFactor;
};

template<class T>
HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix()
{
	badBehavior(__LINE__, __func__, "Warning default constructor called");
	this->numEdges = 0;
	this->numNodes = 0;
	this->indexFactor = 0;
}

template<class T>
HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix(std::vector<Node<T>*> nodeList)
{

	unsigned int numNodes = nodeList.size();
	this->bitList.assign(((numNodes * (numNodes - 1)) / 2), 0);
	this->numNodes = numNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (numNodes - 2));
}

template<class T>
HalfAdjacencyMatrix<T>::HalfAdjacencyMatrix(const HalfAdjacencyMatrix<T> &m)
{
	this->bitList = m.bitList;
	this->numEdges = m.numEdges;
	this->numNodes = m.numNodes;
	this->indexFactor = m.indexFactor;
}

template<class T>
void HalfAdjacencyMatrix<T>::initializeWorkaround(
		const HalfAdjacencyMatrix<T> &m)
{
	this->bitList = m.bitList;
	this->numEdges = m.numEdges;
	this->numNodes = m.numNodes;
	this->indexFactor = m.indexFactor;
}

template<class T>
void HalfAdjacencyMatrix<T>::initializeWorkaround(
		std::vector<Node<T>*> nodeList)
{
	unsigned int numNodes = nodeList.size();
	this->bitList.assign(((numNodes * (numNodes - 1)) / 2), 0);
	this->numNodes = numNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (numNodes - 2));
}

template<class T>
unsigned int HalfAdjacencyMatrix<T>::getNumEdges()
{
	return this->numEdges;
}

template<class T>
void HalfAdjacencyMatrix<T>::connect(unsigned int aNodeIndex,
		unsigned int bNodeIndex)
{
	if (this->bitList[index(aNodeIndex, bNodeIndex)])
	{
		badBehavior(__LINE__, __func__,
				"TRYING TO ADD A CONNECTION THAT WAS ALREADY THERE");
	}
	else
	{
		this->bitList[index(aNodeIndex, bNodeIndex)] = true;
		this->numEdges++;
	}
}

template<class T>
void HalfAdjacencyMatrix<T>::disconnect(unsigned int aNodeIndex,
		unsigned int bNodeIndex)
{
	if (!(this->bitList[index(aNodeIndex, bNodeIndex)]))
	{
		badBehavior(__LINE__, __func__,
				"TRYING TO REMOVE A CONNECTION THAT WAS NOT THERE");
	}
	else
	{
		this->bitList[index(aNodeIndex, bNodeIndex)] = false;
		this->numEdges--;
	}
}

template<class T>
bool HalfAdjacencyMatrix<T>::isConnected(unsigned int aNodeIndex,
		unsigned int bNodeIndex)
{
	if (aNodeIndex == bNodeIndex)
		return false;
	return this->bitList[index(aNodeIndex, bNodeIndex)];
}

template<class T>
unsigned int HalfAdjacencyMatrix<T>::index(const unsigned int aNodeIndex,
		const unsigned int bNodeIndex)
{
	if ((aNodeIndex < this->numNodes) && (bNodeIndex < this->numNodes)
			&& (aNodeIndex != bNodeIndex))
	{
		long long aLongIndex = aNodeIndex;
		long long bLongIndex = bNodeIndex;
		if (aNodeIndex < bNodeIndex)
		{
			return (bLongIndex - aLongIndex * (aLongIndex - indexFactor) / 2)
					- 1;
		}
		else
		{
			return (aLongIndex - bLongIndex * (bLongIndex - indexFactor) / 2)
					- 1;
		}
	}
	else
	{
		badBehavior(__LINE__, __func__, "ERROR DUE TO OUR INDEX PICKS");
		return -1;
	}
}

} //end template graph namespace
#endif //HALF_ADJACENCY_MATRIX_HPP
