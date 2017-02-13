// -------------------- Graph.cpp ---------------------------
#include "Graph.h"

#include <time.h>
#include <list>
using namespace std;

#include "Coloring.h"

void Graph::addVertex(int label)
{
	_vertexSet.push_back(new Vertex(label));
}

void Graph::addEdge(int firstVertIdx, int secondVertIdx)
{
	Vertex* firstVert = _vertexSet[firstVertIdx];
	Vertex* secondVert = _vertexSet[secondVertIdx];
	addEdge(firstVert, secondVert);
}

void Graph::addEdge(Vertex* firstVert, Vertex* secondVert)
{
	// actually add the edge
	Edge theEdge = Edge(firstVert, secondVert);
	_edgeSet.push_back(theEdge);
	
	// grab the two adjacency lists that need to be updated
	VertAdjList& firstList = _vertexSet[theEdge._firstVert->_label]->_adjList;
	VertAdjList& secondList = _vertexSet[theEdge._secondVert->_label]->_adjList;

	// put the adjacency on each list
	firstList.addNeighbor(theEdge._secondVert);
	secondList.addNeighbor(theEdge._firstVert);

	// give each list a pointer to the adjacency info we just put on the
	// "OTHER" list
	firstList.linkUp(&secondList);
	secondList.linkUp(&firstList);
}

pair<int, bool> Graph::nineColorCompletion(bool printSolution, bool printProcess)
{
	Coloring partialColoring(*this);

	// Select the first vertex to color and split up the pair
	pair<Vertex*, list<int> > currPair = partialColoring.pickVertToColor();
	Vertex* currVert = currPair.first;
	list<int> currColorList = currPair.second;
	
	// Iterate over the potential colors of this vertex, recursively trying each one
	int backtracks = 0;
	bool success = false;
	list<int>::iterator listIter = currColorList.begin();
	for(; listIter != currColorList.end(); ++listIter)
	{				
		success = nineColorCompletionHelper(&backtracks, partialColoring, currVert, (*listIter), printProcess);
		if(success)	
			break;
		++backtracks;
	}
	
	if(printSolution)
	{
		cout << "|-----------------------|" << endl 
		<< "| " << _vertexSet[0]->_color << " ";;
		for(int i = 1; i < _vertexSet.size(); ++i)
		{
			if(i % 27 == 0)
				cout << "| " << endl << "|-----------------------|" << endl << "| ";
			else if(i % 9 == 0)
				cout << "|" << endl << "| ";
			else if(i % 3 == 0)
				cout << "| ";
			cout << _vertexSet[i]->_color << " ";
		}
		
		cout << "|" << endl << "|-----------------------|" << endl;
	}
	return pair<int, bool>(backtracks, success);
}

bool Graph::nineColorCompletionHelper(int* backtracks, Coloring partialColoring, Vertex* lastColoredVert, int lastColor, bool printProcess)
{	
	if(!partialColoring.assignColor(lastColoredVert, lastColor))
		return false;
	if(partialColoring.numLeftToColor() == 0)
		return true;
    
    if(printProcess)
    {
        cout << "|-----------------------|" << endl
        << "| " << _vertexSet[0]->_color << " ";;
        for(int i = 1; i < _vertexSet.size(); ++i)
        {
            if(i % 27 == 0)
                cout << "| " << endl << "|-----------------------|" << endl << "| ";
            else if(i % 9 == 0)
                cout << "|" << endl << "| ";
            else if(i % 3 == 0)
                cout << "| ";
            cout << _vertexSet[i]->_color << " ";
        }
        
        cout << "|" << endl << "|-----------------------|" << endl;
    }
	
	// select the vertex to color next
	pair<Vertex*, list<int> > currPair = partialColoring.pickVertToColor();
	Vertex* currVert = currPair.first;
	list<int> currColorList = currPair.second;
	
	// Iterate over the potential colors of this vertex, recursively trying each one
	list<int>::iterator listIter = currColorList.begin();
	for(; listIter != currColorList.end(); ++listIter)
	{
		if(nineColorCompletionHelper(backtracks, partialColoring, currVert, (*listIter), printProcess))
			return true;
		++(*backtracks);
	}
	
	// none of the colors worked
	currVert->_color = 0;
	return false;
}

vector<vector<Vertex*> > Graph::DFS()
{
	// start by copying all the vertices, so we can easily tell when done
	// while we are at it, might as well set flags to false too
	int numVerts = (int)_vertexSet.size();
	vector<Vertex*> disconnectedVerts;
	for(int i = 0; i < numVerts; ++i)
	{
		_vertexSet[i]->_searchFlag = false;
		disconnectedVerts.push_back(_vertexSet[i]);
	}
			
	// search to discover each component
	vector<vector<Vertex*> > connectedComponents;
	while(!disconnectedVerts.empty())
	{
		// start a new component
		connectedComponents.push_back(vector<Vertex*>());
	
		// associate a "root" to this new component
		Vertex* currLoc = disconnectedVerts.back();
		DFSHelper(currLoc, disconnectedVerts, connectedComponents.back());
	}
	return connectedComponents;
}

void Graph::DFSHelper(Vertex* currLoc, vector<Vertex*>& disconnectedVerts, 
							vector<Vertex*>& currComponent)
{
	// add the vertex to the component
	currComponent.push_back(currLoc);
	currLoc->_searchFlag = true;
	
	// remove the vertex from the copy of vertices,
	// because it has been found to be connected
	vector<Vertex*>::iterator iter = disconnectedVerts.begin();
	for( ; iter < disconnectedVerts.end(); ++iter)
		if((*iter) == currLoc)
		{
			disconnectedVerts.erase(iter);
			break;
		}
	
	// search the neighbors, if they haven't been visited already
	list<AdjEntry>::iterator neighborIter;
	for(	neighborIter = currLoc->_adjList._neighbors.begin(); 
			neighborIter != currLoc->_adjList._neighbors.end(); 
			++neighborIter)
	{
		if(!(*neighborIter)._neighbor->_searchFlag)
			DFSHelper((*neighborIter)._neighbor, disconnectedVerts, 
							currComponent);
	}
}

ostream& operator << (ostream& os, Graph& g)
{
	int numVertices = (int)g._vertexSet.size();

	int numEdges = (int)g._edgeSet.size();
/*	os	<< "*****\t\t This graph has " << numVertices  << " Vertices and "
		<< numEdges << " Edges\n";


	os << "Edge Set: [ ";
	
	if(numEdges > 0)
	{
		// iterate one shy to prevent the dangling comma of annoyingness
		for(int i = 0; i < numEdges - 1; ++i)
			os	<< "(" << g._edgeSet[i]._firstVert->_label << ", " 
				<< g._edgeSet[i]._secondVert->_label << "), ";
		os	<< "(" << g._edgeSet[numEdges - 1]._firstVert->_label << ", "
			<< g._edgeSet[numEdges - 1]._secondVert->_label << ")"; 
	}
	os << " ]" << endl;*/

/*
	for(int i = 0; i < numVertices; ++i)
		os << g._vertexSet[i]->_adjList << endl;
*/
    /*
	for(int i = 0; i < numVertices; ++i)
	{
		for(int j = 0; j < numVertices; ++j)
		{
			os << g._vertexSet[i]->isAdjacent(g._vertexSet[j]) << " ";
		}
		os << endl;
	}
    */
	return os;
}