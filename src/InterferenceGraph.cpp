#include "InterferenceGraph.h"

#include <iostream>
#include <iomanip>

using namespace std;


InterferenceGraph* doInterferenceGraph(Instructions& instructions, Variables& registerVariables)
{
	InterferenceGraph* ig = new InterferenceGraph();
	ig->variables = &registerVariables;
	ig->size = (int)registerVariables.size();

	ig->matrix = new int*[ig->size];
	for (int i = 0; i < ig->size; i++)
	{
		ig->matrix[i] = new int[ig->size];
		for (int j = 0; j < ig->size; j++)
			ig->matrix[i][j] = __EMPTY__;
	}

	for (Instructions::iterator it = instructions.begin(); it != instructions.end(); it++)
	{
		Instruction* instr = *it;
		Variables& def = instr->getDef();
		Variables& out = instr->getOut();

		for (Variables::iterator d = def.begin(); d != def.end(); d++)
		{
			for (Variables::iterator o = out.begin(); o != out.end(); o++)
			{
				if (*o == *d)
					continue;

				int a = (*d)->getPosition();
				int b = (*o)->getPosition();
				ig->matrix[a][b] = __INTERFERENCE__;
				ig->matrix[b][a] = __INTERFERENCE__;
			}
		}
	}

	return ig;
}


void printInterferenceGraph(InterferenceGraph* ig)
{
	cout << "\n--- Graf smetnji (1 = smetnja) ---" << endl;

	cout << setw(6) << " ";
	for (Variables::iterator v = ig->variables->begin(); v != ig->variables->end(); v++)
		cout << setw(4) << (*v)->getName();
	cout << endl;

	for (Variables::iterator a = ig->variables->begin(); a != ig->variables->end(); a++)
	{
		cout << setw(6) << (*a)->getName();
		for (Variables::iterator b = ig->variables->begin(); b != ig->variables->end(); b++)
			cout << setw(4) << ig->matrix[(*a)->getPosition()][(*b)->getPosition()];
		cout << endl;
	}
}


void freeInterferenceGraph(InterferenceGraph* ig)
{
	if (ig == nullptr)
		return;

	if (ig->matrix != nullptr)
	{
		for (int i = 0; i < ig->size; i++)
			delete[] ig->matrix[i];
		delete[] ig->matrix;
	}
	delete ig;
}
