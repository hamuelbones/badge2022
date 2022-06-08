//
// Created by Samuel Jones on 6/6/22.
//

#include "hanabi.h"
#include "colors.h"
#include "dynmenu.h"
#include "button.h"
#include "framebuffer.h"
#include "random.h"
#include "ir.h"
#include "menu.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <printf.h>

#define HANABI_SUIT_MASK 0x38
#define HANABI_SUIT_SHIFT 3
#define HANABI_CARD_SUIT(card) ((card & HANABI_SUIT_MASK) >> HANABI_SUIT_SHIFT)
#define HANABI_CARD_MASK 0x07
#define HANABI_CARD_NUMBER(card) (card & HANABI_CARD_MASK)

#define HANABI_CARD(suit, number) ((suit << HANABI_SUIT_SHIFT) | number)

#define HANABI_MAX_DECK_SIZE (10*6)
#define HANABI_NO_RAINBOW_DECK_SIZE (10*5)

#define MAX_RULE_PAGES 11

#define STARTING_BOMBS (3)
#define STARTING_CLUES (8)

#define MAX_PLAYERS (5)
#define MAX_CARDS_IN_HAND (5)


/* Program states.  Initial state is MYPROGRAM_INIT */
enum hanabi_state_t {
    HANABI_INIT,
    HANABI_MENU,
    HANABI_RULES,
    HANABI_GAME,
    HANABI_LOSE,
    HANABI_WIN,
    HANABI_EXIT
};

static enum hanabi_state_t hanabi_app_state = HANABI_INIT;

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


void hanabi_change_players(struct menu_t *menu) {
    return; // TODO
}

void hanabi_change_rainbow(struct menu_t *menu) {
    return; // TODO
}

void hanabi_start_hosting(struct menu_t *menu) {
    return; // TODO
}

void hanabi_start_joining(struct menu_t *menu) {

}

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


    // Game communication information

    uint32_t id; // randomly generate this so we don't get mixed up with other games
    union {
        struct ir_play_message play;
        struct ir_new_player_response response;
        struct ir_new_game_message message;
    } ir_data;
    bool has_ir_data;

    // Game configuration information
    bool rainbow_included;
    bool rainbow_normal_color;
    uint8_t player_number;
    uint8_t player_count;

    // Deck and play information
    uint8_t deck[HANABI_MAX_DECK_SIZE];
    uint8_t deck_index; // Where are we drawing from?
    struct hanabi_action play_history[HANABI_MAX_DECK_SIZE]; // List of past plays
    uint8_t play_index;
    uint8_t my_last_play_index; // Where was my last play? (for sending over IR)
    uint8_t discard_pile[HANABI_MAX_DECK_SIZE]; // What's the discard pile?
    uint8_t board_state[SUIT_MAX];


    uint8_t hand[MAX_PLAYERS][MAX_CARDS_IN_HAND];
    uint8_t cards_per_hand;

    uint8_t clues;
    uint8_t bombs;

    uint8_t current_players_registered; // host only
};

struct hanabi_app {
    uint8_t rulepage;
};

static struct hanabi_game hanabi;
static struct hanabi_app app;


void hanabi_enter_rules(struct menu_t *menu) {

    hanabi_app_state = HANABI_RULES;

    app.rulepage = menu->attrib & 0xF;
}


const char * const rules_overview =
        "Hanabi is a cooper-\n"
        "ative card game\n"
        "for two to five\n"
        "players. You know\n"
        "what cards the\n"
        "other players have,\n"
        "but not your own!\n\n"
        "You are trying to\n"
        "coordinate to put\n"
        "on a fireworks\n"
        "show."
;

const char * const rules_goal =
        "After cards are\n"
        "dealt, you take\n"
        "turns going clock-\n"
        "wise.\n\n"
        "You score points\n"
        "by building stacks\n"
        "of cards of each\n"
        "color in ascending\n"
        "order, 1-5.";


const char * const rules_deck =
        "The deck has five\n"
        "colors: Red, Green,\n"
        "Blue, White, and \n"
        "Yellow. In each\n"
        "color there are\n"
        "three 1s, two 2s,\n"
        "3s, and 4s, and\n"
        "only one 5!\n"
        "\n";

const char * const rules_actions =
        "On your turn, you\n"
        "can take one and\n"
        "only one of three\n"
        "actions:\n"
        "1) You can give a\n"
        "   CLUE to a player\n"
        "2) You can try to\n"
        "   PLAY one card\n"
        "3) You can DISCARD\n"
        "   a card\n";

const char * const rules_clue =
        "You can give a CLUE\n"
        "about a player's\n"
        "hand, which all\n"
        "players hear.\n"
        "You can CLUE either\n"
        "a COLOR or NUMBER.\n"
        "The player gets told\n"
        "ALL cards they have\n"
        "that match.\n";

const char * const rules_play =
        "You can try to PLAY\n"
        "a card. If the card\n"
        "is the next number\n"
        "in any color stack,\n"
        "it goes on top! \n"
        "If not, the team\n"
        "loses a FUSE and the\n"
        "card is DISCARDed.\n"
        "Draw after any PLAY\n"
        "attempt.\n"
        "The game ends with\n"
        "no points if you\n"
        "lose 3 FUSEs.";

const char * const rules_discard =
        "You can DISCARD a\n"
        "card to draw a new\n"
        "one.\n\n"
        "Discarded cards are\n"
        "out of the game!\n";

const char* const rules_information =
        "The game starts\n"
        "with eight INFO.\n"
        "Cluing consumes one\n"
        "INFO. Discarding or\n"
        "successfully playing\n"
        "a five gains one. If\n"
        "you have no INFO,\n "
        "you can't give a \n"
        "CLUE!\n";

const char * const rules_rainbow =
        "You can play with\n"
        "an optional suit.\n"
        "RAINBOW cards are\n"
        "ALL colors! (You\n"
        "cannot use RAINBOW\n"
        "in a clue.\n";


const char * const rules_end =
        "After the last\n"
        "card is drawn, ev-\n"
        "eryone gets ONE\n"
        "more turn. Your\n"
        "score is the \n"
        "number of cards in\n"
        "stacks.\n\n"
        "Try for the elusive\n"
        "max score! :)\n"
        "\n";

const char * const rules_strategy =
        "Building convent-\n"
        "ions with your team\n"
        "is essential. Some\n"
        "handy guidelines:\n"
        "Getting a CLUE\n"
        "often means you\n"
        "should PLAY.\n"
        "When in doubt, PLAY\n"
        "newer cards first,\n"
        "DISCARD older ones\n"
        "first.\n"
        "This is just the\n"
        "start!\n";

const char* rules_text[MAX_RULE_PAGES] = {
        rules_overview,
        rules_goal,
        rules_deck,
        rules_actions,
        rules_clue,
        rules_play,
        rules_discard,
        rules_information,
        rules_rainbow,
        rules_end,
        rules_strategy,
};


void hanabi_draw_rules(void) {
    FbClear();
    FbBackgroundColor(BLUE);
    FbColor(WHITE);
    FbMove(6, 8);
    char title[20];
    snprintf(title, 20, "Page %u/%u", app.rulepage+1, MAX_RULE_PAGES);
    FbWriteDenseString(title);
    FbMove(6, 16);
    FbWriteDenseString(rules_text[app.rulepage]);

}


const struct menu_t hanabi_new_game_menu[4] = {
        {.name="Players: ", .type=FUNCTION, .attrib=VERT_ITEM, .data.func=hanabi_change_players},
        {.name="Rainbow: ", .type=FUNCTION, .attrib=VERT_ITEM, .data.func=hanabi_change_rainbow},
        {.name="Start Hosting", .type=FUNCTION, .attrib=VERT_ITEM, .data.func=hanabi_start_hosting},
        {.name="Back", .type=BACK, .attrib=VERT_ITEM|LAST_ITEM, .data.func=NULL},
};


const struct menu_t hanabi_rules[MAX_RULE_PAGES+1] = {
        {.name="Overview", .type=FUNCTION, .attrib=VERT_ITEM | 0, .data.func=hanabi_enter_rules},
        {.name="Deck", .type=FUNCTION, .attrib=VERT_ITEM | 2, .data.func=hanabi_enter_rules},
        {.name="Actions", .type=FUNCTION, .attrib=VERT_ITEM | 3, .data.func=hanabi_enter_rules},
        {.name="Information", .type=FUNCTION, .attrib=VERT_ITEM | 7, .data.func=hanabi_enter_rules},
        {.name="Rainbow", .type=FUNCTION, .attrib=VERT_ITEM| 8, .data.func=hanabi_enter_rules},
        {.name="End of Game", .type=FUNCTION, .attrib=VERT_ITEM | 9, .data.func=hanabi_enter_rules},
        {.name="Strategy", .type=FUNCTION, .attrib=VERT_ITEM | 10, .data.func=hanabi_enter_rules},
        {.name="Back", .type=BACK, .attrib=VERT_ITEM|LAST_ITEM},
};

const struct menu_t hanabi_menu[4] = {
        {.name="Host New Game", .type=MENU, .attrib=VERT_ITEM, .data.menu=hanabi_new_game_menu},
        {.name="Join Game", .type=FUNCTION, .attrib=VERT_ITEM, .data.func=hanabi_start_joining},
        {.name="Learn the Rules", .type=MENU, .attrib=VERT_ITEM, .data.menu=hanabi_rules},
        {.name="Exit", .type=FUNCTION, .attrib=VERT_ITEM|LAST_ITEM, .data.func=NULL}
};


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
    if (game->play_index == 0) {
        return game->player_number == 0;
    }

    return (game->play_history[game->play_index-1].acting_player + 1) % game->player_count == game->player_number;
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
    game->play_index = 0;
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



static void myprogram_init(void)
{
    FbInit();
    FbClear();
    hanabi_app_state = HANABI_MENU;
}


static void draw_screen()
{;
    FbSwapBuffers();
}

static void myprogram_run()
{
}

static void myprogram_exit()
{
    hanabi_app_state = HANABI_INIT; /* So that when we start again, we do not immediately exit */
    returnToMenus();
}

/* You will need to rename myprogram_cb() something else. */
void hanabi_cb(void)
{

    int button_latches = button_down_latches();
    int encoder = button_get_rotation();

    switch (hanabi_app_state) {
        case HANABI_INIT:
            ir_add_callback(ir_packet_callback, IR_APP7);
            myprogram_init();
            break;
        case HANABI_MENU: {
            genericMenu(hanabi_menu, MAIN_MENU_STYLE, button_latches);
        }
            break;
        case HANABI_RULES: {

            if (encoder < 0 || BUTTON_PRESSED(BADGE_BUTTON_LEFT, button_latches)) {
                if (app.rulepage == 0) {
                    app.rulepage = MAX_RULE_PAGES-1;
                } else {
                    app.rulepage--;
                }
            }
            if (encoder > 0 || BUTTON_PRESSED(BADGE_BUTTON_RIGHT, button_latches)) {
                if (app.rulepage == MAX_RULE_PAGES-1) {
                    app.rulepage = 0;
                } else {
                    app.rulepage++;
                }
            }

            if (BUTTON_PRESSED(BADGE_BUTTON_SW, button_latches)) {
                hanabi_app_state = HANABI_MENU;
            }

            hanabi_draw_rules();
        }
        break;
        case HANABI_EXIT:
            ir_remove_callback(ir_packet_callback, IR_APP7);
            myprogram_exit();
            break;
        default:
            break;
    }

    draw_screen();
}

