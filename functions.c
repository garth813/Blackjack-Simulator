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
    h->house_hand.hand_sum = 0;
    h->house_hand.hand_position = 0;
    h->house_hand.is_soft = false;

    p->hi_lo_hand.hand_sum = 0;
    p->hi_lo_hand.is_soft = false;

    p->red7_hand.hand_sum = 0;
    p->red7_hand.is_soft = false;
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
    printf("Limit: %d\n", h->limit);
    while(h->deck_pos < h->limit)
        deal(p,h);
    // after dealing we should see if anyone has a blackjack
}
void deal(struct player * p, struct house * h)
{
    /*
        called by new_hand, calls new_hand, check_split, player_play
        this is only for the initial deal, any normal hit will be called by player_hit
    */
        clear_hand(p,h);

        int dealer_card1, dealer_card2;
        dealer_card1 = h->shoe[h->deck_pos++];
        dealer_card2 = h->shoe[h->deck_pos++];
        h->house_hand.hand[0] = dealer_card1;
        h->house_hand.hand[1] = dealer_card2;
        h->house_hand.hand_position = 2;

        int card1,card2;
        card1 = h->shoe[h->deck_pos++];
        card2 = h->shoe[h->deck_pos++];

        p->hi_lo_hand.current_hand[0] = card1;
        p->hi_lo_hand.current_hand[1] = card2;
        p->hi_lo_hand.hand_position = 2;   // must set this for each new counting system

        p->red7_hand.current_hand[0] = card1;
        p->red7_hand.current_hand[1] = card2;
        p->red7_hand.hand_position = 2;  // again set the hand_position manually

        // if you add another counting system put it here after creating the new struct
        // and adding it to player, you will also need to update the card_counter_function an

        card_counter_function(p,card1);
        card_counter_function(p,card2);

        // the following loop is useful if you'd like to test the couting in card_counter_function
        /*int input = -1;
        while(input != 0)
        {
            scanf("%d",&input);
            card_counter_function(p,input);
            printf("Hi-Lo %d \n",p->hi_lo_hand.count);
            printf("Red %d \n",p->red7_hand.count);
        }
        */
        check_blackjack(p,h);
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
void check_blackjack(struct player * p, struct house * h)
{
    player_sum(p,RED_COUNT);
    player_sum(p,HI_LO_COUNT);
    house_sum(h);
    record(p,h);
}
void player_sum(struct player * p, char COUNTING_SYSTEM)
{
    /*
    Because we have different counts we have to sum them differently since
    they all may take different amounts of cards (for example Hi-Lo may hit in a situation where Red7 stands).
    That is the reason why this function takes an argument called COUNTING_SYSTEM, so we can know where
    to assign the integer pointer 'sum_this'.
    Sum the cards from the correct counting system and in the end assign the sum to the corresponding system
    */
    int * sum_this;
    if(COUNTING_SYSTEM == RED_COUNT)
        sum_this = p->red7_hand.current_hand;
    else if(COUNTING_SYSTEM == HI_LO_COUNT)
        sum_this = p->hi_lo_hand.current_hand;

    /*
    In Blackjack an ace could be counted as either an 11 or one. Most of the time the hand probably won't contain an
    ace at all which is the reason we'll check. If the hand doesn't contain an ace we can sum it rather quickly and
    return early. If it does contain an ace we need to count it in a slightly more complex way.
    */
    bool has_ace = false;   // assume no ace until we find one
    int i = 0;
    while(sum_this[i] != 0)
            if(sum_this[i++] == 11)
                has_ace = true;

    if(!has_ace)        // if the hand does NOT contain an ace
    {
        int hard_sum = 0;
        i = 0;
        while(sum_this[i] != 0)
             hard_sum += sum_this[i++];

        if(COUNTING_SYSTEM == RED_COUNT)
            p->red7_hand.hand_sum = hard_sum;
        else if(COUNTING_SYSTEM == HI_LO_COUNT)
            p->hi_lo_hand.hand_sum = hard_sum;


        return; // nothing else to do, can return now if there is no ace
    }
    /*
    Because aces can be counted differently we'll have two different results depending
    on how we count them. The variable sum11 will contain the hand total when one ace is allowed to
    equal 11 and the others (if any) will count as 1. Only one ace can be counted as 11 becuase
    counting more than one ace that way would automatically cause a bust.

    Next we will sum the hand counting all the aces as ones and store the total in sum_ace_1.
    At the end of the function we will compare the two totals, sum_ace_11 and sum_ace_1 to decide
    which total is most appropriate to use for the hand_sum field.
    */
    else if(has_ace)
    {
        int sum_ace_11 = 0;
        i = 0;
        // count first ace as 11, all others as 1
        bool count_ace_11 = true;
        while(sum_this[i] != 0)
        {
            if(sum_this[i] == 11)
                if(count_ace_11)
                {
                    sum_ace_11 += 11;
                    count_ace_11 = false; // just count the first ace as 11
                }
                else
                    sum_ace_11 += 1;  // if there are more count them as 1's
            else
                sum_ace_11 += sum_this[i];
            ++i;
        }

        // now count all aces as 1's
        i = 0;
        int sum_ace_1 = 0;
        while(sum_this[i] != 0)
         {
            if(sum_this[i] == 11)
                sum_ace_1 += 1;
            else
                sum_ace_1 += sum_this[i];
            ++i;
        }
        /*
        Now that we have two potentially different results between sum_ace_11 and sum_ace_1
        decide which is best.
        */
        int correct_sum = -1; // if this gets recorded something went wrong here
        if(sum_ace_11 <= 21)
            correct_sum = sum_ace_11;
        else
            correct_sum = sum_ace_1;

        bool is_soft_var;
        if(sum_ace_11 > 21)
            is_soft_var = false;
        else
            is_soft_var = true;

        // record the work in the correct hand
        if(COUNTING_SYSTEM == RED_COUNT)
        {
            p->red7_hand.hand_sum = correct_sum;
            p->red7_hand.is_soft = is_soft_var;
        }
        else if(COUNTING_SYSTEM == HI_LO_COUNT)
        {
            p->hi_lo_hand.hand_sum = correct_sum;
            p->hi_lo_hand.is_soft = is_soft_var;
        }
    } // end if
}
void house_sum(struct house * h)
{
    // this is much like the player_sum function, first see if there's an ace
    bool has_ace = false;
    int i = 0;
    while(h->house_hand.hand[i] != 0)
        if(h->house_hand.hand[i++] == 11)
            has_ace = true;


    i = 0;
    if(!has_ace)       // if the hand doens't have any aces.
    {
        int sum = 0;
        while(h->house_hand.hand[i] != 0)
            sum += h->house_hand.hand[i++];

        h->house_hand.hand_sum = sum;
        return;
    }
    else
    {
        int sum_ace_11 = 0;
        i = 0;
        bool count_ace_as_11 = true;  // count the first ace as 11, all others (if any) as 1
        while(h->house_hand.hand[i] != 0)
        {
            if(h->house_hand.hand[i] == 11)
            {
                if(count_ace_as_11)
                {
                    sum_ace_11 += 11;
                    count_ace_as_11 = false;
                }
                else
                    sum_ace_11 += 1;
            }
            else
                sum_ace_11 += h->house_hand.hand[i];
            ++i;
         }
         // now sum all the aces as 1's
         i = 0;
         int sum_ace_1 = 0;
         while(h->house_hand.hand[i] != 0)
         {
            if(h->house_hand.hand[i] == 11)
                sum_ace_1 += 1;
            else
                sum_ace_1 = h->house_hand.hand[i];

            ++i;
        }

        int correct_sum = -1; // if this gets recorded something went wrong here
        if(sum_ace_11 <= 21)
            correct_sum = sum_ace_11;
        else
            correct_sum = sum_ace_1;

        bool is_soft_var;
        if(sum_ace_11 > 21)
            is_soft_var = false;
        else
            is_soft_var = true;

        h->house_hand.is_soft = is_soft_var;
        h->house_hand.hand_sum = correct_sum;
    }// end else
}// end function
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
void record(struct player * p, struct house * h)
{
    int i = 0;
    int * array;
    array = &h->house_hand.hand[0];
    printf("\nH sum:%d Soft:%d ",h->house_hand.hand_sum,h->house_hand.is_soft);
    while(array[i] != 0)
        printf("%d ",array[i++]);

    i = 0;
    array = &p->hi_lo_hand.current_hand[0];
    printf("\tHi-lo sum:%d soft:%d hand:",p->hi_lo_hand.hand_sum,p->hi_lo_hand.is_soft);
    while(array[i] != 0)
        printf(" %d ",array[i++]);
}
