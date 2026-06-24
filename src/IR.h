#pragma once

#include "Types.h"


/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(VariableType type, std::string name, int pos, int value = 0)
		: m_type(type), m_name(name), m_position(pos), m_assignment(no_assign), m_value(value) {}

	VariableType getType() const { return m_type; }
	void setType(VariableType t) { m_type = t; }
	std::string getName() const { return m_name; }
	void setName(std::string name) { m_name = name; }
	int getPosition() const { return m_position; }
	void setPosition(int pos) { m_position = pos; }
	Regs getAssignment() const { return m_assignment; }
	void setAssignment(Regs r) { m_assignment = r; }
	int getValue() const { return m_value; }
	void setValue(int v) { m_value = v; }

private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	int m_value = 0;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src) {}
	Instruction (int pos, InstructionType type, const std::string& asmString) :
		m_position(pos), m_type(type), m_asmString(asmString) {}

	int getPosition() const { return m_position; }
	void setPosition(int pos) { m_position = pos; }
	InstructionType getType() const { return m_type; }
	void setType(InstructionType t) { m_type = t; }

	std::string getAsmString() const { return m_asmString; }
	void setAsmString(const std::string& s) { m_asmString = s; }

	std::string toString();

	Variables& getDst() { return m_dst; }
	Variables& getSrc() { return m_src; }
	Variables& getUse() { return m_use; }
	Variables& getDef() { return m_def; }
	Variables& getIn()  { return m_in; }
	Variables& getOut() { return m_out; }
	std::list<Instruction*>& getSucc() { return m_succ; }
	std::list<Instruction*>& getPred() { return m_pred; }

private:
	int m_position;
	InstructionType m_type;

	std::string m_asmString;

	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;


void printVariable(Variable* v);
void printInstruction(Instruction* instr);
void printInstructions(Instructions& instrs);
