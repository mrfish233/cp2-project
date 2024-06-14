#include "save_and_load.h"

int32_t findSaveEvent(Script *script, char save_event[SAVE_SIZE][STR_SIZE], char save_datetime[SAVE_SIZE][STR_SIZE], int32_t hasSaveFile[SAVE_SIZE]) {
    for (int32_t i = 0; i < SAVE_SIZE; i++) {
        char save_str[STR_SIZE] = {0};

        if (snprintf(save_str, STR_SIZE, SAVE_FILE_FORMAT, script->dir, i) < 0) {
            return 1;
        }

        if (access(save_str, F_OK) != 0) {
            hasSaveFile[i] = 0;
            continue;
        }

        cJSON *root = NULL;

        int32_t result = loadJSONData(&root, script->dir, i);

        if (result == 2) {
            hasSaveFile[i] = 0;
            continue;
        }
        else if (result != 0) {
            return 1;
        }

        hasSaveFile[i] = 1;

        // Event

        cJSON *event = cJSON_GetObjectItem(root, "event");

        if (event == NULL) {
            cJSON_Delete(root);
            return 1;
        }

        Event *e = getEvent(script, event->valuestring);

        if (e == NULL) {
            cJSON_Delete(root);
            return 1;
        }

        strncpy(save_event[i], e->id, STR_SIZE);

        // Datetime

        cJSON *datetime = cJSON_GetObjectItem(root, "datetime");

        if (datetime == NULL) {
            cJSON_Delete(root);
            return 1;
        }

        strncpy(save_datetime[i], datetime->valuestring, STR_SIZE);

        cJSON_Delete(root);
    }

    return 0;
}

int32_t saveScript(Script *script, SaveSlot slot) {
    if (script == NULL) {
        return 1;
    }

    // Create a JSON object to store the data

    cJSON *root = cJSON_CreateObject();

    if (root == NULL) {
        return 1;
    }

    if (addDataToJSON(&root, script) != 0) {
        cJSON_Delete(root);
        return 1;
    }

    char *json = cJSON_Print(root);

    cJSON_Delete(root);

    if (json == NULL) {
        return 1;
    }

    // Save the JSON object to a file

    FILE *save_file = NULL;
    char save_str[STR_SIZE] = {0};

    if (snprintf(save_str, STR_SIZE, SAVE_FILE_FORMAT, script->dir, slot) < 0) {
        free(json);
        return 1;
    }

    if (openFile(&save_file, save_str, "w") != 0) {
        free(json);
        return 1;
    }

    fwrite(json, sizeof(char), strlen(json), save_file);

    closeFile(save_file);
    free(json);

    return 0;
}

int32_t loadScript(Script *script, SaveSlot slot) {
    if (script == NULL) {
        return 1;
    }

    cJSON *root = NULL;

    if (loadJSONData(&root, script->dir, slot) != 0) {
        return 1;
    }

    cJSON *event = cJSON_GetObjectItem(root, "event");

    if (event == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    strncpy(script->current_event_id, event->valuestring, STR_SIZE);

    cJSON *characters = cJSON_GetObjectItem(root, "characters");

    if (characters == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    for (int32_t i = 0; i < cJSON_GetArraySize(characters); i++) {
        cJSON *chr = cJSON_GetArrayItem(characters, i);

        if (chr == NULL) {
            cJSON_Delete(root);
            return 1;
        }

        cJSON *id = cJSON_GetObjectItem(chr, "id");

        if (id == NULL) {
            cJSON_Delete(root);
            return 1;
        }

        // Find the character

        for (int32_t j = 0; j < script->character_size; j++) {
            if (strncmp(script->characters[j].id, id->valuestring, STR_SIZE) != 0) {
                continue;
            }

            // Status

            cJSON *status = cJSON_GetObjectItem(chr, "status");

            if (status == NULL) {
                cJSON_Delete(root);
                return 1;
            }

            for (int32_t k = 0; k < cJSON_GetArraySize(status); k++) {
                cJSON *st = cJSON_GetArrayItem(status, k);

                if (st == NULL) {
                    cJSON_Delete(root);
                    return 1;
                }

                cJSON *status_name = cJSON_GetObjectItem(st, "status_name");

                if (status_name == NULL) {
                    cJSON_Delete(root);
                    return 1;
                }

                cJSON *value = cJSON_GetObjectItem(st, "value");

                if (value == NULL) {
                    cJSON_Delete(root);
                    return 1;
                }

                for (int32_t l = 0; l < script->characters[j].status_size; l++) {
                    if (strncmp(script->characters[j].status[l].status_name, status_name->valuestring, STR_SIZE) == 0) {
                        script->characters[j].status[l].value = (int32_t) cJSON_GetNumberValue(value);
                        break;
                    }
                }
            }

            // Inventory

            cJSON *inventory = cJSON_GetObjectItem(chr, "inventory");

            if (inventory == NULL) {
                cJSON_Delete(root);
                return 1;
            }

            if (script->characters[j].inventory != NULL) {
                for (int32_t k = 0; k < script->characters[j].inventory_size; k++) {
                    free(script->characters[j].inventory[k]);
                }

                free(script->characters[j].inventory);
                script->characters[j].inventory = NULL;
            }

            script->characters[j].inventory_size = cJSON_GetArraySize(inventory);

            if (script->characters[j].inventory_size == 0) {
                break;
            }

            script->characters[j].inventory = (char **) calloc(script->characters[j].inventory_size, sizeof(char *));

            if (script->characters[j].inventory == NULL) {
                cJSON_Delete(root);
                return 1;
            }

            for (int32_t k = 0; k < cJSON_GetArraySize(inventory); k++) {
                cJSON *inv = cJSON_GetArrayItem(inventory, k);

                if (inv == NULL) {
                    cJSON_Delete(root);
                    return 1;
                }

                script->characters[j].inventory[k] = (char *) calloc(STR_SIZE, sizeof(char));

                if (script->characters[j].inventory[k] == NULL) {
                    cJSON_Delete(root);
                    return 1;
                }

                strncpy(script->characters[j].inventory[k], inv->valuestring, STR_SIZE);
            }

            break;
        }
    }

    cJSON_Delete(root);

    return 0;
}

int32_t loadJSONData(cJSON **root, char *dir, SaveSlot slot) {
    FILE *save_file = NULL;
    char save_str[STR_SIZE] = {0};

    if (snprintf(save_str, STR_SIZE, SAVE_FILE_FORMAT, dir, slot) < 0) {
        return 1;
    }

    if (openFile(&save_file, save_str, "r") != 0) {
        return 2;
    }

    int32_t size = 0;

    fseek(save_file, 0, SEEK_END);
    size = ftell(save_file);
    fseek(save_file, 0, SEEK_SET);

    char *json = (char *) calloc(size + 1, sizeof(char));

    if (json == NULL) {
        closeFile(save_file);
        return 1;
    }

    fread(json, sizeof(char), size, save_file);

    *root = cJSON_Parse(json);

    closeFile(save_file);
    free(json);

    if (*root == NULL) {
        return 1;
    }

    return 0;
}

int32_t addDataToJSON(cJSON **root, Script *script) {
    if (script == NULL || *root == NULL) {
        return 1;
    }

    // Datetime

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char datetime[STR_SIZE] = {0};

    if (snprintf(datetime, STR_SIZE, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec) < 0) {
        return 1;
    }

    if (cJSON_AddStringToObject(*root, "datetime", datetime) == NULL) {
        return 1;
    }

    // Event

    if (cJSON_AddStringToObject(*root, "event", script->current_event_id) == NULL) {
        return 1;
    }

    // Characters

    cJSON *characters = cJSON_AddArrayToObject(*root, "characters");

    if (characters == NULL) {
        return 1;
    }

    for (int32_t i = 0; i < script->character_size; i++) {
        cJSON *chr = cJSON_CreateObject();

        if (chr == NULL) {
            return 1;
        }

        if (cJSON_AddStringToObject(chr, "id", script->characters[i].id) == NULL) {
            return 1;
        }

        // Status

        cJSON *status = cJSON_AddArrayToObject(chr, "status");

        if (status == NULL) {
            return 1;
        }

        for (int32_t j = 0; j < script->characters[i].status_size; j++) {
            cJSON *st = cJSON_CreateObject();

            if (st == NULL) {
                return 1;
            }

            if (cJSON_AddStringToObject(st, "status_name", script->characters[i].status[j].status_name) == NULL) {
                return 1;
            }

            if (cJSON_AddNumberToObject(st, "value", script->characters[i].status[j].value) == NULL) {
                return 1;
            }

            cJSON_AddItemToArray(status, st);
        }

        // Inventory

        cJSON *inventory = cJSON_AddArrayToObject(chr, "inventory");

        if (inventory == NULL) {
            return 1;
        }

        for (int32_t j = 0; j < script->characters[i].inventory_size; j++) {
            cJSON *inv = cJSON_CreateString(script->characters[i].inventory[j]);

            if (inv == NULL) {
                return 1;
            }

            cJSON_AddItemToArray(inventory, inv);
        }

        cJSON_AddItemToArray(characters, chr);
    }

    return 0;
}
