//
// ------------------ Coloring.cpp -----------------------
//
#include "Coloring.h"
#include <set>

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

/////////////////////////////////////////////////////////
//
// pickVertToColor
//
// I think this is where the heuristic goes, right? --Matt
//
/////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////
//
// assignColor
//
// this method implements Rule 3. We try to color a vertex
// with the requested color, returning true if it is
// --what, arc consistent with all connected vertices?
// false otherwise
//
// inputs:
//
//		vert		Vertex*			vertex to color
//		color		int				requested color 1-9 ?
//
//	output:
//		true if successful, false otherwise
//
//	side effects?
//
/////////////////////////////////////////////////////////
bool Coloring::assignColor(Vertex* vert, int color, Graph* g)
{
    
     // RULE 3 To implement this, we want to compute set difference of
    // domain HERE - UNION of neighbors by each constraint type.
    // To implement this, we will need to label edges with column, box, row constraint type
    // We need to be able to union domains, and set difference, and intersect
    
	// actually change the graph model
	
	vert->_color = color;
	
	printf("Coloring vertex (%d, %d): %d\n\n", vert->col, vert->row, color);
	
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
                if(!assignColor(neighborVert,_toColor[neighborVert].front(), g))
                    return false;
            
            // RULE 3: we can reason by process of elimination that a variable is bound by its context
            list<int> boxDomainsU = boxConstraintUnion(neighborVert, g);
            list<int> rowDomainsU = rowConstraintUnion(neighborVert, g);
            list<int> colDomainsU = colConstraintUnion(neighborVert, g);
            list<int> boxRemainingColors = setDifference(_toColor[neighborVert], boxDomainsU);
            list<int> rowRemainingColors = setDifference(_toColor[neighborVert], rowDomainsU);
            list<int> colRemainingColors = setDifference(_toColor[neighborVert], colDomainsU);
            list<int> finalRemainingColors = intersectDomains(boxRemainingColors, intersectDomains(rowRemainingColors, colRemainingColors));
			
			printf("finalRemainingColors: for (%d, %d)\n", neighborVert->col, neighborVert->row);
			for(auto i = finalRemainingColors.begin(); i != finalRemainingColors.end(); i++)
			{
				printf("\t%d", *i);
			}
			printf("\n\n");
			
            if(finalRemainingColors.empty())
                return false;
            if(finalRemainingColors.size() == 1)
                if(!assignColor(neighborVert, finalRemainingColors.front(), g))
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

list<int> Coloring::boxConstraintUnion(Vertex* v, Graph* g)
{
	set<int> boxUnion;
	
	int firstRow = 3 * (v->row / 3), firstCol = 3 * (v->col / 3);
	int lastRow = firstRow + 2, lastCol = firstCol + 2;
	
	// don't get constraints from the graph
	/*
	for(int row = firstRow; row <= lastRow; row++)
	{
		for(int col = firstCol; col <= lastCol; col++)
		{
			if(row == v->row && col == v->col)
			{
				continue;
			}
			
			int c = g->_vertexSet[9 * row + col]->_color;
			if(c != 0)
			{
				boxUnion.insert(c);
			}
		}
	}
	*/
	// or should we use _toColor instead of graph? Or both?
	for(auto i = _toColor.begin(); i != _toColor.end(); i++)
	{
		int row = i->first->row, col = i->first->col;
		if (row == v->row && col == v->col)
			continue;
		
		// see if this vertex is in the same box
		if (row >= firstRow && row <= lastRow && col >= firstCol && col <= lastCol)
		{
			int c = i->first->_color;
			if(c != 0)
			{
				boxUnion.insert(c);
			}
		}
	}
	
	
	// convert to list
	list<int> result;
	for(auto i = boxUnion.begin(); i != boxUnion.end(); i++)
	{
		result.push_back(*i);
	}
	
	printf("Vertex (%d, %d)\n\tBox Constraints Union:\n\t", v->col, v->row);
	for(auto i = result.begin(); i != result.end(); i++)
	{
		printf("%d\t", *i);
	}
	printf("\n\n");
	
    return result;
}


list<int> Coloring::rowConstraintUnion(Vertex* v, Graph* g)
{
	set<int> rowUnion;
	
	// we are interested in the vertices in the same row
	int row = v->row;
	int col = v->col;
	
	// don't check the graph
	/*
	for(col = 0; col < 9; col++)
	{
		if(col == v->col)
		{
			continue;
		}
		
		int c = g->_vertexSet[9 * row + col]->_color;
		if(c != 0)
		{
			rowUnion.insert(c);
		}
	}
	*/
	
	// search coloring instead of graph? Or both?
	for(auto i = _toColor.begin(); i != _toColor.end(); i++)
	{
		if (i->first->row == v->row && i->first->col != v->col)
		{
			int c = i->first->_color;
			if(c != 0)
			{
				rowUnion.insert(c);
			}
		}
	}
	
	
	// convert to list
	list<int> result;
	for(auto i = rowUnion.begin(); i != rowUnion.end(); i++)
	{
		result.push_back(*i);
	}
		
	printf("Vertex (%d, %d)\n\tRow Constraints Union:\n\t", v->col, v->row);
	for(auto i = result.begin(); i != result.end(); i++)
	{
		printf("%d\t", *i);
	}
	printf("\n\n");
	
    return result;
}
list<int> Coloring::colConstraintUnion(Vertex* v, Graph* g)
{
	set<int> colUnion;
	
	// we are interested in the vertices in the same row
	
	int row = v->row;
	int col = v->col;
	
	// don't search graph
	/*
	for(row = 0; row < 9; row++)
	{
		if(row == v->row)
		{
			continue;
		}
		
		int c = g->_vertexSet[9 * row + col]->_color;
		if(c != 0)
		{
			colUnion.insert(c);
		}
	}
	*/
	
	// use coloring instead of graph
	
	for(auto i = _toColor.begin(); i != _toColor.end(); i++)
	{
		if (i->first->col == v->col && i->first->row != v->row)
		{
			int c = i->first->_color;
			if(c != 0)
			{
				colUnion.insert(c);
			}
		}
	}
	
	
	// convert to list
	list<int> result;
	for(auto i = colUnion.begin(); i != colUnion.end(); i++)
	{
		result.push_back(*i);
	}
		
	printf("Vertex (%d, %d)\n\tColumn Constraints Union:\n\t", v->col, v->row);
	for(auto i = result.begin(); i != result.end(); i++)
	{
		printf("%d\t", *i);
	}
	printf("\n\n");
	
    return result;
}

template <class T>
list<T> Coloring::unionDomains(const list<T>& a, const list<T>& b)
{
	list<T> c;
	
	auto i = a.begin();
	for(; i != a.end(); i++)
	{
		c.insert(*i);
	}
	
	for(i = b.begin(); i != b.end(); i++)
	{
		c.insert(*i);
	}
	
	return c;

}

template <class T>
list<T> Coloring::intersectDomains(const list<T>& a, const list<T>& b)
{
	list<T> c;
	
	for (auto i = a.begin(); i != a.end(); i++)
	{
		T query = *i;
		for (auto j = b.begin(); j != b.end(); j++)
		{
			if (query == *j)
				c.push_back(query);
		}
	}
	return c;
}


template <class T>
list<T> Coloring::setDifference(const list<T>& a, const list<T>& b)
{
	list<T> c;
	
	for(auto i = a.begin(); i != a.end(); i++)
	{
		T query = *i;
		bool found = false;
		for (auto j = b.begin(); j != b.end(); j++)
		{
			if (query == *j)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			c.push_back(query);
		}
	}
	return c;
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
