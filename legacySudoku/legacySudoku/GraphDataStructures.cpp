//
// ----------------- GraphDataStructures.cpp ------------------------
//
#include "GraphDataStructures.h"

VertAdjList::VertAdjList(Vertex* center)
:_center(center)		{}

void VertAdjList::addNeighbor(Vertex* neighbor)
{
	AdjEntry adj;
	adj._neighbor = neighbor;
	_neighbors.push_front(adj);
}

void VertAdjList::linkUp(VertAdjList* neighborList)
{
	// make sure that in our adjacency list, that a reverse edge
	// pointer can be used to delete in the neighbors' adjacency list
	// in O(1) time
	AdjEntry& theEntry = _neighbors.front();
	theEntry._neighborList = &(neighborList->_neighbors);
	theEntry._reverseEdge = theEntry._neighborList->begin();
}

Vertex* VertAdjList::deleteFirstEdge()
{
	AdjEntry adj = _neighbors.front();
	
	// destroy the neighbor's pointer to us
	adj._neighborList->erase(adj._reverseEdge);
	
	// save this pointer so we can return it (traversing the edge
	Vertex* result = _neighbors.front()._neighbor;
	
	// destroy our pointer to our neighbor
	_neighbors.pop_front();
	return result;
}

int VertAdjList::getDegreeUnvisited()
{
	int degree = 0;
	list<AdjEntry>::iterator iter = _neighbors.begin();
	for( ; iter != _neighbors.end(); ++iter )
		if(!(*iter)._neighbor->_searchFlag)
			++degree;
	return degree;
}

ostream& operator << (ostream& os, VertAdjList& v)
{
	os << v._center->_label << "(" << v._neighbors.size() << ") : ";
	list<AdjEntry>::iterator iter = v._neighbors.begin();
	for( ; iter != v._neighbors.end(); ++iter )
		os << (*iter)._neighbor->_label << " ";
	return os;
}
