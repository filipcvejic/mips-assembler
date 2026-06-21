#include "IR.h"

#include <iostream>
#include <string>

using namespace std;


/* Naziv stvarnog MIPS registra za dodeljenu boju (t0..t3). */
static string getRegName(Variable* v)
{
	return "$t" + to_string((int)v->getAssignment() - (int)t0);
}


/* Popunjava sablon instrukcije: `d -> dst registri redom, `s -> src registri redom. */
string Instruction::toString()
{
	string result;
	Variables::iterator di = m_dst.begin();
	Variables::iterator si = m_src.begin();

	for (size_t i = 0; i < m_asmString.size(); i++)
	{
		if (m_asmString[i] == '`' && i + 1 < m_asmString.size())
		{
			if (m_asmString[i + 1] == 'd') { result += getRegName(*di); ++di; i++; continue; }
			if (m_asmString[i + 1] == 's') { result += getRegName(*si); ++si; i++; continue; }
		}
		result += m_asmString[i];
	}

	return result;
}


static void printVariableNames(Variables& vars)
{
	cout << "{ ";
	for (Variables::iterator it = vars.begin(); it != vars.end(); it++)
		cout << (*it)->getName() << " ";
	cout << "}";
}


static void printInstrPositions(std::list<Instruction*>& instrs)
{
	cout << "[";
	for (std::list<Instruction*>::iterator it = instrs.begin(); it != instrs.end(); it++)
		cout << (*it)->getPosition() << " ";
	cout << "]";
}


void printVariable(Variable* v)
{
	if (v == nullptr)
		return;

	cout << v->getName();
	if (v->getType() == Variable::MEM_VAR)
		cout << " = " << v->getValue();
}


void printInstruction(Instruction* instr)
{
	if (instr == nullptr)
		return;

	// pozicija + asm sablon (jos uvek sa `d / `s, pre alokacije registara)
	cout << "  [" << instr->getPosition() << "] " << instr->getAsmString();

	cout << "   DEF=";
	printVariableNames(instr->getDef());
	cout << " USE=";
	printVariableNames(instr->getUse());
	cout << "  IN=";
	printVariableNames(instr->getIn());
	cout << " OUT=";
	printVariableNames(instr->getOut());
	cout << "  SUCC=";
	printInstrPositions(instr->getSucc());
	cout << " PRED=";
	printInstrPositions(instr->getPred());
	cout << endl;
}


void printInstructions(Instructions& instrs)
{
	for (Instructions::iterator it = instrs.begin(); it != instrs.end(); it++)
		printInstruction(*it);
}
