#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

    char    updates[5][STR_SIZE];
    int32_t update_size;
    int32_t update_flag;

    char    end_path[STR_SIZE];
    int32_t end_flag;
} Display;

/**
 * Initialize the display data
 * @param script The script to use
 * @param display The display data to initialize
 * @return 0 if successful, 1 if failed
 */
int32_t updateDisplayData(Script *script, Display *display);

/**
 * Get the character
 * @param script The script to search
 * @param character_id The id of the character to search
 * @return The current character if found, NULL otherwise
 */
Character *getCharacter(Script *script, char *character_id);

/**
 * Get the status info
 * @param character The character to search
 * @param status_info_id The id of the status to search
 * @return The current status if found, NULL otherwise
 */
StatusInfo *getStatusInfo(Script *script, char *status_info_id);

/**
 * Get the item
 * @param script The script to search
 * @param item_id The id of the item to search
 * @return The current item if found, NULL otherwise
 */
Item *getItem(Script *script, char *item_id);

/**
 * Get the event
 * @param script The script to search
 * @param event_id The id of the event to search
 * @return The current event if found, NULL otherwise
 */
Event *getEvent(Script *script, char *event_id);

/**
 * Get the scene
 * @param script The script to search
 * @param scene_id The id of the scene to search
 * @return The current scene if found, NULL otherwise
 */
Scene *getScene(Script *script, char *scene_id);

/**
 * Get the dialogue
 * @param script The script to search
 * @param dialogue_id The id of the dialogue to search
 * @return The current dialogue if found, NULL otherwise
 */
Dialogue *getDialogue(Script *script, char *dialogue_id);

/**
 * Get the condition
 * @param script The script to search
 * @param condition_id The id of the condition to search
 * @return The current condition if found, NULL otherwise
 */
Condition *getCondition(Script *script, char *condition_id);

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
 * Update the options
 * @param script The script to use
 * @param display The display data to update
 * @return 0 if successful, 1 if failed
 */
int32_t processUpdateString(Script *script, Update *update, char *str, int32_t size);

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
