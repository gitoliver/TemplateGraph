#ifndef HALF_ADJ_MATRIX_HPP
#define HALF_ADJ_MATRIX_HPP

#include <vector>

//to remove
#include <iostream>
#include <set>

namespace TemplateGraph
{
template<class T>
class HalfAdjMatrix
{
public:
	HalfAdjMatrix();
	HalfAdjMatrix(std::vector<Node<T>*> nodeList);
	HalfAdjMatrix(unsigned int numberNodes);
	///because for some reason the constructor is being annoying
	void initializeWorkaround(const HalfAdjMatrix<T> &m);

	void initializeWorkaround(std::vector<Node<T>*> nodeList);
	//copy constructor
	HalfAdjMatrix(const HalfAdjMatrix<T> &m);

	inline int getNumEdges() const
	{
		return numEdges;
	}

	void connect(unsigned int nodeAIndex, unsigned int nodeBIndex);
	void disconnect(unsigned int nodeAIndex, unsigned int nodeBIndex);
	bool isConnected(unsigned int nodeAIndex, unsigned int nodeBIndex);

	//to validate matricies when doing cycle stuff
	bool validateCycleMatrix(HalfAdjMatrix<T> &m);

	//operator overloads
	inline bool operator()(unsigned int nodeAIndex, unsigned int nodeBIndex)
	{
		return isConnected(nodeAIndex, nodeBIndex);
	}

	inline HalfAdjMatrix<T> operator^(const HalfAdjMatrix<T> &rhs) const
	{
		HalfAdjMatrix<T> result(numNodes);
		if (this->numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL XOR \n~~~~~~~~~~~~~\n\n";
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

	inline HalfAdjMatrix<T>& operator^=(const HalfAdjMatrix<T> &rhs)
	{
		if (this->numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL XOREQUALS\n~~~~~~~~~~~~~\n\n";
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

	inline HalfAdjMatrix<T>& operator=(const HalfAdjMatrix<T> &rhs)
	{
		if (numNodes != rhs.numNodes)
		{
			std::cout
					<< "\n\n~~~~~~~\nWARNING NUMBER OF NODES NOT EQUAL EQUAL\n~~~~~~~~~~~~~\n\n";
		}
		this->bitList = rhs.bitList;
		this->numEdges = rhs.numEdges;
		return *this;
	}

private:
	//proper index lookup since we are dealing with a half adj matrix
	int index(const unsigned int nodeAIndex, const unsigned int nodeBIndex);
	//actual connections present within the adj matrix
	std::vector<bool> bitList;
	//may need to worry regarding our systems bits 64 or 32
	unsigned int numEdges;
	//same as above
	unsigned int numNodes;
	long long indexFactor;

	//validationCheck recursion for cycle matrix
	void validateCycleMatrixRec(HalfAdjMatrix<T> &m, int &pathL, const int i,
			int previousNode, std::set<int> &aVisited);
};

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix()
{
	this->numEdges = 0;
	this->numNodes = 0;
	this->indexFactor = 0;
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(std::vector<Node<T>*> nodeList)
{
	unsigned int nNodes = nodeList.size();
	this->bitList.assign(((nNodes * (nNodes - 1)) / 2), 0);
	this->numNodes = nNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (nNodes - 2));
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(unsigned int numberNodes)
{
	this->bitList.assign(((numberNodes * (numberNodes - 1)) / 2), 0);
	this->numNodes = numberNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (numberNodes - 2));
}

//because constructors are being annoying. Wanted to avoid
template<class T>
void HalfAdjMatrix<T>::initializeWorkaround(const HalfAdjMatrix<T> &m)
{
	this->bitList = m.bitList;
	this->numEdges = m.numEdges;
	this->numNodes = m.numNodes;
	this->indexFactor = m.indexFactor;
}

template<class T>
void HalfAdjMatrix<T>::initializeWorkaround(std::vector<Node<T>*> nodeList)
{
	unsigned int nNodes = nodeList.size();
	this->bitList.assign(((nNodes * (nNodes - 1)) / 2), 0);
	this->numNodes = nNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (nNodes - 2));
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(const HalfAdjMatrix<T> &m)
{
	this->bitList = m.bitList;
	this->numEdges = m.numEdges;
	this->numNodes = m.numNodes;
	this->indexFactor = m.indexFactor;
}

template<class T>
void HalfAdjMatrix<T>::connect(unsigned int nodeAIndex, unsigned int nodeBIndex)
{
	if (bitList[index(nodeAIndex, nodeBIndex)])
	{
		std::cout << "Trying to add a connection that was already there\n";
	}
	else
	{
		bitList[index(nodeAIndex, nodeBIndex)] = true;
		numEdges++;
	}
}

template<class T>
void HalfAdjMatrix<T>::disconnect(unsigned int nodeAIndex,
		unsigned int nodeBIndex)
{
	if (!bitList[index(nodeAIndex, nodeBIndex)])
	{
		std::cout << "Trying to remove a connection that was not there\n";
	}
	else
	{
		bitList[index(nodeAIndex, nodeBIndex)] = false;
		numEdges--;
	}
}

template<class T>
bool HalfAdjMatrix<T>::isConnected(unsigned int nodeAIndex,
		unsigned int nodeBIndex)
{
	if (nodeAIndex == nodeBIndex)
		return false;
	return bitList[index(nodeAIndex, nodeBIndex)];
}

template<class T>
int HalfAdjMatrix<T>::index(const unsigned int nodeAIndex,
		const unsigned int nodeBIndex)
{
	if ((nodeAIndex < numNodes) && (nodeBIndex < numNodes)
			&& (nodeAIndex != nodeBIndex))
	{
		//have to convert to longs so we can do our math
		long long aLongIndex = nodeAIndex, bLongIndex = nodeBIndex;
		if (nodeAIndex < nodeBIndex)
			return (bLongIndex - aLongIndex * (aLongIndex - indexFactor) / 2)
					- 1;
		else
			return (aLongIndex - bLongIndex * (bLongIndex - indexFactor) / 2)
					- 1;
	}
	else
	{
		std::cout << "ERROR due to index picks\n";
		return -1;
	}
}

//to validate a cycle matrix, wanted to hide this away into this class.
template<class T>
bool HalfAdjMatrix<T>::validateCycleMatrix(HalfAdjMatrix<T> &matrixToValidate)
{
	int pathLength = 0;
	for (unsigned int nodeAIndex = 0; nodeAIndex < numNodes; nodeAIndex++)
	{
		for (unsigned int nodeBIndex = 0; nodeBIndex < numNodes; nodeBIndex++)
		{
			if (matrixToValidate.isConnected(nodeBIndex, nodeAIndex))
			{
				++pathLength;
				std::set<int> isVisited;
				isVisited.insert(nodeAIndex);
				validateCycleMatrixRec(matrixToValidate, pathLength, nodeBIndex,
						nodeAIndex, isVisited);
				//the +1 is for our connecting edge that hits back to our original node
				return (pathLength + 1) == matrixToValidate.getNumEdges();
			}
		}
	}
	std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: NO EDGES\n\n";
	return false;
}

template<class T>
void HalfAdjMatrix<T>::validateCycleMatrixRec(
		HalfAdjMatrix<T> &matrixToValidate, int &currentPathLength,
		const int interestingNode, int prevNodeIndex, std::set<int> &aVisited)
{
	if (currentPathLength > 750)
	{
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: TOO LONG PATH\n\n";
	}
	else
	{
		//next connecting to the node we find interesting
		for (unsigned int possiblyInterestingNodeIndex = 0;
				possiblyInterestingNodeIndex < numNodes;
				possiblyInterestingNodeIndex++)
		{
			if (matrixToValidate.isConnected(interestingNode,
					possiblyInterestingNodeIndex)
					&& possiblyInterestingNodeIndex != prevNodeIndex)
			{
				//check if visited
				auto possiblyVisited = aVisited.find(
						possiblyInterestingNodeIndex);
				if (possiblyVisited != aVisited.end())
				{
					return;
				}
				++currentPathLength;
				aVisited.insert(interestingNode);
				validateCycleMatrixRec(matrixToValidate, currentPathLength,
						possiblyInterestingNodeIndex, interestingNode,
						aVisited);
				return;
			}
		}
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: DEAD END\n\n";
	}
}

}
#endif // HALF_ADJ_MATRIX_HPP
