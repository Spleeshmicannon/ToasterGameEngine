#pragma once
#include "symbols.h"
#include <vector>

std::vector<statement> parse(std::vector<symbol> &symList)
{
	std::vector<statement> stmnts;

	for (int i = 0; i < symList.size(); ++i)
	{
		switch (symList[i].type)
		{
			case symbolType::DEBUG_MESSAGE:
			{
				statement s;

				s.exprs = new expression();
				s.exprsCount = 1;
				s.exprs->type = exprType::STRING;

				s.subStatements = nullptr;
				s.subStatementsCount = 0;

				s.type = statementType::DEBUG_MESSAGE;

				stmnts.push_back(s);

				break;
			}
			case symbolType::SET: break;
			case symbolType::PAREN_OPEN: break;
			case symbolType::PAREN_CLOSE: break;
			case symbolType::STR_DATA: 
			{
				if (stmnts[stmnts.size() - 1].exprs->type == exprType::STRING)
				{
					stmnts[stmnts.size() - 1].exprs->data = symList[i].value;
				}

				break;
			}
			case symbolType::NEWLINE: break;
		}
	}

	return stmnts;
}