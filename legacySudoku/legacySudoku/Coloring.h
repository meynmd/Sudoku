// ------------------ Coloring.h -----------------------
#pragma once

#include <iostream>
#include <map>
#include <list>
using namespace std;

#include "Graph.h"

class Coloring {
public:
	Coloring(const Graph& g);
	
	int numLeftToColor() {return (int)_toColor.size();}
	
	pair<Vertex*, list<int> > pickVertToColor();
	bool assignColor(Vertex* vert, int color);

private:
	map<Vertex*, list<int> > _toColor;
	
// friendship
	friend ostream& operator << (ostream& os, Coloring& c);
};
ostream& operator << (ostream& os, Coloring& c);