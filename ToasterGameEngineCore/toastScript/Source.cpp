#include "symbols.h"
#include "lexer.h"
#include "parser.h"
#include "codeGenerator.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
	std::ostringstream buf;

	std::ifstream input(path);
	
	buf << input.rdbuf();
	
	return buf.str();
}

#define getTime(val) std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - val).count()

std::string compileToastScript()
{
	std::vector<symbol> symList = lex(readFile("./helloWorld.tst"));
	
	//for (int i = 0; i < symList.size(); ++i)
	//{
	//	std::cout << symList[i].value << std::endl;
	//}

	std::vector<statement> statements = parse(symList);

	//std::cout << generateCode(statements) << std::endl;

	return generateCode(statements);
}