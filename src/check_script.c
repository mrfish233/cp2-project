#include "check_script.h"

int32_t isValidScript(Script *script) {
    int32_t valid = 1;

    if (script == NULL) {
        printf("Script not found\n");
        return 0;
    }

    /**
     * Check player
     */

    if (getCharacter(script, script->player->character) == NULL) {
        printf("Player character '%s' not found\n", script->player->character);
        valid = 0;
    }

    if (getEvent(script, script->player->start_event) == NULL) {
        printf("Player start event '%s' not found\n", script->player->start_event);
        valid = 0;
    }

    /**
     * Check characters
     */

    for (int32_t i = 0; i < script->character_size; i++) {
        Character *chr = &(script->characters[i]);

        if (chr->tachie_size > 0) {
            for (int32_t j = 0; j < chr->tachie_size; j++) {
                Tachie *tachie = &(chr->tachies[j]);

                char path[STR_SIZE] = {0};

                snprintf(path, STR_SIZE * 2, "%s/%s", script->dir, tachie->path);

                if (access(path, F_OK) != 0) {
                    printf("Character '%s' tachie '%s' not found: %s\n", chr->id, tachie->name, tachie->path);
                    valid = 0;
                }
            }
        }

        if (chr->status_size > 0) {
            for (int32_t j = 0; j < chr->status_size; j++) {
                if (getStatusInfo(script, chr->status[j].status_name) == NULL) {
                    printf("Character '%s' status '%s' not found\n", chr->id, chr->status[j].status_name);
                    valid = 0;
                }
            }
        }

        if (chr->inventory_size > 0) {
            for (int32_t j = 0; j < chr->inventory_size; j++) {
                if (getItem(script, chr->inventory[j]) == NULL) {
                    printf("Character '%s' inventory item '%s' not found\n", chr->id, chr->inventory[j]);
                    valid = 0;
                }
            }
        }

        // Check duplicate character id

        for (int32_t j = i + 1; j < script->character_size; j++) {
            if (strcmp(chr->id, script->characters[j].id) == 0) {
                printf("Duplicate character id: %s\n", chr->id);
                valid = 0;
            }
        }
    }

    /**
     * Check item
     */

    for (int32_t i = 0; i < script->item_size; i++) {
        Item *item = &(script->items[i]);

        char path[STR_SIZE] = {0};

        snprintf(path, STR_SIZE * 2, "%s/%s", script->dir, item->icon);

        if (access(path, F_OK) != 0) {
            printf("Item '%s' icon not found: %s\n", item->id, item->icon);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->item_size; j++) {
            if (strcmp(item->id, script->items[j].id) == 0) {
                printf("Duplicate item id: %s\n", item->id);
                valid = 0;
            }
        }
    }

    /**
     * Check status info
     */

    for (int32_t i = 0; i < script->status_info_size; i++) {
        StatusInfo *status = &(script->status_infos[i]);

        if (status->min > status->max) {
            printf("Status info '%s' min is greater than max\n", status->id);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->status_info_size; j++) {
            if (strcmp(status->id, script->status_infos[j].id) == 0) {
                printf("Duplicate status info id: %s\n", status->id);
                valid = 0;
            }
        }

    }

    /**
     * Check updates
     */

    for (int32_t i = 0; i < script->update_size; i++) {
        Update *update = &(script->updates[i]);

        Character *chr = getCharacter(script, update->character);

        if (chr == NULL) {
            printf("Update '%s' character '%s' not found\n", update->id, update->character);
            valid = 0;
        }

        if (update->condition_type == CONDITION_ITEM) {
            if (getItem(script, update->condition) == NULL) {
                printf("Update '%s' item '%s' not found\n", update->id, update->condition);
                valid = 0;
            }
        }
        else if (update->condition_type == CONDITION_STATUS) {
            if (getStatusInfo(script, update->condition) == NULL) {
                printf("Update '%s' status '%s' not found\n", update->id, update->condition);
                valid = 0;
            }
        }
        else {
            printf("Update '%s' invalid condition type\n", update->id);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->update_size; j++) {
            if (strcmp(update->id, script->updates[j].id) == 0) {
                printf("Duplicate update id: %s\n", update->id);
                valid = 0;
            }
        }
    }

    /**
     * Check triggers
     */

    for (int32_t i = 0; i < script->trigger_size; i++) {
        Trigger *trigger = &(script->triggers[i]);

        Character *chr = getCharacter(script, trigger->character);

        if (chr == NULL) {
            printf("Trigger '%s' character '%s' not found\n", trigger->id, trigger->character);
            valid = 0;
        }

        if (getEvent(script, trigger->event) == NULL) {
            printf("Trigger '%s' event '%s' not found\n", trigger->id, trigger->event);
            valid = 0;
        }

        if (trigger->condition_type == CONDITION_ITEM) {
            if (getItem(script, trigger->condition) == NULL) {
                printf("Trigger '%s' item '%s, not found\n", trigger->id, trigger->condition);
                valid = 0;
            }

            if (trigger->value < 0 || trigger->value > 1) {
                printf("Trigger '%s' item value must be 0 or 1\n", trigger->id);
                valid = 0;
            }

            if ((trigger->logic != LOGIC_EQ && trigger->logic != LOGIC_NE) || trigger->logic == LOGIC_NONE) {
                printf("Trigger '%s' item logic must be EQ or NE\n", trigger->id);
                valid = 0;
            }
        }
        else if (trigger->condition_type == CONDITION_STATUS) {
            StatusInfo *status = getStatusInfo(script, trigger->condition);

            if (status == NULL) {
                printf("Trigger '%s' status not found\n", trigger->id);
                valid = 0;
            }

            if (trigger->value < status->min || trigger->value > status->max) {
                printf("Trigger '%s' status value must be between %d and %d\n", trigger->id, status->min, status->max);
                valid = 0;
            }

            if (trigger->logic == LOGIC_NONE) {
                printf("Trigger '%s' status logic must exist\n", trigger->id);
                valid = 0;
            }
        }
        else {
            printf("Trigger '%s' invalid condition type\n", trigger->id);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->trigger_size; j++) {
            if (strcmp(trigger->id, script->triggers[j].id) == 0) {
                printf("Duplicate trigger id: %s\n", trigger->id);
                valid = 0;
            }
        }
    }

    /**
     * Check events
     */

    for (int32_t i = 0; i < script->event_size; i++) {
        Event *event = &(script->events[i]);

        if (getScene(script, event->scene) == NULL) {
            printf("Event '%s' scene '%s' not found\n", event->id, event->scene);
            valid = 0;
        }

        if (getDialogue(script, event->dialogue) == NULL) {
            printf("Event '%s' dialogue '%s' not found\n", event->id, event->dialogue);
            valid = 0;
        }

        if (strlen(event->bgm) != 0) {
            char path[STR_SIZE] = {0};

            snprintf(path, STR_SIZE * 2, "%s/%s", script->dir, event->bgm);

            if (access(event->bgm, F_OK) != 0) {
                printf("Event '%s' bgm not found: %s\n", event->id, event->bgm);
                valid = 0;
            }
        }

        for (int32_t j = i + 1; j < script->event_size; j++) {
            if (strcmp(event->id, script->events[j].id) == 0) {
                printf("Duplicate event id: %s\n", event->id);
                valid = 0;
            }
        }
    }

    /**
     * Check scenes
     */

    for (int32_t i = 0; i < script->scene_size; i++) {
        Scene *scene = &(script->scenes[i]);

        char path[STR_SIZE] = {0};

        snprintf(path, STR_SIZE * 2, "%s/%s", script->dir, scene->background);

        if (access(path, F_OK) != 0) {
            printf("Scene '%s' background not found: %s\n", scene->id, scene->background);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->scene_size; j++) {
            if (strcmp(scene->id, script->scenes[j].id) == 0) {
                printf("Duplicate scene id: %s\n", scene->id);
                valid = 0;
            }
        }
    }

    /**
     * Check dialogues
     */

    for (int32_t i = 0; i < script->dialogue_size; i++) {
        Dialogue *dialogue = &(script->dialogues[i]);

        char path[STR_SIZE] = {0};

        snprintf(path, STR_SIZE * 2, "%s/%s", script->dir, dialogue->sfx);

        if (access(path, F_OK) != 0) {
            printf("Dialogue '%s' sfx not found: %s\n", dialogue->id, dialogue->sfx);
            valid = 0;
        }

        if (dialogue->character_type != CHARACTER_NARRATOR) {
            if (getCharacter(script, dialogue->character) == NULL) {
                printf("Dialogue '%s' character '%s' not found\n", dialogue->id, dialogue->character);
                valid = 0;
            }
        }

        if (dialogue->next_type == DIALOGUE_OPTION) {
            if (dialogue->option_size == 0) {
                printf("Dialogue '%s' has no options\n", dialogue->id);
                valid = 0;
            }
        }
        else if (dialogue->next_type == DIALOGUE_NORMAL) {
            if (getDialogue(script, dialogue->next) == NULL) {
                printf("Dialogue '%s' next dialogue '%s' not found\n", dialogue->id, dialogue->next);
                valid = 0;
            }
        }
        else if (dialogue->next_type == DIALOGUE_EVENT) {
            if (getEvent(script, dialogue->next) == NULL) {
                printf("Dialogue '%s' next event '%s' not found\n", dialogue->id, dialogue->next);
                valid = 0;
            }
        }
        else if (dialogue->next_type == DIALOGUE_END) {
            if (dialogue->option_size > 0) {
                printf("Dialogue '%s' has options but next is end\n", dialogue->id);
                valid = 0;
            }
        }
        else {
            printf("Dialogue '%s' invalid next type\n", dialogue->id);
            valid = 0;
        }

        for (int32_t j = 0; j < dialogue->option_size; j++) {
            Option *option = &(dialogue->options[j]);

            if (strlen(option->condition) > 0) {
                Condition *condition = getCondition(script, option->condition);

                if (condition == NULL) {
                    printf("Dialogue '%s' option '%s' condition '%s' not found\n", dialogue->id, option->text, option->condition);
                    valid = 0;
                }

                if (condition->condition_type == CONDITION_ITEM) {
                    if (getItem(script, condition->condition) == NULL) {
                        printf("Dialogue '%s' option '%s' condition item '%s' not found\n", dialogue->id, option->text, condition->condition);
                        valid = 0;
                    }

                    if (condition->logic != LOGIC_EQ && condition->logic != LOGIC_NE) {
                        printf("Dialogue '%s' option '%s' condition item logic must be EQ or NE\n", dialogue->id, option->text);
                        valid = 0;
                    }
                }
                else if (condition->condition_type == CONDITION_STATUS) {
                    if (getStatusInfo(script, condition->condition) == NULL) {
                        printf("Dialogue '%s' option '%s' condition status '%s' not found\n", dialogue->id, option->text, condition->condition);
                        valid = 0;
                    }

                    if (condition->logic == LOGIC_NONE) {
                        printf("Dialogue '%s' option '%s' condition status logic must exist\n", dialogue->id, option->text);
                        valid = 0;
                    }
                }
                else {
                    printf("Dialogue '%s' option '%s' invalid condition type\n", dialogue->id, option->text);
                    valid = 0;
                }
            }

            if (option->next_type == DIALOGUE_NORMAL) {
                if (getDialogue(script, option->next) == NULL) {
                    printf("Dialogue '%s' option '%s' next dialogue '%s' not found\n", dialogue->id, option->text, option->next);
                    valid = 0;
                }
            }
            else if (option->next_type == DIALOGUE_EVENT) {
                if (getEvent(script, option->next) == NULL) {
                    printf("Dialogue '%s' option '%s' next event '%s' not found\n", dialogue->id, option->text, option->next);
                    valid = 0;
                }
            }
            else {
                printf("Dialogue '%s' option '%s' invalid next type\n", dialogue->id, option->text);
                valid = 0;
            }
        }

        for (int32_t i = 0; i < dialogue->update_size; i++) {
            if (getUpdate(script, dialogue->updates[i]) == NULL) {
                printf("Dialogue '%s' update '%s' not found\n", dialogue->id, dialogue->updates[i]);
                valid = 0;
            }
        }

        for (int32_t j = i + 1; j < script->dialogue_size; j++) {
            if (strcmp(dialogue->id, script->dialogues[j].id) == 0) {
                printf("Duplicate dialogue id: %s\n", dialogue->id);
                valid = 0;
            }
        }
    }

    /**
     * Check conditions
     */

    // TODO: Check condition logic

    for (int32_t i = 0; i < script->condition_size; i++) {
        Condition *condition = &(script->conditions[i]);

        if (condition->condition_type == CONDITION_ITEM) {
            if (getItem(script, condition->condition) == NULL) {
                printf("Condition '%s' item '%s' not found\n", condition->id, condition->condition);
                valid = 0;
            }
        }
        else if (condition->condition_type == CONDITION_STATUS) {
            if (getStatusInfo(script, condition->condition) == NULL) {
                printf("Condition '%s' status '%s' not found\n", condition->id, condition->condition);
                valid = 0;
            }
        }
        else {
            printf("Condition '%s' invalid type\n", condition->id);
            valid = 0;
        }

        for (int32_t j = i + 1; j < script->condition_size; j++) {
            if (strcmp(condition->id, script->conditions[j].id) == 0) {
                printf("Duplicate condition id: %s\n", condition->id);
                valid = 0;
            }
        }
    }

    return valid;
}

Character *getCharacter(Script *script, char *character_id) {
    if (script == NULL || character_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->character_size; i++) {
        if (strcmp(script->characters[i].id, character_id) == 0) {
            return &(script->characters[i]);
        }
    }

    return NULL;
}

Tachie *getCharacterTachie(Character *character, char *tachie_name) {
    if (character == NULL || tachie_name == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < character->tachie_size; i++) {
        if (strcmp(character->tachies[i].name, tachie_name) == 0) {
            return &(character->tachies[i]);
        }
    }

    return NULL;
}

StatusInfo *getStatusInfo(Script *script, char *status_info_id) {
    if (script == NULL || status_info_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->status_info_size; i++) {
        if (strcmp(script->status_infos[i].id, status_info_id) == 0) {
            return &(script->status_infos[i]);
        }
    }

    return NULL;
}

Item *getItem(Script *script, char *item_id) {
    if (script == NULL || item_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->item_size; i++) {
        if (strcmp(script->items[i].id, item_id) == 0) {
            return &(script->items[i]);
        }
    }

    return NULL;
}

Update *getUpdate(Script *script, char *update_id) {
    if (script == NULL || update_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->update_size; i++) {
        if (strcmp(script->updates[i].id, update_id) == 0) {
            return &(script->updates[i]);
        }
    }

    return NULL;
}

Event *getEvent(Script *script, char *event_id) {
    if (script == NULL || event_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->event_size; i++) {
        if (strcmp(script->events[i].id, event_id) == 0) {
            return &(script->events[i]);
        }
    }

    return NULL;
}

Scene *getScene(Script *script, char *scene_id) {
    if (script == NULL || scene_id == NULL) {
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
    if (script == NULL || dialogue_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->dialogue_size; i++) {
        if (strcmp(script->dialogues[i].id, dialogue_id) == 0) {
            return &(script->dialogues[i]);
        }
    }

    return NULL;
}

Option *getDialogueOption(Script *script, Dialogue *dialogue, char *option_text) {
    if (script == NULL || dialogue == NULL || option_text == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < dialogue->option_size; i++) {
        if (strcmp(dialogue->options[i].text, option_text) == 0) {
            return &(dialogue->options[i]);
        }
    }

    return NULL;
}

Condition *getCondition(Script *script, char *condition_id) {
    if (script == NULL || condition_id == NULL) {
        return NULL;
    }

    for (int32_t i = 0; i < script->condition_size; i++) {
        if (strcmp(script->conditions[i].id, condition_id) == 0) {
            return &(script->conditions[i]);
        }
    }

    return NULL;
}
