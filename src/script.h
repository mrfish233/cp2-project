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
 * @param icon: path to the icon of the item
 * @param hidden: indicate if the item is hidden
 */
typedef struct _Item {
    char id[STR_SIZE];
    char name[STR_SIZE];
    char desc[STR_SIZE];
    char icon[STR_SIZE];
    int32_t hidden;
} Item;

// Character

/**
 * Character struct, contains information about a character
 * @param id: display name of the character
 * @param name: name of the character
 * @param avatar: path to the avatar image
 * @param tachie: path to the tachie image
 * @param status: array of Status
 * @param status_size: size of status
 * @param inventory: array of item names
 * @param inventory_size: size of inventory
 */
typedef struct _Character {
    char id[STR_SIZE];
    char name[STR_SIZE];

    char avatar[STR_SIZE];
    char tachie[STR_SIZE];

    Status *status;
    int32_t status_size;

    char **inventory;
    int32_t inventory_size;
} Character;
 
/**
 * Player struct, contains information about the player
 * @param character: name of the character
 * @param start_event: name of the start event
 */
typedef struct _Player {
    char character[STR_SIZE];
    char start_event[STR_SIZE];
} Player;

// Scene

/**
 * Scene struct, contains information about a scene
 * @param id: display name of the scene
 * @param name: name of the scene
 * @param background: path to the background image
 */
typedef struct _Scene {
    char id[STR_SIZE];
    char name[STR_SIZE];
    char background[STR_SIZE];
} Scene;

// Dialogue

/**
 * CharacterType enum, default is CHARACTER_NARRATOR
 * @param CHARACTER_NARRATOR: narrator character
 * @param CHARACTER_PLAYER: player character
 * @param CHARACTER_NPC: npc character
 */
typedef enum _CharacterType {
    CHARACTER_NARRATOR,
    CHARACTER_PLAYER,
    CHARACTER_NPC
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
 * DialogueType enum, default is DIALOGUE_OPTION
 * @param DIALOGUE_OPTION: dialogue with options
 * @param DIALOGUE_NORMAL: normal dialogue
 * @param DIALOGUE_EVENT:  dialogue pointing to an event
 */
typedef enum _DialogueType {
    DIALOGUE_OPTION,
    DIALOGUE_NORMAL,
    DIALOGUE_EVENT,
    DIALOGUE_END
} DialogueType;

typedef enum _LogicType {
    LOGIC_NONE,
    LOGIC_EQ,
    LOGIC_NE,
    LOGIC_GT,
    LOGIC_GE,
    LOGIC_LT,
    LOGIC_LE
} LogicType;

/**
 * Condition struct, used in Option
 * @param id: id of the condition
 * @param condition_type: type of the condition
 * @param condition: name of the condition
 * @param character_type: type of the character
 * @param character: name of the character
 * @param logic: logic of the condition
 */
typedef struct _Condition {
    char id[STR_SIZE];

    ConditionType condition_type;
    char condition[STR_SIZE];

    int32_t value;

    CharacterType character_type;
    char character[STR_SIZE];

    LogicType logic;
} Condition;

/**
 * Option struct, used in Dialogue
 * @param text: text of the option
 * @param condition: name of the condition
 * @param next_type: type of the next element
 * @param next: name of the next element
 * @param hidden: hidden flag
 */
typedef struct _Option {
    char text[STR_SIZE];
    char condition[STR_SIZE];

    DialogueType next_type;
    char next[STR_SIZE];

    int32_t hidden;
} Option;

/**
 * Dialogue struct
 * @param id: id of the dialogue
 * @param text: text of the dialogue
 * @param sfx: path to the sound effect
 * @param character_type: type of the character
 * @param character: name of the character
 * @param next_type: type of the next element
 * @param next: name of the next element
 * @param options: array of Option
 * @param option_size: size of options
 */
typedef struct _Dialogue {
    char id[STR_SIZE];
    char text[STR_SIZE];
    char sfx[STR_SIZE];

    CharacterType character_type;
    char character[STR_SIZE];

    DialogueType next_type;
    char next[STR_SIZE];

    Option *options;
    int32_t option_size;
} Dialogue;

// Event

/**
 * Event struct, stores all events and the first dialogue of the event
 * @param id: id of the event
 * @param scene: name of the scene
 * @param dialogue: name of the dialogue
 * @param bgm: path to the background music
 */
typedef struct _Event {
    char id[STR_SIZE];
    char scene[STR_SIZE];
    char dialogue[STR_SIZE];
    char bgm[STR_SIZE];
} Event;

// Trigger

/**
 * Trigger struct, contains information about an end trigger
 * @param id: id of the trigger
 * @param character: name of the character
 * @param event: name of the event
 * @param condition_type: type of the condition
 * @param condition: name of the condition
 * @param value: value of the condition
 * @param logic: logic of the condition
 */
typedef struct _Trigger {
    char id[STR_SIZE];
    char character[STR_SIZE];
    char event[STR_SIZE];

    ConditionType condition_type;
    char condition[STR_SIZE];

    int32_t value;

    LogicType logic;
} Trigger;

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

    Trigger *triggers;
    int32_t trigger_size;

    Event *events;
    int32_t event_size;

    Scene *scenes;
    int32_t scene_size;

    Dialogue *dialogues;
    int32_t dialogue_size;

    Condition *conditions;
    int32_t condition_size;
} Script;
