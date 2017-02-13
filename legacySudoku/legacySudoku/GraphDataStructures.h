// ------------ GraphDataStructures.h -----------------
#pragma once

#include <iostream>
#include <list>
using namespace std;

class Vertex;


/////////////////////////////////////////////////////////
class Edge {
public:
	Edge(Vertex* firstVert, Vertex* secondVert)
	:_firstVert(firstVert), _secondVert(secondVert)
	{}
	
// data
	Vertex* _firstVert;
	Vertex* _secondVert;
};

/////////////////////////////////////////////////////////
struct AdjEntry {
    Vertex* _neighbor;
    list<AdjEntry>* _neighborList;
    list<AdjEntry>::iterator _reverseEdge;
};

/////////////////////////////////////////////////////////
class VertAdjList {
public:
    VertAdjList(Vertex* center);
    void addNeighbor(Vertex* neighbor);
    void linkUp(VertAdjList* neighborList);
    Vertex* deleteFirstEdge();
    int getDegree() {return (int)_neighbors.size();}
    int getDegreeUnvisited();
    
    // data
    list<AdjEntry> _neighbors;
private:
    Vertex* _center;
    
    friend ostream& operator << (ostream& os, VertAdjList& v);
};
ostream& operator << (ostream& os, VertAdjList& v);


/////////////////////////////////////////////////////////
class Vertex {
public:
    Vertex(int label)
    :_label(label), _adjList(this), _searchFlag(false), _color(0), _colorKnown(false)
    {}
    
    bool isAdjacent(Vertex* other)
    {
        list<AdjEntry>::iterator iter = _adjList._neighbors.begin();
        for( ; iter != _adjList._neighbors.end(); ++iter )
            if((*iter)._neighbor == other)
                return true;
        return false;
    }
    
    // data
    VertAdjList _adjList;
    int _label; // we will assume that labels are also the index into the vertex arrays
    int _color;
    bool _colorKnown;
    bool _searchFlag;	
};