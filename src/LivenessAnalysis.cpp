#include "LivenessAnalysis.h"

using namespace std;


/* Da li promenljiva v postoji u listi vars (poredjenje po pokazivacu - deljeni objekti). */
static bool variableExists(Variable* v, Variables& vars)
{
	for (Variables::iterator it = vars.begin(); it != vars.end(); it++)
		if (*it == v)
			return true;
	return false;
}


/* Normalizuje skup: sortira i uklanja duplikate, da poredjenje dve liste
   (da li se skup promenio) ne zavisi od redosleda. */
static void normalize(Variables& vars)
{
	vars.sort();
	vars.unique();
}


void doLivenessAnalysis(Instructions& instructions)
{
	bool changed = true;

	// Ponavljamo prolaze dok se neki in/out skup menja (do fiksne tacke).
	while (changed)
	{
		changed = false;

		// Unazad: od poslednje ka prvoj instrukciji.
		for (Instructions::reverse_iterator rit = instructions.rbegin(); rit != instructions.rend(); rit++)
		{
			Instruction* instr = *rit;

			Variables& in  = instr->getIn();
			Variables& out = instr->getOut();

			// out_new = U in[s] za svako s iz succ
			Variables outNew;
			for (Instructions::iterator s = instr->getSucc().begin(); s != instr->getSucc().end(); s++)
			{
				Variables& succIn = (*s)->getIn();
				outNew.insert(outNew.end(), succIn.begin(), succIn.end());
			}
			normalize(outNew);

			// in_new = use U ( out_new \ def )
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
