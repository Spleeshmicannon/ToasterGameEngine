#pragma once
#include <string>

constexpr const char* SYM_SET = "set";
constexpr const char* SYM_DEBUG_MESSAGE = "print";
constexpr const char* SYM_PAREN_OPEN = "(";
constexpr const char* SYM_PAREN_CLOSE = ")";
constexpr const char* SYM_WHITESPACE = " ";
constexpr const char* SYM_STR0 = "'";
constexpr const char* SYM_STR1 = "\"";
constexpr const char* SYM_NEWLINE = "\n";

enum class symbolType
{
	SET,
	PAREN_OPEN,
	PAREN_CLOSE,
	STR_DATA,
	NEWLINE,
	DEBUG_MESSAGE
};

struct symbol
{
	const symbolType type;
	const std::string value;
	symbol(const symbolType& _st, const std::string& _value) : type(_st), value(_value)
	{}
};

enum class statementType
{
	DEBUG_MESSAGE,
	SET,
	IF,
	ENDIF
};

enum class exprType
{
	STRING,
	NUMBER,
	IDENTIFIER
};

struct expression
{
	std::string data;
	exprType type;
};

struct statement
{
	expression* exprs;
	statement* subStatements;

	size_t exprsCount;
	size_t subStatementsCount;

	statementType type;
};
