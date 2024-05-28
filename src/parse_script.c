#include "parse_script.h"

int32_t parseTomlScript(char *filename, Script *script) {
    FILE *file = NULL;

    if (openFile(&file, filename, "r") != 0) {
        return 1;
    }

    char buffer[STR_SIZE] = {0};
    ScriptTableIndex tableIndex = { TABLE_NONE, 0, TABLE_NONE, 0 };

    while (readLine(file, buffer, STR_SIZE) == 0) {
        if (isEmptyOrCommentLine(buffer)) {
            continue;
        }
        else if (isTableLine(buffer)) {
            if (changeScriptTable(script, &tableIndex, buffer) != 0) {
                printf("Error: changeScriptTable\n");
                return 1;
            }
        }
    }

    closeFile(file);
    return 0;
}

int32_t changeScriptTable(Script *script, ScriptTableIndex *tableIndex, char *buffer) {
    if (!script || !tableIndex || !buffer) {
        return 1;
    }

    cleanWhiteSpace(&buffer);

    char *table_name[] = {
        "none",
        "player",
        "character",
        "status", // this status in toml is status_info in script
        "status", // this status in toml is sub table of character
        "item",
        "scene",
        "event",
        "dialogue",
        "option",
        "condition",
        "error"
    };

    enum { FIRST, MID, LAST };

    // Find the table name in the buffer
    // Magic number 1 and 11 are the index of the table_name array

    for (int32_t i = 1; i < 11; i++) {
        if (i == TABLE_ERROR) {
            return 1;
        }
        else if (i == TABLE_STATUS || i == TABLE_OPTION) {
            continue;
        }

        char temp[STR_SIZE] = {0};

        if (getTableLineStr(temp, buffer, FIRST) != 0) {
            return 1;
        }

        if (strcmp(temp, table_name[i]) == 0) {
            tableIndex->table = i;
            printf("Table: %s\n", table_name[i]);
            break;
        }
    }

    // Find the sub table name in the buffer if the table is character or dialogue

    tableIndex->sub_table = TABLE_NONE;

    if (tableIndex->table == TABLE_CHARACTER || tableIndex->table == TABLE_DIALOGUE) {
        char temp[STR_SIZE] = {0};

        if (getTableLineStr(temp, buffer, LAST) != 0) {
            return 1;
        }

        if (strcmp(temp, table_name[TABLE_STATUS]) == 0) {
            tableIndex->sub_table = TABLE_STATUS;
            printf("Sub Table: %s\n", table_name[TABLE_STATUS]);
        }
        else if (strcmp(temp, table_name[TABLE_OPTION]) == 0) {
            tableIndex->sub_table = TABLE_OPTION;
            printf("Sub Table: %s\n", table_name[TABLE_OPTION]);
        }
    }

    if (updateTable(script, tableIndex) != 0) {
        return 1;
    }

    TableField field = { {0}, {0} };

    if (createIDField(&field, tableIndex, buffer)) {
        return 1;
    }

    if (addDataToTable(script, tableIndex, &field) != 0) {
        return 1;
    }
 
    return 0;
}

int32_t updateTable(Script *script, ScriptTableIndex *tableIndex) {
    if (!script) {
        return 1;
    }

    void *table = NULL;
    size_t size = 0;

    switch (tableIndex->table) {
        case TABLE_PLAYER:
            tableIndex->index = 0;

            table = &(script->player);
            size  = sizeof(Player);
            break;

        case TABLE_CHARACTER:
            // Status table is stored in the character table
            if (tableIndex->sub_table == TABLE_STATUS) {
                if (tableIndex->index >= script->character_size) {
                    return 1;
                }

                tableIndex->sub_index = script->characters[tableIndex->index].status_size;
                script->characters[tableIndex->index].status_size++;

                table = &(script->characters[tableIndex->index].status);
                size  = sizeof(Status);
                break;
            }
            else {
                tableIndex->index = script->character_size;
                script->character_size++;

                table = &(script->characters);
                size  = sizeof(Character);
            }
            break;

        case TABLE_STATUS_INFO:
            tableIndex->index = script->status_info_size;
            script->status_info_size++;

            table = &(script->status_infos);
            size  = sizeof(StatusInfo);
            break;

        case TABLE_ITEM:
            tableIndex->index = script->item_size;
            script->item_size++;

            table = &(script->items);
            size  = sizeof(Item);
            break;

        case TABLE_SCENE:
            tableIndex->index = script->scene_size;
            script->scene_size++;

            table = &(script->scenes);
            size  = sizeof(Scene);
            break;

        case TABLE_EVENT:
            tableIndex->index = script->event_size;
            script->event_size++;

            table = &(script->events);
            size  = sizeof(Event);
            break;

        case TABLE_DIALOGUE:
            // Option table is stored in the dialogue table
            if (tableIndex->sub_table == TABLE_OPTION) {
                if (tableIndex->index >= script->dialogue_size) {
                    return 1;
                }

                tableIndex->sub_index = script->dialogues[tableIndex->index].option_size;
                script->dialogues[tableIndex->index].option_size++;

                table = &(script->dialogues[tableIndex->index].options);
                size  = sizeof(DialogueOption);
                break;
            }
            else {
                tableIndex->index = script->dialogue_size;
                script->dialogue_size++;

                table = &(script->dialogues);
                size  = sizeof(Dialogue);
            }
            break;

        case TABLE_CONDITION:
            tableIndex->index = script->condition_size;
            script->condition_size++;

            table = &(script->conditions);
            size  = sizeof(Condition);
            break;

        default:
            break;
    }

    if (createNewTable(table, size, tableIndex->index) != 0) {
        return 1;
    }

    return 0;
}

int32_t createNewTable(void **table, size_t size, int32_t capacity) {
    if (!size) {
        return 1;
    }

    if (capacity == 0) {
        *table = calloc(1, size);

        if (!*table) {
            perror("createNewTable: calloc error");
            return 1;
        }
    }
    else {
        *table = reallocarray(*table, capacity + 1, size);

        if (!*table) {
            perror("createNewTable: reallocarray error");
            return 1;
        }
    }

    // if (!*table) {
    //     printf("Table is NULL\n");
    // }

    return 0;
}

int32_t createIDField(TableField *field, ScriptTableIndex *tableIndex, char *buffer) {
    if (!field || !tableIndex || !buffer) {
        return 1;
    }

    strncpy(field->name, "id", STR_SIZE);

    enum { FIRST, MID, LAST };

    switch (tableIndex->table) {
        case TABLE_CHARACTER:
            if (tableIndex->sub_table != TABLE_STATUS) {
                if (getTableLineStr(field->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            break;

        case TABLE_DIALOGUE:
            if (tableIndex->sub_table != TABLE_OPTION) {
                if (getTableLineStr(field->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            break;

        case TABLE_STATUS_INFO:
        case TABLE_ITEM:
        case TABLE_SCENE:
        case TABLE_EVENT:
        case TABLE_CONDITION:
            if (getTableLineStr(field->value, buffer, LAST) != 0) {
                return 1;
            }
            break;

        default:
            break;
    }

    return 0;
}

int32_t addDataToTable(Script *script, ScriptTableIndex *tableIndex, TableField *field) {
    if (!script || !tableIndex) {
        return 1;
    }

    switch (tableIndex->table) {
        case TABLE_PLAYER:
            break;

        case TABLE_CHARACTER:
            if (tableIndex->sub_table == TABLE_STATUS) {
                // todo
            }
            else {
                Character *character = &(script->characters[tableIndex->index]);

                if (strcmp(field->name, "id") == 0) {
                    strncpy(character->id, field->value, STR_SIZE);
                }
            }
            break;

        case TABLE_STATUS_INFO:
            if (strcmp(field->name, "id") == 0) {
                strncpy(script->status_infos[tableIndex->index].id, field->value, STR_SIZE);
            }
            break;

        case TABLE_ITEM:
            if (strcmp(field->name, "id") == 0) {
                strncpy(script->items[tableIndex->index].id, field->value, STR_SIZE);
            }
            break;

        case TABLE_SCENE:
            if (strcmp(field->name, "id") == 0) {
                strncpy(script->scenes[tableIndex->index].id, field->value, STR_SIZE);
            }
            break;

        case TABLE_EVENT:
            if (strcmp(field->name, "id") == 0) {
                strncpy(script->events[tableIndex->index].id, field->value, STR_SIZE);
            }
            break;

        case TABLE_DIALOGUE:
            if (tableIndex->sub_table == TABLE_OPTION) {
                // todo
            }
            else {
                Dialogue *dialogue = &(script->dialogues[tableIndex->index]);

                if (strcmp(field->name, "id") == 0) {
                    strncpy(dialogue->id, field->value, STR_SIZE);
                }
            }
            break;

        case TABLE_CONDITION:
            if (strcmp(field->name, "id") == 0) {
                strncpy(script->conditions[tableIndex->index].id, field->value, STR_SIZE);
            }
            break;

        default:
            break;
    }

    return 0;
}

bool isEmptyOrCommentLine(char *buffer) {
    if (strlen(buffer) == 0) {
        return true;
    }

    if (strspn(buffer, " \t") == strcspn(buffer, "#")) {
        return true;
    }

    return false;
}

bool isTableLine(char *buffer) {
    if (isEmptyOrCommentLine(buffer)) {
        return false;
    }

    if (strspn(buffer, " \t") == strcspn(buffer, "[")) {
        return true;
    }

    return false;
}

int32_t getTableLineStr(char *str, char *buffer, int32_t pos) {
    // Magic number pos: 0: first, 1: mid, 2: last

    char *check = "abcdefghijklmnopqrstuvwxyz";

    size_t head = 0;
    size_t tail = 0;
    size_t size = 0;

    if (pos == 0) {
        head = strcspn(buffer, check);
        tail = strcspn(buffer, ".]");
        size = tail - head;

        strncpy(str, buffer + head, size);
    }
    else if (pos == 1) {
        char *first_dot = strchr(buffer, '.');

        if (!first_dot) {
            return 1;
        }

        head = strcspn(first_dot, check);
        tail = strcspn(first_dot + 1, ".]");
        size = tail - head;

        printf("head: %ld, tail: %ld, size: %ld\n", head, tail, size);

        strncpy(str, first_dot + head, size);
    }
    else if (pos == 2) {
        char *last_dot = strrchr(buffer, '.');

        if (!last_dot) {
            return 1;
        }

        head = strcspn(last_dot, check);
        tail = strcspn(last_dot, "]");
        size = tail - head;

        strncpy(str, last_dot + head, size);
    }

    return 0;
}

void cleanWhiteSpace(char **buffer) {
    if (!buffer || !(*buffer)) {
        return;
    }

    // Remove leading white space

    while (**buffer == ' ' || **buffer == '\t') {
        (*buffer)++;
    }

    // Remove trailing white space

    char *end = *buffer + strlen(*buffer) - 1;

    while (end > *buffer && (*end == ' ' || *end == '\t')) {
        end--;
    }

    *(end + 1) = '\0';

    // Remove white space between characters

    char *src = *buffer;
    char *dst = *buffer;

    while (*src) {
        if (*src == ' ' || *src == '\t') {
            src++;
        }
        else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';
}
