//
// -------------------- GraphFactory.cpp ---------------------------
//

#include "GraphFactory.h"

#include <fstream>
#include <iostream>
using namespace std;

GraphFactory::GraphFactory(bool loadSudokuDatabase, char dataPath[])
{
	if(!loadSudokuDatabase)
		return;
	if(!dataPath)
		return;
	if(strlen(dataPath) < 1 || strlen(dataPath) > 4096)
		return;
	
	// prepare the file
	ifstream in;
	in.open(dataPath, ifstream::in);
	// FIXME probly should set an error here someplace
	if(!in.is_open())
		return;
	
	// initialize counts
	int fileLinecount = 0;
	int sudokuLinecount = 0;
	int sudokuColcount = 0;
	char line[16];
	
	// initialize our _sukokuList from the data file
	while(in.good())
	{
		in.getline(line, 16);
		
		// every 11 lines, we need to create a new board for the list
		if(fileLinecount % 11 == 0)
		{
			// the sudoku board is a 9x9 array
			int** currSudoku = new int*[9];
			for(int i = 0; i < 9; ++i) {
				currSudoku[i] = new int[9];
			}
			
			_sudokuList.push_back(pair<int**, string>(currSudoku, ""));
			
			// rewind the line count
			// (-2 because the next line is blank, and the one following
			// is a # followed by difficulty)
			sudokuLinecount = -2;
		}
		
		// increment the line counts
		++fileLinecount;
		++sudokuLinecount;
		
		// if this is the 9th line, the line has no data
		if(sudokuLinecount == 9)
			continue;
		
		// -1 means new sudoku board, so first read the difficulty header
		if(sudokuLinecount == -1)
		{
			string difficultyString = "";
			bool startedReadingString = false;
			for(int i = 0; i < 16; ++i)
			{
				if(line[i] >= 'a' && line[i] <= 'z')
				{
					startedReadingString = true;
					difficultyString += line[i];
				}
				else if(startedReadingString)
					break;
			}
			
			_sudokuList.back().second = difficultyString;
					
			continue;
		}
		
		// swallow the line into the columns of a particular row
		for(int i = 0; i < 9; ++i)
		{
			int currChar = line[i] - '0';
			_sudokuList.back().first[sudokuLinecount][sudokuColcount] = currChar;
			++sudokuColcount;
		}
		sudokuColcount = 0;
	}
	
	// whew!
	in.close();
}

/*
Graph GraphFactory::createRandomTree(int numVertices)
{
	Graph g;
	for(int i = 0; i < numVertices; ++i)
		g.addVertex(i);
	connectComponents(g);
	return g;
}

Graph GraphFactory::createPathGraph(int numVertices)
{
	Graph g;
	for(int i = 0; i < numVertices; ++i)
		g.addVertex(i);
	for(int i = 0; i < numVertices-1; ++i)
		g.addEdge(i, i+1);
	return g;
}

Graph GraphFactory::createRandomCycleGraph(int numVertices)
{
	Graph g;
	for(int i = 0; i < numVertices; ++i)
		g.addVertex(i);
	for(int i = 0; i < numVertices; ++i)
		g.addEdge(i, (i+1) % numVertices);
	return g;
}

Graph GraphFactory::createRandomConnected(int numVerts, float probability)
{
	Graph g;
	for(int i = 0; i < numVerts; ++i)
		g.addVertex(i);
	
	for(int i = 0; i < numVerts; ++i)
		for(int j = i+1; j < numVerts; ++j)
		{
			if(probability > ((float)rand() / (float)RAND_MAX))
				g.addEdge(i,j);
		}
		
	connectComponents(g);
	
	return g;
}

Graph GraphFactory::createRandomConnectedEvenDegree(int numVertices, int numEdges)
{
	Graph g;
	for(int i = 0; i < numVertices; ++i)
		g.addVertex(i);

	// Throw some edges in purely randomly 
	// (leave some so we can ensure constraints)
	int numFirstPhaseEdges = numEdges - (g._vertexSet.size()*.5 - 1);
	for(int i = 0; i < numFirstPhaseEdges; ++i)
	{
		int firstIdx = rand() % (int)g._vertexSet.size();
		int secondIdx = rand() % (int)g._vertexSet.size();

		if(!g._vertexSet[firstIdx]->isAdjacent(g._vertexSet[secondIdx]))
			g.addEdge(firstIdx, secondIdx);
	}

	// ensure connectedness
	connectComponents(g);

	// collect the odd degree vertices
	int numVerts = (int)g._vertexSet.size();
	vector<Vertex*> oddDegreeVerts;
	for(int i = 0; i < numVerts; ++i)
		if(g._vertexSet[i]->_adjList.getDegree() % 2 == 1)
			oddDegreeVerts.push_back(g._vertexSet[i]);

	// ensure vertices of even degree
	while(!oddDegreeVerts.empty())
	{
		// pick a random odd-degree vertex
		int firstIdx = rand() % (int)oddDegreeVerts.size();
		Vertex* firstVert = oddDegreeVerts[firstIdx];
		oddDegreeVerts[firstIdx] = oddDegreeVerts.back();
		oddDegreeVerts.pop_back();
		
		// pick another random odd-degree vertex
		int secondIdx = rand() % (int)oddDegreeVerts.size();
		Vertex* secondVert = oddDegreeVerts[secondIdx];
		oddDegreeVerts[secondIdx] = oddDegreeVerts.back();
		oddDegreeVerts.pop_back();
		
		// connect the two vertices
		g.addEdge(firstVert, secondVert);
	}
	return g;
}
*/
/////////////////////////////////////////////////////////
//
// createSudokuGraph
//
// convert our _sudokuList into a graph where each
// alldiff constraint is represented by an edge between
// the verts that cannot share the same "color" or in our
// case, the same number
/////////////////////////////////////////////////////////
Graph GraphFactory::createSudokuGraph(int listIdx, string* difficulty)
{
	(*difficulty) = _sudokuList[listIdx].second;	// difficulty string
	
	// For a 9x9 grid
	int boxSize = 3;
	float f_box = (float)boxSize;
	int gridSize = 9;
	int numVerts = 81;
	Graph g;
	
	// add vertices
	for(int i = 0; i < numVerts; ++i)
	{
		g.addVertex(i);		// label each vertex with a unique number
		
		// each entry in _sudokuList is a pair; first codes its color
		int color = _sudokuList[listIdx].first[i / 9][i % 9];
		g._vertexSet.back()->_color = color;
		
		// color 0 means not yet set
		if(color != 0)
			g._vertexSet.back()->_colorKnown = true;
	}
	
	// edge for every pair of verts participating in alldiff constraint
	for(int i = 0; i < numVerts; ++i)
		for(int j = i; j < numVerts; ++j)
		{
			// no self edges! (coloring is impossible then)
			if(i == j)
				continue;
			
			// div tells us which row; mod tells which column we're in
			int i_div = i / gridSize;
			int j_div = j / gridSize;
			int i_mod = i % gridSize;
			int j_mod = j % gridSize;
			
            
            // are i and j in the same box?
			if(	(int)((float)i_div/f_box + 1.0f)
					==	(int)((float)j_div/f_box + 1.0f)
				&&		(int)((float)i_mod/f_box + 1.0f)
					==	(int)((float)j_mod/f_box + 1.0f)
			) {
                // are i and j in the same row
                if(i_div == j_div) {
                    g.addEdge(i,j, CONSTRAINT_BOX_AND_ROW);
                }
                // or same column
                else if(i_mod == j_mod) {
                    g.addEdge(i,j, CONSTRAINT_BOX_AND_COL);
                }
                
				g.addEdge(i,j, CONSTRAINT_BOX);
			}
            // are i and j in the same row
            else if(i_div == j_div) {
                g.addEdge(i,j, CONSTRAINT_ROW);
            }
            // or same column
            else if(i_mod == j_mod) {
                g.addEdge(i,j, CONSTRAINT_COL);
            }
		}
	
	// return a graph with edges representing alldiff constraints
	return g;
}
/*
Graph GraphFactory::createKnightsTourGraph(int boardWidth, int boardHeight)
{
	Graph g;
	
	// add vertices
	int numVerts = boardWidth*boardHeight;
	for(int i = 0; i < numVerts; ++i)
		g.addVertex(i);
		
	for(int i = 0; i < numVerts; ++i)
	{
		int currRowIdx = i / boardWidth;
		int currColIdx = i % boardWidth;
		
		int neighborRowIdx = currRowIdx + 1;
		int neighborColIdx = currColIdx + 2;
		int neighborConvertedIdx = neighborColIdx + (neighborRowIdx)*boardWidth;
		if(		(neighborColIdx >= 0 && neighborColIdx < boardWidth) 
			&&	(neighborRowIdx >= 0 && neighborRowIdx < boardHeight))
			g.addEdge(i, neighborConvertedIdx);
			
		neighborRowIdx = currRowIdx + 2;
		neighborColIdx = currColIdx + 1;
		neighborConvertedIdx = neighborColIdx + (neighborRowIdx)*boardWidth;
		if(		(neighborColIdx >= 0 && neighborColIdx < boardWidth) 
			&&	(neighborRowIdx >= 0 && neighborRowIdx < boardHeight))
			g.addEdge(i, neighborConvertedIdx);
			
		neighborRowIdx = currRowIdx - 2;
		neighborColIdx = currColIdx + 1;
		neighborConvertedIdx = neighborColIdx + (neighborRowIdx)*boardWidth;
		if(		(neighborColIdx >= 0 && neighborColIdx < boardWidth) 
			&&	(neighborRowIdx >= 0 && neighborRowIdx < boardHeight))
			g.addEdge(i, neighborConvertedIdx);
			
		neighborRowIdx = currRowIdx - 1;
		neighborColIdx = currColIdx + 2;
		neighborConvertedIdx = neighborColIdx + (neighborRowIdx)*boardWidth;
		if(		(neighborColIdx >= 0 && neighborColIdx < boardWidth) 
			&&	(neighborRowIdx >= 0 && neighborRowIdx < boardHeight))
			g.addEdge(i, neighborConvertedIdx);
	}
		
	return g;

}

void GraphFactory::connectComponents(Graph& g)
{
	// discover the connected components
	vector<vector<Vertex*> > connectedComponents = g.DFS();
	
	int numComponents = (int)connectedComponents.size();
	for(int i = 0; i < numComponents - 1; ++i)
	{
		// pick a random component and kill it
		int firstIdx = rand() % connectedComponents.size();
		vector<Vertex*> firstComponent = connectedComponents[firstIdx];
		connectedComponents[firstIdx] = connectedComponents.back();
		connectedComponents.pop_back();
		
		// pick a second random component to merge the first with
		int secondIdx = rand() % connectedComponents.size();
		vector<Vertex*>& secondComponent = connectedComponents[secondIdx];
				
		// pick 2 random vertices (one in each component)
		int firstVertIdx = rand() % firstComponent.size();
		int secondVertIdx = rand() % secondComponent.size();
		g.addEdge(	firstComponent[firstVertIdx], 
					secondComponent[secondVertIdx]);
					
		 // combine components
		secondComponent.insert(	secondComponent.begin(),
					firstComponent.begin(), firstComponent.end());
	}
}
 */
