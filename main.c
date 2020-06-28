#include <stdio.h>
#include <stdlib.h>

#include "headers.h"
#include "functions.c"
int main(void)
{
    // house data
    printf("How many decks would you like to use?\n");
    int temp_decks;
    while(!(temp_decks >= 1 && temp_decks <= 8))
    {
        printf("Enter number from 1 to 8 ");
        scanf("%d",&temp_decks);
    }

    // from now on we'll use the const integer value for the number of decks called 'DECKS'
    const int DECKS = temp_decks;
    struct house h;
    // rules
    h.dealer_hit_soft17 = false;
    h.blackjack_pays = 1.50;    // typical blackjack payout
    h.early_surrender = false;
    h.late_surrender = false;
    h.total_cards = (CARDS * DECKS);

    h.shoe = build_deck(DECKS,h.shoe);
    h.deck_pos = 0;

    double pen;
    getchar();
    printf("Enter deck penetration as a decimal ");
    scanf("%lf",&pen);
    h.limit = pen * h.total_cards;

    struct player p;
    // initalize hi lo
    p.hi_lo_hand.chips = 0;
    p.hi_lo_hand.count = 0;

    // initialize red7
    p.red7_hand.chips = 0;
    // red7 is an unbalanced counting system
    p.red7_hand.count = DECKS * (-2);

    // clear hand does the rest of the initializations
    clear_hand(&p,&h);

    // this while loop will control how many shoes are played
    int number_of_shoes = 1;
    int i = 0;
    while(i < number_of_shoes)
    {
        shuffle_deck(DECKS,h.shoe);
        // after the deck is shuffled we need to put the deck position back to 0
        h.deck_pos = 0;
        play(&p,&h);    // the play function actually controls the depth of the shoe itself
        i++;
    }

    return 0;
}
