#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

// After each hand clear out the data
void clear_hand(struct player * p, struct  house * h)
{
    // Reinitalize array members
    int i = 0;
    while(i < HAND_LIMIT)
    {
        h->house_hand.hand[i] = 0;

        p->hi_lo_hand.current_hand[i] = 0;
        p->hi_lo_hand.hand_states[i] = 0;
        p->hi_lo_hand.hand_totals[i] = 0;

        p->red7_hand.current_hand[i] = 0;
        p->red7_hand.hand_states[i] = 0;
        p->red7_hand.hand_totals[i] = 0;

        i++;
    }
    h->house_hand.current_sum = 0;
    h->house_hand.hand_position = 0;
    h->house_hand.is_soft = false;

    p->hi_lo_hand.current_sum = 0;
    p->hi_lo_hand.is_soft = false;

    p->red7_hand.current_sum = 0;
    p->hi_lo_hand.is_soft = false;
}
int * build_deck(const int n, int * deck_array)
{
    // calls shuffle_deck, called by main()
    const int LIMIT = n;;            // number of decks times 52 cards per deck
    deck_array = (int *) malloc((LIMIT) * sizeof(int));

    int i = 0;
    // now fill the array with the values of 2-11 (11 is an ace)
    // each non-face card obviously must occur 4 times per deck
    // for purposes of the simulation so 10, jack, queen king will all be considered 10's
    int card_value = 2;
    int regular_card_limit = 4 * n;     // 4 cards per kind per deck
    int face_card_limit = 16 * n;       // 16 ten cards per deck
    int aces_limit = n * 4;             // 4 aces per deck
    int number_of_types = 0;            // controls how many of each type is to be inserted
    i = 0;
    while(i < (regular_card_limit * 8))     // regular card limit multi by 8 for cards 2-9
    {
        if(number_of_types == regular_card_limit)   // how the card value is moved to the next one
        {
            card_value++;
            number_of_types = 0;
        }
        if(card_value == 10)
            break;
        deck_array[i] = card_value;
        number_of_types++;
        i++;
    }
    int k = 0;
    while(k < (face_card_limit))
    {
        deck_array[i] = 10;
        k++; i++;
    }
    k = 0;
    while(k < aces_limit)
    {
        deck_array[i] = 11;
        k++; i++;
    }
    k = 0;

    srand(time(0));
    return deck_array;
}
void shuffle_deck(int number_of_decks, int * deck_array)
{
    // called by build_deck and main
    const int LIMIT = CARDS * number_of_decks;
    int temp;
    int swap_position;
    for(int i = 0; i < LIMIT; i++)
    {
        swap_position = rand() % LIMIT;     // picks out a random part of the deck
        temp = deck_array[i];                   // what was in the original position
        deck_array[i] = deck_array[swap_position];
        deck_array[swap_position] = temp;
    }
}
void play(struct player * p, struct house * h)
{
    deal(p,h);
}
void deal(struct player * p, struct house * h)
{
    /*
        called by new_hand, calls new_hand, check_split, player_play
        this is only for the initial deal, any normal hit will be called by player_hit
    */
        int card1,card2;
        card1 = h->shoe[h->deck_pos++];
        card2 = h->shoe[h->deck_pos++];

        p->hi_lo_hand.current_hand[0] = card1;
        p->hi_lo_hand.current_hand[1] = card2;
        p->hi_lo_hand.hand_position = 2;

        p->red7_hand.current_hand[0] = card1;
        p->red7_hand.current_hand[1] = card2;
        p->red7_hand.hand_position = 2;
}
void player_hit(struct player * p, struct house * h, char count_name)
{

    // calls: sum_player_hand  called by: player_play, check_split
    // count_name refers to which hand the card is going to.
    int card = h->shoe[h->deck_pos++];
    card_counter_function(p,card);

    if(count_name == HI_LO_COUNT)
        p->hi_lo_hand.current_hand[p->hi_lo_hand.hand_position++] = card;
    else if(count_name == RED_COUNT)
        p->red7_hand.current_hand[p->red7_hand.hand_position++] = card;
}
void card_counter_function(struct player * p,int card)
{
    if(card >= 2 && card <= 6)
    {
        p->hi_lo_hand.count++;
        p->red7_hand.count++;
    }
    else if(card == 7)
    {
        if(p->red7_hand.count_7)     // if count_7 is true
            {
                p->red7_hand.count++;
                p->red7_hand.count_7 = false;
            }
        else
            p->red7_hand.count_7 = true;
    }
    else if(card == 10 || card == 11)
        {
            p->hi_lo_hand.count--;
            p->red7_hand.count--;
        }
}
