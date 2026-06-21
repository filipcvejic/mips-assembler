#pragma once

#include "IR.h"

/**
 * Graf smetnji (interference graph).
 * Cvorovi su registarske promenljive; ivica = dve promenljive su istovremeno
 * zive i ne smeju dobiti isti registar.
 *
 * Promenljiva se indeksira preko getPosition() (0 .. size-1).
 */
struct InterferenceGraph
{
	Variables* variables;   ///< sve registarske promenljive (indeks = getPosition())
	int** matrix;           ///< size x size: __INTERFERENCE__ / __EMPTY__
	int size;               ///< dimenzija kvadratne matrice
};

/**
 * Gradi graf smetnji iz liste instrukcija (sa popunjenim def/out iz liveness analize).
 * Pravilo: za svako definisanje d, d je u smetnji sa svim promenljivima iz out[instr] (osim sebe).
 */
InterferenceGraph* doInterferenceGraph(Instructions& instructions, Variables& registerVariables);

/** Ispisuje matricu smetnji. */
void printInterferenceGraph(InterferenceGraph* ig);

/** Oslobadja memoriju matrice (ne brise promenljive - njih poseduje SyntaxAnalysis). */
void freeInterferenceGraph(InterferenceGraph* ig);
