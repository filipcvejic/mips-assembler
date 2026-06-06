#include "LexicalAnalysis.h"

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

		LexicalAnalysis lex;

		if (!lex.readInputFile(inputFileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		if (lex.Do())
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}
	}
	catch (runtime_error& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
