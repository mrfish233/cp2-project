#include "play_script.h"

int32_t initGame(Script *script, Display *display, char *dir) {
    if (script == NULL || display == NULL || dir == NULL) {
        return 1;
    }

    if (initScript(script, dir) != 0) {
        return 1;
    }

    if (!isValidScript(script)) {
        printf("error: invalid script\n");
        return 1;
    }

    if (initDisplay(display) != 0) {
        return 1;
    }

    if (updateInventoryPage(script, display) != 0) {
        printf("error: failed to update inventory page\n");
        return 1;
    }

    if (updateStatusPage(script, display) != 0) {
        printf("error: failed to update status page\n");
        return 1;
    }

    return 0;
}

int32_t resetGame(Script *script, Display *display) {
    if (script == NULL || display == NULL) {
        return 1;
    }

    char dir[STR_SIZE] = {0};
    strncpy(dir, script->dir, STR_SIZE);

    clearScript(script);

    script->current_event_id[0] = '\0';
    script->current_dialogue_id[0] = '\0';

    if (initGame(script, display, dir) != 0) {
        return 1;
    }

    return 0;
}

int32_t initScript(Script *script, char *dir) {
    if (script == NULL || dir == NULL) {
        return 1;
    }

    strncpy(script->dir, dir, STR_SIZE);

    if (script->dir[strlen(script->dir) - 1] == '/') {
        script->dir[strlen(script->dir) - 1] = '\0';
    }

    if (parseTomlScript(script) != 0) {
        return 1;
    }

    return 0;
}

int32_t initDisplay(Display *display) {
    if (display == NULL) {
        return 1;
    }

    display->path_background[0] = '\0';
    display->path_tachie[0]     = '\0';

    display->path_bgm[0] = '\0';
    display->path_sfx[0] = '\0';
    display->path_voice[0] = '\0';

    display->character_type = CHARACTER_NARRATOR;
    display->character[0] = '\0';
    display->dialogue[0]  = '\0';

    display->continue_flag = 0;

    display->inventory_size = 0;
    display->inventory_page = 1;

    display->status_size = 0;
    display->status_page = 1;

    display->option_size = 0;
    display->option_flag = 0;
    display->option_select = 0;

    display->update_size = 0;
    display->update_flag = 0;

    display->end_path[0] = '\0';
    display->end_flag = 0;

    return 0;
}

int32_t updateScriptData(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        printf("error: display or script is NULL\n");
        return 1;
    }

    if (updateDialogue(script, display) != 0) {
        printf("error: failed to update dialogue\n");
        return 1;
    }

    if (updateInventoryPage(script, display) != 0) {
        printf("error: failed to update inventory page\n");
        return 1;
    }

    if (updateStatusPage(script, display) != 0) {
        printf("error: failed to update status page\n");
        return 1;
    }

    return 0;
}

int32_t updateInventoryPage(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        return 1;
    }

    Character *player = getCharacter(script, script->player->character);

    if (player == NULL) {
        printf("error: player not found\n");
        return 1;
    }

    // Get the number of non-hidden items

    int32_t non_hidden_size = 0;

    for (int32_t i = 0; i < player->inventory_size; i++) {
        int32_t check = isHiddenItem(script->items, script->item_size, player->inventory[i]);

        if (check == -1) {
            printf("error: item not found: %s\n", player->inventory[i]);
            return 1;
        }
        else if (check == 0) {
            non_hidden_size++;
        }
    }

    if (non_hidden_size == 0) {
        display->inventory_size = 0;
        display->inventory_page = 0;
        return 0;
    }

    // Update the page number

    if (display->inventory_page < 1) {
        display->inventory_page = (non_hidden_size - 1) / 5 + 1;
    }
    else if (display->inventory_page > (non_hidden_size - 1) / 5 + 1) {
        display->inventory_page = 1;
    }

    display->inventory_size = 0;

    // Copy the non-hidden items to the display

    int32_t count = 0;

    for (int32_t i = 0; i < player->inventory_size; i++) {
        Item *item = getItem(script, player->inventory[i]);

        if (item == NULL) {
            printf("error: item not found: %s\n", player->inventory[i]);
            return 1;
        }

        if (item->hidden == 1) {
            continue;
        }

        if ((display->inventory_page - 1) * 5 <= count && count < display->inventory_page * 5) {
            strncpy(display->inventory[display->inventory_size], item->name, STR_SIZE);
            display->inventory_size++;
        }

        if (display->inventory_size >= 5) {
            break;
        }

        count++;
    }

    return 0;
}

int32_t updateStatusPage(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        return 1;
    }

    Character *player = getCharacter(script, script->player->character);

    if (player == NULL) {
        printf("error: player not found\n");
        return 1;
    }

    // Update the page number

    if (display->status_page < 1) {
        display->status_page = (player->status_size - 1) / 5 + 1;
    }
    else if (display->status_page > (player->status_size - 1) / 5 + 1) {
        display->status_page = 1;
    }

    display->status_size = 0;

    // Copy the non-hidden statuses to the display

    int32_t count = 0;

    for (int32_t i = 0; i < player->status_size; i++) {
        StatusInfo *status_info = getStatusInfo(script, player->status[i].status_name);

        if (status_info == NULL) {
            printf("error: status not found: %s\n", player->status[i].status_name);
            return 1;
        }

        if ((display->status_page - 1) * 5 <= count && count < display->status_page * 5) {
            strncpy(display->status[display->status_size].status_name, status_info->name, STR_SIZE);
            display->status[display->status_size].value = player->status[i].value;
            display->status_size++;
        }

        if (display->status_size >= 5) {
            break;
        }

        count++;
    }

    return 0;
}

int32_t updateDialogue(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        return 1;
    }

    Event *event = NULL;
    Scene *scene = NULL;
    Dialogue *dialogue = NULL;

    // Check all triggers

    if (checkAllTriggers(script, display) != 0) {
        printf("error: failed to check all triggers\n");
        return 1;
    }

    event = getEvent(script, script->current_event_id);

    if (event == NULL) {
        printf("error: event '%s' not found\n", script->current_event_id);
        return 1;
    }

    snprintf(display->path_bgm, STR_SIZE * 2, "%s/%s", script->dir, event->bgm);

    // Get the background

    scene = getScene(script, event->scene);

    if (scene == NULL) {
        printf("error: scene '%s' not found\n", event->scene);
        return 1;
    }

    snprintf(display->path_background, STR_SIZE * 2, "%s/%s", script->dir, scene->background);

    // Get the dialogue

    size_t len = strlen(script->current_dialogue_id);

    if (len) {
        dialogue = getDialogue(script, script->current_dialogue_id);
    }
    else {
        dialogue = getDialogue(script, event->dialogue);
        display->continue_flag = 1;
    }

    if (dialogue == NULL) {
        printf("error: dialogue '%s' not found\n", (len) ? script->current_dialogue_id : event->dialogue);
        return 1;
    }

    if (strlen(dialogue->sfx) > 0) {
        snprintf(display->path_sfx, STR_SIZE * 2, "%s/%s", script->dir, dialogue->sfx);
    }
    else {
        display->path_sfx[0] = '\0';
    }

    if (dialogue->next_type == DIALOGUE_END) {
        strncpy(display->end_path, dialogue->next, STR_SIZE);
        display->end_flag = 1;
    }
    else if (display->continue_flag) {
        display->continue_flag = 0;
    }
    else if (dialogue->next_type == DIALOGUE_EVENT) {
        event = getEvent(script, dialogue->next);

        if (event == NULL) {
            printf("error: event '%s' not found\n", dialogue->next);
            return 1;
        }

        strncpy(script->current_event_id, event->id, STR_SIZE);
        script->current_dialogue_id[0] = '\0';

        return updateDialogue(script, display);
    }
    else if ((dialogue->next_type == DIALOGUE_OPTION && display->option_select == 0) ||
             (dialogue->next_type == DIALOGUE_NORMAL)) {
        Dialogue *next = getDialogue(script, dialogue->next);

        if (next == NULL) {
            printf("error: dialogue '%s' not found\n", dialogue->next);
            return 1;
        }

        dialogue = next;
    }
    else if (dialogue->next_type == DIALOGUE_OPTION && display->option_select > 0) {
        if (display->option_select <= 0 || display->option_select > display->option_size) {
            printf("error: invalid option\n");
            return 1;
        }

        int32_t opt = display->option_select - 1;
        display->option_select = 0;

        Option *option = getDialogueOption(script, dialogue, display->options[opt]);

        if (option == NULL) {
            printf("error: option '%s' not found\n", display->options[opt]);
            return 1;
        }

        if (option->next_type == DIALOGUE_EVENT) {
            event = getEvent(script, option->next);

            if (event == NULL) {
                printf("error: event '%s' not found\n", option->next);
                return 1;
            }

            strncpy(script->current_event_id, event->id, STR_SIZE);
            script->current_dialogue_id[0] = '\0';

            return updateDialogue(script, display);
        }
        else if (option->next_type == DIALOGUE_NORMAL) {
            Dialogue *next = getDialogue(script, option->next);

            if (next == NULL) {
                printf("error: dialogue '%s' not found\n", option->next);
                return 1;
            }

            dialogue = next;
        }
        else {
            printf("error: invalid dialogue '%s'\n", option->next);
            return 1;
        }
    }
    else {
        printf("error: invalid dialogue '%s'\n", dialogue->text);
        return 1;
    }

    strncpy(script->current_dialogue_id, dialogue->id, STR_SIZE);
    strncpy(display->dialogue, dialogue->text, STR_SIZE);

    // Find the character

    display->character_type = dialogue->character_type;

    if (dialogue->character_type != CHARACTER_NARRATOR) {
        Character *character = getCharacter(script, dialogue->character);

        if (character == NULL) {
            printf("error: character '%s' not found\n", dialogue->character);
            return 1;
        }

        strncpy(display->character, character->name, STR_SIZE);

        if (strlen(character->voice) > 0) {
            snprintf(display->path_voice, STR_SIZE * 2, "%s/%s", script->dir, character->voice);
        }
        else {
            display->path_voice[0] = '\0';
        }

        Tachie *tachie = NULL;

        if (strlen(dialogue->tachie) == 0) {
            tachie = getCharacterTachie(character, "default");
        }
        else {
            tachie = getCharacterTachie(character, dialogue->tachie);
        }

        if (tachie == NULL) {
            printf("error: tachie '%s' not found\n", dialogue->tachie);
            return 1;
        }

        snprintf(display->path_tachie, STR_SIZE * 2, "%s/%s", script->dir, tachie->path);
    }
    else {
        // display->path_tachie[0] = '\0';
        // display->path_voice[0]  = '\0';
        // display->character[0]   = '\0';
        memset(display->path_tachie, 0, STR_SIZE);
        memset(display->path_voice,  0, STR_SIZE);
        memset(display->character,   0, STR_SIZE);
    }

    // Find options if any

    display->option_flag   = 0;
    display->option_size   = 0;
    display->option_select = 0;

    if (dialogue->option_size > 0) {
        display->option_flag = 1;

        for (int32_t i = 0; i < dialogue->option_size; i++) {
            int32_t met = 1;

            if (strlen(dialogue->options[i].condition) != 0) {
                Condition *condition = getCondition(script, dialogue->options[i].condition);

                if (condition == NULL) {
                    printf("error: condition '%s' not found\n", dialogue->options[i].condition);
                    return 1;
                }

                met = isConditionMet(script, condition);

                if (met == -1) {
                    printf("error: failed to check condition\n");
                    return 1;
                }
                else if (met == 0 && dialogue->options[i].hidden == 1) {
                    continue;
                }
            }

            strncpy(display->options[display->option_size], dialogue->options[i].text, STR_SIZE);
            display->options_selectable[display->option_size] = met;
            display->option_size++;

            if (display->option_size >= 5) {
                break;
            }
        }
    }

    // Find updates if any

    if (dialogue->update_size > 0) {
        display->update_flag = 1;

        for (int32_t i = 0; i < dialogue->update_size; i++) {
            if (updateCharacterData(script, dialogue->updates[i]) != 0) {
                printf("error: failed to update character data\n");
                return 1;
            }

            Update *update = getUpdate(script, dialogue->updates[i]);

            if (update == NULL) {
                printf("error: update '%s' not found\n", dialogue->updates[i]);
                return 1;
            }

            if (update->condition_type == CONDITION_ITEM) {
                Item *item = getItem(script, update->condition);

                if (item == NULL) {
                    printf("error: item '%s' not found\n", update->condition);
                    return 1;
                }

                if (item->hidden == 1) {
                    continue;
                }
            }

            processUpdateString(script, dialogue->updates[i], display->updates[display->update_size], STR_SIZE);
            display->update_size++;
        }
    }
    else {
        display->update_flag = 0;
        display->update_size = 0;
    }

    return 0;
}

int32_t checkAllTriggers(Script *script, Display *display) {
    if (script == NULL || display == NULL) {
        return 1;
    }

    for (int32_t i = 0; i < script->trigger_size; i++) {
        Trigger *trigger = &(script->triggers[i]);

        Condition temp = {
            .character = {0},
            .condition = {0},
            .condition_type = trigger->condition_type,
            .value = trigger->value,
            .logic = trigger->logic
        };

        strncpy(temp.character, trigger->character, STR_SIZE);
        strncpy(temp.condition, trigger->condition, STR_SIZE);

        if (isConditionMet(script, &temp) == 1) {
            Event *event = getEvent(script, trigger->event);

            if (event == NULL) {
                printf("error: event '%s' not found\n", trigger->event);
                return 1;
            }

            strncpy(script->current_event_id, event->id, STR_SIZE);
            script->current_dialogue_id[0] = '\0';

            break;
        }
    }

    return 0;
}

int32_t updateCharacterData(Script *script, char *update_id) {
    if (script == NULL || update_id == NULL) {
        return 1;
    }

    Update *update = getUpdate(script, update_id);

    if (update == NULL) {
        printf("error: update '%s' not found\n", update_id);
        return 1;
    }

    Character *character = getCharacter(script, update->character);

    if (character == NULL) {
        printf("error: character '%s' not found\n", update->character);
        return 1;
    }

    if (update->condition_type == CONDITION_STATUS) {
        StatusInfo *status_info = getStatusInfo(script, update->condition);

        if (status_info == NULL) {
            printf("error: status '%s' not found\n", update->condition);
            return 1;
        }

        for (int32_t i = 0; i < character->status_size; i++) {
            if (strcmp(character->status[i].status_name, update->condition) == 0) {
                character->status[i].value += update->change;

                if (character->status[i].value < status_info->min) {
                    character->status[i].value = status_info->min;
                }
                else if (character->status[i].value > status_info->max) {
                    character->status[i].value = status_info->max;
                }

                return 0;
            }
        }

        printf("error: status '%s' not found\n", update->condition);
        return 1;
    }
    else if (update->condition_type == CONDITION_ITEM) {
        if (update->change == 1) {
            for (int32_t i = 0; i < character->inventory_size; i++) {
                if (strcmp(character->inventory[i], update->condition) == 0) {
                    return 0;
                }
            }

            // Add the item to the inventory

            if (character->inventory_size == 0) {
                character->inventory = (char **) calloc(1, sizeof(char *));
            }
            else {
                character->inventory = (char **) reallocarray(character->inventory, (character->inventory_size + 1), sizeof(char *));
            }

            if (character->inventory == NULL) {
                printf("error: failed to allocate memory\n");
                return 1;
            }

            character->inventory[character->inventory_size] = (char *) calloc(STR_SIZE, sizeof(char));

            if (character->inventory[character->inventory_size] == NULL) {
                printf("error: failed to allocate memory\n");
                return 1;
            }

            strncpy(character->inventory[character->inventory_size], update->condition, STR_SIZE);

            character->inventory_size++;

            return 0;
        }
        else if (update->change == 0) {
            for (int32_t i = 0; i < character->inventory_size; i++) {
                // Remove the item from the inventory

                if (strcmp(character->inventory[i], update->condition) == 0) {
                    free(character->inventory[i]);

                    for (int32_t j = i; j < character->inventory_size - 1; j++) {
                        character->inventory[j] = character->inventory[j + 1];
                    }

                    character->inventory_size--;

                    return 0;
                }
            }

            printf("error: item '%s' not found\n", update->condition);
            return 1;
        }
        else {
            printf("error: invalid change\n");
            return 1;
        }
    }

    printf("error: invalid condition type\n");
    return 1;
}

int32_t processUpdateString(Script *script, char *update_id, char *str, int32_t size) {
    if (script == NULL || update_id == NULL || str == NULL) {
        return 1;
    }

    Update *update = getUpdate(script, update_id);

    if (update == NULL) {
        printf("error: update '%s' not found\n", update_id);
        return 1;
    }

    Character *character = getCharacter(script, update->character);

    if (character == NULL) {
        printf("error: character '%s' not found\n", update->character);
        return 1;
    }

    if (update->condition_type == CONDITION_STATUS) {
        StatusInfo *status_info = getStatusInfo(script, update->condition);

        if (status_info == NULL) {
            printf("error: status '%s' not found\n", update->condition);
            return 1;
        }

        char change_str[STR_SIZE] = {0};
        int32_t change_val = update->change;

        if (update->change >= 0) {
            strncpy(change_str, "增加了", STR_SIZE);
        }
        else {
            strncpy(change_str, "減少了", STR_SIZE);
            change_val = -change_val;
        }

        snprintf(str, size, "%s 的 %s %s %d", character->name, status_info->name, change_str, change_val);
    }
    else if (update->condition_type == CONDITION_ITEM) {
        Item *item = getItem(script, update->condition);

        if (item == NULL) {
            printf("error: item '%s' not found\n", update->condition);
            return 1;
        }

        char change_str[STR_SIZE] = {0};

        if (update->change == 1) {
            strncpy(change_str, "獲得了", STR_SIZE);
        }
        else if (update->change == 0) {
            strncpy(change_str, "失去了", STR_SIZE);
        }
        else {
            return 1;
        }

        snprintf(str, size, "%s %s %s", character->name, change_str, item->name);
    }
    else {
        return 1;
    }

    return 0;
}

int32_t isHiddenItem(Item *items, int32_t size, char *item_id) {
    if (items == NULL || item_id == NULL) {
        return -1;
    }

    for (int32_t i = 0; i < size; i++) {
        if (strcmp(items[i].id, item_id) == 0) {
            return items[i].hidden;
        }
    }

    return -1;
}

int32_t isConditionMet(Script *script, Condition *condition) {
    if (script == NULL || condition == NULL) {
        return -1;
    }

    Character *character = getCharacter(script, condition->character);

    if (character == NULL) {
        printf("error: character '%s' not found\n", condition->character);
        return -1;
    }

    if (condition->condition_type == CONDITION_STATUS) {
        for (int32_t i = 0; i < character->status_size; i++) {
            if (strcmp(character->status[i].status_name, condition->condition) == 0) {
                if (condition->logic == LOGIC_EQ) {
                    return character->status[i].value == condition->value;
                }
                else if (condition->logic == LOGIC_NE) {
                    return character->status[i].value != condition->value;
                }
                else if (condition->logic == LOGIC_GT) {
                    return character->status[i].value > condition->value;
                }
                else if (condition->logic == LOGIC_GE) {
                    return character->status[i].value >= condition->value;
                }
                else if (condition->logic == LOGIC_LT) {
                    return character->status[i].value < condition->value;
                }
                else if (condition->logic == LOGIC_LE) {
                    return character->status[i].value <= condition->value;
                }
                else {
                    printf("error: invalid logic\n");
                    return -1;
                }
            }
        }
    }
    else if (condition->condition_type == CONDITION_ITEM) {
        for (int32_t i = 0; i < character->inventory_size; i++) {
            if (strcmp(character->inventory[i], condition->condition) == 0) {
                if (condition->logic == LOGIC_EQ) {
                    return 1;
                }
                else if (condition->logic == LOGIC_NE) {
                    return 0;
                }
                else {
                    printf("error: invalid logic\n");
                    return -1;
                }
            }
        }

        if (condition->logic == LOGIC_EQ) {
            return 0;
        }
        else if (condition->logic == LOGIC_NE) {
            return 1;
        }
        else {
            printf("error: invalid logic\n");
            return -1;
        }
    }

    printf("error: condition not found\n");
    return -1;
}
