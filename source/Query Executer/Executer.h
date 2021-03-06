#include "..\..\Operations.h"
#include "..\Parser\statement.h"
#include "..\..\JoinCondition.h"
#include "..\..\ExpressionStack.h"

bool executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, statement* statementObj, std::ofstream& fWriteExecute, vector<Relation*>& selectStmtOutput = vector<Relation*>(), bool returnOutput=false);

bool executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, parseTree* pTree, std::ofstream& fWriteExecute, vector<Relation*>& selectStmtOutput = vector<Relation*>(), bool returnOutput = false);