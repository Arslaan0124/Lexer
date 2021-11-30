#pragma once
#include<iostream>
#include<string>
#include <algorithm>
#include <vector>
#include <map>
#include<fstream>
using namespace std;


#define Letter 'L'
#define Number 'N'
#define maxCol 24

struct Pos {
	int row = 0;
	int col = 0;
};

class Reader
{
public:
	ifstream read;
	Pos pos;

	Reader()
	{

	}
	char lookAhead()
	{
		return read.peek();
	}
	char getNext()
	{
		char d = read.get();
		if (d == '\n')
		{
			pos.row++;
			pos.col = 0;
		}
		else
			pos.col++;
		return d;
	}
	void goBackOne()
	{
		//row, col checks later..
		read.unget();
	}
	bool isSrcEof()
	{
		return read.eof();
	}


};

class Lexer
{
private:
	Reader reader;
	vector<string>symbolTable;

public:

	vector<string> Keywords;
	vector<string> Punctuation;
	vector<string> Operators;
	vector<vector<int>> TransitionTable;
	map<char, int> mmap;


	Lexer()
	{

	}
	void init(string* files)
	{
		this->initKeywords(files[0]);
		this->initOperators(files[1]);
		this->initPunctuation(files[2]);
		this->initTTable(files[3]);
	}
	void initKeywords(string fileName)
	{
		reader.read.open(fileName);
		if (!reader.read.is_open())
			cerr << "Unable to open file : " << fileName << endl;
		string dummy;
		while (reader.read >> dummy)
			Keywords.push_back(dummy);
		reader.read.close();
	}
	void initOperators(string fileName)
	{
		reader.read.open(fileName);
		string dummy;
		while (reader.read >> dummy)
			Operators.push_back(dummy);
		reader.read.close();
	}
	void initPunctuation(string fileName)
	{
		reader.read.open(fileName);
		string dummy;
		while (reader.read >> dummy)
			Punctuation.push_back(dummy);
		reader.read.close();
	}
	void initTTable(string fileName)
	{
		reader.read.open(fileName);
		if (!reader.read.is_open())
			cerr << "Unable to open file :" << fileName << endl;
		int row = 0;
		int col = 0;
		reader.read >> row;
		reader.read >> col;
		int temp = 0;
		for (int i = 0; i < row; i++)
		{
			vector<int>dummy;
			for (int j = 0; j < col; j++)
			{
				reader.read >> temp;
				dummy.push_back(temp);
			}
			TransitionTable.push_back(dummy);
		}

		reader.read.close();
	}
	void initMap(string fileName)
	{
		char d;
		int t = 0;
		reader.read.open(fileName);
		while (!reader.read.eof())
		{
			reader.read >> t;
			reader.read >> d;
			mmap.insert({ d,t });
		}

		reader.read.close();
	}
	void ShowMap()
	{
		for (auto i = mmap.begin(); i != mmap.end(); i++)
		{
			cout << i->first << " " << i->second << endl;
		}
	}
	void ShowTTable()
	{
		for (int i = 0; i < TransitionTable.size(); i++)
		{
			for (int j = 0; j < TransitionTable[i].size(); j++)
			{
				cout << TransitionTable[i][j] << " ";
			}
			cout << endl;
		}
	}
	void ShowKeywords()
	{
		for (auto i = Keywords.begin(); i != Keywords.end(); i++)
			cout << *i << " ";
	}
	void ShowOperators()
	{
		for (auto i = Operators.begin(); i != Operators.end(); i++)
			cout << *i << " ";
	}
	void ShowPunctuation()
	{
		for (auto i = Operators.begin(); i != Operators.end(); i++)
			cout << *i << " ";
	}
	void initSrc(string fileName)
	{
		//call at the end.
		reader.pos.row = 0;
		reader.pos.col = 0;
		reader.read.open(fileName);
		if (!reader.read.is_open())
			cerr << "Unable to open file :" << fileName << endl;
		reader.read.close();
	}
	char ConvertToPseudo(char iB,char preIb)
	{
		if (isalpha(iB) && !isdigit(preIb))
			return 'L';
		else if (isalpha(iB) && isdigit(preIb))
			return 'E';
		else if (isdigit(iB))
			return 'N';
		else
			return iB;
	}
	bool IsFinalState(int currentState,char iB)
	{
		if(currentState != -1)
			return (TransitionTable[currentState][maxCol - 1] || iB == ' ' || iB == '\n');
		return false;
	}
	void Insert_SymbolTable(string lex)
	{
		if (find(symbolTable.begin(), symbolTable.end(), lex) == symbolTable.end())
			symbolTable.push_back(lex);
	}
	bool IsDigit(string s)
	{
		bool error = false;
		//-2.2E+4     
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] == '-' || s[i] == '+')
			{
				if (error)
					return false;
				error = true;
			}
			else if (isdigit(s[i]))
			{
				error = false;
			}
			else if (s[i] == '.')
			{
				if (error)
					return false;
				error = true;
			}
			else if (s[i] == 'E' && (s[i+1] == '+' || s[i+1] == '-'))
			{
				if (error)
					return false;
				error = true;
				i++;
			}
			else
			{
				return false;
			}
		}
		return !error;
	}
	int getSymbolTableEntry(string lex)
	{
		for (int i = 0; i < symbolTable.size(); i++)
			if (symbolTable[i] == lex)
				return i;
		return -1;
	}
	bool isStringVariable(string s)
	{
		if (std::find(Keywords.begin(), Keywords.end(), s) != Keywords.end() || (s.find('_') != std::string::npos))
			return true;
		return false;
	}
	void Tokenization(vector<string> lexemes)
	{
		ofstream write("tokens.txt");
		ofstream writeError("errors.txt");
		ofstream writeSymbolTable("symbolTable.txt");

		int count = 0;
		for (auto i = lexemes.begin(); i != lexemes.end(); i++)
		{
			string s = *i;
			int ind = getSymbolTableEntry(s);

			if (std::find(Operators.begin(), Operators.end(), s) != Operators.end())
			{
				if(ind != -1)
					write <<"< "<<ind << ", Operator "  << " >" << endl;
				else
					write << "< " << s << " , Operator " << " >" << endl;
			}
			else if (std::find(Punctuation.begin(), Punctuation.end(), s) != Punctuation.end())
			{
				if (ind != -1)
					write << "< " << ind << ", Punctuation, " << " >" << endl;
				else
					write << "< " << s << " , Punctuation " << " >" << endl;
			}
			else if (std::find(Keywords.begin(), Keywords.end(), s) != Keywords.end())
			{
				if (ind != -1)
					write << "< " << ind << ", Keyword " << " >" << endl;
				else
					write << "< "<< " , Keyword " << " >" << endl;
				
			}
			else if (s.find('_') != std::string::npos)
			{
				if (ind != -1)
					write << "< " << ind << " , Identifier " << " >" << endl;
				else
					write << "< "<< " , Identifier " << " >" << endl;
			}
			else if(IsDigit(s))
			{
				if (ind != -1)
					write << "< " << ind << ", Number " << " >" << endl;
				else
					write << "< " <<s<< " , Number " << " >" << endl;
			}
			else
			{
				   writeError  << " < Error, " << s << " >" << endl;
			}

			count++;
		}
		count = 0;
		for (auto i = symbolTable.begin(); i != symbolTable.end(); i++)
			writeSymbolTable <<"< "<< count++<<", "<< *i <<" >"<< endl;
	}
	void GetTokens(string fileName)
	{
		reader.read.open(fileName);
		int currentState = 0;
		int prevState = currentState;
		char iB = '\0';
		char preIb = '\0';
		vector<string> lexemes;
		string lexeme;

		while (!reader.read.eof())
		{
			preIb = iB;
			iB = reader.getNext();
			if (currentState == 0 && (iB == ' ' || iB == '\n'))
				continue;
			int mapIndex = mmap[ConvertToPseudo(iB,preIb)];
			prevState = currentState;
			currentState = TransitionTable[currentState][mapIndex];
			if (currentState == -1)
				currentState = prevState;
			if (IsFinalState(currentState,iB))
			{
				if (prevState != 0)
					reader.goBackOne();
				else
					lexeme += iB;
				prevState = currentState = 0;
				if(isStringVariable(lexeme))
					Insert_SymbolTable(lexeme);
				lexemes.push_back(lexeme);
				lexeme = "";
			}
			else
				lexeme += iB;
				
		}
		Tokenization(lexemes);
	}
};