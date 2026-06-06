#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"

/**
 * Sintaksna analiza MAVN jezika (rekurzivni spust).
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
 */
class SyntaxAnalysis
{
public:
	/**
	 * Konstruktor. Prima referencu na zavrsenu leksicku analizu.
	 */
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	 * Pokrece sintaksnu analizu.
	 * @return true ako nema sintaksnih gresaka, false inace.
	 */
	bool Do();

private:
	/**
	 * Ispisuje sintaksnu gresku i token koji ju je izazvao.
	 */
	void printSyntaxError(Token token);

	/**
	 * "Pojede" tekuci token ako je ocekivanog tipa t, inace prijavljuje gresku.
	 */
	void eat(TokenType t);

	/**
	 * Vraca sledeci token iz liste (preskace komentare).
	 */
	Token getNextToken();

	/* Neterminali gramatike */
	void Q();
	void S();
	void L();
	void E();

	/** Referenca na modul leksicke analize. */
	LexicalAnalysis& lexicalAnalysis;

	/** Indikator pronadjene sintaksne greske. */
	bool errorFound;

	/** Iterator kroz listu tokena (izlaz leksicke analize). */
	TokenList::iterator tokenIterator;

	/** Token koji se trenutno analizira. */
	Token currentToken;
};
