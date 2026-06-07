#include "SyntaxAnalysis.h"

#include <iostream>
#include <cstdlib>

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

	// Kada je program sintaksno ispravan, izgradi graf toka kontrole (CFG).
	if (!errorFound)
		buildControlFlowGraph();

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


TokenType SyntaxAnalysis::peekNextTokenType()
{
	TokenList& tokens = lexicalAnalysis.getTokenList();

	// tokenIterator vec pokazuje na token IZA tekuceg; preskacemo komentare.
	for (TokenList::iterator it = tokenIterator; it != tokens.end(); it++)
	{
		if (it->getType() == T_COMMENT)
			continue;
		return it->getType();
	}
	return T_END_OF_FILE;
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


/* ===================== Izgradnja IR-a ===================== */

Variable* SyntaxAnalysis::getOrCreateReg(const std::string& name)
{
	map<string, Variable*>::iterator it = regVariables.find(name);
	if (it != regVariables.end())
		return it->second;

	// pozicija = redni broj promenljive (indeks u matrici smetnji, kasnije)
	Variable* v = new Variable(Variable::REG_VAR, name, (int)regVariables.size());
	regVariables[name] = v;
	registerVariables.push_back(v);
	return v;
}


void SyntaxAnalysis::addMemoryVariable(const std::string& name, int value)
{
	Variable* v = new Variable(Variable::MEM_VAR, name, -1, value);
	memoryVariables.push_back(v);
}


void SyntaxAnalysis::emit(InstructionType type, const std::string& asmTemplate,
                          const std::vector<std::string>& dstNames,
                          const std::vector<std::string>& srcNames)
{
	if (errorFound)
		return;

	Instruction* instr = new Instruction((int)instructions.size() + 1, type, asmTemplate);

	// dst registri (ujedno def skup za liveness)
	for (size_t i = 0; i < dstNames.size(); i++)
	{
		Variable* v = getOrCreateReg(dstNames[i]);
		instr->getDst().push_back(v);
		instr->getDef().push_back(v);
	}

	// src registri (ujedno use skup za liveness)
	for (size_t i = 0; i < srcNames.size(); i++)
	{
		Variable* v = getOrCreateReg(srcNames[i]);
		instr->getSrc().push_back(v);
		instr->getUse().push_back(v);
	}

	// labela koja je prethodila ovoj instrukciji (S -> id : E)
	if (!pendingLabel.empty())
	{
		labels[pendingLabel] = instr;
		pendingLabel.clear();
	}

	instructions.push_back(instr);
}


void SyntaxAnalysis::buildControlFlowGraph()
{
	for (Instructions::iterator it = instructions.begin(); it != instructions.end(); it++)
	{
		Instruction* cur = *it;

		Instructions::iterator nextIt = it;
		nextIt++;
		Instruction* fallThrough = (nextIt != instructions.end()) ? *nextIt : nullptr;

		InstructionType type = cur->getType();

		if (type == I_B || type == I_BLTZ || type == I_BGEZ)
		{
			// Ciljna labela ovog skoka.
			std::map<Instruction*, std::string>::iterator bt = branchTargets.find(cur);
			if (bt == branchTargets.end())
				continue;

			std::map<std::string, Instruction*>::iterator lab = labels.find(bt->second);
			if (lab == labels.end())
			{
				cout << "Semantic error! Undefined label: \"" << bt->second << "\"" << endl;
				errorFound = true;
				return;
			}
			Instruction* target = lab->second;

			// Uslovni skok (bltz/bgez): moguc prelaz i na sledecu instrukciju (fall-through) i na cilj.
			// Bezuslovni skok (b): kontrola nikada ne "propada" na sledecu instrukciju, samo cilj.
			if ((type == I_BLTZ || type == I_BGEZ) && fallThrough != nullptr)
			{
				cur->getSucc().push_back(fallThrough);
				fallThrough->getPred().push_back(cur);
			}

			cur->getSucc().push_back(target);
			target->getPred().push_back(cur);
		}
		else
		{
			// Sve ostale instrukcije: prelaz na sledecu instrukciju (ako postoji).
			if (fallThrough != nullptr)
			{
				cur->getSucc().push_back(fallThrough);
				fallThrough->getPred().push_back(cur);
			}
		}
	}
}


/* ===================== Gramatika ===================== */

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
	{
		eat(T_MEM);
		string memName = currentToken.getValue(); eat(T_M_ID);
		string memVal  = currentToken.getValue(); eat(T_NUM);
		if (!errorFound)
			addMemoryVariable(memName, atoi(memVal.c_str()));
		break;
	}

	case T_REG:		// _reg rid
	{
		eat(T_REG);
		string regName = currentToken.getValue(); eat(T_R_ID);
		if (!errorFound)
			getOrCreateReg(regName);	// deklaracija registarske promenljive
		break;
	}

	case T_FUNC:	// _func id
	{
		eat(T_FUNC);
		string fName = currentToken.getValue(); eat(T_ID);
		if (!errorFound)
			functionName = fName;
		break;
	}

	case T_ID:
	{
		// T_ID moze biti labela (id : E) ili mnemonik dodatne instrukcije (and/or/bgez).
		// Razlikujemo ih pogledom na sledeci token: ako sledi ':' -> labela, inace -> instrukcija.
		if (peekNextTokenType() == T_COL)
		{
			string lbl = currentToken.getValue(); eat(T_ID);
			eat(T_COL);
			if (!errorFound)
				pendingLabel = lbl;
			E();
		}
		else
		{
			E();
		}
		break;
	}

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
	{
		eat(T_ADD);
		string d  = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string s1 = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string s2 = currentToken.getValue(); eat(T_R_ID);
		emit(I_ADD, "add `d, `s, `s", { d }, { s1, s2 });
		break;
	}

	case T_ADDI:	// addi rid , rid , num
	{
		eat(T_ADDI);
		string d   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string s   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string imm = currentToken.getValue(); eat(T_NUM);
		emit(I_ADDI, "addi `d, `s, " + imm, { d }, { s });
		break;
	}

	case T_SUB:		// sub rid , rid , rid
	{
		eat(T_SUB);
		string d  = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string s1 = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string s2 = currentToken.getValue(); eat(T_R_ID);
		emit(I_SUB, "sub `d, `s, `s", { d }, { s1, s2 });
		break;
	}

	case T_LA:		// la rid , mid
	{
		eat(T_LA);
		string d   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string mid = currentToken.getValue(); eat(T_M_ID);
		emit(I_LA, "la `d, " + mid, { d }, { });
		break;
	}

	case T_LW:		// lw rid , num ( rid )
	{
		eat(T_LW);
		string d   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string off = currentToken.getValue(); eat(T_NUM);
		eat(T_L_PARENT);
		string base = currentToken.getValue(); eat(T_R_ID);
		eat(T_R_PARENT);
		emit(I_LW, "lw `d, " + off + "(`s)", { d }, { base });
		break;
	}

	case T_LI:		// li rid , num
	{
		eat(T_LI);
		string d   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string imm = currentToken.getValue(); eat(T_NUM);
		emit(I_LI, "li `d, " + imm, { d }, { });
		break;
	}

	case T_SW:		// sw rid , num ( rid )
	{
		eat(T_SW);
		string s   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string off = currentToken.getValue(); eat(T_NUM);
		eat(T_L_PARENT);
		string base = currentToken.getValue(); eat(T_R_ID);
		eat(T_R_PARENT);
		emit(I_SW, "sw `s, " + off + "(`s)", { }, { s, base });
		break;
	}

	case T_B:		// b id
	{
		eat(T_B);
		string lbl = currentToken.getValue(); eat(T_ID);
		emit(I_B, "b " + lbl, { }, { });
		if (!errorFound)
			branchTargets[instructions.back()] = lbl;
		break;
	}

	case T_BLTZ:	// bltz rid , id
	{
		eat(T_BLTZ);
		string s   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
		string lbl = currentToken.getValue(); eat(T_ID);
		emit(I_BLTZ, "bltz `s, " + lbl, { }, { s });
		if (!errorFound)
			branchTargets[instructions.back()] = lbl;
		break;
	}

	case T_NOP:		// nop
		eat(T_NOP);
		emit(I_NOP, "nop", { }, { });
		break;

	case T_ID:		// dodatne instrukcije (mnemonik leksiran kao T_ID): and / or / bgez
	{
		string mnem = currentToken.getValue();

		if (mnem == "and")			// and rid , rid , rid
		{
			eat(T_ID);
			string d  = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
			string s1 = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
			string s2 = currentToken.getValue(); eat(T_R_ID);
			emit(I_AND, "and `d, `s, `s", { d }, { s1, s2 });
		}
		else if (mnem == "or")		// or rid , rid , rid
		{
			eat(T_ID);
			string d  = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
			string s1 = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
			string s2 = currentToken.getValue(); eat(T_R_ID);
			emit(I_OR, "or `d, `s, `s", { d }, { s1, s2 });
		}
		else if (mnem == "bgez")	// bgez rid , id
		{
			eat(T_ID);
			string s   = currentToken.getValue(); eat(T_R_ID); eat(T_COMMA);
			string lbl = currentToken.getValue(); eat(T_ID);
			emit(I_BGEZ, "bgez `s, " + lbl, { }, { s });
			if (!errorFound)
				branchTargets[instructions.back()] = lbl;
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
		break;
	}

	default:
		printSyntaxError(currentToken);
		errorFound = true;
		break;
	}
}


/* ===================== Ispis IR-a ===================== */

void SyntaxAnalysis::printIR()
{
	cout << "\n================== IR (intermediate representation) ==================" << endl;
	cout << "Funkcija (.globl): " << (functionName.empty() ? "(nema)" : functionName) << endl;

	cout << "Memorijske promenljive (.data): ";
	for (Variables::iterator it = memoryVariables.begin(); it != memoryVariables.end(); it++)
	{
		printVariable(*it);
		cout << "  ";
	}
	cout << endl;

	cout << "Registarske promenljive: ";
	for (map<string, Variable*>::iterator it = regVariables.begin(); it != regVariables.end(); it++)
		cout << it->first << " ";
	cout << endl;

	cout << "Instrukcije:" << endl;
	printInstructions(instructions);

	if (!labels.empty())
	{
		cout << "Labele: ";
		for (map<string, Instruction*>::iterator it = labels.begin(); it != labels.end(); it++)
			cout << it->first << " -> [" << it->second->getPosition() << "]  ";
		cout << endl;
	}
	cout << "======================================================================" << endl;
}
