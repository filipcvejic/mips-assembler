#include "Simplification.h"

#include <vector>

using namespace std;


std::stack<Variable*>* doSimplification(InterferenceGraph* ig, int degree)
{
	int n = ig->size;

	vector<Variable*> byPos(n, nullptr);
	for (Variables::iterator v = ig->variables->begin(); v != ig->variables->end(); v++)
		byPos[(*v)->getPosition()] = *v;

	vector<bool> removed(n, false);
	int remaining = n;

	std::stack<Variable*>* simplificationStack = new std::stack<Variable*>();

	while (remaining > 0)
	{
		int pick = -1;
		for (int i = 0; i < n && pick == -1; i++)
		{
			if (removed[i])
				continue;

			int deg = 0;
			for (int j = 0; j < n; j++)
				if (j != i && !removed[j] && ig->matrix[i][j] == __INTERFERENCE__)
					deg++;

			if (deg < degree)
				pick = i;
		}

		if (pick == -1)
		{
			delete simplificationStack;
			return nullptr;
		}

		removed[pick] = true;
		simplificationStack->push(byPos[pick]);
		remaining--;
	}

	return simplificationStack;
}
