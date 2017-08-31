#include <cctype>
#include <set>
#include "lexAnalyzer.h"
#include "conColor.h"

using namespace std;

int typeColor[] =
{
	COLOR_BLUE,
	COLOR_LIGHT,
	COLOR_GREY,
	COLOR_CYAN,
	COLOR_WHITE,
	COLOR_WHITE,
	COLOR_RED,
	COLOR_GREEN
};
//#define TYPE_KEY 0
//#define TYPE_VAR 1
//#define TYPE_CON 2
//#define TYPE_OPR 3
//#define TYPE_DEL 4
//#define TYPE_FMT 5
//#define TYPE_ERR 6
//#define TYPE_NOTE 7

set<string>reserved = { "int", "real", "string", "while", "if", "else",
		"out", "in", "return" };

lexAn::lexAn()
{
	return;
}

int lexAn::getCharType(char x)                  
{
	if (isdigit(x))
	{
		return CHAR_DIGIT;
	}
	if (isalpha(x))
	{
		return CHAR_ALPHA;
	}
	if (x == '.')
	{
		return CHAR_DOT;
	}
	if (x == '=')
	{
		return CHAR_EQUAL;
	}
	if (x == '>')
	{
		return CHAR_LARGE;
	}
	if (x == '<')
	{
		return CHAR_SMALL;
	}
	if (x == '\\')              //转义符
	{
		return CHAR_ESCAP;
	}
	if (x == ' ' || x == '\t' || x == '\n' || x == '\0')
	{
		return CHAR_SPACE;
	}
	if (x == '"')
	{
		return CHAR_QUOTE;
	}
	if (x == '/')
	{
		return CHAR_DIVIS;
	}
	if (DEBUG)
	{
		cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";
	}
	return CHAR_OPERA;
	return 0;
}

int lexAn::getTokenType(int status)
{
	switch (status)
	{
	case CHAR_DIVIS | 1:  
	case CHAR_DIVIS | 2:	//note
		return TYPE_NOTE;
	case CHAR_SPACE:		//' ', '\n', '\t', '\0'
		return TYPE_FMT;
	case CHAR_DIGIT:		//integer
	case CHAR_DIGIT | 1:	//real
	case CHAR_QUOTE | 1:	//string
		return TYPE_CON;
	case CHAR_ALPHA:		//variable
		return TYPE_VAR;
	case CHAR_OPERA:		//operator
	case CHAR_EQUAL:		//'='
	case CHAR_EQUAL | 1:	//"=="
	case CHAR_EQUAL | 2:	//"<="
	case CHAR_EQUAL | 3:	//">="
	case CHAR_SMALL:		//'<'
	case CHAR_SMALL | 1:	//'<>'
	case CHAR_LARGE:		//'>'
	case CHAR_DIVIS:		//'/'
		return TYPE_OPR;
	}
	return 0;
}

vector<token> lexAn::analyze(string inputData)
{
	vector<token> tokenList;
	int chartype;
	int status = CHAR_SPACE;
	string save = "";

	for (char i : inputData)
	{

		// if (status == (CHAR_DIVIS | 1))//notes status
		// {
		// 	save += i;
		// 	if (i == '\n')
		// 	{
		// 		status = (CHAR_DIVIS | 2);
		// 	}
		// 	continue;
		// }

		chartype = lexAn::getCharType(i);

		switch (chartype)
		{
		case CHAR_SPACE:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_SPACE;
			}
			break;

		case CHAR_DIGIT:
			switch (status)
			{
			case CHAR_DIGIT:
			case (CHAR_DIGIT | 1):
			case CHAR_ALPHA:
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_ESCAP:
			    cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";  //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_DIGIT;
			}
			break;

		case CHAR_ALPHA:
			switch (status)
			{
			case CHAR_ALPHA:
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case (CHAR_DIGIT | 1):
			case CHAR_DIGIT:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";    //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_ALPHA;
			}
			break;

		case CHAR_OPERA:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_DIVIS:
			case CHAR_EQUAL:		//'='
			case CHAR_EQUAL | 1:	//"=="
			case CHAR_EQUAL | 2:	//"<="
			case CHAR_EQUAL | 3:	//">="
			case CHAR_SMALL:		//'<'
			case CHAR_SMALL | 1:	//'<>'
			case CHAR_LARGE:
			case CHAR_ESCAP:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";     //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_OPERA;
			}
			break;

		case CHAR_ENTER:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
				status = (CHAR_DIVIS | 2); //注释结束只有换行这一种可能
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				break;
			}
			break;

		case CHAR_EQUAL:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_EQUAL:
				save += i;
				status = (CHAR_EQUAL | 1);
				break;
			case CHAR_SMALL:
				save += i;
				status = (CHAR_EQUAL | 2);
				break;
			case CHAR_LARGE:
				save += i;
				status = (CHAR_EQUAL | 3);
			case CHAR_DIGIT:		//integer
			case CHAR_DIGIT | 1:	//real
			case CHAR_QUOTE | 1:
			case CHAR_OPERA:
			case CHAR_ESCAP:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";   //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_EQUAL;
			}
			break;

		case CHAR_LARGE:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_SMALL:
				save += i;
				status = (CHAR_SMALL | 1);
				break;
			case CHAR_OPERA:
			case CHAR_EQUAL:
			case CHAR_ESCAP:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";  //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_LARGE;
			}
			break;

		case CHAR_SMALL:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_OPERA:
			case CHAR_LARGE：
			case CHAR_ESCAP:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";  //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_SMALL;
			}
			break;

		case CHAR_DIVIS:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_DIVIS:
				save += i;
				status = (CHAR_DIVIS | 1);
				break;
			case CHAR_OPERA:
			case CHAR_EQUAL:		//'='
			case CHAR_EQUAL | 1:	//"=="
			case CHAR_EQUAL | 2:	//"<="
			case CHAR_EQUAL | 3:	//">="
			case CHAR_SMALL:		//'<'
			case CHAR_SMALL | 1:	//'<>'
			case CHAR_LARGE:
				cout << "DEBUG: char\'" << x << "\' classificated as type \"operate\"\n";  //报错
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_DIVIS;
			}
			break;

		case CHAR_ESCAP:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				status = CHAR_ESCAP;
				break;
			case CHAR_ESCAP:
				save += i;
				status = CHAR_QUOTE;
				break;
			}
			break;

		case CHAR_QUOTE:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
				save += i;
				status = (CHAR_DIVIS | 1);
				break;
			case CHAR_QUOTE:
				save += i;
				status = (CHAR_QUOTE | 1);
				break;
			case CHAR_ESCAP:
				save += i;
				status = CHAR_QUOTE;
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_QUOTE;
			}
			break;

		case CHAR_DOT:
			switch (status)
			{
			case (CHAR_DIVIS | 1):
			case CHAR_QUOTE:
				save += i;
				break;
			case CHAR_DIGIT:
				save += i;
				status = (CHAR_DIGIT | 1);
				break;
			default:
				tokenList.push_back(token(getTokenType(status), save));
				save = "";
				save += i;
				status = CHAR_DIGIT;	
			}
			break;
		}		
	}
	for (auto i = tokenList.begin();i != tokenList.end();i++)
	{
		i->trans();
	}
	return tokenList;
}

token::token(int type, string val)
{
	this->type = type;
	this->val = val;
	return;
}

void token::print()
{
	if (COLOR)
	{
		setColor(typeColor[type]);
	}
	cout << val;
	return;
}

void token::trans()      //从变量名中将保留字分出来
{
	if (this->type == TYPE_VAR && reserved.find(this->val) != reserved.end())
	{
		this->type = TYPE_KEY;
	}
	return;
}