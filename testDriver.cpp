#include "includes/Node.hpp"
#include "includes/Edge.hpp"
#include "includes/Graph.hpp"
#include "includes/Algorithms/CycleDetector/CycleDetector.hpp"
//#include "../includes/Algorithms/SubGraphMatching/SubGraphMatcher.hpp"

using namespace TemplateGraph;
class Atom
{
public:
	Atom(std::string name) :
			name_(name)
	{
		atomNodePtr_ = std::make_shared<Node<Atom>>(this, name);
	} // default node label is atom name
	inline void AddBond(Atom *otherAtom)
	{
		this->GetNode()->AddEdge(otherAtom->GetNode(),
				this->GetName() + "->" + otherAtom->GetName());
	}
	inline void RemoveBond(Atom *otherAtom)
	{
		this->GetNode()->RemoveEdge(otherAtom->GetNode());
	}
	inline std::vector<Atom*> GetNeighbors()
	{
		return this->GetNode()->GetNodesNeighborsObjects();
	}
	inline std::string GetName()
	{
		return name_;
	}

	inline Node<Atom>* GetNode()
	{
		return atomNodePtr_.get();
	}
private:
	//friend std::shared_ptr<Node<Atom>> Graph<Atom>::GetNodeGraph(const Atom& atom);
	//inline std::shared_ptr<Node<Atom>> GetNode() {return atomNodePtr_;}
	std::shared_ptr<Node<Atom>> atomNodePtr_;
	std::string name_;
};
//endAtom

class Molecule
{
public:

	//Atom(std::string name) :
	//	name_(name) {
	//	atomNodePtr_ = std::make_shared<Node<Atom>>(this, name);
	//} // default node label is atom name

	Molecule(std::string name, Atom *atomRoot)
	{

		//Graph<Atom>(atomRoot->GetNode());
		this->name = name;
		this->graphPtr = NULL;
		std::cout << "ATOM ROOTS NODE: " << atomRoot->GetNode() << "\n\n";
		this->graphLol = Graph<Atom>(atomRoot->GetNode());
		//std::cout << "our temp graph: " << &tGraph << "\n";
		//std::cout << "our temp graph rootnode: " << tGraph.PoobGetRoot() << "\n";
		this->graphPtr = &this->graphLol;

	}

	~Molecule()
	{
		std::cout << "Molly destructor" << "\n\n";
	}

	Graph<Atom>* getGraphPtr()
	{
		return this->graphPtr;
	}

private:
	std::string name;
	Graph<Atom> *graphPtr;
	Graph<Atom> graphLol; //prevent nuking early
};
//endMolecule

int main()
{
	Atom *atom0 = new Atom("Bobie");
	//Molecule *mol1 = new Molecule("mol1", atom0);
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

	//to show our mega cycle decomp works
	//b 1 -> cyc 1
	atom9->AddBond(atom10);
	//cyc 1 -> cyc 2
	atom10->AddBond(atom11);
	//cyc 2 -> cyc 3
	atom11->AddBond(atom12);
	//cyc 1 -> cyc 3
	atom10->AddBond(atom12);
	//our mini cycle to the normal cycle
	atom10->AddBond(atom1);

	atom0->AddBond(atom1);
	atom1->AddBond(atom2);
	atom2->AddBond(atom3);
	atom3->AddBond(atom4);
	atom4->AddBond(atom5);
	atom1->AddBond(atom6);
	atom5->AddBond(atom6);
	atom2->AddBond(atom5);
	atom2->AddBond(atom6);
	atom5->AddBond(atom3);
	atom2->AddBond(atom7);
	atom7->AddBond(atom8);

	//std::cout <<"\n\nGraph ptr in mol: " << mol1->getGraphPtr() << " okay \n\n";
	//std::cout <<"\n\nOur supposed roots node that it owns: " << atom0->GetNode() << " okay \n\n";
	//std::cout <<"\n\nGraph ptr root node: " << mol1->getGraphPtr()->PoobGetRoot() << " okay \n\n"; //this is issue
	//CycleDetector<Atom> sike("cycleOfMol1", mol1->getGraphPtr());

	Graph<Atom> atomGraph(atom0->GetNode());

	CycleDetector<Atom> sike("cycles", &atomGraph);

	std::cout << "Deleting " << atom6->GetName() << "\n";
	delete atom6;

	std::cout << "Deleting " << atom4->GetName() << "\n";
	delete atom4;
	std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	std::cout
			<< "Visualize the graph here:\nhttps://dreampuf.github.io/GraphvizOnline/#digraph%20G%20%7B%0ABobie-%3ESteve%0A%7D\n";
	//CycleDetector<Atom> cycleDetector(atom0->GetNode());
	//cycleDetector.DetectCyclesInDFSGraph();

	std::cout << "Deleting " << atom5->GetName() << "\n";
	delete atom5;
	//  std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	std::cout << "Deleting bonds: \n";
	//atom1->RemoveBond(atom6);
	atom1->RemoveBond(atom2);
	// atom3->RemoveBond(atom4);
	//  std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	// std::cout << "Deleting atoms.\n";
	// // delete atom3;
	// delete atom4;
	// delete atom5;
	// delete atom6;
	// std::cout << "Graph:\n" << atomGraph.Print() << "\n\n";
	std::cout << "Finished atom section\n\n";

	// SUBGRAPH MATCH TESTING

	Atom *atomA = new Atom("Ronne");
	Atom *atomB = new Atom("Bingo");
	Atom *atomC = new Atom("Marsh");
	Atom *atomD = new Atom("Delux");

	atomA->AddBond(atomB);
	atomB->AddBond(atomC);
	// atomC->AddBond(atomD);
	atomA->AddBond(atomD);
	// atomD->AddBond(atomB);

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

//     // Subgraph matching test
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
//     Edge<int> edge5(&gal, &roh);
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
