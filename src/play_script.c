#include "play_script.h"

int32_t updateDisplayData(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        printf("error: display or script is NULL\n");
        return 1;
    }

    display->inventory_page = 1;
    display->status_page    = 1;

    if (updateInventoryPage(script, display) != 0) {
        printf("error: failed to update inventory page\n");
        return 1;
    }

    if (updateStatusPage(script, display) != 0) {
        printf("error: failed to update status page\n");
        return 1;
    }

    if (updateDialogue(script, display) != 0) {
        printf("error: failed to update dialogue\n");
        return 1;
    }

    return 0;
}

Character *getCharacter(Script *script, char *character_id) {
    if (script == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->character_size; i++) {
        if (strcmp(script->characters[i].id, character_id) == 0) {
            return &(script->characters[i]);
        }
    }

    return NULL;
}

Event *getEvent(Script *script, char *id) {
    if (script == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->event_size; i++) {
        if (strcmp(script->events[i].id, id) == 0) {
            return &(script->events[i]);
        }
    }

    return NULL;
}

Scene *getScene(Script *script, char *scene_id) {
    if (script == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->scene_size; i++) {
        if (strcmp(script->scenes[i].id, scene_id) == 0) {
            return &(script->scenes[i]);
        }
    }

    return NULL;
}

Dialogue *getDialogue(Script *script, char *dialogue_id) {
    if (script == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->dialogue_size; i++) {
        if (strcmp(script->dialogues[i].id, dialogue_id) == 0) {
            return &(script->dialogues[i]);
        }
    }

    return NULL;
}

Condition *getCondition(Script *script, char *condition_id) {
    if (script == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->condition_size; i++) {
        if (strcmp(script->conditions[i].id, condition_id) == 0) {
            return &(script->conditions[i]);
        }
    }

    return NULL;
}

int32_t updateInventoryPage(Script *script, Display *display) {
    if (display == NULL || script == NULL) {
        return 1;
    }

    Character *player = getCharacter(script, script->player->character);

    if (player == NULL) {
        return 1;
    }

    // Get the number of non-hidden items

    int32_t non_hidden_size = 0;

    for (int32_t i = 0; i < player->inventory_size; i++) {
        int32_t check = isHiddenItem(script->items, script->item_size, player->inventory[i]);

        if (check == -1) {
            printf("Item not found: %s\n", player->inventory[i]);
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
        int32_t check = isHiddenItem(script->items, script->item_size, player->inventory[i]);

        if (check != 0) {
            continue;
        }

        if ((display->inventory_page - 1) * 5 <= count && count < display->inventory_page * 5) {
            strncpy(display->inventory[display->inventory_size], player->inventory[i], STR_SIZE);
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
        if ((display->status_page - 1) * 5 <= count && count < display->status_page * 5) {
            strncpy(display->status[display->status_size].status_name, player->status[i].status_name, STR_SIZE);
            display->status[display->status_size].status_name[STR_SIZE - 1] = '\0';
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

    // If the dialogue is empty, get from the event
    // Else, check the next dialogue

    if (strlen(script->current_dialogue_id) == 0) {
        event = getEvent(script, script->current_event_id);

        if (event == NULL) {
            printf("error: event '%s' not found\n", script->current_event_id);
            return 1;
        }

        // Get the scene and background

        scene = getScene(script, event->scene);

        if (scene == NULL) {
            printf("error: scene '%s' not found\n", event->scene);
            return 1;
        }

        strncpy(display->path_background, scene->background, STR_SIZE);

        // Get the dialogue

        dialogue = getDialogue(script, event->dialogue);

        if (dialogue == NULL) {
            printf("error: dialogue '%s' not found\n", event->dialogue);
            return 1;
        }
    }
    else {
        dialogue = getDialogue(script, script->current_dialogue_id);

        if (dialogue == NULL) {
            printf("error: dialogue '%s' not found\n", script->current_dialogue_id);
            return 1;
        }

        if (dialogue->next_type == DIALOGUE_EVENT) {
            event = getEvent(script, dialogue->next);

            if (event == NULL) {
                printf("error: event '%s' not found\n", dialogue->next);
                return 1;
            }

            strncpy(script->current_event_id, event->id, STR_SIZE);
            script->current_dialogue_id[0] = '\0';

            return updateDialogue(script, display);
        }
        else if (dialogue->next_type == DIALOGUE_END) {
            strncpy(display->end_path, dialogue->next, STR_SIZE);
            display->end_flag = 1;

            return 0;
        }
        else if (dialogue->next_type == DIALOGUE_NORMAL) {
            Dialogue *next = getDialogue(script, dialogue->next);

            if (next == NULL) {
                printf("error: dialogue '%s' not found\n", dialogue->next);
                return 1;
            }

            dialogue = next;
        }
        else {
            printf("error: invalid dialogue '%s'\n", dialogue->text);
            return 1;
        }
    }

    strncpy(script->current_dialogue_id, dialogue->id, STR_SIZE);
    strncpy(display->dialogue, dialogue->text, STR_SIZE);

    // Find the character

    if (dialogue->character_type != CHARACTER_NARRATOR) {
        Character *character = getCharacter(script, dialogue->character);

        if (character == NULL) {
            printf("error: character '%s' not found\n", dialogue->character);
            return 1;
        }

        strncpy(display->path_tachie, character->tachie, STR_SIZE);
        strncpy(display->character, character->name, STR_SIZE);
    }
    else {
        display->path_tachie[0] = '\0';
        display->character[0]   = '\0';
    }

    // Find options if any

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
        }
    }
    else {
        display->option_flag = 0;
        display->option_size = 0;
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
    }

    return -1;
}
