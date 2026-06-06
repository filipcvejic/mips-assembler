#include "IR.h"

#include <iostream>

using namespace std;


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
