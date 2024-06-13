#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "parse_script.h"
#include "script.h"

/**
 * Check if the script is valid
 * @param script The script to check
 * @return 1 if the script is valid, 0 otherwise
 */
int32_t isValidScript(Script *script);

/**
 * Get the character
 * @param script The script to search
 * @param character_id The id of the character to search
 * @return The current character if found, NULL otherwise
 */
Character *getCharacter(Script *script, char *character_id);

/**
 * Get the character tachie
 * @param character The character to search
 * @param tachie_name The name of the tachie to search
 * @return The current tachie if found, NULL otherwise
 */
Tachie *getCharacterTachie(Character *character, char *tachie_name);

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
 * Get the update
 * @param script The script to search
 * @param update_id The id of the update to search
 * @return The current update if found, NULL otherwise
 */
Update *getUpdate(Script *script, char *update_id);

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
 * Get the dialogue option
 * @param script The script to search
 * @param dialogue The dialogue to search
 * @param option_text The text of the option to search
 * @return The current option if found, NULL otherwise
 */
Option *getDialogueOption(Script *script, Dialogue *dialogue, char *option_text);

/**
 * Get the condition
 * @param script The script to search
 * @param condition_id The id of the condition to search
 * @return The current condition if found, NULL otherwise
 */
Condition *getCondition(Script *script, char *condition_id);

