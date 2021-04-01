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
	HalfAdjMatrix(std::vector<Node<T>*> nList);
	HalfAdjMatrix(unsigned int nNodes);
	///because for some reason the constructor is being annoying
	void initializeWorkaround(const HalfAdjMatrix<T> &m);

	void initializeWorkaround(std::vector<Node<T>*> nList);
	//copy constructor
	HalfAdjMatrix(const HalfAdjMatrix<T> &m);

	inline int getNumEdges() const
	{
		return numEdges;
	}

	void connect(unsigned int a, unsigned int b);
	void disconnect(unsigned int a, unsigned int b);
	bool isConnected(unsigned int a, unsigned int b);

	//to validate matricies when doing cycle stuff
	bool validateCycleMatrix(HalfAdjMatrix<T> &m);

	//operator overloads
	inline bool operator()(int a, int b)
	{
		return isConnected(a, b);
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
			for (unsigned int i = 0; i < bitList.size(); i++)
			{
				if ((bitList[i] || rhs.bitList[i])
						&& (bitList[i] != rhs.bitList[i]))
				{
					result.bitList[i] = 1;
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
		for (unsigned int i = 0; i < bitList.size(); i++)
		{
			if ((bitList[i] || rhs.bitList[i])
					&& (bitList[i] != rhs.bitList[i]))
			{
				bitList[i] = 1;
				numEdges++;
			}
			else
				bitList[i] = 0;
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
	int index(const int a, const int b);
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
HalfAdjMatrix<T>::HalfAdjMatrix(std::vector<Node<T>*> nList)
{
	unsigned int nNodes = nList.size();
	this->bitList.assign(((nNodes * (nNodes - 1)) / 2), 0);
	this->numNodes = nNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (nNodes - 2));
}

template<class T>
HalfAdjMatrix<T>::HalfAdjMatrix(unsigned int nNodes)
{
	this->bitList.assign(((nNodes * (nNodes - 1)) / 2), 0);
	this->numNodes = nNodes;
	this->numEdges = 0;
	this->indexFactor = (1 + 2 * (nNodes - 2));
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
void HalfAdjMatrix<T>::initializeWorkaround(std::vector<Node<T>*> nList)
{
	unsigned int nNodes = nList.size();
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
void HalfAdjMatrix<T>::connect(unsigned int a, unsigned int b)
{
	if (bitList[index(a, b)])
	{
		std::cout << "Trying to add a connection that was already there\n";
	}
	else
	{
		bitList[index(a, b)] = true;
		numEdges++;
	}
}

template<class T>
void HalfAdjMatrix<T>::disconnect(unsigned int a, unsigned int b)
{
	if (!bitList[index(a, b)])
	{
		std::cout << "Trying to remove a connection that was not there\n";
	}
	else
	{
		bitList[index(a, b)] = false;
		numEdges--;
	}
}
template<class T>
int HalfAdjMatrix<T>::index(const int a, const int b)
{
	if ((a < numNodes) && (b < numNodes) && (a != b))
	{
		long long la = a, lb = b;
		if (a < b)
			return (lb - la * (la - indexFactor) / 2) - 1;
		else
			return (la - lb * (lb - indexFactor) / 2) - 1;
	}
	else
	{
		std::cout << "ERROR due to index picks\n";
		return -1;
	}
}

template<class T>
bool HalfAdjMatrix<T>::isConnected(unsigned int a, unsigned int b)
{
	if (a == b)
		return false;
	return bitList[index(a, b)];
}
//to validate a cycle matrix, wanted to hide this away into this class.
template<class T>
bool HalfAdjMatrix<T>::validateCycleMatrix(HalfAdjMatrix<T> &m)
{
	int pathL = 0;
	for (unsigned int i = 0; i < numNodes; i++)
	{
		for (unsigned int j = 0; j < numNodes; j++)
		{
			if (m.isConnected(j, i))
			{
				++pathL;
				std::set<int> isVisited;
				isVisited.insert(i);
				validateCycleMatrixRec(m, pathL, j, i, isVisited);
				return (pathL + 1) == m.getNumEdges();
			}
		}
	}
	std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: NO EDGES\n\n";
}
template<class T>
void HalfAdjMatrix<T>::validateCycleMatrixRec(HalfAdjMatrix<T> &m, int &pathL,
		const int i, int prevNode, std::set<int> &aVisited)
{
	if (pathL > 750)
	{
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: TOO LONG PATH\n\n";
	}
	else
	{
		for (unsigned int j = 0; j < numNodes; j++)
		{
			if (m.isConnected(i, j) && j != prevNode)
			{
				auto ppVisited = aVisited.find(j);
				if (ppVisited != aVisited.end())
				{
					return;
				}
				++pathL;
				aVisited.insert(i);
				validateCycleMatrixRec(m, pathL, j, i, aVisited);
				return;
			}
		}
		std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~WARNING: DEAD END\n\n";
	}
}

}
#endif // HALF_ADJ_MATRIX_HPP
