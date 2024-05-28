#include "parse_script.h"

int32_t parseTomlScript(char *filename, Script *script) {
    FILE *file = NULL;

    if (openFile(&file, filename, "r") != 0) {
        return 1;
    }

    char buffer[STR_SIZE] = {0};
    Table table = { TABLE_NONE, 0, TABLE_NONE, 0, {0}, {0} };

    while (readLine(file, buffer, STR_SIZE) == 0) {
        if (isEmptyOrCommentLine(buffer)) {
            continue;
        }

        if (isTableLine(buffer)) {
            if (changeParseTable(script, &table, buffer) != 0) {
                printf("error: script error\n");
                return 1;
            }

            if (createIDField(&table, buffer)) {
                printf("error: id error\n");
                return 1;
            }
        }
        else {
            if (createTableField(&table, buffer) != 0) {
                printf("error: field error\n");
                return 1;
            }
        }

        if (addDataToScript(script, &table) != 0) {
            printf("error: add data error\n");
            return 1;
        }
    }

    printScript(script);

    closeFile(file);
    return 0;
}

int32_t changeParseTable(Script *script, Table *table, char *buffer) {
    if (!script || !table || !buffer) {
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
            printf("error: table error\n");
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
            table->table_name = i;
            break;
        }
    }

    // Find the sub table name in the buffer if the table is character or dialogue

    table->sub_table_name = TABLE_NONE;

    if (table->table_name == TABLE_CHARACTER || table->table_name == TABLE_DIALOGUE) {
        char temp[STR_SIZE] = {0};

        if (getTableLineStr(temp, buffer, LAST) != 0) {
            return 1;
        }

        if (strcmp(temp, table_name[TABLE_STATUS]) == 0) {
            table->sub_table_name = TABLE_STATUS;
        }
        else if (strcmp(temp, table_name[TABLE_OPTION]) == 0) {
            table->sub_table_name = TABLE_OPTION;
        }
    }

    if (updateParseTable(script, table) != 0) {
        return 1;
    }
 
    return 0;
}

int32_t updateParseTable(Script *script, Table *table) {
    if (!script) {
        return 1;
    }

    void *temp  = NULL;
    size_t size = 0;

    switch (table->table_name) {
        case TABLE_PLAYER:
            table->index = 0;

            temp = &(script->player);
            size = sizeof(Player);
            break;

        case TABLE_CHARACTER:
            // Status table is stored in the character table
            if (table->sub_table_name == TABLE_STATUS) {
                if (table->index >= script->character_size) {
                    return 1;
                }

                table->sub_index = script->characters[table->index].status_size;
                script->characters[table->index].status_size++;

                temp = &(script->characters[table->index].status);
                size = sizeof(Status);
                break;
            }
            else {
                table->index = script->character_size;
                script->character_size++;

                temp = &(script->characters);
                size = sizeof(Character);
            }
            break;

        case TABLE_STATUS_INFO:
            table->index = script->status_info_size;
            script->status_info_size++;

            temp = &(script->status_infos);
            size = sizeof(StatusInfo);
            break;

        case TABLE_ITEM:
            table->index = script->item_size;
            script->item_size++;

            temp = &(script->items);
            size = sizeof(Item);
            break;

        case TABLE_SCENE:
            table->index = script->scene_size;
            script->scene_size++;

            temp = &(script->scenes);
            size = sizeof(Scene);
            break;

        case TABLE_EVENT:
            table->index = script->event_size;
            script->event_size++;

            temp = &(script->events);
            size = sizeof(Event);
            break;

        case TABLE_DIALOGUE:
            // Option table is stored in the dialogue table
            if (table->sub_table_name == TABLE_OPTION) {
                if (table->index >= script->dialogue_size) {
                    return 1;
                }

                table->sub_index = script->dialogues[table->index].option_size;
                script->dialogues[table->index].option_size++;

                temp = &(script->dialogues[table->index].options);
                size = sizeof(Option);
                break;
            }
            else {
                table->index = script->dialogue_size;
                script->dialogue_size++;

                temp = &(script->dialogues);
                size = sizeof(Dialogue);
            }
            break;

        case TABLE_CONDITION:
            table->index = script->condition_size;
            script->condition_size++;

            temp = &(script->conditions);
            size = sizeof(Condition);
            break;

        default:
            break;
    }

    if (allocateTable(temp, size, table->index) != 0) {
        return 1;
    }

    return 0;
}

int32_t allocateTable(void **table, size_t size, int32_t capacity) {
    if (!size) {
        return 1;
    }

    if (capacity == 0) {
        *table = calloc(1, size);

        if (!*table) {
            perror("allocateTable: calloc error");
            return 1;
        }
    }
    else {
        *table = reallocarray(*table, capacity + 1, size);

        if (!*table) {
            perror("allocateTable: reallocarray error");
            return 1;
        }
    }

    return 0;
}

int32_t createIDField(Table *table, char *buffer) {
    if (!table || !buffer) {
        return 1;
    }

    strncpy(table->field, "id", STR_SIZE);
    strncpy(table->value, "", STR_SIZE);

    enum { FIRST, MID, LAST };

    switch (table->table_name) {
        case TABLE_CHARACTER:
            if (table->sub_table_name != TABLE_STATUS) {
                if (getTableLineStr(table->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            break;

        case TABLE_DIALOGUE:
            if (table->sub_table_name != TABLE_OPTION) {
                if (getTableLineStr(table->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            break;

        case TABLE_STATUS_INFO:
        case TABLE_ITEM:
        case TABLE_SCENE:
        case TABLE_EVENT:
        case TABLE_CONDITION:
            if (getTableLineStr(table->value, buffer, LAST) != 0) {
                return 1;
            }
            break;

        default:
            break;
    }

    return 0;
}

int32_t createTableField(Table *table, char *buffer) {
    if (!table || !buffer) {
        return 1;
    }

    cleanWhiteSpace(&buffer);

    char *equal = strchr(buffer, '=');

    if (equal) {
        strncpy(table->field, buffer, equal - buffer);
    }

    return 0;
}

int32_t addDataToScript(Script *script, Table *table) {
    if (!script || !table) {
        return 1;
    }

    switch (table->table_name) {
        case TABLE_PLAYER:
            break;

        case TABLE_CHARACTER:
            if (table->sub_table_name == TABLE_STATUS) {
                // todo
            }
            else {
                Character *character = &(script->characters[table->index]);

                if (strcmp(table->field, "id") == 0) {
                    strncpy(character->id, table->value, STR_SIZE);
                }
            }
            break;

        case TABLE_STATUS_INFO:
            if (strcmp(table->field, "id") == 0) {
                strncpy(script->status_infos[table->index].id, table->value, STR_SIZE);
            }
            break;

        case TABLE_ITEM:
            if (strcmp(table->field, "id") == 0) {
                strncpy(script->items[table->index].id, table->value, STR_SIZE);
            }
            break;

        case TABLE_SCENE:
            if (strcmp(table->field, "id") == 0) {
                strncpy(script->scenes[table->index].id, table->value, STR_SIZE);
            }
            break;

        case TABLE_EVENT:
            if (strcmp(table->field, "id") == 0) {
                strncpy(script->events[table->index].id, table->value, STR_SIZE);
            }
            break;

        case TABLE_DIALOGUE:
            if (table->sub_table_name == TABLE_OPTION) {
                // todo
            }
            else {
                Dialogue *dialogue = &(script->dialogues[table->index]);

                if (strcmp(table->field, "id") == 0) {
                    strncpy(dialogue->id, table->value, STR_SIZE);
                }
            }
            break;

        case TABLE_CONDITION:
            if (strcmp(table->field, "id") == 0) {
                strncpy(script->conditions[table->index].id, table->value, STR_SIZE);
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

void printScript(Script *script) {
    if (!script) {
        return;
    }

    // printf("player: %s\n", script->player->id);

    for (int32_t i = 0; i < script->character_size; i++) {
        printf("character: %s\n", script->characters[i].id);

        // for (int32_t j = 0; j < script->characters[i].status_size; j++) {
        //     printf("status: %s\n", script->characters[i].status[j].id);
        // }
    }

    for (int32_t i = 0; i < script->status_info_size; i++) {
        printf("status_info: %s\n", script->status_infos[i].id);
    }

    for (int32_t i = 0; i < script->item_size; i++) {
        printf("item: %s\n", script->items[i].id);
    }

    for (int32_t i = 0; i < script->scene_size; i++) {
        printf("scene: %s\n", script->scenes[i].id);
    }

    for (int32_t i = 0; i < script->event_size; i++) {
        printf("event: %s\n", script->events[i].id);
    }

    for (int32_t i = 0; i < script->dialogue_size; i++) {
        printf("dialogue: %s\n", script->dialogues[i].id);

        // for (int32_t j = 0; j < script->dialogues[i].option_size; j++) {
        //     printf("option: %s\n", script->dialogues[i].options[j].next_name);
        // }
    }

    for (int32_t i = 0; i < script->condition_size; i++) {
        printf("condition: %s\n", script->conditions[i].id);
    }
}
