#include "ResourceAllocation.h"

using namespace std;


static Regs getColor(Variable* notColoredVariable, InterferenceGraph* ig)
{
	for (int color = 0; color < __REG_NUMBER__; color++)
	{
		Regs reg = (Regs)(t0 + color);
		bool colorAvailable = true;

		for (Variables::iterator it = ig->variables->begin(); it != ig->variables->end(); it++)
		{
			Variable* variable = *it;
			if (variable == notColoredVariable)
				continue;

			if (variable->getAssignment() == reg &&
			    ig->matrix[notColoredVariable->getPosition()][variable->getPosition()] == __INTERFERENCE__)
			{
				colorAvailable = false;
				break;
			}
		}

		if (colorAvailable)
			return reg;
	}

	return no_assign;
}


bool doResourceAllocation(std::stack<Variable*>* simplificationStack, InterferenceGraph* ig)
{
	while (!simplificationStack->empty())
	{
		Variable* current = simplificationStack->top();
		simplificationStack->pop();

		Regs color = getColor(current, ig);
		if (color == no_assign)
			return false;

		current->setAssignment(color);
	}

	return true;
}


bool checkResourceAllocation(InterferenceGraph* ig)
{
	for (Variables::iterator a = ig->variables->begin(); a != ig->variables->end(); a++)
	{
		for (Variables::iterator b = ig->variables->begin(); b != ig->variables->end(); b++)
		{
			if (*a == *b)
				continue;

			if (ig->matrix[(*a)->getPosition()][(*b)->getPosition()] == __INTERFERENCE__ &&
			    (*a)->getAssignment() != no_assign &&
			    (*a)->getAssignment() == (*b)->getAssignment())
			{
				return false;
			}
		}
	}
	return true;
}
