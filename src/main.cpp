#include "../includes/Node.hpp"
#include "../includes/Edge.hpp"
#include "../includes/Graph.hpp"

int main ()
{
	using namespace TemplateGraph;
	int intStance = 1, intStance2 = 24, intStance3 = 999;
	Node<int> intNode(&intStance);
	Node<int> intNoder(&intStance2);
	Node<int> intNodest(&intStance3);
	//intNode.SetObjectPtr(&intStance);
	int *test = intNode.GetObjectPtr();
	std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;
	intStance = 2;
	std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;

	Edge<int> edger(&intNode, &intNoder);
	Edge<int> edgier(&intNode, &intNodest);
	
	int *test2 = edgier.GetTarget()->GetObjectPtr();

	std::cout << "Test2 is " << *test2 << std::endl;

	std::vector<Node<int>*> neighbors;
	neighbors.push_back(edgier.GetTarget());
	neighbors = intNode.GetNeighbors();

	std::vector<int*> allDaNeighborObjects = intNode.GetNodesNeighborsObjects();
	std::cout << "Neighbors are: ";
	for (auto & element : allDaNeighborObjects)
	{
		std::cout << *element << ", ";
	}
	std::cout << std::endl;
	//std::vector<std::shared_ptr<int*>> steve = intNode.GetNodesNeighborsObjects();
	//std::vector<std::shared_ptr<TemplateGraph::Node<int>>> steve = intNode.GetNodesNeighbors();

	std::vector<Node<int>* > vectorOfNodes;
	Graph<int> intGraph;
	std::vector<int> vect1(100);
	for (int i = 0; i < vect1.size(); ++i)
	{
		vect1.at(i) = i;
		vectorOfNodes.push_back(new Node<int>(&(vect1.at(i))));
    }

    int count = 1;
    for (auto it1 = vectorOfNodes.begin(); it1 != (vectorOfNodes.end()-1); it1++)
    {
    	auto it2 = (it1 + 1);
    	intGraph.AddEdge(new Edge<int>(*it1, *it2));
    	if ((count % 6) == 0) 
    	{
    		auto it3 = (it1 - 5);
    		intGraph.AddEdge(new Edge<int>(*it1, *it3));
    	}
    	count++;
    }

    for(auto &edge : intGraph.GetEdges())
    {
    	//std::cout << "Edge: " << *(edge->GetSource()->GetObjectPtr()) << "-->" << *(edge->GetTarget()->GetObjectPtr()) << std::endl;
 	    std::cout << "Edge: " << edge->GetSource()->GetIndex() << "-->" << edge->GetTarget()->GetIndex() << std::endl;

    }

    if (intGraph.DetectCycles())
    {
    	std::cout << "Cyclic!" << std::endl;
    }
	// Edge<int> edgeA(Node<int> intNode1(1), &intNoder);

	// std::vector<Edge<int>* > vectorOfEdges = {&edger, &edgier};
	// Graph<int> theBestGraph(vectorOfEdges);
	
	return 0;
}