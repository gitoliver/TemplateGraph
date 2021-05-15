#ifndef TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GENERICGRAPHOBJECT_HPP
#define TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GENERICGRAPHOBJECT_HPP

#include "../../AbstractObject/includes/AbstractObject.hpp"

namespace TemplateGraph
{

enum class connectivityType
{
	UNKNOWN, BRIDGE, LEAF, INCYCLE
};

class GenericGraphObject: public Abstract::AbstractObject
{
private:
	//	Quick tl;dr about each type & why we care.
	// 		- LEAF: Only a node can be a <leaf>, a <leaf> is a node with a SINGLE edge. In
	// 			multigraph we can consider a <leaf> as a node with a single neighbor.
	// 		- BRIDGE: A <bridge> is an edge (or node) that when deleted results in an increase
	// 			of the graphs connected-components. Think of 2 cycles (cycle1, cycle2)
	// 			connected by a single node (let us call it nodeA) where said node has 2 edges
	// 			(edgeA1, edgeA2) where edgeA1 connects nodeA to cycle1 and edgeA2 connects
	// 			nodeA to cycle2. Thus, if we delete edgeA1, edgeA2, or nodeA we will now
	// 			have two seperate connected components being cycle1 and cycle2. Thus we know
	// 			that nodeA, edgeA1, and edgeA2 are all bridges. We know a node is considered
	// 			a bridge node if all connecting edges are bridge-edges. Please note that an
	// 			edge that connects to a leaf node is also considered a bridge edge.
	// 		- INCYCLE: Due to some fun properties of graphs, we know that if an edge or
	// 			node is neither a <bridge> or leaf> we know that said element must be a
	// 			part of a cycle. In the future, this can allow for some fun optimizations
	// 			where we only run our total cycle detection on elements that we know are in
	// 			a cycle. This will help down on computational time (not always, but depending on
	// 			input it will help) by reducing the amount of elements we must run our cycle
	// 			detection algorithms on.
	//
	// //TODO: ENSURE MY REASONING IS CORRECT, FIGURE OUT BETTER NAMING TOO MUCH TYPING
	//
	// 	Why care?
	// 		- This makes oligosaccharide detection much easier due to the ability to break our
	// 			detection down into "parts". So at first we check if we are dealing with
	// 			monosaccharides, then we check the connectivity of our monosaccharides. We can
	// 			"walk" our connection between the two sugars and we need 2 conditions being that
	// 			the atoms we observe are the correct atoms and that the edges and atom that
	// 			create our connection between the two monosaccharides are considered "bridges".
	// 			If we encounter an incorrect atom, or something that is not considered a bridge
	// 			during our walk through the connection we know that the observed molecule cannot
	// 			be a oligosaccharide.
	//
	//	Quick blurb about enums: If we dont set a variable that uses our enum to a specific value
	//								the variable will be equal to our first element (in our case
	//								<UNKNOWN>). The only reason why I included a specific value
	//								in our variable decleration is to just drive the point home
	//								that we are going to be defaulting to UNKNOWN. It is not
	//								needed but does help ensure there is clarity.
	//

public:

	enum connectivityType getConnectivityTypeIdentifier();

	void setConnectivityTypeIdentifier(
			connectivityType newStructuralIdentifier);

private:

	//our deafult enum value is <UNKNOWN>, default enum value is the first element
	//just added the strutualType::UNKNOWN to make obvious
	connectivityType connectivityTypeIdentifier = connectivityType::UNKNOWN;

};

inline enum connectivityType GenericGraphObject::getConnectivityTypeIdentifier()
{
	return this->connectivityTypeIdentifier;
}

inline void GenericGraphObject::setConnectivityTypeIdentifier(
		connectivityType newStructuralIdentifier)
{
	this->connectivityTypeIdentifier = newStructuralIdentifier;
}

}

#endif // end TEMPLATEGRAPH_GRAPHSTRUCTURE_INCLUDE_GENERICGRAPHOBJECT_hpp
