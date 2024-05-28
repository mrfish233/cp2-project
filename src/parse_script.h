#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parse_file.h"
#include "script.h"

typedef enum _ScriptTable {
    TABLE_NONE = 0,
    TABLE_PLAYER,
    TABLE_CHARACTER,
    TABLE_STATUS_INFO,
    TABLE_STATUS,
    TABLE_ITEM,
    TABLE_SCENE,
    TABLE_EVENT,
    TABLE_DIALOGUE,
    TABLE_OPTION,
    TABLE_CONDITION,
    TABLE_ERROR
} ScriptTable;

typedef struct _TableField {
    char name[STR_SIZE];
    char value[STR_SIZE];
} TableField;

typedef struct _ScriptTableIndex {
    ScriptTable table;
    int32_t index;

    ScriptTable sub_table;
    int32_t sub_index;
} ScriptTableIndex;

int32_t parseTomlScript(char *filename, Script *script);

int32_t changeScriptTable(Script *script, ScriptTableIndex *tableIndex, char *buffer);

int32_t updateTable(Script *script, ScriptTableIndex *tableIndex);

int32_t createNewTable(void **table, size_t size, int32_t capacity);

int32_t createIDField(TableField *field, ScriptTableIndex *tableIndex, char *buffer);

int32_t addDataToTable(Script *script, ScriptTableIndex *tableIndex, TableField *field);

bool isEmptyOrCommentLine(char *buffer);

bool isTableLine(char *buffer);

int32_t getTableLineStr(char *str, char *buffer, int32_t pos);

void cleanWhiteSpace(char **buffer);
