#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

#include <map>
#include <string>

/**
 * Sintaksna analiza MAVN jezika (rekurzivni spust) + izgradnja IR-a.
 *
 * Gramatika:
 *   Q -> S ; L
 *   L -> eof | Q
 *   S -> _mem mid num | _reg rid | _func id | id : E | E
 *   E -> add  rid , rid , rid
 *      | addi rid , rid , num
 *      | sub  rid , rid , rid
 *      | la   rid , mid
 *      | lw   rid , num ( rid )
 *      | li   rid , num
 *      | sw   rid , num ( rid )
 *      | b    id
 *      | bltz rid , id
 *      | nop
 *      | and  rid , rid , rid   (dodatna instrukcija)
 *      | or   rid , rid , rid   (dodatna instrukcija)
 *      | bgez rid , id          (dodatna instrukcija, van ALU)
 *
 * Napomena: mnemonici and/or/bgez (kao i labele) leksika daje kao T_ID.
 * U S() se labela (id : E) razlikuje od dodatne instrukcije pogledom na sledeci token.
 */
class SyntaxAnalysis
{
public:
	/**
	 * Konstruktor. Prima referencu na zavrsenu leksicku analizu.
	 */
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	 * Pokrece sintaksnu analizu i (uz uspeh) gradi IR.
	 * @return true ako nema sintaksnih gresaka, false inace.
	 */
	bool Do();

	/* --- Pristup izgradjenom IR-u (za naredne faze) --- */
	Instructions& getInstructions()                         { return instructions; }
	Variables& getMemoryVariables()                         { return memoryVariables; }
	std::map<std::string, Variable*>& getRegVariables()     { return regVariables; }
	Variables& getRegisterVariableList()                    { return registerVariables; }
	std::map<std::string, Instruction*>& getLabels()        { return labels; }
	std::string getFunctionName() const                     { return functionName; }

	/**
	 * Ispisuje izgradjeni IR (za proveru/debug).
	 */
	void printIR();

private:
	/* --- Parser --- */
	void printSyntaxError(Token token);
	void eat(TokenType t);
	Token getNextToken();
	/** Vraca tip sledeceg (ne-komentar) tokena bez konzumiranja - za razlikovanje labele od instrukcije. */
	TokenType peekNextTokenType();

	void Q();
	void S();
	void L();
	void E();

	/* --- Izgradnja IR-a --- */
	/** Vraca (ili kreira ako ne postoji) registarsku promenljivu po imenu. */
	Variable* getOrCreateReg(const std::string& name);
	/** Dodaje memorijsku promenljivu (_mem) sa pocetnom vrednoscu. */
	void addMemoryVariable(const std::string& name, int value);
	/** Kreira instrukciju i puni dst/src (=def/use) iz datih registara (do 1 dst + do 2 src);
	    nullptr je operand kojeg nema. */
	void emit(InstructionType type, const std::string& asmTemplate,
	          Variable* dst = nullptr, Variable* src1 = nullptr, Variable* src2 = nullptr);
	/** Postavlja succ/pred veze (CFG), ukljucujuci ciljeve skokova (b/bltz -> labela). */
	void buildControlFlowGraph();

	/** Referenca na modul leksicke analize. */
	LexicalAnalysis& lexicalAnalysis;

	/** Indikator pronadjene sintaksne greske. */
	bool errorFound;

	/** Iterator kroz listu tokena (izlaz leksicke analize). */
	TokenList::iterator tokenIterator;

	/** Token koji se trenutno analizira. */
	Token currentToken;

	/* --- IR --- */
	Instructions instructions;                          ///< lista instrukcija (redosled = tok programa)
	std::map<std::string, Variable*> regVariables;      ///< registarske promenljive po imenu (deljeni pokazivaci)
	Variables registerVariables;                        ///< iste reg. promenljive kao lista (redosled = pozicija = indeks u grafu smetnji)
	Variables memoryVariables;                          ///< memorijske promenljive (_mem) za .data
	std::map<std::string, Instruction*> labels;         ///< labela -> instrukcija na koju pokazuje
	std::string functionName;                           ///< ime funkcije (_func) za .globl i labelu
	std::string pendingLabel;                           ///< labela koja ceka da se zakaci na sledecu instrukciju
	std::map<Instruction*, std::string> branchTargets;  ///< instrukcija skoka -> ime ciljne labele (za CFG)
};
