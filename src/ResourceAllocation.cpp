#include "ResourceAllocation.h"

using namespace std;


/* Vraca prvi registar (t0..t3) koji ne koristi nijedan sused u smetnji,
   ili no_assign ako takav ne postoji (stvarni spill). */
static Regs getColor(Variable* notColoredVariable, InterferenceGraph* ig)
{
	// Probaj redom svaki od __REG_NUMBER__ registara.
	// Pravi registar je (t0 + color), jer src/IR.h ima no_assign=0 a registre t0..t3.
	for (int color = 0; color < __REG_NUMBER__; color++)
	{
		Regs reg = (Regs)(t0 + color);
		bool colorAvailable = true;

		for (Variables::iterator it = ig->variables->begin(); it != ig->variables->end(); it++)
		{
			Variable* variable = *it;
			if (variable == notColoredVariable)
				continue;

			// Sused u smetnji vec koristi ovaj registar -> nije slobodan.
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
			return false;	// stvarni spill: nema slobodnog registra

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
