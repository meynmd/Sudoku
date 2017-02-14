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
    
     // RULE 3 To implement this, we want to compute set difference of
    // domain HERE - UNION of neighbors by each constraint type.
    // To implement this, we will need to label edges with column, box, row constraint type
    // We need to be able to union domains, and set difference, and intersect

    
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
            // RULE 1 is happening here
			// remove the color from the potential colors list
			_toColor[neighborVert].remove(color);

            // RULE 2 goes here, if size == 1, assign
            if(_toColor[neighborVert].size() == 1)
                if(!assignColor(neighborVert,_toColor[neighborVert].front()))
                    return false;
            
            // RULE 3 goes here, we can reason by process of elimination that a variable is bound by its context
            list<int> boxDomainsU = boxConstraintUnion(neighborVert);
            list<int> rowDomainsU = rowConstraintUnion(neighborVert);
            list<int> colDomainsU = colConstraintUnion(neighborVert);
            list<int> boxRemainingColors = setDifference(_toColor[neighborVert], boxDomainsU);
            list<int> rowRemainingColors = setDifference(_toColor[neighborVert], rowDomainsU);
            list<int> colRemainingColors = setDifference(_toColor[neighborVert], colDomainsU);
            list<int> finalRemainingColors = intersectDomains(boxRemainingColors, intersectDomains(rowRemainingColors, colRemainingColors));
            if(finalRemainingColors.empty())
                return false;
            if(finalRemainingColors.size() == 1)
                if(!assignColor(neighborVert, finalRemainingColors.front()))
                    return false;
            
			// if we just removed the last potential color, 
			// the neighbor is now not colorable. A contradiciton has been found
			if(_toColor[neighborVert].empty())
				return false;
		}
	}
	// the assignment completed successfully.
	return true;
}

list<int> Coloring::boxConstraintUnion(Vertex* v)
{
    //FIXME write this function
    return list<int>();
}
list<int> Coloring::rowConstraintUnion(Vertex* v)
{
    //FIXME write this function
    return list<int>();
}
list<int> Coloring::colConstraintUnion(Vertex* v)
{
    //FIXME write this function
    return list<int>();
}

list<int> Coloring::intersectDomains(const list<int>& domainA, const list<int>& domainB)
{
    //FIXME write this function
    return list<int>();
}

list<int> Coloring::setDifference(const list<int>& domainA, const list<int>& domainB)
{
    //FIXME write this function
    return list<int>();
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
