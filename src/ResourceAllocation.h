#pragma once

#include "InterferenceGraph.h"

#include <stack>

/**
 * Dodela registara (bojenje grafa), po uzoru na vezbu 10.
 * Skida promenljive sa steka simplifikacije i svakoj dodeljuje prvi slobodan
 * registar (t0..t3) koji ne koristi nijedan sused u smetnji.
 *
 * @return true ako su sve promenljive obojene; false ako je doslo do stvarnog spill-a.
 */
bool doResourceAllocation(std::stack<Variable*>* simplificationStack, InterferenceGraph* ig);

/**
 * Provera korektnosti: nijedne dve promenljive u smetnji nemaju isti registar.
 * @return true ako je alokacija korektna.
 */
bool checkResourceAllocation(InterferenceGraph* ig);
