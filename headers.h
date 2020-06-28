#ifndef headers.h
#define headers.h
#include <stdbool.h>

// Eval codes
#define BLACKJACK 21
#define BUST 22
#define STAND 0
#define SURRENDER 1
#define DOUBLE 2

// Stuff for the hand struct
#define HAND_LIMIT  15   // How many cards each hand can store
#define TOTAL_HANDS 15  // The player could keep resplitting and have multiple hand totals

#define CARDS 52
#define RED_COUNT 'R'
#define HI_LO_COUNT 'H'
#define CASINO_HAND 'C'

struct red7
{
    double chips;
    double bet;
    int count;
    bool count_7;    // only modified in the card_counter function
                    // count_7 is unique only to this counting system
    int hand_position;
    int hand_sum;
    bool is_soft;

    int current_hand[HAND_LIMIT];
    int hand_totals[TOTAL_HANDS];
    int hand_states[TOTAL_HANDS];
};
struct hi_lo
{
    double chips;
    double bet;
    int count;
    int hand_position; // keeps track of where the next card will go in the array
    int hand_sum;
    bool is_soft;

    int current_hand[HAND_LIMIT];
    int hand_totals[TOTAL_HANDS];
    int hand_states[TOTAL_HANDS];
};
struct player
{
    struct hi_lo hi_lo_hand;
    struct red7 red7_hand;
};
struct casino_hand
{
    int hand[HAND_LIMIT];
    int hand_sum;
    int hand_position;
    bool is_soft;
};
struct house
{
    // like the players hand but a bit simpler
    struct casino_hand house_hand;

    // Establish rules
    bool early_surrender;
    bool late_surrender;
    bool dealer_hit_soft17;
    double blackjack_pays;

    // shoe stuff
    int total_cards; // how many total cards go in the shoe
    int limit;       // where the shoe will stop dealing
    int deck_pos;
    int * shoe;
};
// Function prototypes
void clear_hand(struct player * p, struct house * h);
int * build_deck(int n, int * deck);
void player_hit(struct player * p, struct house * h, char count_name);
void house_hit(struct house * h);
void card_counter_function(struct player * p, int card);
void play(struct player * p, struct house * h);
void deal(struct player * p, struct house * h);
void check_blackjack(struct player * p, struct house * h);
void player_sum(struct player * p,char COUNTING_SYSTEM);
void sum_hand(struct player * p, struct house * h, const char hand_id);
void house_sum(struct house * h);
void print_status(struct player * p, struct house * h);
#endif
