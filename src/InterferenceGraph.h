#pragma once

#include "IR.h"

/* Graf smetnji. */
struct InterferenceGraph
{
	Variables* variables;
	int** matrix;
	int size;
};

InterferenceGraph* doInterferenceGraph(Instructions& instructions, Variables& registerVariables);

void printInterferenceGraph(InterferenceGraph* ig);

void freeInterferenceGraph(InterferenceGraph* ig);
