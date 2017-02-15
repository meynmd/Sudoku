//
//  setOps.cpp
//

#include "setOps.h"

using namespace std;

template <class T>
set<T> takeUnion(set<T> a, set<T> b)
{
	set<T> c;
	
	auto i = a.begin();
	for(; i != a.end(); i++)
	{
		c.add(*i);
	}
	
	for(i = b.begin(); i != b.end(); i++)
	{
		c.add(*i);
	}
	
	return c;
}
