#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Defined to mimic the boolean type
#define TRUE 1
#define FALSE 0

// Maximum length for char arrays
#define STRMAX 100

// Starting balance in dollars
#define STARTING_BALANCE 200

// Maximum Cards in a regular deck
#define MAXCARDS 52

// Minimum Betting amount per game
#define ANTE 1

// Card struct for info about a certain Card
typedef struct {
	char suit;
	char rank[STRMAX];
	int value;
} Card;

// Game Inital State & Information
int cardsInHand = 0;
Card deck[MAXCARDS];
int cardsInDeck = MAXCARDS;
Card usedDeck[MAXCARDS];
int usedTopIndex = 0;
int dealerStayed = FALSE;
int pot = 0;
int topIndex = 0;

// Player Profile Info
int numberOfWins = 0;
int numberOfLosses = 0;
int numberOfStalemates = 0;
int balance;
int playerStayed = FALSE;

// Note: in a NEW deck of cards, the card order is set such that it goes from Ace to King for each suit set in the order of Diamond, Club, Hearts, and then Spades
char* getRank(int i)
{
	/* Get the rank based on the index of card from a new deck */
	char* rank[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
	return rank[i % 13];
}

char getSuit(int i)
{
	/* Gets the suit of the card based on the index of card from a new deck */
	char suit[] = {'D', 'C', 'H', 'S'};
	return suit[(i/13) % 4];
}

int getValue(char* rank)
{
	/* Gets the point value of a given rank */

	// Converts string to integer value
	int value = atoi(rank);

	// For case of non-integer strings
	if (value == 0)
	{
		if (strcmp(rank, "A") == 0)
			return 1; // Normally could be 1 or 11, but default is 1 for now
		else /* The rest must be a J, Q, or K */
			return 10;
	}
	return value;	
}

void setCard(Card* card, int i)
{
	/* Given a Card struct and the index of a card from a new deck, apply the info into the Card struct */
	card->suit = getSuit(i);
	strcpy(card->rank, getRank(i));
	card->value = getValue(card->rank);
}

void swapCard(Card* c1, Card* c2)
{
	/* Swaps the card information */
	char tempRank[STRMAX];
	strcpy(tempRank, c1->rank);
	char tempSuit = c1->suit;
	int tempValue = c1->value;
	
	strcpy(c1->rank, c2->rank);
	c1->value = c2->value;
	c1->suit = c2->suit;

	strcpy(c2->rank, tempRank);
	c2->value = tempValue;
	c2->suit = tempSuit;
}

void printCard(Card* c)
{
	/* Prints out the card's rank and suit */
	printf("%2s%c ", c->rank, c->suit);
}

void makeNewDeck()
{
	/* Creates a NEW starting deck to the deck */
	for (int i = 0; i < MAXCARDS; ++i)
	{
		setCard(&deck[i], i);
	}
}

void shuffleDeck()
{
	/* Shuffles the deck */

	int numberOfSwaps = rand();
	for (int i = numberOfSwaps-1; i > 0; --i)
	{
		srand(time(NULL)); // New random seed each interation
		int j = rand();
		swapCard(&deck[i], &deck[j % (i + 1)]);
	}
}

void printDeck()
{
	/* Prints out the entire unused deck */
	printf("--------------Deck--------------");
	for (int i = topIndex; i < cardsInDeck; ++i)
	{
		if ((i - topIndex) % 8 == 0)
			printf("\n\t");
		printCard(&deck[i]);
	}
	printf("\nNumber of cards in deck: %d\n", cardsInDeck - topIndex);
	printf("--------------------------------\n");
}

void printUsedDeck()
{
	/* Prints out the entire used deck pile */
	printf("-----------Used Deck------------");
	for (int i = 0; i < usedTopIndex; ++i)
	{
		if (i % 8 == 0)
			printf("\n\t");
		printCard(&usedDeck[i]);
	}
	printf("\nNumber of cards in used deck: %d\n", usedTopIndex);
	printf("--------------------------------\n");
}

int isValidBet(int bet)
{
	/* Check if the value is a valid bet*/
	if (bet > balance)
	{
		printf("You do not have enough money\n");
		return FALSE;
	}
	if (bet < ANTE)
	{
		printf("You must bet at least a minimum of %d dollars\n", ANTE);
		return FALSE;
	}
	return TRUE;
}

void bet()
{
	/* Apply the bet event to the game */
	char bet[STRMAX];
	printf("CURRENT BALANCE: %d dollar(s).\n", balance);
	printf("How much do you want to bet (minimum $1)?\n");
	do
	{
		scanf("%s", bet);
	} while (!isValidBet(atoi(bet)));
	printf("You decided to make a bet of %d dollar(s).\n", atoi(bet));
	fflush(stdout);
	pot = atoi(bet);
	balance -= pot;
	pot = pot*2;
	
}

int handValue(Card* hand, int size)
{
	/* Given the hand and the size of your hand, return the point value total */
	int numberOfA = 0;
	int total = 0;
	for (int i = 0; i < size; ++i)
	{
		if (strcmp(hand[i].rank, "A") == 0)
			++numberOfA;		
		total += hand[i].value;
	}	
	while (numberOfA > 0 && total <= 11)
	{
		--numberOfA;
		total += 10;
	}
	return total;
}

void showHand(Card* hand, int size)
{
	/* Given a hand of cards and the size, show hand's point value */
	for (int i = 0; i < size; ++i)
	{
		printf("%s%c ", hand[i].rank, hand[i].suit);
	}
	printf("| Value: %d\n", handValue(hand, size));
}

void gameStats()
{
	/* Prints out the game stats to the console */
	printf("Here are the stats for your game:\n\tWins: %d\n\tLoss: %d\n\tTies: %d\n\tEnding Balance: %d\n", numberOfWins, numberOfLosses, numberOfStalemates, balance);
}

void surrender()
{
	/* Surrender option */
	printf("You've decided to surrender and quit.\n");
	++numberOfLosses;
	gameStats();
	exit(0);
}

void addBackUsedDeck(int topIndex)
{
	/* Adds back all the cards in the used deck to the original dealing deck */
	for (int i = 0; i < topIndex; ++i)
	{
		deck[i].suit = usedDeck[i].suit;
		strcpy(deck[i].rank, usedDeck[i].rank);
		deck[i].value = usedDeck[i].value;
	}
	usedTopIndex = 0;
	cardsInDeck = topIndex;
}

void hit(Card* hand, int currentSize)
{
	/* Applies the "hit" event in Blackjack, where person receive a card from the dealing deck */
	++cardsInHand;
	Card c = deck[topIndex++];
	strcpy(hand[currentSize].rank, c.rank);
	hand[currentSize].suit = c.suit;
	hand[currentSize].value = c.value;
	
	if(topIndex == cardsInDeck)
	{
		printf("Ran out of cards. Reshuffling used cards into the deck\n");
		topIndex = 0;
		addBackUsedDeck(usedTopIndex);
		shuffleDeck(cardsInDeck);
	}
}

void stay(int playerNumber)
{
	/* Apply the "stay" option in Blackjack, where player stops receiving card till they have to show their hand */
	if (playerNumber == 0)
		dealerStayed = TRUE;
	else if (playerNumber == 1)
		playerStayed = TRUE;
}


void printOptions()
{
	/* Prints out to console the valid inputs for the game */
	printf("Here are the commands when it's your turn:\n");
	printf("\tInput 's' if you want to stay\n");
	printf("\tInput 'h' if you want to get hit\n");
	printf("\tInput 'q' if you want to surrender and quit\n");
	printf("\tInput 'p' if you want to print what's left in the deck\n");
	printf("\tInput 'u' if you want to print what's in the used deck\n");
	fflush(stdout);
}

void printHands(Card* yourHand, int playerHandSize, Card* dealerHand, int dealerHandSize)
{
	/* Prints your hand and the dealer's hand */
	printf("\tYOUR HAND: ");
	showHand(yourHand, playerHandSize);
	printf("\tDEALER'S HAND: ");
	showHand(dealerHand, dealerHandSize);
}

int dealerMove(Card* dealerHand, int dealerHandSize)
{
	/* Applies the dealer's move in Blackjack, which is based on their hand value */
	if (dealerStayed)
		return dealerHandSize;
	if (handValue(dealerHand, dealerHandSize) < 17)
	{
		hit(dealerHand, dealerHandSize++);
		printf("Dealer decided to hit.\n");
		printf("\tDEALER: ");
		showHand(dealerHand, dealerHandSize);
	}
	else
	{
		printf("Dealer chose to stay.\n");
		stay(0);
	}
	return dealerHandSize;
}

int playerMove(Card* yourHand, int playerHandSize)
{
	/* Given your hand and the hand's size, it will apply the move based on the player's input in the console */
	if (playerStayed)
		return playerHandSize;
	int moved = FALSE;
	char c;
	while ((c = fgetc(stdin)) && !moved)
	{	
		switch(c) {
		case 's':
			moved = TRUE;
			stay(1);
			printf("You chose to stay.\n");
			break;
		case 'h':
			moved = TRUE;
			hit(yourHand, playerHandSize++);
			printf("You chose to hit.\n");
			printf("\tYOUR HAND: ");
			showHand(yourHand, playerHandSize);
			break;
		case 'q':
			surrender();
			break;
		case 'p':
			printDeck();
			break;
		case 'u':
			printUsedDeck();
			break;
		}
		fflush(stdout);
	}
	return playerHandSize;
}

int isTie(Card* yourHand, int playerHandSize, Card* dealerHand, int dealerHandSize)
{	/* Determines if two hands are a tie */
	if (handValue(yourHand, playerHandSize) == handValue(dealerHand, dealerHandSize))
		return TRUE;
	return FALSE;
}

int determineWinner(Card* yourHand, int playerHandSize, Card* dealerHand, int dealerHandSize)
{
	/* Given two hands, it determines the winner:
		-1 : Tie
		0 : Dealer wins
		1 : Player wins
	*/
	int playerScore = handValue(yourHand, playerHandSize);
	int dealerScore = handValue(dealerHand, dealerHandSize);
	if (playerScore > 21 && dealerScore > 21)
		return -1;
	else if (playerScore > 21)
		return 0;
	else if (dealerScore > 21)
		return 1;
	if (isTie(yourHand, playerHandSize, dealerHand, dealerHandSize))
	{
		if (playerScore == 21 && playerHandSize == 2 && dealerHandSize != 2)
			return 1;
		else if (playerScore == 21 && playerHandSize != 2 && dealerHandSize == 2)
			return 0;
		else
			return -1; /* If tie and no blackjack */
	}
	else if (playerScore > dealerScore)
		return 1; /* You are the winner */
	else
		return 0; /* Dealer is winner */
}

void initiateRound()
{
	/* Starts a new round */
	playerStayed = FALSE;
	dealerStayed = FALSE;
	cardsInHand = 0;
	bet();
}

void discard(Card* hand, int numberOfCardsInHand)
{
	/* Discards all the cards in the hand */
	for (int i = 0; i < numberOfCardsInHand; ++i)
	{
		if (usedTopIndex == MAXCARDS + 1)
			usedTopIndex = 0;
		strcpy(usedDeck[usedTopIndex].rank, hand[i].rank);
		usedDeck[usedTopIndex].suit = hand[i].suit;
		usedDeck[usedTopIndex].value = hand[i].value;
		usedTopIndex += 1;
	}
} 


void startGame(int money)
{	/* Game loop to start the game on console with a starting balance */
	balance = money;
	int playerHandSize = 0;
	int dealerHandSize = 0;
	int round = 1;
	printOptions();
	while (balance > 0)
	{
		printf("\n********************Start of Round [%d]*******************\n", round);	
		initiateRound();
		Card yourHand[STRMAX];
		Card dealerHand[STRMAX];
		hit(yourHand, playerHandSize++);
		hit(dealerHand, dealerHandSize++);
		hit(yourHand, playerHandSize++);
		hit(dealerHand, dealerHandSize++);
		printHands(yourHand, playerHandSize, dealerHand, dealerHandSize);
		while((handValue(yourHand, playerHandSize) <= 21 && handValue(dealerHand, dealerHandSize) <= 21) && (!playerStayed || !dealerStayed))
		{
			playerHandSize = playerMove(yourHand, playerHandSize);
			dealerHandSize = dealerMove(dealerHand, dealerHandSize);
		}	
		printf("Determining winner...\n");
		printHands(yourHand, playerHandSize, dealerHand, dealerHandSize);
		int winner = determineWinner(yourHand, playerHandSize, dealerHand, dealerHandSize);
		if (winner == 1)
		{
			printf("You won. Congrats.\n");
			balance += pot;
			pot = 0;
			++numberOfWins;
		}
		else if( winner == 0)
		{
			printf("You lose.\n");
			++numberOfLosses;
			pot = 0;
		}
		else
		{
			printf("It was a stalemate\n");
			balance += pot/2;
			++numberOfStalemates;
			pot = 0;
		}
		discard(yourHand, playerHandSize);
		discard(dealerHand, dealerHandSize);
		playerHandSize = 0;
		dealerHandSize = 0;
		printf("*********************End of Round [%d]********************\n", round++);	
	}
	printf("Your balance has reached 0.\n");
	gameStats();
}

int main()
{
	makeNewDeck();
	shuffleDeck();
	startGame(STARTING_BALANCE);
	return 0;
}
