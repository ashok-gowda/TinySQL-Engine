#include <string>
#include "parseTree.h"
#include <locale>

#define LETTER 1
#define DIGIT 2
#define INTEGER 3
#define COMPOP 4
#define TABLENAME 5
#define ATTRIBUTENAME 6
#define COLUMNNAME 7
#define LITERAL 8
#define CREATETABLESTATEMENT 9
#define DATATYPE 10
#define ATTRIBUTETYPELIST 11
#define DROPTABLESTATEMENT 12
#define SELECTSTATEMENT 13
#define SELECTLIST 14
#define SELECTSUBLIST 15
#define TABLELIST 16
#define DELETESTATEMENT 17
#define INSERTSTATEMENT 18
#define INSERTTUPLES 19
#define ATTRIBUTELIST 20
#define VALUE 21
#define VALUELIST 22
#define SEARCHCONDITION 23
#define BOOLEANTERM 24
#define BOOLEANFACTOR 25
#define EXPRESSION 26
#define TERM 27
#define TERMINALS 28

parseTree* createNode(parseTree* parent, int type, char* value);

void deleteNode(parseTree* node);
bool matchString(std::string line, int& index, char* str);

bool isLiteral(std::string line, int& index, parseTree* current);

bool isComparisonOperator(std::string line, int& index, parseTree* current);
bool isInteger(std::string line, int& index, parseTree* current);

bool isName(std::string line, int& index, int type, parseTree* current);
bool isColumnName(std::string line, int& index, parseTree* cur);

bool isDataType(std::string line, int& index, parseTree* current);

bool isTerm(std::string line, int& index, parseTree* current);

bool isExpression(std::string line, int& index, parseTree* current);

bool isBooleanFactor(std::string line, int& index, parseTree* current);

bool isBooleanTerm(std::string line, int& index, parseTree* current);

bool checkSearchCondition(std::string line, int& index, parseTree* current);