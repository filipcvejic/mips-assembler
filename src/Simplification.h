#pragma once

#include "InterferenceGraph.h"

#include <stack>

/* Simplifikacija grafa smetnji. */
std::stack<Variable*>* doSimplification(InterferenceGraph* ig, int degree);
