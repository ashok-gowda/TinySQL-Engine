#include "..\..\Operations.h"
#include "..\Parser\statement.h"

void executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, statement* statementObj, std::ofstream& fWriteExecute);