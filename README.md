# CP2 Final Project - IFE

The final project for the course Computer Programming II in NTNU. The project is Interactive Fiction Engine (IFE) and the rules about this project can be seen [here](https://hackmd.io/@cp2023/final-project-spec).

Team:

- 41247039S 韓欣劭
- 41247050S 郝東彥

## Features

About characters:

- Customizable number of characters
- Customizable number of status
- Customizable number of items
- Each character has different images to show different expressions
- Each character accepts a voice
- Each character has a unique inventory
- Each character has a unique status
- Each item is able to be *hidden* to hide some information to the player
- All unhidden items and status of player will be showed in game
- Items and status are able to be updated in between dialogues

About stories:

- Customizable scenes for different background images
- Customizable events for indicating the current scene and start dialogue
- Customizable dialogues for connecting all the dialogues
- Each dialogue will show the tachie of the character who is speaking
- Each dialogue accepts different tachie of that character
- Each dialogue can have customizable updates to update the inventory or status of characters
- Each dialogue can have customizable options to create the branches for the story
- Each option is able to be *hidden* if the condition is not met
- Each option can have condition to determine if the option is selectable
- Each condition can check either a status or a item of any character
- Customizable triggers for global checking the status or inventory of any characters

About engines:

- Easy to setup
- Auto save system for saving current event
- Manual save system for saving current event and dialogue
- A total of 3 manual save slots and 1 auto save slot are available
- The save files are human-kind `json` files
- Script checking system for ensuring the script is correct

## How to Use

### Preparation

1. A folder with script and assets with following file structure:

    ```console
    .
    ├── script.toml
    └── assets
        └── images, audio, bgm, etc.
    ```

2. The script is written in [toml](https://toml.io/en/v1.0.0#table).
3. All the assets are recommended to be placed in a single folder (for example, `assets`).

This folder is recommended to be placed inside the project, with the following structure (assume `example-game` is your script):

```console
.
├── example-game/
│   ├── script.toml
│   └── assets/
│       └── image, audio, etc...
├── src/
│   ├── engine.c
│   └── ...
├── resources/
│   ├── font
│   ├── images
│   └── sounds
├── LICENSE.txt
├── Makefile
└── README.md
```

### Execute

First, execute the following command:

```console
make
```

Then, execute the following command:

```console
./engine [script-dir]
```

The `[script-dir]` is the directory name of your script.

### Rules of Script

The non-table fields include:

- name: the name of the script, displayed in the main menu
- author: the author of the script

The valid tables are:

Tables about character and status:

- [Character](#table-character)
- [Player](#table-player)
- [Item](#table-item)
- [Status](#table-status)

Tables about stories and events

- [Scene](#table-scene)
- [Event](#table-event)
- [Dialogue](#table-dialogue)
- [Condition](#table-condition)
- [Update](#table-update)
- [Trigger](#table-trigger)

The script should include these tables only. If any script error is written, the engine will terminate and tell you which part of the script is wrong.

You can only execute your script after all errors from your script is fixed. You can find the details of each table fields in the following part.

## Tables and Fields of TOML

### Table Status

```toml
[status.hp]
name = "health"
desc = "health of character"
min = 0
max = 100

[status.pow]
name = "power"
desc = "power of character"
min = -20
max = 20
```

- name: STRING, the name of the status that will be displayed in the game
- desc: STRING, the description of the status
- min: INTEGER, the minimum value of the status, can be negative
- max: INTEGER, the maximum value of the status

Each status **must have** different id (hp and pow for the example above) for running the script

### Table Item

```toml
[item.cabbage]
name = "cabbage"
desc = "a vegetable"
hidden = false

[item.apple]
name = "apple"
desc = "a fruit"
hidden = true # Will not show when update
```

- name: STRING, the name of the item that will be displayed in the game
- desc: STRING, the description of the item
- hidden: BOOL, if this is set to **true**, then when the character gets item from update, it will not show to the player. Default is false

### Table Character

```toml
[character.lulu]
name = "露露"
voice = "assets/character/lulu/voice.mp3"
inventory = [
    "cabbage",
    "apple"
]

[character.lulu.tachie]
default = "assets/character/lulu/tachie/default.png"
happy   = "assets/character/lulu/tachie/happy.png"
sad     = "assets/character/lulu/tachie/sad.png"
angry   = "assets/character/lulu/tachie/angry.png"

[character.lulu.status]
hp  = 20
mp  = 20
san = 20
```

- name: STRING, the display name of the character
- voice: STRING, the path to the voice of this character
- inventory: ARRAY of STRING, the **initial** inventory of this character
- tachie: TABLE, the name of the image (left side) and the path of the image (right side, STRING), can include any numbers of field; the **default** must exist in each of the character
- status: TABLE, the name of the status (left side) and the value of the status (right side, INTEGER), the status **must exist** here if you want to update the value of this character

### Table Player

```toml
[player]
character = "lulu"
start_event = "back-home"
```

- character: STRING, the **ID** of the character, showing which one is the player
- start_event: the initial event of the game

### Table Scene

```toml
[scene.lulu-house]
name = "露露家"
background = "assets/background/lulu-house.png"
```

- name: STRING, the name of the scene
- background: STRING, the path of the background

### Table Event

```toml
[event.back-home]
scene = "lulu-house"
dialogue = "back-home-1"
```

- scene: STRING, the **ID** of the scene to indicate which scene is used in the event
- dialogue: STRING, the **ID** of the dialogue to indicate the first dialogue of the event

### Table Dialogue

There are 4 types of dialogue: the NORMAL dialogue, the EVENT dialogue, the OPTION dialogue and the **END Dialogue**.

**NORMAL Dialogue**:

```toml
# Dialogue with tachie will show the given tachie
[dialogue.back-home-2]
character = "lulu"
tachie = "happy"
text = "阿嬤!"
next = "back-home-3"

# Dialogue without character is NARRATOR
[dialogue.back-home-3]
text = "撲進了阿嬤的懷中,阿嬤被你衝擊得踉蹌了一下"
next = "back-home-4"
update = [
    "lulu-add-san-5",
    "lulu-add-hp-5"
]

# Dialugue without tachie will show the default tachie
[dialogue.back-home-4]
character = "lulu"
text = "我剛才碰到一個很奇怪的人..."
next = "back-home-5"
```

- character: STRING, the **ID** of the character, indicate who is saying this dialogue, if there is no character then it will be treated as **NARRATOR**, which will not show any tachie in game
- tachie: STRING, the **name** of the tachie, indicate which tachie will be displayed, if not exist then the default tachie will be used
- text: STRING, the dialogue text
- next: STRING, the **ID** of the dialogue, indicate the next dialogue
- update: ARRAY of STRING, the **ID** of the update, indicate the updates of a character

**EVENT Dialogue**:

```toml
[dialogue.back-home-30]
character = "grandma"
text = "明天是你的生日，阿嬤要做好東西給你。"
event = "decision-1"
```

The **EVENT** dialogue is similar with **NORMAL** dialogue, the only difference is the `next` field is replaced with `event` field:

- event: STRING, the **ID** of the event, indicate the next event

**OPTION Dialogue**:

```toml
[dialogue.decision-1]
text = "你決定..."
[[dialogue.decision-1.option]]
text = "去圖書館"
event = "go-to-library"
[[dialogue.decision-1.option]]
text = "去森林"
event = "go-to-forest"
condition = "lulu-item-purple-flower"
hidden = true
[[dialogue.decision-1.option]]
text = "搜查地下室"
event = "search-basement"
condition = "lulu-item-baton"
```

The **OPTION Dialogue** includes the subtable *option*, and the `next` and `event` field is placed in each of the options.

The option table:

- text: STRING, the text of the option, will be displayed in the button text
- next / event: STRING, the **ID** of the dialogue or event, indicate the next dialogue or event if this option is selected
- condition: STRING, the **ID** of the condition, if the condition is not met, then the option is **not selectable**.
- hidden: BOOL, alongside with *condition*, if this is set to **true** then the option will be **hidden** if the condition is not met, instead of unselectable.

**END Dialogue**:

```toml
[dialogue.weird-glimpse-escape-3]
text = "你感覺自己的胸口正在向外噴灑著什麼,一隻巨爪穿過你的胸口。"
end = ""
```

The **END Dialogue** includes the field *end*:

- end: STRING, empty string, indicate this is the end of the game

### Table Condition

There are 2 types of condition: **STATUS Condition** and **Item Condition**. The condition can be reused in different option.

**STATUS Condition**:

```toml
[condition.lulu-status-harmony-1]
character = "lulu"
status = "harmony"
logic = "EQ"
value = 1
```

- character: STRING, the **ID** of the character, indicate which character is used to check the current condition
- status: STRING, the **ID** of the status, indicate which status is used, the status **must exist** when writing the status of character table
- logic: STRING, the only accept strings are:
  - "EQ": equal
  - "NE": not equal
  - "GT": greater than
  - "GE": greater than or equal to
  - "LT": less than
  - "LE": less than or equal to
- value: INTEGER, the value of status, the value to check based on the logic field

**ITEM Condition**:

```toml
[condition.lulu-item-purple-flower]
character = "lulu"
item = "purple-flower"
logic = "NE"
```

- character: STRING, the **ID** of the character, indicate which character is used to check the current condition
- item: STRING, the **ID** of the item, indicate which item is used
- logic: STRING, the only accept strings are:
  - "EQ": equal
  - "NE": not equal

### Table Update

There are 2 types of condition: **STATUS Update** and **Item Update**. The updates can be reused in different dialogue.

**STATUS Update**:

```toml
[update.lulu-san-add-5]
character = "lulu"
status = "san"
change = 5
```

- character: the **ID** of the character, indicate which chacracter will be updated
- status: STRING, the **ID** of the status, indicate which status is used, the status **must exist** when writing the status of character table
- change: INTEGER, the change value of the given status, negative integer will decrease the status

**ITEM Update**:

```toml
[update.lulu-item-add-meat]
character = "lulu"
item = "meat"
change = 1
```

- character: the **ID** of the character, indicate which chacracter will be updated
- item: STRING, the **ID** of the item, indicate which item is used
- change: 0 or 1, if 0 means the item will be removed, 1 means the item will be added

### Table Trigger

The trigger table is similar to *condition* table, but this is used for **global** checking, which means the trigger will force the game to another event if the trigger condition is met.

There are 2 types of trigger: **STATUS Trigger** and **Item Trigger**.

**STATUS Trigger**:

```toml
[trigger.simple-death]
character = "lulu"
status = "hp"
logic = "LE"
value = 0
event = "simple-death"
```

**ITEM Trigger**:

```toml
[trigger.lulu-has-meat]
character = "lulu"
item = "meat"
logic = "EQ"
event = "eat-food"
```

The field `character`, `status`, `logic` and `value` are same as the status condition table, while `character`, `item`, `logic` are same as the item condition table.

The only extra field is `event`:

- event: STRING, the **ID** of the event, indicate which event will be processed if the trigger is true

## Third-party

- [SDL](https://www.libsdl.org/license.php)
- [ffmpeg](https://www.ffmpeg.org/legal.html)

## Example Game Material Source

- [Stability AI](https://stability.ai/use-policy) (for generating example game's images)
- [chinaz.com](https://sc.chinaz.com/tag_yinxiao/anniu.html) (for sound effects)

## License

This project is licensed under the [WTFPL License](LICENSE.txt).
