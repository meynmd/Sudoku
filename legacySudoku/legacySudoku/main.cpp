// ---------------- main.cpp ---------------------
#include <iostream>
#include <ctime>
using namespace std;

#include "GraphFactory.h"

void singleTestSudoku()
{
	string difficulty = "";
	int singleTestIdx = 12;
	GraphFactory factory(true);
	Graph g = factory.createSudokuGraph(singleTestIdx, &difficulty);
	cout << g;
	
	bool printSolution = true;
    bool printProcess = true;
	pair<int, bool> results = g.nineColorCompletion(printSolution, printProcess);
	
	cout << singleTestIdx+1 << "\t" << difficulty << "\t" << results.first << "\t";
	if(results.second)
		cout << "Success!" << endl;
	else
		cout << "*Failure!" << endl;
	
    //FIXME print instead?
	//LayoutMgr::layoutGrid(g, 9, 9, .3);
	//g.draw();
}

void multiTestSudoku()
{
	GraphFactory factory(true);
	int numSudokus = factory.getNumSudokus();
	for(int i = 0; i < numSudokus; ++i)
	{
		string difficulty = "";
		Graph g = factory.createSudokuGraph(i, &difficulty);
		
		pair<int, bool> results = g.nineColorCompletion();
		cout << i+1 << "\t" << difficulty << "\t" << results.first << "\t";
		if(results.second)
			cout << "Success!" << endl;
		else
			cout << "*Failure!" << endl;
		
		if(i == numSudokus - 1)
		{
            //FIXME print instead?
			//LayoutMgr::layoutGrid(g, 9, 9, .3);
			//g.draw();
		}
	}
}

int main (int argc, char* argv[]) 
{
    singleTestSudoku();
    //multiTestSudoku();
}