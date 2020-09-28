#include "../includes/Node.hpp"
#include "../includes/Edge.hpp"
#include "../includes/Graph.hpp"



int main ()
{
	using namespace TemplateGraph;
	
	std::vector<Node<int>* > vectorOfNodes;
	Graph<int> intGraph1;
	std::vector<int> vect1(18);
	for (int i = 0; i < vect1.size(); ++i)
	{
		vect1.at(i) = i;
		vectorOfNodes.push_back(new Node<int>(&(vect1.at(i))));
    }

    //BFS
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(16)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(17)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(15)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(7)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(17), vectorOfNodes.at(13)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(15), vectorOfNodes.at(14)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(7), vectorOfNodes.at(6)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(14), vectorOfNodes.at(13)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(11)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(12)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(8)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(10)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(12), vectorOfNodes.at(10)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(9)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(10)));


//     DFS
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(16)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(7)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(7)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(8)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(9)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(10)));
	intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(10), vectorOfNodes.at(11)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(10), vectorOfNodes.at(12)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(13)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(12), vectorOfNodes.at(13)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(14)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(14), vectorOfNodes.at(15)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(15), vectorOfNodes.at(16)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(17)));
    intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(17)));

    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(3)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(7)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));

    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(4)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(5)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(6)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(7)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(7)));

    // int count = 1;
    // for (auto it1 = vectorOfNodes.begin(); it1 != (vectorOfNodes.end()-1); it1++)
    // {
    // 	auto it2 = (it1 + 1);
    // 	intGraph1.AddEdge(new Edge<int>(*it1, *it2));
    //  	if ((count % 6) == 0) 
    // 	{
    // 		auto it3 = (it1 - 5);
    // 		intGraph1.AddEdge(new Edge<int>(*it1, *it3));
    // 	}
    // 	count++;
    // }
    // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(5)));

    for(auto &edge : intGraph1.GetEdges())
    {
    	//std::cout << "Edge: " << *(edge->GetSource()->GetObjectPtr()) << "-->" << *(edge->GetTarget()->GetObjectPtr()) << std::endl;
 	    std::cout << "Edge: " << edge->GetSource()->GetIndex() << "-->" << edge->GetTarget()->GetIndex() << std::endl;
    }
    intGraph1.DetectCyclesInDFSGraph();



    // Graphs of graphs

    Graph<int> intGraph2;

    Node<Graph<int>> graphNode1(&intGraph1);
    Node<Graph<int>> graphNode2(&intGraph2);

    Graph<Graph<int>> garyTheGraphGraph;
    garyTheGraphGraph.AddEdge(new Edge<Graph<int>>(&graphNode1, &graphNode2));


    // Subgraph matching test
    int intStance = 1, intStance2 = 24, intStance3 = 999;
    Graph<int> mainGraph, queryGraph;
    Node<int> mana(&intStance, "Man");
    Node<int> gala(&intStance2, "Gal");
    Node<int> man(&intStance, "Man");
    Node<int> gal(&intStance2, "Gal");
    Node<int> glc(&intStance3, "Glc");
    Node<int> roh(&intStance3, "ROH");

    Node<int> man1(&intStance, "Man");
    Node<int> gal2(&intStance2, "Gal");
    //intNode.SetObjectPtr(&intStance);
    // int *test = intNode.GetObjectPtr();
    // std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;
    // intStance = 2;
    // std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;

    Edge<int> edge1(&man, &gal);
    Edge<int> edge2(&glc, &gal);
    Edge<int> edge3(&mana, &gala);
    Edge<int> edge4(&gala, &man);
    Edge<int> edge5(&gal, &roh);
    edge1.AddLabel("1-4");
    edge2.AddLabel("1-3");
    edge3.AddLabel("1-4");
    edge4.AddLabel("1-2");
    edge5.AddLabel("1-");
    mainGraph.AddEdge(&edge1);
    mainGraph.AddEdge(&edge2);
    mainGraph.AddEdge(&edge3);
    mainGraph.AddEdge(&edge4);
    mainGraph.AddEdge(&edge5);

    Edge<int> edge9(&man1, &gal2);
    edge9.AddLabel("1-4");
    queryGraph.AddEdge(&edge9);

    std::vector<Graph<int>> matchingSubGraphs = mainGraph.SubGraphMatch(queryGraph);
    std::cout << "Found " << matchingSubGraphs.size() << " subgraphs that matched\n";

    // int *test2 = edgier.GetTarget()->GetObjectPtr();

    // std::cout << "Test2 is " << *test2 << std::endl;

    // std::vector<Node<int>*> neighbors;
    // neighbors.push_back(edgier.GetTarget());
    // neighbors = intNode.GetNeighbors();

   // std::vector<int*> allDaNeighborObjects = intNode.GetNodesNeighborsObjects();
 //    std::cout << "Neighbors are: ";
 //    for (auto & element : allDaNeighborObjects)
 //    {
 //     std::cout << *element << ", ";
 //    }
 //    std::cout << std::endl;
 //    std::vector<std::shared_ptr<int*>> steve = intNode.GetNodesNeighborsObjects();
 //    std::vector<std::shared_ptr<TemplateGraph::Node<int>>> steve = intNode.GetNodesNeighbors();

 //    Graph<Graph<Atom>> residueGraph;

	// Edge<int> edgeA(Node<int> intNode1(1), &intNoder);

	// std::vector<Edge<int>* > vectorOfEdges = {&edger, &edgier};
	// Graph<int> theBestGraph(vectorOfEdges);
	
	return 0;
}

bool compareLabel(TemplateGraph::Edge<int> &edge, TemplateGraph::Edge<int> &otherEdge) {
    return (edge.GetLabel() == otherEdge.GetLabel());
}
