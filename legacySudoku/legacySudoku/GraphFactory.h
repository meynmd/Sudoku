// ------------------ GraphFactory.h -----------------------
#pragma once

#include "Graph.h"

#include <vector>
using namespace std;


/////////////////////////////////////////////////////////
//
// GraphFactory
//
// a factory for creating graphs of various types
//
/////////////////////////////////////////////////////////
class GraphFactory {
public:
	//GraphFactory(bool loadSudokuDatabase);
	GraphFactory(bool loadSudokuDatabase, char dataPath[]);
	Graph createRandomTree(int numVertices);
	Graph createPathGraph(int numVertices);
	Graph createRandomCycleGraph(int numVertices);
	Graph createRandomConnected(int numVertices, float probability);
	Graph createRandomConnectedEvenDegree(int numVertices, int numEdges);
	Graph createSudokuGraph(int listIdx, string* difficulty);
	Graph createKnightsTourGraph(int boardWidth, int boardHeight);
	
	void connectComponents(Graph& g);
	int getNumSudokus() {return (int)_sudokuList.size();}

private:	
	vector<pair<int**, string> > _sudokuList;
};
