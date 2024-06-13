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

typedef enum SaveSlot {
    AUTO  = 0,
    SLOT1 = 1,
    SLOT2 = 2,
    SLOT3 = 3
} SaveSlot;

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
 * Add the script data to a JSON object.
 * @param root: the JSON object
 * @param script: the script
 * @return 0 if success, 1 otherwise
 */
int32_t addDataToJSON(cJSON **root, Script *script);
