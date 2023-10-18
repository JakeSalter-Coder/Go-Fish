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

//initialize user and computer scores
int userScore{};
int compScore{};

//Function Prototypes
void printArray(deckType);
void printHand(deckType);
void dealCard(deckType &, deckType &, int &);
void endOfGame();

int faceToNum(std::string);
int getCompGuess(deckType);

bool checkGuess(int, deckType &, deckType &, int &, int &);
bool checkForPairs(deckType &, deckType &, int &);

//initialize a random number generator object
std::mt19937 gen(time(nullptr));
std::uniform_int_distribution<> suitDis(0,SUIT_NUM-1);
std::uniform_int_distribution<> faceDis(0,FACE_NUM-1);

int main(){
    //initialize game variables
    deckType deck{};
    deckType discard{};

    std::string guess{};

    deckType userHand{};
    int userHandSize{};

    deckType compHand;
    int compHandSize{};

    //fills deck
    for(int face{0}; face < FACE_NUM; face++){
        deck.at(face).at(0) = 1;
        for(int suit{0}; suit < SUIT_NUM; suit++){
            deck.at(face).at(suit) = 1;
        }
    }

    //starts each hand by dealing cards from the deck
    for(int i{}; i < STARTING_HAND_SIZE; i++){
        dealCard(userHand, deck, userHandSize);
        dealCard(compHand, deck, compHandSize);
    }    

    std::cout << std::endl;

    //the game
    while(userHandSize > 0 && compHandSize > 0){
        //user turn
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << "Your turn! [Press enter to continue]";
        std::string throwaway{};
        getline(std::cin,  throwaway);
        bool success{true};

        while(success){
            std::cout << "\nYour hand: " << std::endl;
            printHand(userHand);
        
            if(checkForPairs(userHand, discard, userScore)){
                printHand(userHand);
            };

            std::cout << "\nYou:      do you have a(n)...(type a word): ";
            getline(std::cin, guess);
            if(guess == "exit"){
                exit(0);
            }

            int guessInt{FACE_NUM+1};
            //loop to accept user input in the correct form
            while(guessInt == (FACE_NUM+1)){
                guessInt = faceToNum(guess);
                if(guessInt == (FACE_NUM+1)){
                    std::cout << "\nOnly input a valid face (Ace, Two, Queen, ect.)" << std::endl;

                    std::cout << "\nYou: do you have a(n)...(type a word): ";
                    getline(std::cin, guess);
                    if(guess == "exit"){
                        exit(0);
                    }
                }
            }
        
            if(checkGuess(guessInt, userHand, compHand, userHandSize, compHandSize)){
                std::cout << "Computer: I do have a(n) " << guess << std::endl;
                success = true;
                checkForPairs(userHand, discard, userScore);
            } else{
                std::cout << "Computer: I do not have a(n) " << guess << ", Go Fish!" << std::endl;
                dealCard(userHand, deck, userHandSize);
                printHand(userHand);
                success = false;
            }
        }

        //computer turn
        std::cout << "----------------------------------------------------" << std::endl;
        success = true;
        std::cout << "Computer's Turn! [Press enter to continue]";
        getline(std::cin,  throwaway);
        
        checkForPairs(compHand, discard, compScore);

        while(success){
            int compGuess = getCompGuess(compHand);
            std::cout << "\nComputer: do you have a(n) " << CARD_FACE.at(compGuess) << std::endl;

            if(checkGuess(compGuess, compHand, userHand, compHandSize, userHandSize)){
                std::cout << "\nYou:      I do have a(n) " << CARD_FACE.at(compGuess) << std::endl;
                success = true;
                checkForPairs(compHand, discard, compScore);
            } else{
                std::cout << "\nYou:      I do not have a(n) " << CARD_FACE.at(compGuess) << ", Go Fish!" << std::endl;
                dealCard(compHand, deck, compHandSize);
                success = false;
            }
        }
    }
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

bool checkForPairs(deckType &hand, deckType &discard, int &score){
    bool success{false};
    for(int face{}; face < FACE_NUM; face++){
        for(int suit{}; suit < SUIT_NUM; suit++){
            for(int i{suit+1}; i < SUIT_NUM; i++){
                if(hand.at(face).at(suit) == 1 && hand.at(face).at(i) == 1){
                    hand.at(face).at(suit) = hand.at(face).at(i) = 0;
                    discard.at(face).at(suit) = discard.at(face).at(i) = 0;
                    std::cout << "\nPair found! Removing " << CARD_FACE.at(face) << " of " << CARD_SUIT.at(suit) << " and " << CARD_SUIT.at(i) << std::endl;
                    score++;
                    std::cout << "New score: " << score << std::endl;
                    success = true;
                }
            }
        }
    }
    return success;
}

//takes a guess and sees if it's contained in the other's hand. If it is, it is removed from the other's hand
//and put into the guesser's hand. Finally, returns either true or false.
bool checkGuess(int guess, deckType &askHand, deckType &tellHand, int &askHandSize, int &tellHandSize){
    for(int suit{}; suit < SUIT_NUM; suit++){
        if(tellHand.at(guess).at(suit) == 1){
            askHand.at(guess).at(suit) = 1;
            tellHand.at(guess).at(suit) = 0;
            askHandSize++;
            tellHandSize--;
            return true;
        }
    }
    return false;
}


//deals with the end of the game. Compares scores of user and computer
void endOfGame(){
    if(userScore > compScore){
        std::cout << "You win!" << std::endl;
    } else if(compScore > userScore){
        std::cout << "You lose!" << std::endl;
    } else if(compScore == userScore){
        std::cout << "It's a draw! You both had " << userScore << " points!" << std::endl;
    }

    exit(0);
}
//deals a card into a hand
void dealCard(deckType &hand, deckType &deck, int &handSize){
    std::cout << "Dealing card...\n";

    int deckSize{52};

    int face{faceDis(gen)};
    int suit{suitDis(gen)};
    
    while(true){
        if(deck.at(face).at(suit) == 1){
            hand.at(face).at(suit) = 1;
            deck.at(face).at(suit) = 0;
            handSize++;
            return;
        } else{
            face = faceDis(gen);
            suit = suitDis(gen);
            deckSize --;

            if(deckSize <= 0){
                std::cout << "The deck is empty! Ending game..." << std::endl;
                endOfGame();
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