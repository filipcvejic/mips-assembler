#include "SyntaxAnalysis.h"

#include <iostream>

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex),
	  errorFound(false),
	  tokenIterator(lex.getTokenList().begin())
{
}


bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();

	// Pocetni neterminal gramatike.
	Q();

	// Nakon korektnog programa moramo stici tacno do kraja ulaza (eof).
	if (!errorFound && currentToken.getType() != T_END_OF_FILE)
	{
		printSyntaxError(currentToken);
		errorFound = true;
	}

	return !errorFound;
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Unexpected token: \"" << token.getValue() << "\"" << endl;
}


Token SyntaxAnalysis::getNextToken()
{
	TokenList& tokens = lexicalAnalysis.getTokenList();

	// Komentari nisu deo gramatike - preskacemo ih.
	while (tokenIterator != tokens.end())
	{
		Token t = *tokenIterator++;
		if (t.getType() == T_COMMENT)
			continue;
		return t;
	}

	throw runtime_error("\nException! Reached end of token list during syntax analysis!\n");
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound)
		return;

	if (currentToken.getType() == t)
	{
		currentToken = getNextToken();
	}
	else
	{
		printSyntaxError(currentToken);
		errorFound = true;
	}
}


// Q -> S ; L
void SyntaxAnalysis::Q()
{
	if (errorFound)
		return;

	S();
	eat(T_SEMI_COL);
	L();
}


// L -> eof | Q
void SyntaxAnalysis::L()
{
	if (errorFound)
		return;

	// L -> eof : kraj programa, eof se ne "jede" (ostaje kao tekuci token).
	if (currentToken.getType() == T_END_OF_FILE)
		return;

	// L -> Q
	Q();
}


// S -> _mem mid num | _reg rid | _func id | id : E | E
void SyntaxAnalysis::S()
{
	if (errorFound)
		return;

	switch (currentToken.getType())
	{
	case T_MEM:		// _mem mid num
		eat(T_MEM);
		eat(T_M_ID);
		eat(T_NUM);
		break;

	case T_REG:		// _reg rid
		eat(T_REG);
		eat(T_R_ID);
		break;

	case T_FUNC:	// _func id
		eat(T_FUNC);
		eat(T_ID);
		break;

	case T_ID:		// id : E  (labela ispred instrukcije)
		eat(T_ID);
		eat(T_COL);
		E();
		break;

	default:		// S -> E  (instrukcija)
		E();
		break;
	}
}


// E -> jedna od podrzanih instrukcija
void SyntaxAnalysis::E()
{
	if (errorFound)
		return;

	switch (currentToken.getType())
	{
	case T_ADD:		// add rid , rid , rid
		eat(T_ADD);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_R_ID);
		break;

	case T_ADDI:	// addi rid , rid , num
		eat(T_ADDI);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_NUM);
		break;

	case T_SUB:		// sub rid , rid , rid
		eat(T_SUB);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_R_ID);
		break;

	case T_LA:		// la rid , mid
		eat(T_LA);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_M_ID);
		break;

	case T_LW:		// lw rid , num ( rid )
		eat(T_LW);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT); eat(T_R_ID); eat(T_R_PARENT);
		break;

	case T_LI:		// li rid , num
		eat(T_LI);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_NUM);
		break;

	case T_SW:		// sw rid , num ( rid )
		eat(T_SW);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_NUM);
		eat(T_L_PARENT); eat(T_R_ID); eat(T_R_PARENT);
		break;

	case T_B:		// b id
		eat(T_B);
		eat(T_ID);
		break;

	case T_BLTZ:	// bltz rid , id
		eat(T_BLTZ);
		eat(T_R_ID); eat(T_COMMA);
		eat(T_ID);
		break;

	case T_NOP:		// nop
		eat(T_NOP);
		break;

	default:
		printSyntaxError(currentToken);
		errorFound = true;
		break;
	}
}