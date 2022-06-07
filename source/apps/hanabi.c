//
// Created by Samuel Jones on 6/6/22.
//

#include "hanabi.h"
#include "colors.h"
#include "menu.h"
#include "button.h"
#include "framebuffer.h"
#include "random.h"
#include "ir.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define HANABI_SUIT_MASK 0x38
#define HANABI_SUIT_SHIFT 3
#define HANABI_CARD_SUIT(card) ((card & HANABI_SUIT_MASK) >> HANABI_SUIT_SHIFT)
#define HANABI_CARD_MASK 0x07
#define HANABI_CARD_NUMBER(card) (card & HANABI_CARD_MASK)

#define HANABI_CARD(suit, number) ((suit << HANABI_SUIT_SHIFT) | number)

#define HANABI_MAX_DECK_SIZE (10*6)
#define HANABI_NO_RAINBOW_DECK_SIZE (10*5)

#define STARTING_BOMBS (3)
#define STARTING_CLUES (8)

#define MAX_PLAYERS (5)
#define MAX_CARDS_IN_HAND (5)

enum hanabi_suit{
    SUIT_BLUE = 0,
    SUIT_GREEN,
    SUIT_RED,
    SUIT_WHITE,
    SUIT_YELLOW,
    SUIT_RAINBOW,
    SUIT_MAX
};

struct hanabi_action {
    uint8_t acting_player:     4;
    // Fields set if play or discard
    uint8_t card_index:        3;
    uint8_t was_discarded:     1;
    // Fields set if clued
    uint8_t clued_player:      4;
    uint8_t clue_is_suit:      1;
    uint8_t clue_value:        3;
};

enum hanabi_message_type{
    msg_type_new_game,
    msg_type_new_player,
    msg_type_play,
};



struct __attribute__((__packed__)) ir_new_game_message {
    uint32_t game_id;
    uint8_t msg_type;
    uint8_t num_players;
    uint8_t player_id;
    bool rainbow_included;
    bool rainbow_normal;
    uint8_t deck[HANABI_MAX_DECK_SIZE];
};

struct __attribute__((__packed__)) ir_new_player_response {
    uint32_t game_id;
    uint8_t msg_type;
    uint8_t player_id;
};

struct __attribute((__packed__)) ir_play_message {
    uint32_t game_id;
    uint8_t msg_type;
    uint8_t play_id;
    struct hanabi_action play;
};



struct hanabi_game {

    union {
        struct ir_play_message play;
        struct ir_new_player_response response;
        struct ir_new_game_message message;
    } ir_data;
    bool has_ir_data;

    uint8_t deck[HANABI_MAX_DECK_SIZE];
    struct hanabi_action play_history[HANABI_MAX_DECK_SIZE];
    uint8_t my_last_play_index;
    uint32_t id; // randomly generate this so we don't get mixed up with other games

    uint8_t discard_pile[HANABI_MAX_DECK_SIZE];
    uint8_t board_state[SUIT_MAX];
    uint8_t hand[MAX_PLAYERS][MAX_CARDS_IN_HAND];
    uint8_t cards_per_hand;
    uint8_t plays;

    uint8_t deck_index;
    uint8_t clues;
    uint8_t bombs;

    bool rainbow_included;
    bool rainbow_normal_color;
    uint8_t player_number;
    uint8_t player_count;
    uint8_t current_players_registered; // host only
};

static struct hanabi_game hanabi;


static void ir_packet_callback(const IR_DATA *data) {

    if (data->data_length > sizeof(struct ir_new_game_message)) {
        return;
    }
    if (hanabi.has_ir_data) {
        return;
    }

    memcpy(&hanabi.ir_data, data->data, data->data_length);
    hanabi.has_ir_data = true;
}


static void send_new_game_message(struct hanabi_game* game) {

    struct ir_new_game_message ng_message;
    ng_message.msg_type = msg_type_new_game;
    ng_message.game_id = game->id;
    ng_message.player_id = game->current_players_registered;
    ng_message.num_players = game->player_count;
    ng_message.rainbow_included = game->rainbow_included;
    ng_message.rainbow_normal = game->rainbow_normal_color;
    memcpy(ng_message.deck, game->deck, sizeof(game->deck));

    IR_DATA data = {
            .data = (uint8_t*)&ng_message,
            .app_address = IR_APP7,
            .recipient_address = IR_BADGE_ID_BROADCAST,
            .data_length = sizeof(ng_message),
    };

    ir_send_complete_message(&data);
}

static void send_new_player_message(struct hanabi_game* game) {
    struct ir_new_player_response np_response;
    np_response.msg_type = msg_type_new_player;
    np_response.player_id = game->player_number;
    np_response.game_id = game->id;

    IR_DATA data ={
        .data = (uint8_t*)&np_response,
        .app_address = IR_APP7,
        .recipient_address = IR_BADGE_ID_BROADCAST,
        .data_length = sizeof(np_response),
    };

    ir_send_complete_message(&data);
}

static void send_my_last_move(struct hanabi_game *game) {
    struct ir_play_message my_play;
    my_play.game_id = game->id;
    my_play.msg_type = msg_type_play;
    my_play.play = game->play_history[game->my_last_play_index];
    my_play.play_id = game->my_last_play_index;

    IR_DATA data ={
            .data = (uint8_t*)&my_play,
            .app_address = IR_APP7,
            .recipient_address = IR_BADGE_ID_BROADCAST,
            .data_length = sizeof(my_play),
    };

    ir_send_complete_message(&data);
}

static bool game_wins(struct hanabi_game* game) {
    size_t suit_count = game->rainbow_included ? SUIT_MAX: SUIT_MAX-1;
    for (size_t i=0; i<suit_count; i++) {
        if (game->board_state[i] != 5) {
            return false;
        }
    }
    return true;
}

static bool game_loses(struct hanabi_game* game) {
    return game->bombs == 0;
}


static bool is_player_turn(struct hanabi_game* game) {
    if (game->plays == 0) {
        return game->player_number == 0;
    }

    return (game->play_history[game->plays-1].acting_player + 1) % game->player_count == game->player_number;
}

static const uint8_t unshuffled_deck[HANABI_MAX_DECK_SIZE] = {
    HANABI_CARD(SUIT_BLUE, 1), HANABI_CARD(SUIT_BLUE, 1), HANABI_CARD(SUIT_BLUE, 1),
    HANABI_CARD(SUIT_BLUE, 2), HANABI_CARD(SUIT_BLUE, 2),
    HANABI_CARD(SUIT_BLUE, 3), HANABI_CARD(SUIT_BLUE, 3),
    HANABI_CARD(SUIT_BLUE, 4), HANABI_CARD(SUIT_BLUE, 4),
    HANABI_CARD(SUIT_BLUE, 5),

    HANABI_CARD(SUIT_GREEN, 1), HANABI_CARD(SUIT_GREEN, 1), HANABI_CARD(SUIT_GREEN, 1),
    HANABI_CARD(SUIT_GREEN, 2), HANABI_CARD(SUIT_GREEN, 2),
    HANABI_CARD(SUIT_GREEN, 3), HANABI_CARD(SUIT_GREEN, 3),
    HANABI_CARD(SUIT_GREEN, 4), HANABI_CARD(SUIT_GREEN, 4),
    HANABI_CARD(SUIT_GREEN, 5),

    HANABI_CARD(SUIT_RED, 1), HANABI_CARD(SUIT_RED, 1), HANABI_CARD(SUIT_RED, 1),
    HANABI_CARD(SUIT_RED, 2), HANABI_CARD(SUIT_RED, 2),
    HANABI_CARD(SUIT_RED, 3), HANABI_CARD(SUIT_RED, 3),
    HANABI_CARD(SUIT_RED, 4), HANABI_CARD(SUIT_RED, 4),
    HANABI_CARD(SUIT_RED, 5),

    HANABI_CARD(SUIT_WHITE, 1), HANABI_CARD(SUIT_WHITE, 1), HANABI_CARD(SUIT_WHITE, 1),
    HANABI_CARD(SUIT_WHITE, 2), HANABI_CARD(SUIT_WHITE, 2),
    HANABI_CARD(SUIT_WHITE, 3), HANABI_CARD(SUIT_WHITE, 3),
    HANABI_CARD(SUIT_WHITE, 4), HANABI_CARD(SUIT_WHITE, 4),
    HANABI_CARD(SUIT_WHITE, 5),

    HANABI_CARD(SUIT_YELLOW, 1), HANABI_CARD(SUIT_YELLOW, 1), HANABI_CARD(SUIT_YELLOW, 1),
    HANABI_CARD(SUIT_YELLOW, 2), HANABI_CARD(SUIT_YELLOW, 2),
    HANABI_CARD(SUIT_YELLOW, 3), HANABI_CARD(SUIT_YELLOW, 3),
    HANABI_CARD(SUIT_YELLOW, 4), HANABI_CARD(SUIT_YELLOW, 4),
    HANABI_CARD(SUIT_YELLOW, 5),

    HANABI_CARD(SUIT_RAINBOW, 1), HANABI_CARD(SUIT_RAINBOW, 1), HANABI_CARD(SUIT_RAINBOW, 1),
    HANABI_CARD(SUIT_RAINBOW, 2), HANABI_CARD(SUIT_RAINBOW, 2),
    HANABI_CARD(SUIT_RAINBOW, 3), HANABI_CARD(SUIT_RAINBOW, 3),
    HANABI_CARD(SUIT_RAINBOW, 4), HANABI_CARD(SUIT_RAINBOW, 4),
    HANABI_CARD(SUIT_RAINBOW, 5),
};

static void generate_board(struct hanabi_game* game,
                           // game options
                           bool rainbow_included, bool rainbow_normal,
                           uint8_t players, uint8_t player_num,
                           // Generated here if host (player 1). If not these are inputs from IR
                           uint8_t* deck, uint32_t id) {

    memset(game->deck, 0, sizeof(game->deck));
    memset(game->board_state, 0, sizeof(game->board_state));
    memset(game->discard_pile, 0, sizeof(game->discard_pile));
    memset(game->play_history, 0, sizeof(game->play_history));
    memset(game->hand, 0, sizeof(game->hand));
    game->bombs = STARTING_BOMBS;
    game->clues = STARTING_CLUES;
    game->deck_index = 0;
    game->rainbow_included = rainbow_included;
    game->rainbow_normal_color = rainbow_normal;
    game->player_count = players;
    game->player_number = player_num;
    game->plays = 0;
    game->current_players_registered = 1;

    size_t deck_size = game->rainbow_included ? HANABI_MAX_DECK_SIZE : HANABI_NO_RAINBOW_DECK_SIZE;

    if (!deck) {
        // Fisher-yates shuffle
        memcpy(game->deck, unshuffled_deck, deck_size*sizeof(game->deck[0]));
        for (size_t i=deck_size-1; i>1; i--) {
            unsigned short random;
            random_insecure_bytes((uint8_t*)&random, 2);
            unsigned int j = random % i;
            uint8_t temp = game->deck[j];
            game->deck[j] = game->deck[i];
            game->deck[i] = temp;
        }
        random_insecure_bytes((uint8_t*)&game->id, sizeof(game->id));
    } else {
        // deck provided by host
        memcpy(game->deck, deck, deck_size*sizeof(game->deck[0]));
        game->id = id;
    }

    if (game->player_count == 2 || game->player_count == 3) {
        game->cards_per_hand = 5;
    } else { // 4 or 5 players;
        game->cards_per_hand = 4;
    }

    for (int i=0; i<game->player_count; i++) {
        for (int j=0; j<game->cards_per_hand; j++) {
            game->hand[i][j] = game->deck[game->deck_index++];
        }
    }
}


/* Program states.  Initial state is MYPROGRAM_INIT */
enum hanabi_state_t {
    HANABI_INIT,
    HANABI_MENU,
    HANABI_GAME,
    HANABI_LOSE,
    HANABI_WIN,
    HANABI_EXIT
};

static enum hanabi_state_t hanabi_app_state = HANABI_INIT;

static void myprogram_init(void)
{
    FbInit();
    FbClear();
    hanabi_app_state = HANABI_MENU;
}

static void check_buttons()
{
    int down_latches = button_down_latches();
    if (BUTTON_PRESSED(BADGE_BUTTON_SW, down_latches)) {
        /* Pressing the button exits the program. You probably want to change this. */
        hanabi_app_state = HANABI_EXIT;
    } else if (BUTTON_PRESSED(BADGE_BUTTON_LEFT, down_latches)) {
    } else if (BUTTON_PRESSED(BADGE_BUTTON_RIGHT, down_latches)) {
    } else if (BUTTON_PRESSED(BADGE_BUTTON_UP, down_latches)) {
    } else if (BUTTON_PRESSED(BADGE_BUTTON_DOWN, down_latches)) {
    }
}

static void draw_screen()
{
    FbColor(WHITE);
    FbMove(10, LCD_YSIZE / 2);
    FbWriteLine("HOWDY!");
    FbSwapBuffers();
}

static void myprogram_run()
{
    check_buttons();
    draw_screen();
}

static void myprogram_exit()
{
    hanabi_app_state = HANABI_INIT; /* So that when we start again, we do not immediately exit */
    returnToMenus();
}

/* You will need to rename myprogram_cb() something else. */
void myprogram_cb(void)
{
    switch (hanabi_app_state) {
        case HANABI_INIT:
            ir_add_callback(ir_packet_callback, IR_APP7);
            myprogram_init();
            break;
        case HANABI_MENU:
            myprogram_run();
            break;
        case HANABI_EXIT:
            ir_remove_callback(ir_packet_callback, IR_APP7);
            myprogram_exit();
            break;
        default:
            break;
    }
}

