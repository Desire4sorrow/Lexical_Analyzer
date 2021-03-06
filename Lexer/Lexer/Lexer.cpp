#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

const string PREV_INT2 = "0b";
const string PREV_INT8 = "0o";
const string PREV_INT16 = "0h";
const char DOT = { '.' };

namespace Lexer {

	struct TokenType
	{
		int lexema;
		string lexemaName;
	};

	vector<TokenType> table_of_lexema;
	enum STATE_OF_TOKEN : int {
		BEGIN = 0,
		ERROR = 1,
		KEYWORD = 2,
		ID = 3,
		INT = 4,
		CHAR = 5,
		STRING = 6,
		REAL = 7,
		FLOAT = 8,
		OPERATION = 9, 
		SEPARATION = 10,
		COMMENT = 11,
		INT2 = 12,
		INT8 = 13,
		INT16 = 14
	};

	const vector<string>KEYWORDS = { "if", "for", "do", "else", "then", "while" };
	const vector<char>INTS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	const vector<char>INTS2 = { '0', '1' };
	const vector<char>INTS8 = { '0', '1', '2', '3', '4', '5', '6', '7' };
	const vector<char>INTS16 = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	const vector<char>SEPARATIONS = { '{', '}','[', ']', '(', ')', ';', ',' };
	const vector<char>OPERATIONS = { '+', '-', '*', '/', '=' };
	const vector<char>LETTERS = { 'a','b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_' };

}

bool isInt2(char ch)
{
	for (int i = 0; i < Lexer::INTS2.size(); ++i)
	{
		if(ch == Lexer::INTS2[i])
			return true;
	}
	return false;
}

bool isInt8(char ch)
{
	for (int i = 0; i < Lexer::INTS8.size(); ++i)
	{
		if (ch == Lexer::INTS8[i])
			return true;
	}
	return false;
}

bool isInt16(char ch)
{
	for (int i = 0; i < Lexer::INTS16.size(); ++i)
	{
		if (ch == Lexer::INTS16[i])
			return true;
	}
	return false;
}

bool isInt(char ch)
{
	for (int i = 0; i < Lexer::INTS.size(); ++i)
	{
		if (ch == Lexer::INTS[i])
			return true;
	}
	return false;
}

bool isLetter(char ch)
{
	for (int i = 0; i < Lexer::LETTERS.size(); ++i)
	{
		if (ch == Lexer::LETTERS[i])
			return true;
	}
	return false;
}

bool isKeyword(string word)
{
	for (int i = 0; i < Lexer::KEYWORDS.size(); ++i)
	{
		if(word == Lexer::KEYWORDS[i])
			return true;
	}
	return false;
}

bool isSeparation(char ch)
{
	for (int i = 0; i < Lexer::SEPARATIONS.size(); ++i)
	{
		if (ch == Lexer::SEPARATIONS[i])
			return true;
	}
	return false;
}

bool isOperation(char ch)
{
	for (int i = 0; i < Lexer::OPERATIONS.size(); ++i)
	{
		if (ch == Lexer::OPERATIONS[i])
			return true;
	}
	return false;
}

void analysisWord(int state, string lexemaName)
{
	if(lexemaName.size() == 1 && isOperation(lexemaName[0]))
	{
		state = Lexer::STATE_OF_TOKEN::OPERATION;
		Lexer::table_of_lexema.push_back({ state, lexemaName });
	}
	if(lexemaName.size() == 1 && isSeparation(lexemaName[0]))
	{
		state = Lexer::STATE_OF_TOKEN::SEPARATION;
		Lexer::table_of_lexema.push_back({ state, lexemaName });
	}
	state = Lexer::STATE_OF_TOKEN::BEGIN;
}

void analysisDigit(int state, string lexemaName)
{
	if(isInt(lexemaName[0]))
	{
		int countDot = 0, countE = 0, countSign = 0, countDigit = 0, dot = 0, eps = 0, sign = 0;
		for (int i = 0; i < lexemaName.size(); ++i)
		{
			if (isInt(lexemaName[i]))
				++countDigit;
			if (lexemaName[i] == DOT)
				++countDot;
			if (lexemaName[i] == 'e')
				++countE;
			if (lexemaName[i] == '+' || lexemaName[i] == '-')
				++countSign;
		}
		if (countDigit == lexemaName.size())
			state = Lexer::STATE_OF_TOKEN::INT;
		if (countDot > 1 || countE > 1 || countSign > 1)
			state = Lexer::STATE_OF_TOKEN::ERROR;
		if(countDot == 1 && countE == 0 && countSign == 0)
		{
			if (lexemaName[lexemaName.size()- 1] == DOT)
				state = Lexer::STATE_OF_TOKEN::ERROR;
			else
				state = Lexer::STATE_OF_TOKEN::REAL;
		}
		if (countDot == 1 && countE == 1 && countSign == 1)
		{
			for (int i = 0; i < lexemaName.size(); ++i)
			{
				if (lexemaName[i] == DOT)
					dot = i;
				if (lexemaName[i] == 'e')
					eps = i;
				if (lexemaName[i] == '+' || lexemaName[i] == '-')
					sign = i;
			}
			if (dot < eps && eps == sign - 1)
				state = Lexer::STATE_OF_TOKEN::FLOAT;
			else
				state = Lexer::STATE_OF_TOKEN::ERROR;
		}
		Lexer::table_of_lexema.push_back({ state, lexemaName });
		state = Lexer::STATE_OF_TOKEN::BEGIN;
	}
	else
	{
		analysisWord(state, lexemaName);
	}
}

void analysisLexema(int state, string lexemaName) 
{
	if (isLetter(lexemaName[0])) 
	{
		if (isKeyword(lexemaName))
		{
			state = Lexer::STATE_OF_TOKEN::KEYWORD;
			Lexer::table_of_lexema.push_back({ state, lexemaName });
		}
		else
		{
			for (int i = 0; i < lexemaName.size(); ++i)
			{
				if ((!isLetter(lexemaName[i])) && (!isInt(lexemaName[i])))
				{
					state = Lexer::STATE_OF_TOKEN::ERROR;
				}
			}
			if (state != Lexer::STATE_OF_TOKEN::ERROR)
			{
				state = Lexer::STATE_OF_TOKEN::ID;
				Lexer::table_of_lexema.push_back({ state, lexemaName });
			}
			else
			{
					Lexer::table_of_lexema.push_back({ state, lexemaName });
					state = Lexer::STATE_OF_TOKEN::BEGIN;
			}
		}
	}
	else
	{
		state = Lexer::STATE_OF_TOKEN::BEGIN;
		bool key = false;
		if (lexemaName.size() > 2)
		{
			string substr = lexemaName.substr(0, 2);
			if (substr == PREV_INT2)
			{
				for (int i = 2; i < lexemaName[i]; ++i)
				{
					if (isInt2(lexemaName[i]))
					{
						state = Lexer::STATE_OF_TOKEN::INT2;
					}
					else
					{
						key = true;
					}
				}
				if (key)
				{
					state = Lexer::STATE_OF_TOKEN::ERROR;
				}
				Lexer::table_of_lexema.push_back({ state, lexemaName });
			}




			else if (substr == PREV_INT8)
			{
				for (int i = 2; i < lexemaName[i]; ++i)
				{
					if (isInt8(lexemaName[i]))
					{
						state = Lexer::STATE_OF_TOKEN::INT8;
					}
					else
					{
						key = true;
					}
				}
				if (key)
				{
					state = Lexer::STATE_OF_TOKEN::ERROR;
				}
				Lexer::table_of_lexema.push_back({ state, lexemaName });
			}
			else if (substr == PREV_INT16)
			{
				for (int i = 2; i < lexemaName[i]; ++i)
				{
					if (isInt16(lexemaName[i]))
					{
						state = Lexer::STATE_OF_TOKEN::INT16;
					}
					else
					{
						key = true;
					}
				}
				if (key)
				{
					state = Lexer::STATE_OF_TOKEN::ERROR;
				}
				Lexer::table_of_lexema.push_back({ state, lexemaName });
			}
			else
			{
				analysisDigit(state, lexemaName);
			}
		}
		else
		{
			analysisDigit(state, lexemaName);
		}
	}
}



int main()
{
	ifstream inFile;
	ofstream outFile;
	inFile.open("input.txt");
	outFile.open("output.txt");
	string lex = "";


	if (!inFile.is_open()) {
		cout << "File not found!" << endl;
		return 1;
	}

	int state = Lexer::STATE_OF_TOKEN::BEGIN;
	while (!inFile.eof()) {

		string str = "";
		getline(inFile, str);
		string lex = "";
		for(int i = 0; i <= str.size(); ++i) 
		{
			if (str[i] == ' ' || i == str.size()) 
			{
				lex = str.substr(0, i);
				bool isStr = false;
				if(lex[0] == '/' && lex[1] == '/')
				{
					lex = str.substr(0, str.size());
					string comment = "//";
					state = Lexer::STATE_OF_TOKEN::COMMENT;
					Lexer::table_of_lexema.push_back({ state, comment });
					lex = "";
					str = "";
					getline(inFile, str);
					isStr = true;
					i = 0;
				}
				if (lex[0] == '/' && lex[1] == '*')
				{
					lex = str.substr(0, str.size());
					bool found = false;
					for (int j = 0; j < lex.size(); ++j)
					{
						if (lex[j] == '*' && lex[j + 1] == '/')
						{
							found = true;
						}
						if (!found && j == lex.size()-1)
						{
							getline(inFile, str);
							lex = str.substr(0, str.size());
							j = 0;
						}
						if(found)
						{
							string comm = "/* */";
							state = Lexer::STATE_OF_TOKEN::COMMENT;
							Lexer::table_of_lexema.push_back({ state, comm });
							lex.erase(0, j + 3);
							str.erase(0, j + 3);
							j = lex.size();
							lex = lex.substr(0, lex.size());
						}
					}
				}
				if (lex[0] == '"') 
				{
					lex = str.substr(0, str.size());
					int k = 0, j = 0;
					for (int m = 1; m < lex.size(); ++m)
					{
						if (j < 1 && lex[m] == '"')
						{
							k = m;
							++j;
						}
					}
					string substr = str.substr(0, k + 1);
					if (j == 1 && substr.size() == 3)
					{
						state = Lexer::STATE_OF_TOKEN::CHAR;
						Lexer::table_of_lexema.push_back({ state, substr });
						lex.erase(0, k+2);
						str.erase(0, k+2);
					}
					else {
						if (j == 1 && substr.size() == 3 && k == lex.size() - 1)
						{
							state = Lexer::STATE_OF_TOKEN::CHAR;
							Lexer::table_of_lexema.push_back({ state, substr });
							i = str.size();
						}
						else {
							if (j == 1 && k == lex.size() - 1) {
								state = Lexer::STATE_OF_TOKEN::STRING;
								Lexer::table_of_lexema.push_back({ state, lex });
							}
							else {
								substr = lex.substr(0, k + 1);
								state = Lexer::STATE_OF_TOKEN::STRING;
								Lexer::table_of_lexema.push_back({ state, substr });
								lex.erase(0, k+2);
								str.erase(0, k+2);
								i = str.size();

							}
						}
					}
				}
				else
				{
					cout << lex <<"   "<< str << endl;
					if (!isStr) {
						str.erase(0, i + 1);
					}
					analysisLexema(state, lex);
					i = 0;
				}
			}
		}
		analysisLexema(state, str);
	}

	inFile.close();

	for (int i = 0; i < Lexer::table_of_lexema.size(); ++i) {
		switch (Lexer::table_of_lexema[i].lexema)
		{
		case Lexer::STATE_OF_TOKEN::ID:
			outFile << "Position: "<< __LINE__ << " " << Lexer::table_of_lexema[i].lexema << " indentifier: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::KEYWORD:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " keyword: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::INT:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " integer: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::CHAR:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " char: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::STRING:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " string: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::REAL:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " real: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::FLOAT:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " float: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::SEPARATION:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " separation: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::OPERATION:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " operation: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::ERROR:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " error: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::COMMENT:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " comment: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::INT2:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " binary num: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::INT8:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " octal num: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		case Lexer::STATE_OF_TOKEN::INT16:
			outFile << "Position: " << Lexer::table_of_lexema[i].lexema << " hex num: " << Lexer::table_of_lexema[i].lexemaName << endl;
			break;
		default:
			break;
		}
	}

	outFile.close();

    return 0;
}