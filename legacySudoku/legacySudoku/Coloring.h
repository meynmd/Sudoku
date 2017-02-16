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
    
    list<int> boxConstraintUnion(Vertex* v);
    list<int> rowConstraintUnion(Vertex* v);
    list<int> colConstraintUnion(Vertex* v);
    
	template <class T> list<T> unionDomains(const list<T>& a, const list<T>& b);
	template <class T> list<T> intersectDomains(const list<T>& a, const list<T>& b);
    template <class T> list<T> setDifference(const list<T>& domainA, const list<T>& domainB);


private:
	map<Vertex*, list<int> > _toColor;
	
// friendship
	friend ostream& operator << (ostream& os, Coloring& c);
};
ostream& operator << (ostream& os, Coloring& c);
