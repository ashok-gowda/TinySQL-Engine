#include "..\..\Operations.h"
#include "..\Parser\statement.h"
#include "..\..\JoinCondition.h"
#include "..\..\ExpressionStack.h"

void executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, statement* statementObj, std::ofstream& fWriteExecute, Relation* selectStmtOutput = NULL, bool returnOutput=false);

void executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, parseTree* pTree, std::ofstream& fWriteExecute, Relation* selectStmtOutput = NULL, bool returnOutput = false);