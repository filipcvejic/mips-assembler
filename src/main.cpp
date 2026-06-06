#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "InterferenceGraph.h"
#include "Simplification.h"
#include "ResourceAllocation.h"

#include <stack>

#include <iostream>
#include <string>
#include <exception>

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		// Ogranicavamo se na jednu ulaznu datoteku (.mavn).
		// Putanju uzimamo iz argumenta komandne linije; ako nije data,
		// koristimo podrazumevani primer (po preporuci postavke: simple.mavn).
		string inputFileName = (argc > 1) ? argv[1] : "examples/simple.mavn";

		// --- Leksicka analiza ---
		LexicalAnalysis lex;

		if (!lex.readInputFile(inputFileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		if (!lex.Do())
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}
		cout << "Lexical analysis finished successfully!" << endl;

		// --- Sintaksna analiza ---
		SyntaxAnalysis syntax(lex);

		if (!syntax.Do())
			throw runtime_error("\nException! Syntax analysis failed!\n");

		cout << "Syntax analysis finished successfully!" << endl;

		// --- Analiza zivotnog veka ---
		doLivenessAnalysis(syntax.getInstructions());
		cout << "Liveness analysis finished." << endl;

		// Ispis IR-a sa rezultatima analize zivotnog veka (in/out).
		syntax.printIR();

		// --- Alokacija registara (graf smetnji + simplifikacija + bojenje) ---
		Variables& regVars = syntax.getRegisterVariableList();
		InterferenceGraph* ig = doInterferenceGraph(syntax.getInstructions(), regVars);
		printInterferenceGraph(ig);

		std::stack<Variable*>* simplificationStack = doSimplification(ig, __REG_NUMBER__);

		if (simplificationStack == nullptr)
		{
			std::cout << "\nSpill detected! Program zahteva vise od " << __REG_NUMBER__
			          << " istovremeno zivih registara (alokacija sa t0-t3 nije moguca)." << std::endl;
		}
		else if (!doResourceAllocation(simplificationStack, ig))
		{
			std::cout << "\nActual spill! Bojenje nije uspelo." << std::endl;
			delete simplificationStack;
		}
		else if (!checkResourceAllocation(ig))
		{
			std::cout << "\nGreska: alokacija nije korektna (dve promenljive u smetnji dele registar)." << std::endl;
			delete simplificationStack;
		}
		else
		{
			std::cout << "\n--- Dodela registara (uspesno) ---" << std::endl;
			for (Variables::iterator v = regVars.begin(); v != regVars.end(); v++)
				std::cout << "  " << (*v)->getName() << " -> $t" << ((*v)->getAssignment() - t0) << std::endl;
			delete simplificationStack;
		}

		freeInterferenceGraph(ig);
	}
	catch (runtime_error& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
