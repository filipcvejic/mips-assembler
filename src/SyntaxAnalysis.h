#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

#include <map>
#include <string>

/**
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
 *      | and  rid , rid , rid
 *      | xor  rid , rid , rid
 *      | bgez rid , id
 */
class SyntaxAnalysis
{
public:
	SyntaxAnalysis(LexicalAnalysis& lex);

	bool Do();

	/* IR */
	Instructions& getInstructions()                         { return instructions; }
	Variables& getMemoryVariables()                         { return memoryVariables; }
	std::map<std::string, Variable*>& getRegVariables()     { return regVariables; }
	Variables& getRegisterVariableList()                    { return registerVariables; }
	std::map<std::string, Instruction*>& getLabels()        { return labels; }
	std::string getFunctionName() const                     { return functionName; }

	void printIR();

private:
	/* Parser */
	void printSyntaxError(Token token);
	void eat(TokenType t);
	Token getNextToken();
	TokenType peekNextTokenType();

	void Q();
	void S();
	void L();
	void E();

	/* IR */
	Variable* getOrCreateReg(const std::string& name);
	void addMemoryVariable(const std::string& name, int value);
	void emit(InstructionType type, const std::string& asmTemplate,
	          Variable* dst = nullptr, Variable* src1 = nullptr, Variable* src2 = nullptr);
	void buildControlFlowGraph();

	LexicalAnalysis& lexicalAnalysis;

	bool errorFound;

	TokenList::iterator tokenIterator;

	Token currentToken;

	Instructions instructions;
	std::map<std::string, Variable*> regVariables;
	Variables registerVariables;
	Variables memoryVariables;
	std::map<std::string, Instruction*> labels;
	std::string functionName;
	std::string pendingLabel;
	std::map<Instruction*, std::string> branchTargets;
};
