#include "LivenessAnalysis.h"

using namespace std;


static bool variableExists(Variable* v, Variables& vars)
{
	for (Variables::iterator it = vars.begin(); it != vars.end(); it++)
		if (*it == v)
			return true;
	return false;
}


static void normalize(Variables& vars)
{
	vars.sort();
	vars.unique();
}


void doLivenessAnalysis(Instructions& instructions)
{
	bool changed = true;

	while (changed)
	{
		changed = false;

		for (Instructions::reverse_iterator rit = instructions.rbegin(); rit != instructions.rend(); rit++)
		{
			Instruction* instr = *rit;

			Variables& in  = instr->getIn();
			Variables& out = instr->getOut();

			// out = unija in skupova naslednika
			Variables outNew;
			for (Instructions::iterator s = instr->getSucc().begin(); s != instr->getSucc().end(); s++)
			{
				Variables& succIn = (*s)->getIn();
				outNew.insert(outNew.end(), succIn.begin(), succIn.end());
			}
			normalize(outNew);

			// in = use + (out - def)
			Variables inNew = instr->getUse();
			for (Variables::iterator o = outNew.begin(); o != outNew.end(); o++)
			{
				if (!variableExists(*o, instr->getDef()) && !variableExists(*o, inNew))
					inNew.push_back(*o);
			}
			normalize(inNew);

			if (out != outNew || in != inNew)
				changed = true;

			out = outNew;
			in  = inNew;
		}
	}
}
