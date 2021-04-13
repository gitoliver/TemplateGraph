# GrYB

**Graph for glYcam aBstraction** *(needs work)*

A (hopefully) efficient library to represent a graph and to run algorithms to completely decompose all cycles and perform subgraph matching. In order to allow this to be as extensible as possible, we have used templating and generics thouroughly thorughout our code. 

## Issue
We were looking for a way to efficiently represent molecules and identify substrucutres within said molecules but no such "toolkit" existed that was simple, yet powerful enough, for our use case. Our specific use case is an undirected possibly-cyclic graph where cycles can be both numerous and fused to one another.

## Design Goals
We want to make something that anyone can use, not just computational chemists. That way this library can help many more people.

* Allow for the creation of graphs for any type (int, string, Atom class, residue class, etc.)
* Use smart pointers to avoid memory leaks and to designate ownership. 
* Have the ability to delete nodes and edges and associated data, and relationships, are properly handled. For example:
    * Prevent dangling pointers that refer back to a structure that has already been deleted
    * Properly delete all edges connected to a node when a node is deleted (prevent any edge objects from just floating in our memory)
* Properly use and enforce encapsulation, which ties into our ownership issue 

## Algorithm Goals
Due to our use case, and our structure, we are presented with the issue of actually implementing the algorithms we want. For instance, we are not only looking for *if* a specific node is a part of a cycle, but *all* cycles a node is a part of.

* Subgraph matching
    * We must be able to tell if GraphB is a subgraph of GraphA
    * Get all subgraphs within GraphA that match GraphB
* Total Cycle Detection
    * Instead of cycle detection please think of this more as cycle decomposition where we find every cycle within our graph.
    * This must be able to run and provide correct cycle decomposition on both fused and normal cycles.

## Extra Algorithms
* The ability to check if each node and bridge are a:
    * Leaf (i.e. only one neighbor)
    * Bridge (i.e. removal will cause a disconnect in the graph, thus creating two. Note that edges can only be bridges and not leafs)
* Mega-Cycle Finding (**maybe include a cool diagram**)
    * We go through each node and check if it even has a possibility of being within a cycle. A megacycle is the set created by getting all traversable nodes where our traversal does not "walk" over any bridges or edges

## Memory Structure Approach
Think of a doubly-linked-list but as a graph. Our nodes are a class that instead of directly pointing to one another they point to an edge object. The edge object then points both to the next node and the previous node. This way we can go from our original node to our parent node and in reverse by traversing our memory relationships.

* Graph Relationships
    * A blurb about *how* (literally how) our graph is related in memory to our other classes. Basically how we end up using smart pointers
* Node Relationships
    * Same as above
* Edge Relationships
    * Same as above above

Due to our observations we want to enforce the following relationships within memory.
**PUT COOL DIAGRAMS HERE REGARDING OUR GRAPH STRUCTURE, THINK THE PEPE-SILVA DIAGRAMS I SHOWED YOU**


## Schematic
![schematic](documentation/schemaGraphClass.png)

## GraphClassConcept
![GraphClass](documentation/GraphClassConcept.png)

## Interitance Concept
![GraphClass](documentation/InheritanceConcept.png)
This inheritance concept was replaced by:
## Generic Object Interitance Concept
![GraphClass](documentation/GenericObjectInheritanceConcept.png)


### Notes
Project is under development as of Sept 2020, contact olgrant "at" uga.edu with queries. 
Written by Oliver Grant as he learned how to use templates, smart pointers and some OOP concepts.
Has been tested on linux, but should work on both Mac and Windows with appropriate C++ complilers.

### Compilation
See compiling.sh, or just run that file.

### Usage
See testDriver.cpp for examples

### Reading Materials
* If you would like, I could compile a list of all worthwhile reading materials I have encountered throughout the creation of this library. 
