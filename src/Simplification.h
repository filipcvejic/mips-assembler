#pragma once

#include "InterferenceGraph.h"

#include <stack>

/**
 * Simplifikacija grafa smetnji.
 * Iterativno skida cvor stepena manjeg od 'degree' i stavlja ga na stek.
 *
 * @param ig graf smetnji
 * @param degree broj registara na platformi (k = __REG_NUMBER__)
 * @return stek redosleda bojenja; NULL ako je detektovan spill
 *         (preostali cvorovi svi imaju stepen >= degree).
 */
std::stack<Variable*>* doSimplification(InterferenceGraph* ig, int degree);
