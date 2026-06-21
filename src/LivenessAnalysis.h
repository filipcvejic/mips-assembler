#pragma once

#include "IR.h"

/**
 * Analiza zivotnog veka promenljivih.
 *
 * Iterativno, unazad, do fiksne tacke:
 *   out[i] = U  in[s]   za svako s iz succ(i)
 *   in[i]  = use[i] U ( out[i] \ def[i] )
 *
 * Rezultat (in/out po instrukciji) se upisuje direktno u instrukcije
 * i predstavlja ulaz za graf smetnji i alokaciju registara.
 *
 * @param instructions lista instrukcija sa popunjenim use/def i succ/pred (CFG).
 */
void doLivenessAnalysis(Instructions& instructions);
