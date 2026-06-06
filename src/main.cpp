#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"

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
	}
	catch (runtime_error& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
