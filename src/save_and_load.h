#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <cjson/cJSON.h>

#include "script.h"
#include "parse_file.h"
#include "check_script.h"

#define SAVE_SIZE 4
#define SAVE_FILE_FORMAT "%s/save%d.json"

typedef enum SaveSlot {
    SAVE_SLOT_1 = 0,
    SAVE_SLOT_2 = 1,
    SAVE_SLOT_3 = 2,
    SAVE_SLOT_AUTO = 3
} SaveSlot;

/**
 * Find the save event from save file if the file exists.
 * @param script: the script
 * @param save_event: the save event
 * @param save_datetime: the save datetime
 * @param hasSaveFile: whether the save file exists
 * @return 0 if success, 1 otherwise
 */
int32_t findSaveEvent(Script *script, char save_event[SAVE_SIZE][STR_SIZE], char save_datetime[SAVE_SIZE][STR_SIZE], int32_t hasSaveFile[SAVE_SIZE]);

/**
 * Save the script to a file.
 * @param script: the script
 * @param slot: the save slot
 * @return 0 if success, 1 otherwise
 */
int32_t saveScript(Script *script, SaveSlot slot);

/**
 * Load the script from a file.
 * @param script: the script
 * @param slot: the save slot
 * @return 0 if success, 1 otherwise
 */
int32_t loadScript(Script *script, SaveSlot slot);

/**
 * Load the JSON data from a file.
 * @param root: the JSON object
 * @param dir: the directory
 * @param slot: the save slot
 * @return 0 if success, 1 if file not found, 2 if file is empty, 3 otherwise
 */
int32_t loadJSONData(cJSON **root, char *dir, SaveSlot slot);

/**
 * Add the script data to a JSON object.
 * @param root: the JSON object
 * @param script: the script
 * @param slot: the save slot
 * @return 0 if success, 1 otherwise
 */
int32_t addDataToJSON(cJSON **root, Script *script, SaveSlot slot);
