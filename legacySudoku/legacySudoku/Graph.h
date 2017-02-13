// ------------------ Graph.h -----------------------
#pragma once

#include <iostream>
#include <vector>
using namespace std;

#include "GraphDataStructures.h"

class GraphFactory;
class Coloring;

class Graph {
public:
	Graph() {}
	
	void draw();

	pair<int, bool> nineColorCompletion(bool printSolution = false, bool printProcess = false);
	
	int getNumVerts() const {return (int)_vertexSet.size();}
	int getNumEdges() const {return (int)_edgeSet.size();}
	
private:
	void addVertex(int label);
	void addEdge(int firstVertIdx, int secondVertIdx);
	void addEdge(Vertex* firstVert, Vertex* secondVert);
	
	vector<vector<Vertex*> > DFS();
	void DFSHelper(	Vertex* currLoc, vector<Vertex*>& disconnectedVerts,
					vector<Vertex*>& currComponent);
					
	bool nineColorCompletionHelper(int* backtracks, Coloring partialColoring, Vertex* lastColoredVert, int lastColor, bool printProcess);

// data
	vector<Vertex*> _vertexSet;
	vector<Edge> _edgeSet;
	
// friendship
	friend ostream& operator << (ostream& os, Graph& g);
	friend class GraphFactory;
	friend class Coloring;
};
ostream& operator << (ostream& os, Graph& s);