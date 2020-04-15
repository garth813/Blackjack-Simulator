#include <stdio.h>
#include <stdlib.h>

#include "headers.h"
#include "functions.c"
int main()
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
    h.blackjack_even_money = false;
    h.dealer_hit_soft17 = false;
    h.early_surrender = false;
    h.late_surrender = false;
    h.total_cards = (CARDS * DECKS);

    h.shoe = build_deck(DECKS,h.shoe);
    h.deck_pos = 0;
    shuffle_deck(DECKS,h.shoe);

    double pen;
    getchar();
    printf("Enter deck penetration as a decimal ");
    scanf("%lf",&pen);
    h.limit = pen * h.total_cards;
    printf("%lf\n",pen);

    struct player p;
    p.hi_lo_hand.chips = 0;
    p.red7_hand.chips = 0;

    clear_hand(&p,&h);
    //play(&p,&h);

    return 0;
}
