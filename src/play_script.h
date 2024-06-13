#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "script.h"
#include "check_script.h"
#include "parse_script.h"

typedef struct Display {
    char path_background[STR_SIZE];
    char path_tachie[STR_SIZE];

    char character[STR_SIZE];
    char dialogue[STR_SIZE];

    char    inventory[5][STR_SIZE];
    int32_t inventory_size;
    int32_t inventory_page;

    Status  status[5];
    int32_t status_size;
    int32_t status_page;

    char    options[5][STR_SIZE];
    int32_t options_selectable[5];
    int32_t option_size;
    int32_t option_flag;
    int32_t option_select;

    char    updates[5][STR_SIZE];
    int32_t update_size;
    int32_t update_flag;

    char    end_path[STR_SIZE];
    int32_t end_flag;
} Display;

/**
 * Update the script data
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t updateScriptData(Script *script, Display *display);

/**
 * Update the inventory page
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t updateInventoryPage(Script *script, Display *display);

/**
 * Update the status page
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t updateStatusPage(Script *script, Display *display);

/**
 * Update the dialogue
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t updateDialogue(Script *script, Display *display);

/**
 * Check all triggers
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t checkAllTriggers(Script *script, Display *display);

/**
 * Update the character data
 * @param script The script to use
 * @param update_id The id of the update to search
 * @return 0 if successful, 1 if failed
 */
int32_t updateCharacterData(Script *script, char *update_id);

/**
 * Process the update string to be displayed
 * @param script The script to use
 * @param update_id The id of the update to search
 * @param str The string to process
 * @param size The size of the string
 * @return 0 if successful, 1 if failed
 */
int32_t processUpdateString(Script *script, char *update_id, char *str, int32_t size);

/**
 * Check if an item is hidden
 * @param items The items struct
 * @param size The size of the items
 * @param item_id The id of the item to search
 * @return 1 if hidden, 0 if not hidden, -1 if failed
 */
int32_t isHiddenItem(Item *items, int32_t size, char *item_id);

/**
 * Check if a condition is met
 * @param script The script to use
 * @param condition The condition to check
 * @return 1 if met, 0 if not met, -1 if failed
 */
int32_t isConditionMet(Script *script, Condition *condition);
