#include <iostream>
#include <array>
#include <string>
#include <random>
#include <iomanip>

//initialize global constant variables
const int STARTING_HAND_SIZE{6};

const int FACE_NUM{13};
const std::array<std::string, FACE_NUM> CARD_FACE {
    "Ace",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Ten",
    "Jack",
    "Queen",
    "King",
};

const int SUIT_NUM{4};
const std::array<std::string, SUIT_NUM> CARD_SUIT {
    "Hearts",
    "Diamonds",
    "Clubs",
    "Spades",
};

//defining a type for the deck and each hand. Initializes a 13x4 boolean matrix which holds true or false
//for each value. The x,y coordinate represents the face and suit of each card.
typedef std::array<std::array<bool, SUIT_NUM>, FACE_NUM> deckType;

//struct to contain information about each hand (user, comp, or dealers)
struct hand{
    std::string owner;
    deckType hand;
    int size;
    int score;
};

//initialize user and computer scores
int userScore{};
int compScore{};

//Function Prototypes
void printArray(deckType);
void printHand(deckType);
void dealCard(hand &, hand &, const hand);
void endOfGame(hand, hand);

int faceToNum(std::string);
int getCompGuess(deckType);

bool checkGuess(int, hand &, hand &);
bool checkForPairs(hand &);

//initialize a random number generator object
std::mt19937 gen(time(nullptr));
std::uniform_int_distribution<> suitDis(0,SUIT_NUM-1);
std::uniform_int_distribution<> faceDis(0,FACE_NUM-1);

int main(){
    //initialize game variables
    hand deck{};
    hand user{};
    user.owner = "you";
    hand comp{};
    comp.owner = "the computer";

    //fills deck
    for(int face{0}; face < FACE_NUM; face++){
        deck.hand.at(face).at(0) = 1;
        for(int suit{0}; suit < SUIT_NUM; suit++){
            deck.hand.at(face).at(suit) = 1;
        }
    }

    //starts each hand by dealing cards from the deck
    for(int i{}; i < STARTING_HAND_SIZE; i++){
        dealCard(user, deck, comp);
        dealCard(comp, deck, user);
    }    

    //the game nested inside a while loop that terminates if either the user or 
    //the computer has a hand size of 0.
    while(user.size > 0 && comp.size > 0){
        /*********************************************************************************************
        *********************************************************************************************/
        //user turn
        std::cout << "\n----------------------------------------------------" << std::endl;
        std::cout << "Your turn! [Press enter to continue]";
        std::string throwaway{};
        getline(std::cin,  throwaway);
        std::cout << std::endl;
        bool success{true};

        while(success && user.size > 0){
            std::cout << "Your hand: " << std::endl;
            printHand(user.hand);
        
            if(checkForPairs(user)){
                std::cout << "Your hand: " << std::endl;
                printHand(user.hand);
            }

            std::string guess{};
            int guessInt{FACE_NUM+1};

            std::cout << ">>>>>You: do you have a(n)...(type a word): ";
            getline(std::cin, guess);
            if(guess == "exit"){
                exit(0);
            }

            //loop to accept user input in the correct form
            while(guessInt == (FACE_NUM+1)){
                guessInt = faceToNum(guess);
                if(guessInt == (FACE_NUM+1)){
                    std::cout << "\nOnly input a valid face (Ace, Two, Queen, ect.)" << std::endl;

                    std::cout << "\n>>>>>You: do you have a(n)...(type a word): ";
                    getline(std::cin, guess);
                    if(guess == "exit"){
                        exit(0);
                    }
                }
            }
        
            if(checkGuess(guessInt, user, comp)){
                std::cout << "Computer: I do have a(n) " << guess << '\n' << std::endl;
                success = true;
                checkForPairs(user);
            } else{
                std::cout << "Computer: I do not have a(n) " << guess << ", Go Fish!\n" << std::endl;
                dealCard(user, deck, comp);
                std::cout << "\nYour hand: " << std::endl;
                printHand(user.hand);
                success = false;
            }
        }

        /*********************************************************************************************
        *********************************************************************************************/
        //computer's turn
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "Computer's Turn! [Press enter to continue]";
        getline(std::cin,  throwaway);
        std::cout << std::endl;
        success = true;
        
        checkForPairs(comp);

        while(success && comp.size > 0){
            int compGuess = getCompGuess(comp.hand);
            std::cout << "Computer: do you have a(n) " << CARD_FACE.at(compGuess) << std::endl;

            if(checkGuess(compGuess, comp, user)){
                std::cout << ">>>>>You: I do have a(n) " << CARD_FACE.at(compGuess) << '\n' << std::endl;
                success = true;
                checkForPairs(comp);
            } else{
                std::cout << ">>>>>You: I do not have a(n) " << CARD_FACE.at(compGuess) << ", Go Fish!\n" << std::endl;
                dealCard(comp, deck, user);
                success = false;
            }
        }
    }
    endOfGame(user, comp);
}

//generates a valid guess for the computer
//(the computer would only guess a face that it actually has in its hand)
int getCompGuess(deckType hand){
    int guess{0};
    bool inHand{false};
    while(!inHand){
        guess = faceDis(gen);
        for(int suit{}; suit < SUIT_NUM; suit++){
            if(hand.at(guess).at(suit) == 1){
                return guess;
            }
        }
    }
    return 0;
}
//converts a users guess(a string) into an int value 
int faceToNum(std::string face){
    for(int i{}; i < FACE_NUM; i++){
        if(face == CARD_FACE.at(i)){
            return i;
        }
    }
    return FACE_NUM+1;
}

bool checkForPairs(hand &player){
    bool success{false};
    for(int face{}; face < FACE_NUM; face++){
        for(int suit{}; suit < SUIT_NUM; suit++){
            for(int i{suit+1}; i < SUIT_NUM; i++){
                if(player.hand.at(face).at(suit) == 1 && player.hand.at(face).at(i) == 1){
                    player.hand.at(face).at(suit) = player.hand.at(face).at(i) = 0;
                    std::cout << "Pair found! Removing " << CARD_FACE.at(face) << " of " << CARD_SUIT.at(suit) << " and " << CARD_SUIT.at(i) << std::endl;
                    player.score++;
                    std::cout << "New score: " << player.score << '\n' << std::endl;
                    player.size -= 2;
                    success = true;
                }
            }
        }
    }
    return success;
}

//takes a guess and sees if it's contained in the other's hand. If it is, it is removed from the other's hand
//and put into the guesser's hand. Finally, returns either true or false.
bool checkGuess(int guess, hand &ask, hand &tell){
    for(int suit{}; suit < SUIT_NUM; suit++){
        if(tell.hand.at(guess).at(suit) == 1){
            ask.hand.at(guess).at(suit) = 1;
            tell.hand.at(guess).at(suit) = 0;
            ask.size++;
            tell.size--;
            return true;
        }
    }
    return false;
}


//deals with the end of the game. Compares scores of user and computer
void endOfGame(hand player1, hand player2){
    if(player1.owner == "you"){
        if(player1.score > player2.score){
            std::cout << "You win! You had a score of " << player1.score << "!" << std::endl;
            std::cout << "The computer had a score of " << player2.score << "!" << std::endl;
        } else if(player1.score < player2.score){
            std::cout << "You lost! You had a score of " << player1.score << "!" << std::endl;
            std::cout << "The computer had a score of " << player2.score << "!" << std::endl;
        } else if(player1.score == player2.score){
            std::cout << "It was a draw! You both had a score of " << player1.score << "!" << std::endl;
        }
    } else if(player1.owner == "the computer"){
        if(player1.score > player2.score){
            std::cout << "You lost! You had a score of " << player2.score << "!" << std::endl;
            std::cout << "The computer had a score of " << player1.score << "!" << std::endl;
        } else if(player1.score < player2.score){
            std::cout << "You win! You had a score of " << player2.score << "!" << std::endl;
            std::cout << "The computer had a score of " << player1.score << "!" << std::endl;
        } else if(player1.score == player2.score){
            std::cout << "It was a draw! You both had a score of " << player1.score << "!" << std::endl;
        }
    }

    exit(0);
}

//deals a card into a hand
void dealCard(hand &player, hand &deck, const hand opponent){
    std::cout << "Dealing card...\n";

    int deckSize{52};

    int face{faceDis(gen)};
    int suit{suitDis(gen)};
    
    while(true){
        if(deck.hand.at(face).at(suit) == 1){
            player.hand.at(face).at(suit) = 1;
            deck.hand.at(face).at(suit) = 0;
            player.size++;
            return;
        } else{
            face = faceDis(gen);
            suit = suitDis(gen);
            deckSize --;

            if(deckSize <= 0){
                std::cout << "The deck is empty! Ending game..." << std::endl;
                endOfGame(player, opponent);
            }
        }
    }
}

//prints a hand
void printHand(deckType hand){
    int count{1};
    for(int face{}; face < FACE_NUM; face++){
        for(int suit{}; suit < SUIT_NUM; suit++){
            if(hand.at(face).at(suit) == 1){
                std::cout << count << ".) " << std::left << std::setw(6) << CARD_FACE.at(face) << "of " << CARD_SUIT.at(suit) << std::endl;
                count ++;
            }
        }
    }
    std::cout << std::endl;
}

//prints the entirety of an array (for debuggin purposes)
//each ROW represents a FACE and each COLUMN represents a SUIT
//each array is a binary array and indicates if a card is present in that array or not
void printArray(deckType deck){
    std::cout << std::endl;
    for(int i{0}; i < FACE_NUM; i++){
        for(int j{0}; j < SUIT_NUM; j++){
            std::cout << std::setw(4) << deck.at(i).at(j)<< " ";
        }
        std::cout << std::endl;
    }
}