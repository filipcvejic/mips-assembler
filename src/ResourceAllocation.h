#pragma once

#include "InterferenceGraph.h"

#include <stack>

/* Bojenje grafa. */
bool doResourceAllocation(std::stack<Variable*>* simplificationStack, InterferenceGraph* ig);

/* Provera alokacije. */
bool checkResourceAllocation(InterferenceGraph* ig);
