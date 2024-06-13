#include "parse_script.h"

int32_t parseTomlScript(Script *script) {
    FILE *file = NULL;

    char filename[STR_SIZE] = {0};

    if (snprintf(filename, STR_SIZE, "%s/script.toml", script->dir) < 0) {
        printf("error: script error\n");
        return 1;
    }

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

            if (createIDField(script, &table, buffer)) {
                printf("error: id error\n");
                return 1;
            }
        }
        else {
            if (createTableField(script, &table, buffer) != 0) {
                printf("error: field error\n");
                return 1;
            }
        }
    }

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
        "tachie",
        "status", // this status in toml is status_info in script
        "status", // this status in toml is sub table of character
        "item",
        "scene",
        "event",
        "update",
        "trigger",
        "dialogue",
        "option",
        "condition",
        "error"
    };

    enum { FIRST, MID, LAST };

    // Find the table name in the buffer
    // The magic numbers are the index of the table_name array

    int32_t size = sizeof(table_name) / sizeof(table_name[0]);

    for (int32_t i = 1; i < size; i++) {
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
        else if (strcmp(temp, table_name[TABLE_TACHIE]) == 0) {
            table->sub_table_name = TABLE_TACHIE;
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
    int32_t idx = 0;

    switch (table->table_name) {
        case TABLE_PLAYER: {
            table->index = 0;

            temp = &(script->player);
            size = sizeof(Player);
            break;
        }

        case TABLE_CHARACTER: {
            // Status table is stored in the character table
            if (table->sub_table_name == TABLE_STATUS || table->sub_table_name == TABLE_TACHIE) {
                return 0;
            }
            else {
                table->index = script->character_size;
                script->character_size++;

                temp = &(script->characters);
                size = sizeof(Character);
            }

            break;
        }

        case TABLE_STATUS_INFO: {
            table->index = script->status_info_size;
            script->status_info_size++;

            temp = &(script->status_infos);
            size = sizeof(StatusInfo);

            break;
        }

        case TABLE_ITEM: {
            table->index = script->item_size;
            script->item_size++;

            temp = &(script->items);
            size = sizeof(Item);
            break;
        }

        case TABLE_SCENE: {
            table->index = script->scene_size;
            script->scene_size++;

            temp = &(script->scenes);
            size = sizeof(Scene);

            break;
        }

        case TABLE_EVENT: {
            table->index = script->event_size;
            script->event_size++;

            temp = &(script->events);
            size = sizeof(Event);

            break;
        }

        case TABLE_UPDATE: {
            table->index = script->update_size;
            script->update_size++;

            temp = &(script->updates);
            size = sizeof(Update);

            break;
        }

        case TABLE_TRIGGER: {
            table->index = script->trigger_size;
            script->trigger_size++;

            temp = &(script->triggers);
            size = sizeof(Trigger);

            break;
        }

        case TABLE_DIALOGUE: {
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
        }

        case TABLE_CONDITION: {
            table->index = script->condition_size;
            script->condition_size++;

            temp = &(script->conditions);
            size = sizeof(Condition);

            break;
        }

        default:
            break;
    }

    if (table->table_name == TABLE_DIALOGUE  && table->sub_table_name == TABLE_OPTION) {
        idx = table->sub_index;
    }
    else {
        idx = table->index;
    }

    if (allocateTable(temp, size, idx) != 0) {
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

        memset(*table + capacity * size, 0, size);
    }

    return 0;
}

int32_t createIDField(Script *script, Table *table, char *buffer) {
    if (!table || !buffer) {
        return 1;
    }

    strncpy(table->field, "id", STR_SIZE);
    memset(table->value, 0, STR_SIZE);

    enum { FIRST, MID, LAST };

    switch (table->table_name) {
        case TABLE_CHARACTER: {
            if (table->sub_table_name != TABLE_STATUS && table->sub_table_name != TABLE_TACHIE) {
                if (getTableLineStr(table->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            else {
                return 0;
            }

            break;
        }

        case TABLE_DIALOGUE: {
            if (table->sub_table_name != TABLE_OPTION) {
                if (getTableLineStr(table->value, buffer, LAST) != 0) {
                    return 1;
                }
            }
            else {
                return 0;
            }

            break;
        }

        case TABLE_STATUS_INFO:
        case TABLE_ITEM:
        case TABLE_SCENE:
        case TABLE_EVENT:
        case TABLE_UPDATE:
        case TABLE_TRIGGER:
        case TABLE_CONDITION: {
            if (getTableLineStr(table->value, buffer, LAST) != 0) {
                return 1;
            }

            break;
        }

        default:
            break;
    }

    if (addDataToScript(script, table) != 0) {
        return 1;
    }

    return 0;
}

int32_t createTableField(Script *script, Table *table, char *buffer) {
    if (!table || !buffer) {
        return 1;
    }

    cleanWhiteSpace(&buffer);

    char *equal = strchr(buffer, '=');

    if (equal) {
        // Clear field and value
        memset(table->field, 0, STR_SIZE);
        memset(table->value, 0, STR_SIZE);

        strncpy(table->field, buffer, equal - buffer);
        strncpy(table->value, equal + 1, buffer + strlen(buffer) - equal);
    }
    else {
        strncpy(table->value, buffer, STR_SIZE);
    }

    if ((table->table_name == TABLE_CHARACTER && strcmp(table->field, "inventory") == 0) ||
        (table->table_name == TABLE_DIALOGUE  && strcmp(table->field, "update") == 0)) {
        char tmp[STR_SIZE] = {0};
        char val[STR_SIZE] = {0};
        char *head = NULL;
        char *tail = NULL;

        strncpy(tmp, table->value, STR_SIZE);

        for (size_t i = 0; i < strlen(tmp); i++) {
            if (tmp[i] != '\"') {
                continue;
            }

            if (!head) {
                head = tmp + i + 1;
            }
            else {
                tail = tmp + i;
                strncpy(val, head, tail - head);

                memset(table->value, 0, STR_SIZE);
                strncpy(table->value, val, STR_SIZE);

                if (addDataToScript(script, table) != 0) {
                    return 1;
                }

                head = NULL;
                tail = NULL;
            }
        }
    }
    else {
        removeQuotes(table->value);

        if (addDataToScript(script, table) != 0) {
            return 1;
        }
    }

    return 0;
}

int32_t addDataToScript(Script *script, Table *table) {
    if (!script || !table) {
        return 1;
    }

    switch (table->table_name) {
        case TABLE_NONE: {
            if (strcmp(table->field, "name") == 0) {
                strncpy(script->name, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "author") == 0) {
                strncpy(script->author, table->value, STR_SIZE);
            }

            break;
        }

        case TABLE_PLAYER: {
            if (strcmp(table->field, "character") == 0) {
                strncpy(script->player->character, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "start_event") == 0) {
                strncpy(script->player->start_event, table->value, STR_SIZE);
                strncpy(script->current_event_id, table->value, STR_SIZE);
            }

            break;
        }

        case TABLE_CHARACTER: {
            if (table->sub_table_name == TABLE_STATUS) {
                void  *temp = &(script->characters[table->index].status);
                size_t size = sizeof(Status);
                int32_t idx = script->characters[table->index].status_size;
                script->characters[table->index].status_size++;

                if (allocateTable(temp, size, idx) != 0) {
                    return 1;
                }

                Status *status = &(script->characters[table->index].status[idx]);

                strncpy(status->status_name, table->field, STR_SIZE);
                status->value = strtol(table->value, NULL, 10);
            }
            else if (table->sub_table_name == TABLE_TACHIE) {
                void  *temp = &(script->characters[table->index].tachies);
                size_t size = sizeof(Tachie);
                int32_t idx = script->characters[table->index].tachie_size;
                script->characters[table->index].tachie_size++;

                if (allocateTable(temp, size, idx) != 0) {
                    return 1;
                }

                Tachie *tachie = &(script->characters[table->index].tachies[idx]);

                strncpy(tachie->name, table->field, STR_SIZE);
                strncpy(tachie->path, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "inventory") == 0) {
                void  *temp = &(script->characters[table->index].inventory);
                size_t size = sizeof(char *);
                int32_t idx = script->characters[table->index].inventory_size;
                script->characters[table->index].inventory_size++;

                if (allocateTable(temp, size, idx) != 0) {
                    return 1;
                }

                char **inventory = &(script->characters[table->index].inventory[idx]);
                *inventory = calloc(1, STR_SIZE);

                if (!*inventory) {
                    perror("addDataToScript: calloc error");
                    return 1;
                }

                strncpy(*inventory, table->value, STR_SIZE);
            }
            else {
                Character *character = &(script->characters[table->index]);

                if (strcmp(table->field, "id") == 0) {
                    strncpy(character->id, table->value, STR_SIZE);
                }
                else if (strcmp(table->field, "name") == 0) {
                    strncpy(character->name, table->value, STR_SIZE);
                }
            }

            break;
        }

        case TABLE_STATUS_INFO: {
            StatusInfo *status_info = &(script->status_infos[table->index]);
            char *endptr = NULL;

            if (strcmp(table->field, "id") == 0) {
                strncpy(status_info->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "name") == 0) {
                strncpy(status_info->name, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "desc") == 0) {
                strncpy(status_info->desc, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "min") == 0) {
                status_info->min = strtol(table->value, &endptr, 10);

                if (*endptr != '\0' && *endptr != '\n') {
                    printf("error: min error\n");
                    return 1;
                }
            }
            else if (strcmp(table->field, "max") == 0) {
                status_info->max = strtol(table->value, &endptr, 10);

                if (*endptr != '\0' && *endptr != '\n') {
                    printf("error: max error\n");
                    return 1;
                }
            }

            break;
        }

        case TABLE_ITEM: {
            Item *item = &(script->items[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(item->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "name") == 0) {
                strncpy(item->name, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "desc") == 0) {
                strncpy(item->desc, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "icon") == 0) {
                strncpy(item->icon, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "hidden") == 0) {
                if (strcmp(table->value, "true") == 0) {
                    item->hidden = 1;
                }
                else if (strcmp(table->value, "false") == 0) {
                    item->hidden = 0;
                }
                else {
                    return 1;
                }
            }

            break;
        }

        case TABLE_SCENE: {
            Scene *scene = &(script->scenes[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(scene->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "name") == 0) {
                strncpy(scene->name, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "background") == 0) {
                strncpy(scene->background, table->value, STR_SIZE);
            }

            break;
        }

        case TABLE_EVENT: {
            Event *event = &(script->events[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(event->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "scene") == 0) {
                strncpy(event->scene, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "dialogue") == 0) {
                strncpy(event->dialogue, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "bgm") == 0) {
                strncpy(event->bgm, table->value, STR_SIZE);
            }

            break;
        }

        case TABLE_UPDATE: {
            Update *update = &(script->updates[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(update->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "character") == 0) {
                strncpy(update->character, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "item") == 0) {
                if (update->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(update->condition, table->value, STR_SIZE);
                update->condition_type = CONDITION_ITEM;
            }
            else if (strcmp(table->field, "status") == 0) {
                if (update->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(update->condition, table->value, STR_SIZE);
                update->condition_type = CONDITION_STATUS;
            }
            else if (strcmp(table->field, "change") == 0) {
                char *endptr = NULL;
                update->change = strtol(table->value, &endptr, 10);

                if (*endptr != '\0' && *endptr != '\n') {
                    printf("error: change value error\n");
                    return 1;
                }
            }

            break;
        }

        case TABLE_TRIGGER: {
            Trigger *trigger = &(script->triggers[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(trigger->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "character") == 0) {
                strncpy(trigger->character, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "event") == 0) {
                strncpy(trigger->event, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "item") == 0) {
                if (trigger->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(trigger->condition, table->value, STR_SIZE);
                trigger->condition_type = CONDITION_ITEM;
            }
            else if (strcmp(table->field, "status") == 0) {
                if (trigger->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(trigger->condition, table->value, STR_SIZE);
                trigger->condition_type = CONDITION_STATUS;
            }
            else if (strcmp(table->field, "value") == 0) {
                char *endptr = NULL;
                trigger->value = strtol(table->value, &endptr, 10);

                if (*endptr != '\0' && *endptr != '\n') {
                    printf("error: value error\n");
                    return 1;
                }
            }
            else if (strcmp(table->field, "logic") == 0) {
                if (strcmp(table->value, "EQ") == 0) {
                    trigger->logic = LOGIC_EQ;
                }
                else if (strcmp(table->value, "NE") == 0) {
                    trigger->logic = LOGIC_NE;
                }
                else if (strcmp(table->value, "GT") == 0) {
                    trigger->logic = LOGIC_GT;
                }
                else if (strcmp(table->value, "LT") == 0) {
                    trigger->logic = LOGIC_LT;
                }
                else if (strcmp(table->value, "GE") == 0) {
                    trigger->logic = LOGIC_GE;
                }
                else if (strcmp(table->value, "LE") == 0) {
                    trigger->logic = LOGIC_LE;
                }
                else {
                    printf("error: logic error\n");
                    return 1;
                }
            }

            break;
        }

        case TABLE_DIALOGUE: {
            if (table->sub_table_name == TABLE_OPTION) {
                Option *option = &(script->dialogues[table->index].options[table->sub_index]);

                if (strcmp(table->field, "text") == 0) {
                    strncpy(option->text, table->value, STR_SIZE);
                }
                else if (strcmp(table->field, "condition") == 0) {
                    strncpy(option->condition, table->value, STR_SIZE);
                }
                else if (strcmp(table->field, "next") == 0) {
                    if (option->next_type != DIALOGUE_OPTION) {
                        printf("error: next type error\n");
                        return 1;
                    }

                    strncpy(option->next, table->value, STR_SIZE);
                    option->next_type = DIALOGUE_NORMAL;
                }
                else if (strcmp(table->field, "event") == 0) {
                    if (option->next_type != DIALOGUE_OPTION) {
                        printf("error: next type error\n");
                        return 1;
                    }

                    strncpy(option->next, table->value, STR_SIZE);
                    option->next_type = DIALOGUE_EVENT;
                }
                else if (strcmp(table->field, "hidden") == 0) {
                    if (strcmp(table->value, "true") == 0) {
                        option->hidden = 1;
                    }
                    else if (strcmp(table->value, "false") == 0) {
                        option->hidden = 0;
                    }
                    else {
                        return 1;
                    }
                }

                break;
            }

            Dialogue *dialogue = &(script->dialogues[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(dialogue->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "text") == 0) {
                strncpy(dialogue->text, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "sfx") == 0) {
                strncpy(dialogue->sfx, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "character") == 0) {
                strncpy(dialogue->character, table->value, STR_SIZE);

                if (strcmp(table->value, script->player->character) == 0) {
                    dialogue->character_type = CHARACTER_PLAYER;
                }
                else {
                    dialogue->character_type = CHARACTER_NPC;
                }
            }
            else if (strcmp(table->field, "tachie") == 0) {
                strncpy(dialogue->tachie, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "update") == 0) {
                void  *temp = &(script->dialogues[table->index].updates);
                size_t size = sizeof(char *);
                int32_t idx = script->dialogues[table->index].update_size;
                script->dialogues[table->index].update_size++;

                if (allocateTable(temp, size, idx) != 0) {
                    return 1;
                }

                char **update = &(script->dialogues[table->index].updates[idx]);
                *update = calloc(1, STR_SIZE);

                if (!*update) {
                    perror("addDataToScript: calloc error");
                    return 1;
                }

                strncpy(*update, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "next")  == 0 ||
                     strcmp(table->field, "event") == 0 ||
                     strcmp(table->field, "end")   == 0) {
                if (dialogue->next_type != DIALOGUE_OPTION) {
                    printf("error: next type error\n");
                    return 1;
                }

                strncpy(dialogue->next, table->value, STR_SIZE);

                if (strcmp(table->field, "next") == 0) {
                    dialogue->next_type = DIALOGUE_NORMAL;
                }
                else if (strcmp(table->field, "event") == 0) {
                    dialogue->next_type = DIALOGUE_EVENT;
                }
                else if (strcmp(table->field, "end") == 0) {
                    dialogue->next_type = DIALOGUE_END;
                }
            }

            break;
        }

        case TABLE_CONDITION: {
            Condition *condition = &(script->conditions[table->index]);

            if (strcmp(table->field, "id") == 0) {
                strncpy(condition->id, table->value, STR_SIZE);
            }
            else if (strcmp(table->field, "character") == 0) {
                strncpy(condition->character, table->value, STR_SIZE);

                if (strcmp(table->value, script->player->character) == 0) {
                    condition->character_type = CHARACTER_PLAYER;
                }
                else {
                    condition->character_type = CHARACTER_NPC;
                }
            }
            else if (strcmp(table->field, "item") == 0) {
                if (condition->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(condition->condition, table->value, STR_SIZE);
                condition->condition_type = CONDITION_ITEM;
            }
            else if (strcmp(table->field, "status") == 0) {
                if (condition->condition_type != CONDITION_NONE) {
                    return 1;
                }

                strncpy(condition->condition, table->value, STR_SIZE);
                condition->condition_type = CONDITION_STATUS;
            }
            else if (strcmp(table->field, "value") == 0) {
                char *endptr = NULL;
                condition->value = strtol(table->value, &endptr, 10);

                if (*endptr != '\0' && *endptr != '\n') {
                    printf("error: value error\n");
                    return 1;
                }
            }
            else if (strcmp(table->field, "logic") == 0) {
                if (strcmp(table->value, "EQ") == 0) {
                    condition->logic = LOGIC_EQ;
                }
                else if (strcmp(table->value, "NE") == 0) {
                    condition->logic = LOGIC_NE;
                }
                else if (strcmp(table->value, "GT") == 0) {
                    condition->logic = LOGIC_GT;
                }
                else if (strcmp(table->value, "LT") == 0) {
                    condition->logic = LOGIC_LT;
                }
                else if (strcmp(table->value, "GE") == 0) {
                    condition->logic = LOGIC_GE;
                }
                else if (strcmp(table->value, "LE") == 0) {
                    condition->logic = LOGIC_LE;
                }
                else {
                    printf("error: logic error\n");
                    return 1;
                }
            }

            break;
        }

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

void removeQuotes(char *buffer) {
    if (!buffer) {
        return;
    }

    char *src = buffer;
    char *dst = buffer;

    while (*src) {
        if (*src == '"') {
            src++;
            continue;
        }

        *dst++ = *src++;
    }

    *dst = '\0';
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

    // Remove white space between characters unless it is a string

    char *src = *buffer;
    char *dst = *buffer;

    bool quote = false;

    while (*src) {
        if (*src == '"') {
            quote = !quote;
        }

        if (*src == ' ' || *src == '\t') {
            if (!quote) {
                src++;
                continue;
            }
        }

        *dst++ = *src++;
    }

    *dst = '\0';
}

void printScript(Script *script) {
    if (!script) {
        return;
    }

    printf("======================================================================\n");
    printf("PLAYER\n");
    printf("----------------------------------------------------------------------\n");

    printf("character      | %s\n", script->player->character);
    printf("start_event    | %s\n", script->player->start_event);

    printf("======================================================================\n");
    printf("CHARACTER\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->character_size; i++) {
        printf("id             | %s\n", script->characters[i].id);
        printf("name           | %s\n", script->characters[i].name);
        // printf("tachie         | %s\n", script->characters[i].tachie);

        for (int32_t j = 0; j < script->characters[i].tachie_size; j++) {
            if (j == 0) {
                printf("tachie         | ");
            }
            else {
                printf("               | ");
            }

            printf("%s: %s\n", script->characters[i].tachies[j].name, script->characters[i].tachies[j].path);
        }

        for (int32_t j = 0; j < script->characters[i].status_size; j++) {
            if (j == 0) {
                printf("status         | ");
            }
            else {
                printf("               | ");
            }
            printf("%s: %d\n", script->characters[i].status[j].status_name, script->characters[i].status[j].value);
        }

        for (int32_t j = 0; j < script->characters[i].inventory_size; j++) {
            if (j == 0) {
                printf("inventory      | ");
            }
            else {
                printf("               | ");
            }

            printf("%s\n", script->characters[i].inventory[j]);
        }

        if (i < script->character_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("STATUS_INFO\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->status_info_size; i++) {
        printf("id             | %s\n", script->status_infos[i].id);
        printf("name           | %s\n", script->status_infos[i].name);
        printf("desc           | %s\n", script->status_infos[i].desc);
        printf("min            | %d\n", script->status_infos[i].min);
        printf("max            | %d\n", script->status_infos[i].max);

        if (i < script->status_info_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("ITEM\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->item_size; i++) {
        printf("id             | %s\n", script->items[i].id);
        printf("name           | %s\n", script->items[i].name);
        printf("desc           | %s\n", script->items[i].desc);
        printf("icon           | %s\n", script->items[i].icon);
        printf("hidden         | %d\n", script->items[i].hidden);

        if (i < script->item_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("SCENE\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->scene_size; i++) {
        printf("id             | %s\n", script->scenes[i].id);
        printf("name           | %s\n", script->scenes[i].name);
        printf("background     | %s\n", script->scenes[i].background);

        if (i < script->scene_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("UPDATE\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->update_size; i++) {
        printf("id             | %s\n", script->updates[i].id);
        printf("character      | %s\n", script->updates[i].character);
        printf("condition      | %s\n", script->updates[i].condition);
        printf("condition_type | %d\n", script->updates[i].condition_type);
        printf("change         | %d\n", script->updates[i].change);

        if (i < script->update_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("TRIGGER\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->trigger_size; i++) {
        printf("id             | %s\n", script->triggers[i].id);
        printf("character      | %s\n", script->triggers[i].character);
        printf("event          | %s\n", script->triggers[i].event);
        printf("condition      | %s\n", script->triggers[i].condition);
        printf("condition_type | %d\n", script->triggers[i].condition_type);
        printf("value          | %d\n", script->triggers[i].value);
        printf("logic          | %d\n", script->triggers[i].logic);

        if (i < script->trigger_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("EVENT\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->event_size; i++) {
        printf("id             | %s\n", script->events[i].id);
        printf("scene          | %s\n", script->events[i].scene);
        printf("dialogue       | %s\n", script->events[i].dialogue);
        printf("bgm            | %s\n", script->events[i].bgm);

        if (i < script->event_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("DIALOGUE\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->dialogue_size; i++) {
        printf("id             | %s\n", script->dialogues[i].id);
        printf("text           | %s\n", script->dialogues[i].text);
        printf("sfx            | %s\n", script->dialogues[i].sfx);
        printf("character      | %s\n", script->dialogues[i].character);
        printf("tachie         | %s\n", script->dialogues[i].tachie);

        for (int32_t j = 0; j < script->dialogues[i].update_size; j++) {
            if (j == 0) {
                printf("update         | ");
            }
            else {
                printf("               | ");
            }

            printf("%s\n", script->dialogues[i].updates[j]);
        }

        printf("next           | %s\n", script->dialogues[i].next);
        printf("next_type      | ");

        switch (script->dialogues[i].next_type) {
            case DIALOGUE_NORMAL:
                printf("DIALOGUE\n");
                break;
            case DIALOGUE_EVENT:
                printf("EVENT\n");
                break;
            case DIALOGUE_OPTION:
                printf("OPTION\n");
                break;
            case DIALOGUE_END:
                printf("END\n");
                break;
            default:
                printf("ERROR\n");
                break;
        }

        for (int32_t j = 0; j < script->dialogues[i].option_size; j++) {
            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

            printf("option         | %s\n", script->dialogues[i].options[j].text);
            printf("condition      | %s\n", script->dialogues[i].options[j].condition);
            printf("next           | %s\n", script->dialogues[i].options[j].next);
            printf("next_type      | %d\n", script->dialogues[i].options[j].next_type);
            printf("hidden         | %d\n", script->dialogues[i].options[j].hidden);
        }

        if (script->dialogues[i].option_size > 0) {
            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        }

        if (i < script->dialogue_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
    printf("CONDITION\n");
    printf("----------------------------------------------------------------------\n");

    for (int32_t i = 0; i < script->condition_size; i++) {
        printf("id             | %s\n", script->conditions[i].id);
        printf("character      | %s\n", script->conditions[i].character);
        printf("character_type | %d\n", script->conditions[i].character_type);
        printf("value          | %d\n", script->conditions[i].value);
        printf("condition      | %s\n", script->conditions[i].condition);
        printf("condition_type | %d\n", script->conditions[i].condition_type);
        printf("logic          | %d\n", script->conditions[i].logic);

        if (i < script->condition_size - 1) {
            printf("----------------------------------------------------------------------\n");
        }
    }

    printf("======================================================================\n");
}

void clearScript(Script *script) {
    if (!script) {
        return;
    }

    if (script->player) {
        free(script->player);
        script->player = NULL;
    }

    if (script->characters) {
        for (int32_t i = 0; i < script->character_size; i++) {
            if (script->characters[i].status) {
                free(script->characters[i].status);
                script->characters[i].status = NULL;
                script->characters[i].status_size = 0;
            }

            if (script->characters[i].tachies) {
                free(script->characters[i].tachies);
                script->characters[i].tachies = NULL;
                script->characters[i].tachie_size = 0;
            }

            if (script->characters[i].inventory_size) {
                for (int32_t j = 0; j < script->characters[i].inventory_size; j++) {
                    free(script->characters[i].inventory[j]);
                    script->characters[i].inventory[j] = NULL;
                }

                free(script->characters[i].inventory);
                script->characters[i].inventory = NULL;
                script->characters[i].inventory_size = 0;
            }
        }

        free(script->characters);
        script->characters = NULL;
        script->character_size = 0;
    }

    if (script->items) {
        free(script->items);
        script->items = NULL;
        script->item_size = 0;
    }

    if (script->status_infos) {
        free(script->status_infos);
        script->status_infos = NULL;
        script->status_info_size = 0;
    }

    if (script->scenes) {
        free(script->scenes);
        script->scenes = NULL;
        script->scene_size = 0;
    }

    if (script->triggers) {
        free(script->triggers);
        script->triggers = NULL;
        script->trigger_size = 0;
    }

    if (script->updates) {
        free(script->updates);
        script->updates = NULL;
        script->update_size = 0;
    }

    if (script->events) {
        free(script->events);
        script->events = NULL;
        script->event_size = 0;
    }

    if (script->dialogues) {
        for (int32_t i = 0; i < script->dialogue_size; i++) {
            if (script->dialogues[i].options) {
                free(script->dialogues[i].options);
                script->dialogues[i].options = NULL;
                script->dialogues[i].option_size = 0;
            }

            if (script->dialogues[i].updates) {
                for (int32_t j = 0; j < script->dialogues[i].update_size; j++) {
                    free(script->dialogues[i].updates[j]);
                    script->dialogues[i].updates[j] = NULL;
                }

                free(script->dialogues[i].updates);
                script->dialogues[i].updates = NULL;
                script->dialogues[i].update_size = 0;
            }
        }

        free(script->dialogues);
        script->dialogues = NULL;
        script->dialogue_size = 0;
    }

    if (script->conditions) {
        free(script->conditions);
        script->conditions = NULL;
        script->condition_size = 0;
    }
}
