/*Title: Pair War
 * a simple card game with one dealer 3 player and a single deck of cards. The object of the game it to obtain a pair of cards.
 * There are three round. A player can only have two cards in their hand at one time. If a third card is given the player must
 * discard one of their 2 cards at random and replace it with the new card being dealt. the discarded card is added to the bottom
 * of the deck. After each round the dealer shuffles the cards. The game ends either after  3 rounds are up or a winner declares a
 * pair, and sends the message to each other the other players and dealer.
 *
 *
 */

/*
 * File:   main.cpp
 * Author: Anneka Bath,
 *
 * Created on February 27, 2020, 1:33 PM
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#define NUM_THREADS 3 //number of players

FILE *logfile;
using namespace std;

int roundNum = 0;
int const deck_size = 52;
int card = 0;
vector <int> deck;
static int numPlayers = 3;
static int seed = 0;
static bool win = false;
struct thread_hand {int threadid, card1, card2; bool win= false;};

struct thread_hand player_data [NUM_THREADS];
pthread_t dealer_Thread; //dealer thread populated
pthread_mutex_t mutex;


void buildDeck();
void printDeck ();
void card_discard (int card);
int card_dealt();
void shuffle();
void *Game(void *playerThreads);
void player(int card);
void *roundRobin(void *playerThreads);
void *dealer(void *arg);

/* Function: void deck()
 * tasks:
 * state 1: When the round is set to 0 a deck of 52 cards. The values range from 1 to 13.
 * state 2: if it isn't round 0 then the deck of cards are printed out.
 * Created on February 27, 2020, 1:33 PM
 */
void buildDeck()
{
    if (roundNum == 0)
    {
        int index;
        for(int i = 0; i < deck_size; i++)//initialize deck
        {
            index = (i % 13) + 1;
            deck.push_back(index);
        }
        shuffle();
        roundNum++;
    }
}
/* Function: printDeck ()
 * tasks:
 * prints the remaining deck of cards
 */
void printDeck ()
{
    int index;
    fprintf(logfile, "Deck: ");
    for(int i = 0; i < deck.size(); i++)//initialize deck
    {
        fprintf(logfile, "%d, ", deck[i]);//print to log file
        printf( "%d, ", deck[i]);//print to log file
    }
    fprintf(logfile, "\n");
    printf( "\n");
}

/* Function: card_discard (int card)
 * tasks:
 * player discards a card it's added to the bottom of the deck vector.
 * Created on February 27, 2020, 1:33 PM
 */
void card_discard (int card)
{
    deck.push_back(card);
    fprintf(logfile, "Discard: %d\n", card);

}

/* Function: card_dealt ()
 * tasks:
 * card is taken from the top of the deck. This card is erased from the deck and
 * that card would be returned.
 * Created on February 27, 2020, 1:33 PM
 */
int card_dealt()
{
    int card = deck.front();
    deck.erase(deck.begin());
    return card;
}

/* Function: shuffle()
 * tasks:
 * the remaining cards in the deck are shuffled by swapping two elements, at random
 * index values, in the deck vector. This done 52 times to insure a good
 * shuffle within the deck of cards.
 */
void shuffle()
{
    int index1, index2;
    srand(seed);
    for(int i = 0; i < deck.size(); i++)//insure good shuffle
    {
        index1 = rand()%deck.size();
        index2 = rand()%deck.size();
        iter_swap(deck.begin()+ index1, deck.begin() + index2);//swaps two random elements in their respective index value.
    }
    fprintf(logfile, "Dealer: Shuffling\n");
}

/* Function: thread_hand *player(struct thread_hand *player_hand_data)
 * tasks:
 * Receives struct player_hand_data pointer and saves it as a local pointer p.
 * The player function then is dealt a card from the deck and saved as card.
 * then the roundNum in checked to determine if the card is in the card1 or card2
 * slot.It is also determined if win is true, which happens if the player gets a
 * pair. In roundNum 3 player has both card slots filled. One of the cards is returned
 * to the cards from the card slots is returned to the bottom of the deck. The card
 * that is returned is at random unless the card being dealt matches the one of
 * the already held cards.
 */
struct thread_hand *player(struct thread_hand *player_hand_data)
{
    struct thread_hand *p;
    p = player_hand_data;

    int card = card_dealt();//player gets their card from the deck.
    fprintf(logfile, "Dealt: %d\n", card);

    if (roundNum == 1)
    {
        p->card1 = card;
    }
    else if (roundNum == 2)
    {
        p->card2=card;
        if (p->card2 == p->card1)
        {
            p->win = true;
            win = true;
        }
    }
    else if (roundNum == 3)
    {

        if (p->card2 == card ) //first win true case
        {
            p->win = true;
            win = true;
            card_discard (p->card2);
            p->card2=card;
        }
        else if ( p->card1 == card) //second win true case
        {
            p->win = true;
            win = true;
            card_discard (p->card1);
            p->card1=card;
        }

        int random = rand() % 2+1; //random card slot chosen to determine which is discarded.
        if (random == 1)
        {
            card_discard (p->card1);
            p->card1=card;
        }
        else if (random == 2)
        {
            card_discard (p->card2);
            p->card2=card;
        }
    }
    return p;
}

/* Function: dealer()
 * tasks:
 * just a place to send the dealer once the thread is made.
 */
void *dealer(void *arg)
{
    long pID = 0;
}

/* Function: roundRobin()
 * tasks:
 * Receives playerThreads and constructs the thread_hand struct pointer called
 * player data. Player data sends a copy of itself to player which is modified
 * and returned back to the player data pointer.
 */
void *roundRobin(void *playerThreads)
{
    pthread_mutex_lock (&mutex);
    struct thread_hand  *player_data;
    player_data = (struct thread_hand *) playerThreads;
    pthread_mutex_unlock (&mutex);
    player_data = player(player_data);


    if(player_data->win == true)
    {
        printf("Player %d: Cards:%d ,%d, win: %d\n", player_data->threadid,player_data->card1, player_data->card2, player_data->win);
        fprintf(logfile, "Player %d: Hand: %d, %d\n", player_data->threadid, player_data->card1, player_data->card2, player_data->win);

    }
    else
    {
        printf("Player %d: Cards:%d ,%d, win: %d\n", player_data->threadid,player_data->card1, player_data->card2, player_data->win);
        fprintf(logfile, "Player %d: Hand: %d, %d\n", player_data->threadid, player_data->card1, player_data->card2, player_data->win);
    }   
}


/* Function: *GameRound()
 * tasks:
 * does majority of the work in handling the the pthreads. 3 player_threads are
 * populated and 1 dealer thread is created. A for loop is constructed to handle
 * the pthread creations for each round. the for loop first checks the roundNum
 * so it knows in which order the player threading can be created. Once all 3
 * identified player threads are created numRound is increased. In round 2 player 2
 * starts the round then 3 and 1. In round 3 player 3 starts and then 1 and 2.
 * Between each round the cards are shuffled. The dealer is then given the ability
 * to join the player threads. this insures that each thread will finish their tasks
 * before main is completed.
 */
void *GameRound()
{
    pthread_t player_threads[NUM_THREADS]; //player threads populated

    pthread_create(&dealer_Thread, NULL, dealer, NULL);
    int rP;
    int roundCounter = 3;
    for(int i =0; i<roundCounter; i++)
    {

        if (roundNum == 1)
        {       
            printf("Round: %d \n", roundNum);
            fprintf(logfile, "Round # %d \n", roundNum);//print to log file

            for(int t=1; t<NUM_THREADS+1; t++) //threadids starting from 0-2
            {
               
                player_data[t].threadid = t; //players struct thread given threadid t (1-3)
                rP = pthread_create(&player_threads[t], NULL, roundRobin,(void *)&player_data[t]);
                
                if(rP)
                {
                    printf("ERROR; Return code from create is %d\n", rP);
                }
            }
            shuffle();
            printDeck();
            roundNum++;
        }
        
        if (roundNum == 2)
        {   
            printf("Round: %d \n", roundNum);
            fprintf(logfile, "Round # %d \n", roundNum);//print to log file

            for(int t=2; t<NUM_THREADS+1; t++) //threadids starting from 0-2
            {
                
                printf("creating thread %d\n", t);
                
              
                if(win == true)
                {
                    fprintf(logfile, "Player %d Exiting\n", player_data[1].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[1].threadid,player_data[1].card1, player_data[1].card2, player_data[1].win);
                    
                    fprintf(logfile, "Player %d Exiting\n", player_data[2].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[2].threadid,player_data[2].card1, player_data[2].card2, player_data[2].win);

                    fprintf(logfile, "Player %d Exiting\n",player_data[3].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n\n\n\n\n", player_data[3].threadid,player_data[3].card1, player_data[3].card2, player_data[3].win);
                    printDeck();
                    pthread_exit(NULL);
                }
                
                player_data[t].threadid = t; //players struct thread given threadid t (2-3)
                rP = pthread_create(&player_threads[t], NULL, roundRobin,(void *)&player_data[t]);
                if(rP)
                {
                    printf("ERROR; Return code from create is %d\n", rP);
                }
                //pthread_mutex_unlock (&mutex);
            }
            for(int t=1; t<2; t++) //threadids starting from 0-2
            {
               
                
                if(win == true)
                {
                    fprintf(logfile, "Player %d Exiting\n", player_data[1].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[1].threadid,player_data[1].card1, player_data[1].card2, player_data[1].win);
                    
                    fprintf(logfile, "Player %d Exiting\n", player_data[2].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[2].threadid,player_data[2].card1, player_data[2].card2, player_data[2].win);

                    fprintf(logfile, "Player %d Exiting\n",player_data[3].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n\n\n\n\n", player_data[3].threadid,player_data[3].card1, player_data[3].card2, player_data[3].win);
                    printDeck();
                    pthread_exit(NULL);
                }
                
                player_data[t].threadid = t; //players struct thread given threadid t (1)
                rP = pthread_create(&player_threads[t], NULL, roundRobin,(void *)&player_data[t]);
                if(rP)
                {
                    printf("ERROR; Return code from create is %d\n", rP);
                }
               
            }
            shuffle();
            printDeck();
            roundNum++;
        }
        
        if (roundNum == 3)
        {
            printf("Round: %d \n", roundNum);
            fprintf(logfile, "Round # %d \n", roundNum);//print to log file

            for(int t=3; t<NUM_THREADS+1; t++) //threadids starting from 0-2
            {
                
                printf("creating thread %d\n", t);
                
                
                if(win == true)
                {
                    fprintf(logfile, "Player %d Exiting\n", player_data[1].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[1].threadid,player_data[1].card1, player_data[1].card2, player_data[1].win);
                    
                    fprintf(logfile, "Player %d Exiting\n", player_data[2].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[2].threadid,player_data[2].card1, player_data[2].card2, player_data[2].win);

                    fprintf(logfile, "Player %d Exiting\n",player_data[3].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n\n\n\n\n", player_data[3].threadid,player_data[3].card1, player_data[3].card2, player_data[3].win);
                    printDeck();
                    pthread_exit(NULL);
                }
                
                player_data[t].threadid = t; //players struct thread given threadid t (3)
                rP = pthread_create(&player_threads[t], NULL, roundRobin,(void *)&player_data[t]);
                if(rP)
                {
                    printf("ERROR; Return code from create is %d\n", rP);
                }
            }
            for(int t=1; t<3; t++) //threadids starting from 1-2
            {
                
                printf("creating thread %d\n", t);
                
                
                if(win == true)
                {
                    fprintf(logfile, "Player %d Exiting\n", player_data[1].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[1].threadid,player_data[1].card1, player_data[1].card2, player_data[1].win);
                    
                    fprintf(logfile, "Player %d Exiting\n", player_data[2].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n", player_data[2].threadid,player_data[2].card1, player_data[2].card2, player_data[2].win);

                    fprintf(logfile, "Player %d Exiting\n",player_data[3].threadid);
                    fprintf(logfile, "Player %d: Cards:%d ,%d, win: %d\n\n\n\n\n", player_data[3].threadid,player_data[3].card1, player_data[3].card2, player_data[3].win);
                    printDeck();
                    pthread_exit(NULL);
                }
                 
                player_data[t].threadid = t; //players struct thread given threadid t (1-2)
                rP = pthread_create(&player_threads[t], NULL, roundRobin,(void *)&player_data[t]);
                if(rP)
                {
                    printf("ERROR; Return code from create is %d\n", rP);
                } 
                fprintf(logfile, "Player %d Exiting\n\n", player_data->threadid);
                
            }
            roundNum++;
            printDeck();
        }
        
        fprintf(logfile, "\n\n\n\n\n\n\n\n");
    }
    
    pthread_join(dealer_Thread, NULL);
    for (int i = 0; i<NUM_THREADS; i++)
    {
        pthread_join(player_threads[i],NULL);
    }
}

int main(int argc, char** argv)
{
    logfile = fopen("log.text", "a");
    printf("Enter seed: ");
    scanf("%d", &seed);

    buildDeck();
    GameRound();
    pthread_exit(NULL);
    fclose(logfile);
    exit(EXIT_SUCCESS);
    return(0);
}


