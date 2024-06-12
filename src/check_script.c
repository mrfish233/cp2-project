#include "check_script.h"


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
