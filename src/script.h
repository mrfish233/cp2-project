#pragma once

#include <stdint.h>

#define STR_SIZE 512

// Status

/**
 * StatusInfo struct, contains information about a status
 * @param id: id of the status
 * @param name: name of the status
 * @param desc: description of the status
 * @param min: minimum value of the status
 * @param max: maximum value of the status
 */
typedef struct _StatusInfo {
    char id[STR_SIZE];
    char name[STR_SIZE];
    char desc[STR_SIZE];
    int32_t min;
    int32_t max;
} StatusInfo;

/**
 * Status struct, contains a StatusInfo and a value
 * @param status_name: name of the status
 * @param value: value of the status
 */
typedef struct _Status {
    char status_name[STR_SIZE];
    int32_t value;
} Status;

// Items

/**
 * Item struct, contains information about an item
 * @param id: display name of the item
 * @param name: name of the item
 * @param desc: description of the item
 * @param icon_path: path to the icon of the item
 * @param hidden: indicate if the item is hidden
 */
typedef struct _Item {
    char id[STR_SIZE];
    char name[STR_SIZE];
    char desc[STR_SIZE];
    char icon_path[STR_SIZE];
    int32_t hidden;
} Item;

// Character

/**
 * Character struct, contains information about a character
 * @param id: display name of the character
 * @param name: name of the character
 * @param avatar_path: path to the avatar image
 * @param tachie_path: path to the tachie image
 * @param status: array of Status
 * @param status_size: size of status
 * @param inventory: array of item names
 * @param inventory_size: size of inventory
 */
typedef struct _Character {
    char id[STR_SIZE];
    char name[STR_SIZE];

    char avatar_path[STR_SIZE];
    char tachie_path[STR_SIZE];

    Status *status;
    int32_t status_size;

    char **inventory;
    int32_t inventory_size;
} Character;
 
/**
 * Player struct, contains information about the player
 * @param character_name: name of the character
 * @param start_event: name of the start event
 */
typedef struct _Player {
    char character_name[STR_SIZE];
    char start_event[STR_SIZE];
} Player;

// Scene

/**
 * Scene struct, contains information about a scene
 * @param id: display name of the scene
 * @param name: name of the scene
 * @param background_path: path to the background image
 */
typedef struct _Scene {
    char id[STR_SIZE];
    char name[STR_SIZE];
    char background_path[STR_SIZE];
} Scene;

// Dialogue

/**
 * CharacterType enum
 * @param CHARACTER_PLAYER: player character
 * @param CHARACTER_NPC: npc character
 * @param CHARACTER_NARRATOR: narrator character
 */
typedef enum _CharacterType {
    CHARACTER_NONE,
    CHARACTER_PLAYER,
    CHARACTER_NPC,
    CHARACTER_NARRATOR
} CharacterType;

/**
 * ConditionType enum
 * @param CONDITION_STATUS: condition based on status
 * @param CONDITION_ITEM: condition based on item
 */
typedef enum _ConditionType {
    CONDITION_NONE,
    CONDITION_ITEM,
    CONDITION_STATUS
} ConditionType;

/**
 * DialogueType enum
 * @param DIALOGUE_NORMAL: normal dialogue
 * @param DIALOGUE_OPTION: dialogue with options
 * @param DIALOGUE_EVENT:  dialogue pointing to an event
 */
typedef enum _DialogueType {
    DIALOGUE_NONE,
    DIALOGUE_NORMAL,
    DIALOGUE_OPTION,
    DIALOGUE_EVENT
} DialogueType;

/**
 * Condition struct, used in Option
 * @param id: id of the condition
 * @param condition_type: type of the condition
 * @param condition_name: name of the condition
 * @param character_type: type of the character
 * @param character_name: name of the character
 * @param logic: logic of the condition
 */
typedef struct _Condition {
    char id[STR_SIZE];

    ConditionType condition_type;
    char condition_name[STR_SIZE];

    CharacterType character_type;
    char character_name[STR_SIZE];

    enum {
        LOGIC_NONE,
        LOGIC_EQ,
        LOGIC_NE,
        LOGIC_GT,
        LOGIC_GE,
        LOGIC_LT,
        LOGIC_LE
    } logic;
} Condition;

/**
 * Option struct, used in Dialogue
 * @param text: text of the option
 * @param condition: name of the condition
 * @param next_type: type of the next element
 * @param next_name: name of the next element
 * @param hidden: hidden flag
 */
typedef struct _Option {
    char text[STR_SIZE];
    char condition[STR_SIZE];

    DialogueType next_type;
    char next_name[STR_SIZE];

    int32_t hidden;
} Option;

/**
 * Dialogue struct
 * @param id: id of the dialogue
 * @param text: text of the dialogue
 * @param sfx_path: path to the sound effect
 * @param character_type: type of the character
 * @param character_name: name of the character
 * @param next_type: type of the next element
 * @param next_name: name of the next element
 * @param options: array of Option
 * @param option_size: size of options
 */
typedef struct _Dialogue {
    char id[STR_SIZE];
    char text[STR_SIZE];
    char sfx_path[STR_SIZE];

    CharacterType character_type;
    char character_name[STR_SIZE];

    DialogueType next_type;
    char next_name[STR_SIZE];

    Option *options;
    int32_t option_size;
} Dialogue;

// Event

/**
 * Event struct, stores all events and the first dialogue of the event
 * @param id: id of the event
 * @param scene_name: name of the scene
 * @param dialogue_name: name of the dialogue
 * @param bgm_path: path to the background music
 * @param background_path: path to the background image
 */
typedef struct _Event {
    char id[STR_SIZE];
    char scene_name[STR_SIZE];
    char dialogue_name[STR_SIZE];
    char bgm_path[STR_SIZE];
    char background_path[STR_SIZE];
} Event;

// Script

/**
 * Script struct, contains all the data
 * @param name: name of the script
 * @param author: author of the script
 * @param player: pointer to Player
 * @param characters: array of Character
 * @param character_size: size of characters
 * @param items: array of Item
 * @param item_size: size of items
 * @param status_infos: array of StatusInfo
 * @param status_info_size: size of status_infos
 * @param scenes: array of Scene
 * @param scene_size: size of scenes
 * @param events: array of Event
 * @param event_size: size of events
 * @param dialogues: array of Dialogue
 * @param dialogue_size: size of dialogues
 * @param conditions: array of Condition
 * @param condition_size: size of conditions
 */
typedef struct _Script {
    char name[STR_SIZE];
    char author[STR_SIZE];

    Player *player;

    Character *characters;
    int32_t character_size;

    Item *items;
    int32_t item_size;

    StatusInfo *status_infos;
    int32_t status_info_size;

    Event *events;
    int32_t event_size;

    Scene *scenes;
    int32_t scene_size;

    Dialogue *dialogues;
    int32_t dialogue_size;

    Condition *conditions;
    int32_t condition_size;
} Script;
