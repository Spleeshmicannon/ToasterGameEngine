#pragma once
#include "symbols.h"
#include <vector>

std::string generateCode(std::vector<statement> statements)
{
	std::string includes = "";
	std::string main = "int main() {";
	bool stdio = false;

	for (int i = 0; i < statements.size(); ++i)
	{
		switch (statements[i].type)
		{
			case statementType::DEBUG_MESSAGE: 
				if (!stdio)
				{
					stdio = true;
					includes += "#include <stdio.h>\n";
				}
				main += "printf(\"" + statements[i].exprs->data + "\");";
				break;
		}
	}

	main += "return 0;}";
	return includes + main;
}