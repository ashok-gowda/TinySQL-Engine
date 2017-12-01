#include<iostream>
#include<fstream>
#include<iterator>
#include<cstdlib>
#include<string>
#include<ctime>
#include <stack>
#include<math.h>
#include "StorageManager\Block.h"
#include "StorageManager\Config.h"
#include "StorageManager\Disk.h"
#include "StorageManager\Field.h"
#include "StorageManager\MainMemory.h"
#include "StorageManager\Relation.h"
#include "StorageManager\Schema.h"
#include "StorageManager\SchemaManager.h"
#include "StorageManager\Tuple.h"
#include "JoinCondition.h"


Relation * createTable(SchemaManager& schema_manager, string relation_name, vector<string> field_names, vector<enum FIELD_TYPE> field_types, std::ofstream& fWriteExecute);

void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple);

bool insertTable(string tableName, SchemaManager& schema_manager, std::map<string, string> map_of_attributes, MainMemory& mem, std::ofstream& fWriteExecute);

bool dropTable(string table_name, ofstream &file_output, SchemaManager &schema_manager);

void verifySchema(Schema schema, vector<vector<JoinCondition*>> &listOflistOfJoinConditions, string table_name);

int getValueFromConversionOfPrefixToInfix(vector<OperandOperator *> vectorOfOperands, Tuple& tuple);

bool checkIfTupleSatisfiesConditions(Tuple& tuple, Schema& schema, vector<vector<JoinCondition*>> &listOflistOfJoinConditions);

vector<Relation*> sortSubList(Relation* relationPtr, SchemaManager &schema_manager, int sizeOfSubList, vector<OperandOperator*>& attributesList, MainMemory& mem, bool& success, std::ofstream& fWriteExecute);

Relation * sortOperation(vector<Relation*>  vectorOfSubLists, SchemaManager& schema_manager, MainMemory& mem, vector<string> fieldName);

Relation * removeDuplicatesOperation(vector<Relation*>  vectorOfSubLists, SchemaManager& schema_manager, MainMemory& mem, vector<string> fieldName);

Relation* selectTable(string table_name, SchemaManager &schema_manager, vector<vector<JoinCondition*>> &listOfJoinConditions, MainMemory& mem, vector<OperandOperator*> &projectionList, bool renameSchema, std::ofstream& fWriteExecute);

bool deleteTable(string table_name, SchemaManager &schema_manager, vector<vector<JoinCondition*>> &listOfJoinConditions, MainMemory& mem, std::ofstream& fWriteExecute);

Relation* cartesianProductOnePass(vector<Relation*>&  tables, SchemaManager& schema_manager, MainMemory& mem, vector<vector<JoinCondition*>> &listOfJoinConditions, std::ofstream& fWriteExecute);

Relation* cartesianProductOnePass(vector<string>&  tablesNames, SchemaManager& schema_manager, MainMemory& mem, vector<vector<JoinCondition*>> &listOfJoinConditions, std::ofstream& fWriteExecute);


Relation * joinTables(vector<Relation*>& subListsOfTable1, vector<Relation*>& subListsOfTable2,
	SchemaManager &schema_manager, MainMemory &mem, vector<JoinCondition*>& joinConditions, Relation *joinedTable);

Relation * createSchemaForJoinTable(vector<Relation*> subListsOfTable1, vector<Relation*> subListsOfTable2, SchemaManager& schemaManager);

Relation * onePassDistinct(Relation * table_name, MainMemory& mem, SchemaManager& schema_manager, vector<OperandOperator*>& attributesList, std::ofstream& fWriteExecute);

Relation * onePassOrdering(Relation * table_name, MainMemory& mem, SchemaManager& schema_manager, vector<OperandOperator*>& attributesList, std::ofstream& fWriteExecute);








