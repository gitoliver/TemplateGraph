#include "./Algorithms/include/SubgraphMatching.hpp"
#include "./Algorithms/include/TotalCycleDecomposition.hpp"

#include "./GraphStructure/include/Edge.hpp"
#include "./GraphStructure/include/Node.hpp"
#include "./GraphStructure/include/Graph.hpp"

#include "./LazyPrints/LazyPrinters.hpp"

#include <map>
#include <set>
#include "Algorithms/include/ConnectivityIdentifier.hpp"

using namespace TemplateGraph;
class Atom: public Node<Atom>
{
public:

	inline Atom(std::string inName) :
		Node(inName, this),
		atomNodePtr_(std::shared_ptr<Atom>(this))
	{
	}

	inline ~Atom()
	{
		lazyInfo(__LINE__, __func__, "Destroying Node: " + this->getName() + "\n\tMem Addr: ");
		std::cout << this << "\n";
	}

	//copy constructor
	inline Atom(const Atom &rhs) :
		Node(*rhs.atomNodePtr_.get())
	{
		lazyInfo(__LINE__, __func__, "Calling atom copy constructor");
		this->atomNodePtr_ = std::shared_ptr<Atom>(this);
	}

	//move constructor
	inline Atom(Atom &&rhs)
	{
		lazyInfo(__LINE__, __func__, "Calling atom move constructor");
	}

	//copy assignment
	inline Atom& operator=(const Atom &rhs)
	{
		lazyInfo(__LINE__, __func__, "Calling atom copy assignment");
	}

	//move assignment
	inline Atom& operator=(Atom &&rhs)
	{
		lazyInfo(__LINE__, __func__, "Calling atom move assignment");
	}

	inline void addBond(Atom *otherAtom)
	{
		this->addNeighbor(this->getName() + " -> " + otherAtom->getName(),
				otherAtom->getSharedAtom());
	}
	inline void removeBond(Atom *otherAtom)
	{
		this->removeEdgeBetween(otherAtom->getSharedAtom());
	}
	inline std::shared_ptr<Node<Atom>> getSharedAtom()
	{
		return this->shared_from_this();
	}

	inline std::vector<Atom*> getBondedAtoms()
	{
		std::vector<Atom*> bondedAtomVec;
		for (Node<Atom> *currAtom : this->getRawNeighbors())
		{
			bondedAtomVec.push_back(currAtom->getObjectPtr());
		}
		return bondedAtomVec;
	}
private:
	std::shared_ptr<Node<Atom>> atomNodePtr_;
};
//endAtom

int main()
{
	Atom *atom0 = new Atom("Bobie");
	Atom *atom1 = new Atom("Steve");
	Atom *atom2 = new Atom("Ronne");
	Atom *atom3 = new Atom("Bingo");
	Atom *atom4 = new Atom("Marsh");
	Atom *atom5 = new Atom("Delux");
	Atom *atom6 = new Atom("Frank");
	Atom *atom7 = new Atom("Bingo1");
	Atom *atom8 = new Atom("Marsh1");
	Atom *atom9 = new Atom("Bridge1");
	Atom *atom10 = new Atom("cycle1");
	Atom *atom11 = new Atom("cycle2");
	Atom *atom12 = new Atom("cycle3");

	//test vector for our graph constructor overload using a vec of nodes
	std::vector<std::shared_ptr<Node<Atom>>> testGraphVec;

	/*testGraphVec.push_back(atom0->getSharedAtom());
	testGraphVec.push_back(atom1->getSharedAtom());
	testGraphVec.push_back(atom2->getSharedAtom());
	testGraphVec.push_back(atom3->getSharedAtom());
	testGraphVec.push_back(atom4->getSharedAtom());
	testGraphVec.push_back(atom5->getSharedAtom());
	testGraphVec.push_back(atom6->getSharedAtom());
	testGraphVec.push_back(atom7->getSharedAtom());
	testGraphVec.push_back(atom8->getSharedAtom());
	testGraphVec.push_back(atom9->getSharedAtom());
	testGraphVec.push_back(atom10->getSharedAtom());
	testGraphVec.push_back(atom11->getSharedAtom());
	testGraphVec.push_back(atom12->getSharedAtom());*/

	//to show our mega cycle decomp works
	//b 1 -> cyc 1
	//atom9->AddBond(atom10);
	atom9->addBond(atom10);
	//cyc 1 -> cyc 2
	atom10->addBond(atom11);
	//cyc 2 -> cyc 3
	atom11->addBond(atom12);
	//cyc 1 -> cyc 3
	atom10->addBond(atom12);
	//our mini cycle to the normal cycle
	atom10->addBond(atom1);

	atom0->addBond(atom1);
	atom1->addBond(atom2);
	atom2->addBond(atom3);
	atom3->addBond(atom4);
	atom4->addBond(atom5);
	atom1->addBond(atom6);
	atom5->addBond(atom6);
	atom2->addBond(atom5);
	atom2->addBond(atom6);
	atom5->addBond(atom3);
	atom2->addBond(atom7);
	atom7->addBond(atom8);
	atom6->addBond(atom3);
	atom6->addBond(atom4);



	Atom copyTester(*atom6);



	Graph<Atom> *g1 = new Graph<Atom>(atom0->getSharedAtom());
	//connectivityIdentifier::identifyConnectivity(*g1);
	lazyInfo(__LINE__, __func__,
			"Graph 1 grapviz link: \n\t" + g1->getGraphvizLink());

	Atom *atomA = new Atom("Ronne");
	Atom *atomB = new Atom("Bingo");
	Atom *atomC = new Atom("Marsh");
	Atom *atomD = new Atom("Delux");

	atomA->addBond(atomB);
	atomB->addBond(atomC);
	atomA->addBond(atomD);
	Graph<Atom> *g2 = new Graph<Atom>(atomA->getSharedAtom());
	connectivityIdentifier::identifyConnectivity(*g2);
	lazyInfo(__LINE__, __func__,
			"Graph 2 grapviz link: \n\t" + g2->getGraphvizLink());

	//Showing cycle decomp works
	std::vector<
			std::pair<std::unordered_set<TemplateGraph::Node<Atom>*>,
					std::unordered_set<TemplateGraph::Edge<Atom>*>>> g1Cycles =
			cycleDetector::totalCycleDetect(*g1);

	//Showing subgraph matching works
	std::unordered_map<TemplateGraph::Node<Atom>*,
			std::pair<std::vector<TemplateGraph::Node<Atom>*>,
					std::vector<TemplateGraph::Edge<Atom>*>>> g2MatchesIng1 =
			subgraphMatcher::findSubgraphs(*g1, *g2);

	lazyInfo(__LINE__, __func__, "Printing out all cycles of g1\n");
	int prettyCounter = 0;
	//for (Node<Atom> *lole : g1->getRawNodes())
	//{
	//std::cout << "Cycles containing node: " + lole->getName() + "\n";
	for (std::pair<std::unordered_set<TemplateGraph::Node<Atom>*>,
			std::unordered_set<TemplateGraph::Edge<Atom>*>> currCyclePair : g1Cycles)
	{
		//if (currCyclePair.first.count(lole) > 0)
		//{
		std::cout << "Cycle #" + std::to_string(prettyCounter) + "\n\tNodes: ";
		for (Node<Atom> *currAtom : currCyclePair.first)
		{
			std::cout << currAtom->getName() + ", ";
		}
		std::cout << "\n\tEdges: ";
		for (Edge<Atom> *currEdge : currCyclePair.second)
		{
			std::cout << currEdge->getName() + ", ";
		}
		std::cout << "\n";
		prettyCounter++;
		//}
	}
	prettyCounter = 0;
	std::cout << "\n\n";
	//}

	lazyInfo(__LINE__, __func__, "Printing out our nodes in g1 that match g2");

	for (std::pair<TemplateGraph::Node<Atom>*,
			std::pair<std::vector<TemplateGraph::Node<Atom>*>,
					std::vector<TemplateGraph::Edge<Atom>*>>> currMatchPair : g2MatchesIng1)
	{
		std::cout
				<< "Node Key: " + currMatchPair.first->getName()
						+ "\n\tNodes: ";
		for (Node<Atom> *currAtom : currMatchPair.second.first)
		{
			std::cout << currAtom->getName() + ", ";
		}
		std::cout << "\n\tEdges: ";
		for (Edge<Atom> *currEdge : currMatchPair.second.second)
		{
			std::cout << currEdge->getName() + ", ";
		}
		std::cout << "\n\n";
	}

//graph all cycles
//std::vector<std::unordered_set<Node<Atom>*>> cyclesG1 =
	/*		cycleDetector::totalCycleDetect(*g1);

	 //Print all our cycles
	 lazyInfo(__LINE__, __func__, "Printing out all of our g1 cycles");
	 int printerCounter = 0;
	 for (std::unordered_set<Node<Atom>*> currUnSet : cyclesG1)
	 {
	 std::cout
	 << "Printing nodes in cycle #" + std::to_string(printerCounter)
	 + "\n\t";
	 for (Node<Atom> *currAtom : currUnSet)
	 {
	 std::cout << currAtom->getName() + ", ";
	 }
	 std::cout << "\n";
	 printerCounter++;
	 }
	 printerCounter = 0;
	 std::cout << "\nCompleted printing out all our cycles \n\n";

	 delete atom12;

	 //Print all our cycles
	 lazyInfo(__LINE__, __func__,
	 "Printing out all of our g1 cycles after deleting cycle 3 node");
	 cyclesG1 = cycleDetector::totalCycleDetect(*g1);

	 printerCounter = 0;
	 for (std::unordered_set<Node<Atom>*> currUnSet : cyclesG1)
	 {
	 std::cout
	 << "Printing nodes in cycle #" + std::to_string(printerCounter)
	 + "\n\t";
	 for (Node<Atom> *currAtom : currUnSet)
	 {
	 std::cout << currAtom->getName() + ", ";
	 }
	 std::cout << "\n";
	 printerCounter++;
	 }
	 printerCounter = 0;
	 std::cout << "\nCompleted printing out all our cycles \n\n";
	 */
	/*
	 std::unordered_map<TemplateGraph::Node<Atom>*,
	 std::pair<std::vector<TemplateGraph::Node<Atom>*>,
	 std::vector<TemplateGraph::Edge<Atom>*>>> blarf =
	 subgraphMatcher::findSubgraphs(*g1, *g2);

	 lazyInfo(__LINE__, __func__,
	 "Printing out all of g1 subgraphs that match g2\n\n");

	 for (std::pair<TemplateGraph::Node<Atom>*,
	 std::pair<std::vector<TemplateGraph::Node<Atom>*>,
	 std::vector<TemplateGraph::Edge<Atom>*>>> currSubgraphPair : blarf)
	 {
	 std::cout
	 << "Matched subgraph using node "
	 + currSubgraphPair.first->getName() + "\n\tNodes: ";
	 for (Node<Atom>* currNode : currSubgraphPair.second.first)
	 {
	 std::cout << currNode->getName() + ", ";
	 }
	 std::cout <<"\n\tEdges: ";
	 for (Edge<Atom>* currEdge : currSubgraphPair.second.second)
	 {
	 std::cout << currEdge->getName() + ", ";
	 }
	 std::cout << "\n\n";
	 }
	 */
	/*for (std::pair<TemplateGraph::Node<Atom>*,
	 std::vector<TemplateGraph::Node<Atom>*>> currPair : matchedSubgraphs)
	 {
	 std::cout
	 << "Node <" + currPair.first->getName()
	 + "> is a part of cycle(s):\n\t";
	 for (TemplateGraph::Node<Atom> *currNode : currPair.second)
	 {
	 std::cout << currNode->getName() + ", ";
	 }
	 std::cout << "\n";
	 }
	 */
//Graph<Atom> queryGraph(atomA->GetNode());
//SubgraphMatcher<Atom> sumthin(&atomGraph, &queryGraph);
	/*std::cout << "Deleting " << atom6->GetName() << "\n";
	 delete atom6;

	 std::cout << "Deleting " << atom4->GetName() << "\n";
	 delete atom4;
	 //std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	 std::cout
	 << "Visualize the graph here:\nhttps://dreampuf.github.io/GraphvizOnline/#digraph%20G%20%7B%0ABobie-%3ESteve%0A%7D\n";
	 //CycleDetector<Atom> cycleDetector(atom0->GetNode());
	 //cycleDetector.DetectCyclesInDFSGraph();

	 std::cout << "Deleting " << atom5->GetName() << "\n";
	 delete atom5;
	 //  std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	 std::cout << "Deleting bonds: \n";
	 //atom1->RemoveBond(atom6);
	 //std::string neighMsg =
	 //		(atom1->GetNode().get()->isNeighbor(atom2->GetNode())) ?
	 //				"is Neighbor" : "is not neighbor";
	 atom1->RemoveBond(atom2);
	 */
//lazyInfo(__LINE__, __func__, neighMsg);
// atom3->RemoveBond(atom4);
//  std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
// std::cout << "Deleting atoms.\n";
// // delete atom3;
// delete atom4;
// delete atom5;
// delete atom6;
// std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	std::cout << "Finished atom section\n\n";

// Graph<Atom> queryAtomGraph(atomA->GetNode());
// std::cout << "queryGraph:\n" << queryAtomGraph.Print() << "\n\n";
// // std::vector<SubGraph<Atom>> foundSubGraphs = atomGraph.SubGraphMatch(queryAtomGraph);
//  std::cout << "Found these subgraphs:\n";
//  for (auto &subGraph : foundSubGraphs)
//  {
//      std::cout << "SubGraph:\n" << subGraph.Print() << "\n\n";
//  }

	std::cout << "********************************************\n";

// SubGraphMatcher<Atom> steveTheSubGraphFinder(atomGraph, queryAtomGraph);
// for (auto &subGraph : steveTheSubGraphFinder.GetMatches())
// {
//     std::cout << "Steve SubGraph:\n" << subGraph.Print() << "\n\n";
// }

// std::cout << "********************************************\n";

//     std::vector<Graph<int>> matchingSubGraphs = mainGraph.SubGraphMatch(queryGraph);
//     std::cout << "Found " << matchingSubGraphs.size() << " subgraphs that matched\n";

//     //atom1.atomNodePtr_ = std::make_shared<Node<Atom>>(&atom1);
// 	std::vector<std::shared_ptr<Node<int>> > vectorOfNodes;
// // 	Graph<int> intGraph1;
// 	std::vector<int> vect1(18);
// 	for (int i = 0; i < vect1.size(); ++i)
// 	{
// 		vect1.at(i) = i;
// 		vectorOfNodes.push_back(std::make_shared<Node<int>>(&(vect1.at(i))));
//     }
//     vectorOfNodes.at(1)->AddEdge(vectorOfNodes.at(2));
//     for (auto &neighbor : vectorOfNodes.at(2)->GetNeighbors())
//     {
//         std::cout << "Neighbor index: " << neighbor->GetIndex() << "\n";
//     }
//     for (auto &neighbor : vectorOfNodes.at(1)->GetNeighbors())
//     {
//         std::cout << "Other side Neighbor index: " << neighbor->GetIndex() << "\n";
//     }
//     vectorOfNodes.at(1)->RemoveEdge(vectorOfNodes.at(2));
//     for (auto &neighbor : vectorOfNodes.at(2)->GetNeighbors())
//     {
//         std::cout << "And now Neighbor index: " << neighbor->GetIndex() << "\n";
//     }
//     for (auto &neighbor : vectorOfNodes.at(1)->GetNeighbors())
//     {
//         std::cout << "And now Other side Neighbor index: " << neighbor->GetIndex() << "\n";
//     }

	std::cout << "Finishing!" << std::endl;

//     //BFS
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(16)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(17)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(15)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(7)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(17), vectorOfNodes.at(13)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(15), vectorOfNodes.at(14)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(7), vectorOfNodes.at(6)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(14), vectorOfNodes.at(13)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(11)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(12)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(8)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(10)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(12), vectorOfNodes.at(10)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(9)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(10)));

// //     DFS
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(16)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(7)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(7)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(8)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(9)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(8), vectorOfNodes.at(10)));
// 	intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(10), vectorOfNodes.at(11)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(10), vectorOfNodes.at(12)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(13)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(12), vectorOfNodes.at(13)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(14)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(14), vectorOfNodes.at(15)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(15), vectorOfNodes.at(16)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(13), vectorOfNodes.at(17)));
//     intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(16), vectorOfNodes.at(17)));

//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(2)
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(3)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(1), vectorOfNodes.at(7)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(3)));

//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(2), vectorOfNodes.at(4)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(4)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(5)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(6)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(3), vectorOfNodes.at(7)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(4), vectorOfNodes.at(5)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(5), vectorOfNodes.at(6)));
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(6), vectorOfNodes.at(7)));

//     // int count = 1;
//     // for (auto it1 = vectorOfNodes.begin(); it1 != (vectorOfNodes.end()-1); it1++)
//     // {
//     // 	auto it2 = (it1 + 1);
//     // 	intGraph1.AddEdge(new Edge<int>(*it1, *it2));
//     //  	if ((count % 6) == 0)
//     // 	{
//     // 		auto it3 = (it1 - 5);
//     // 		intGraph1.AddEdge(new Edge<int>(*it1, *it3));
//     // 	}
//     // 	count++;
//     // }
//     // intGraph1.AddEdge(new Edge<int>(vectorOfNodes.at(11), vectorOfNodes.at(5)));

//     for(auto &edge : intGraph1.GetEdges())
//     {
//     	//std::cout << "Edge: " << *(edge->GetSource()->GetObjectPtr()) << "-->" << *(edge->GetTarget()->GetObjectPtr()) << std::endl;
//  	    std::cout << "Edge: " << edge->GetSource()->GetIndex() << "-->" << edge->GetTarget()->GetIndex() << std::endl;
//     }
//     intGraph1.DetectCyclesInDFSGraph();

//     // Graphs of graphs

//     Graph<int> intGraph2;

//     Node<Graph<int>> graphNode1(&intGraph1);
//     Node<Graph<int>> graphNode2(&intGraph2);

//     Graph<Graph<int>> garyTheGraphGraph;
//     garyTheGraphGraph.AddEdge(new Edge<Graph<int>>(&graphNode1, &graphNode2));

//     // Subgraph matching tNULLest
//     int intStance = 1, intStance2 = 24, intStance3 = 999;
//     Graph<int> mainGraph, queryGraph;
//     Node<int> mana(&intStance, "Man");
//     Node<int> gala(&intStance2, "Gal");
//     Node<int> man(&intStance, "Man");
//     Node<int> gal(&intStance2, "Gal");
//     Node<int> glc(&intStance3, "Glc");
//     Node<int> roh(&intStance3, "ROH");

//     Node<int> man1(&intStance, "Man");
//     Node<int> gal2(&intStance2, "Gal");
//     //intNode.SetObjectPtr(&intStance);
//     // int *test = intNode.GetObjectPtr();
//     // std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;
//     // intStance = 2;
//     // std::cout << "Test is " << *test << " while intStance is " << intStance << std::endl;

//     Edge<int> edge1(&man, &gal);
//     Edge<int> edge2(&glc, &gal);
//     Edge<int> edge3(&mana, &gala);
//     Edge<int> edge4(&gala, &man);
//     Edge<int> edge5(&gal, &rohNULL);
//     edge1.AddLabel("1-4");
//     edge2.AddLabel("1-3");
//     edge3.AddLabel("1-4");
//     edge4.AddLabel("1-2");
//     edge5.AddLabel("1-");
//     mainGraph.AddEdge(&edge1);
//     mainGraph.AddEdge(&edge2);
//     mainGraph.AddEdge(&edge3);
//     mainGraph.AddEdge(&edge4);
//     mainGraph.AddEdge(&edge5);

//     Edge<int> edge9(&man1, &gal2);
//     edge9.AddLabel("1-4");
//     queryGraph.AddEdge(&edge9);

//     std::vector<Graph<int>> matchingSubGraphs = mainGraph.SubGraphMatch(queryGraph);
//     std::cout << "Found " << matchingSubGraphs.size() << " subgraphs that matched\n";

//     // Testing deletion of Nodes or edges:

//     for (auto &element : vectorOfNodes)
//     {
//         delete element;
//     }
//     //mana.~Node();
//     std::cout << "\n\nNODE HAS BEEN DELETED\n\n" << std::endl;

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

// bool compareLabel(TemplateGraph::Edge<int> &edge, TemplateGraph::Edge<int> &otherEdge) {
//     return (edge.GetLabel() == otherEdge.GetLabel());
// }

