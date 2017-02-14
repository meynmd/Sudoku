//
// ------------------ Coloring.cpp -----------------------
//
#include "Coloring.h"

Coloring::Coloring(const Graph& g)
{
	// set up a list for the unconstrained vertex case
	list<int> allOptions;
	for(int j = 0; j < 9; ++j)
		allOptions.push_back(j+1);
				
	// collect vertices we need to color
	int numVerts = g.getNumVerts();
	for(int i = 0; i < numVerts; ++i)
		if(!g._vertexSet[i]->_colorKnown)
		{
			Vertex* currVert = g._vertexSet[i];
			_toColor[currVert] = allOptions;
			
			// restrict potential colors for colorless vertices based on neighbors' constraints
			list<AdjEntry>::iterator neighborIter = currVert->_adjList._neighbors.begin();
			for(; neighborIter != currVert->_adjList._neighbors.end(); ++neighborIter)
			{
				Vertex* neighborVert = (*neighborIter)._neighbor;
				if(neighborVert->_colorKnown)
				{
					// the neighbor has a known color, cross it off of our list (if its still there)
					list<int>::iterator colorIter = _toColor[currVert].begin();
					for(; colorIter != _toColor[currVert].end(); ++colorIter)
						if((*colorIter) == neighborVert->_color)
						{
							_toColor[currVert].erase(colorIter);
							break;
						}
				}
			}		
		}
}

pair<Vertex*, list<int> > Coloring::pickVertToColor()
{
	// search strategy one: pick the vertex with the smallest list
	map<Vertex*, list<int> >::iterator mapIter = _toColor.begin();
	map<Vertex*, list<int> >::iterator bestIter = mapIter;
	for(; mapIter != _toColor.end(); ++mapIter)
	{
		if((*mapIter).second.size() < (*bestIter).second.size())
			bestIter = mapIter;
	}
	
	pair<Vertex*, list<int> > result = pair<Vertex*, list<int> >(
			(*bestIter).first, (*bestIter).second
	);
	
	return result;
}

bool Coloring::assignColor(Vertex* vert, int color)
{
	// actually change the graph model
	vert->_color = color;
	
	// the vertex is colored, remove it from the map 
	_toColor.erase(vert);

	// inform all vertices who need to update their potential colors because they would
	// conflict with my "new" color
	list<AdjEntry>::iterator neighborIter = vert->_adjList._neighbors.begin();
	for(; neighborIter != vert->_adjList._neighbors.end(); ++neighborIter)
	{
		Vertex* neighborVert = (*neighborIter)._neighbor;
		
		// only check lists of vertices which are uncolored
		if(neighborVert->_color == 0)
		{
			// remove the color from the potential colors list
			_toColor[neighborVert].remove(color);
			
			// if we just removed the last potential color, 
			// the neighbor is now not colorable. A contradiciton has been found
			if(_toColor[neighborVert].empty())
				return false;
		}
	}
	// the assignment completed successfully.
	return true;
}

ostream& operator << (ostream& os, Coloring& c)
{
	map<Vertex*, list<int> >::iterator mapIter = c._toColor.begin();
	for(; mapIter != c._toColor.end(); ++mapIter)
	{
		cout << (*mapIter).first->_label << ": ";
		list<int>::iterator listIter = (*mapIter).second.begin();
		for(; listIter != (*mapIter).second.end(); ++listIter)
			cout << (*listIter) << " ";
		cout << endl;
	}
	
	return os;
}
