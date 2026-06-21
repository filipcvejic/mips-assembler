#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "InterferenceGraph.h"
#include "Simplification.h"
#include "ResourceAllocation.h"

#include <stack>
#include <fstream>
#include <map>

#include <iostream>
#include <string>
#include <exception>

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		// Ulazna datoteka (.mavn) iz argumenta komandne linije; ako nije data,
		// koristi se podrazumevani primer.
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

		stack<Variable*>* simplificationStack = doSimplification(ig, __REG_NUMBER__);

		if (simplificationStack == nullptr)
		{
			cout << "\nSpill detected! Program zahteva vise od " << __REG_NUMBER__
			          << " istovremeno zivih registara (alokacija sa t0-t3 nije moguca)." << endl;
		}
		else if (!doResourceAllocation(simplificationStack, ig))
		{
			cout << "\nActual spill! Bojenje nije uspelo." << endl;
			delete simplificationStack;
		}
		else if (!checkResourceAllocation(ig))
		{
			cout << "\nGreska: alokacija nije korektna (dve promenljive u smetnji dele registar)." << endl;
			delete simplificationStack;
		}
		else
		{
			cout << "\n--- Dodela registara (uspesno) ---" << endl;
			for (Variables::iterator v = regVars.begin(); v != regVars.end(); v++)
				cout << "  " << (*v)->getName() << " -> $t" << ((*v)->getAssignment() - t0) << endl;
			delete simplificationStack;

			// --- Generisanje MIPS koda (.s) ---
			// Izlazno ime: ulazna datoteka sa ekstenzijom .s
			string outputFileName = inputFileName;
			size_t dot = outputFileName.find_last_of('.');
			if (dot != string::npos)
				outputFileName = outputFileName.substr(0, dot);
			outputFileName += ".s";

			string fn = syntax.getFunctionName();

			// Obrnuta mapa: instrukcija -> labela koja pokazuje na nju.
			map<Instruction*, string> labelOf;
			map<string, Instruction*>& labels = syntax.getLabels();
			for (map<string, Instruction*>::iterator it = labels.begin(); it != labels.end(); it++)
				labelOf[it->second] = it->first;

			ofstream out(outputFileName.c_str());

			if (!fn.empty())
				out << ".globl " << fn << "\n";

			out << "\n.data\n";
			Variables& memVars = syntax.getMemoryVariables();
			for (Variables::iterator m = memVars.begin(); m != memVars.end(); m++)
				out << (*m)->getName() << ":\t.word " << (*m)->getValue() << "\n";

			out << "\n.text\n";
			if (!fn.empty())
				out << fn << ":\n";

			Instructions& instrs = syntax.getInstructions();
			for (Instructions::iterator it = instrs.begin(); it != instrs.end(); it++)
			{
				map<Instruction*, string>::iterator l = labelOf.find(*it);
				if (l != labelOf.end())
					out << l->second << ":\n";
				out << "\t" << (*it)->toString() << "\n";
			}

			out.close();
			cout << "MIPS izlaz generisan: " << outputFileName << endl;
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
