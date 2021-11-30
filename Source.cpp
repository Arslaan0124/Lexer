#include<iostream>
#include<fstream>
using namespace std;
#include"Lexer.h"

void LexerInit(string* fileArgs)
{
	Lexer lexer;							//Instantiating lexer object.
	lexer.init(fileArgs);					//Initializing lexer with files containg keywords,operators and such.
	lexer.initMap("map.txt");				//Initialzing  the key-value map of symbol-index values.
	lexer.GetTokens("input.txt");			//Get tokens function is passed a file and it populates the following:
											//1 . symbolTable.txt (containing symbol table entries)
											//2 . tokens.txt (containing tokens)
											//3 . errors.txt (containing errors) 
}



int main()
{
	string fileArgs[] = { "keywords.txt","Operators.txt","Punctuation.txt","mapping.txt" };
	LexerInit(fileArgs);
	return 0;
}