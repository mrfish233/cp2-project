#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parse_file.h"
#include "script.h"

/**
 * Script table enum, contains all the tables in the script.
 * @param TABLE_NONE: no table
 * @param TABLE_PLAYER: player table
 * @param TABLE_CHARACTER: character table
 * @param TABLE_STATUS_INFO: status info table
 * @param TABLE_STATUS: status table
 * @param TABLE_ITEM: item table
 * @param TABLE_SCENE: scene table
 * @param TABLE_EVENT: event table
 * @param TABLE_DIALOGUE: dialogue table
 * @param TABLE_OPTION: option table
 * @param TABLE_CONDITION: condition table
 * @param TABLE_ERROR: error
 */
typedef enum _TableName {
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
} TableName;

/**
 * Script table index struct, contains the table and index.
 * @param table: the table
 * @param index: the index
 * @param sub_table: the sub table
 * @param sub_index: the sub index
 */
typedef struct _Table {
    TableName table_name;
    int32_t index;

    TableName sub_table_name;
    int32_t sub_index;

    char field[STR_SIZE];
    char value[STR_SIZE];
} Table;

/**
 * Parse a TOML script file.
 * @param filename: the script file name
 * @param script: the script to store the parsed data
 * @return 0 if the script is parsed successfully, 1 otherwise.
 */
int32_t parseTomlScript(char *filename, Script *script);

/**
 * Change current script table to be parsed.
 * @param script: the script to store the parsed data
 * @param table: the table index to change
 * @param buffer: the buffer to store the line
 * @return 0 if the table is changed successfully, 1 otherwise.
 */
int32_t changeParseTable(Script *script, Table *table, char *buffer);

/**
 * Update the current script table with the parsed data.
 * @param script: the script to store the parsed data
 * @param table: the table index to update
 * @return 0 if the table is updated successfully, 1 otherwise.
 */
int32_t updateParseTable(Script *script, Table *table);

/**
 * Allocate memory for a table.
 * @param table: the table to store the data
 * @param size: the size of the table
 * @param capacity: the capacity of the table
 * @return 0 if the table is created successfully, 1 otherwise.
 */
int32_t allocateTable(void **table, size_t size, int32_t capacity);

/**
 * Create a ID field.
 * @param script: the script to store the parsed data
 * @param table: the table index to create the field
 * @param buffer: the buffer to store the line
 * @return 0 if the ID field is created successfully, 1 otherwise
 */
int32_t createIDField(Script *script, Table *table, char *buffer);

/**
 * Create a table field.
 * @param script: the script to store the parsed data
 * @param table: the table index to create the field
 * @param buffer: the buffer to store the line
 * @return 0 if the table field is created successfully, 1 otherwise
 */
int32_t createTableField(Script *script, Table *table, char *buffer);

/**
 * Add data to a table.
 * @param script: the script to store the parsed data
 * @param table: the table index to add the data
 * @return 0 if the data is added successfully, 1 otherwise
 */
int32_t addDataToScript(Script *script, Table *table);

/**
 * Check if a line is empty or a comment.
 * @param buffer: the buffer to check
 * @return true if the line is empty or a comment, false otherwise
 */
bool isEmptyOrCommentLine(char *buffer);

/**
 * Check if a line is a table line.
 * @param buffer: the buffer to check
 * @return true if the line is a table line, false otherwise
 */
bool isTableLine(char *buffer);

/**
 * Get part of a table line string.
 * @param str: the string to store the part
 * @param buffer: the buffer to get the part
 * @param pos: the position to get the part
 * @return the position after the part
 */
int32_t getTableLineStr(char *str, char *buffer, int32_t pos);

/**
 * Remove quotes from a string.
 * @param buffer: the buffer to remove quotes
 */
void removeQuotes(char *buffer);

/**
 * Remove white space from a string.
 * @param buffer: the buffer to remove white space
 */
void cleanWhiteSpace(char **buffer);

/**
 * Print the script data.
 * @param script: the script to print
 */
void printScript(Script *script);

/**
 * Clear the script data.
 * @param script: the script to clear
 */
void clearScript(Script *script);
