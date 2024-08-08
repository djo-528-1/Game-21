#include <iostream>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <limits>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

bool yes_or_no ()
{
    bool y_or_n = true;
    string input_key;
    for (;;)
    {
        cin >> input_key;
        if (input_key == "Y" || input_key == "y")
        {
            return true;
        }
        else if (input_key == "N" || input_key == "n")
        {
            return false;
        }
        else
        {
            cout << "You have entered a character other than Y/N!" << endl;
        }
    }
}

void Rules ()
{
    cout << "The rules are simple." << endl;
    cout << "You need to score the number 21 or as close to this number as possible than your opponent. The deck consists of 11 cards numbered from 1 to 11." << endl;
    cout << "If you win, your balance will change as follows: balance + bet. In case of a draw, your bet will be returned. In case of a loss, you will lose your bet." << endl;
    cout << "As soon as 2 participants make a skip move (fold) the game ends. If your balance is empty, the game will automatically end" << endl;
}

void Input_int (unsigned long int &number)
{
    for (;;)
    {
        string input;
        cin >> input;
        if (input.find_first_not_of("0123456789") != input.npos || input == "0")
        {
            cin.clear();
            cout << "Incorrect input! Try again" << endl;
        }
        else
        {
            try
            {
                number = stoul (input);
                break;
            }
            catch(const out_of_range)
            {
                cout << "You entered a number greater than 4,294,967,295! Repeat the entry" << endl;
            }
        }
    }
}

void Filling_deck (vector<int> &fil_deck)
{
    int number = 1;
    for (int i = 0; i < 11; i++)
    {
        fil_deck[i] = number;
        number++;
    }
}

void Remove_card (vector<int> &rm_deck, int rm_card)
{
    vector<int>::iterator rm_pos_card = find(rm_deck.begin(), rm_deck.end(), rm_card);
    rm_deck.erase(rm_pos_card);
}

void Print_decks (vector<int> &player_deck, vector<int> &dealer_deck)
{
    cout << "Your cards: ";
    copy(player_deck.begin(), player_deck.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    cout << "Dealer cards: * ";
    copy(dealer_deck.begin()+1, dealer_deck.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
}

void Take (vector<int> &game_deck, vector<int> &any_deck)
{
    int card = game_deck[rand() % game_deck.size()];
    any_deck.push_back(card);
    Remove_card(game_deck, card);
}

void Player_turn (vector<int> &game_deck, vector<int> &player_deck, bool &take_player_flag, bool &take_dealer_flag)
{
    cout << "*Your turn*" << endl;
    cout << "Do you want to take a card from the deck or skip a turn? (T/F)" << endl;
    string input_key;
    for (;;)
    {
        cin >> input_key;
        if (input_key == "T" || input_key == "t")
        {
            take_player_flag = true;
            take_dealer_flag = false;
            Take(game_deck, player_deck);
            break;
        }
        else if (input_key == "F" || input_key == "f")
        {
            take_player_flag = false;
            break;
        }
        else
            cout << "You have entered a character other than T/F!" << endl;
    }
}

void Dealer_turn (vector<int> &game_deck, vector<int> &dealer_deck, bool &take_player_flag, bool &take_dealer_flag)
{
    cout << "*Dealer turn*" << endl;
    int dealer_score = accumulate(dealer_deck.begin(), dealer_deck.end(), 0);
    if (rand() % 2 == 1 && dealer_score < 21)
    {
        take_dealer_flag = true;
        take_player_flag = false;
        Take(game_deck, dealer_deck);
    }
    else
        take_dealer_flag = false;
}

void Main_turn (vector<int> &game_deck, vector<int> &player_deck, vector<int> &dealer_deck, bool &take_player_flag, bool &take_dealer_flag)
{
    if (!take_player_flag || take_dealer_flag)
    {
        Player_turn(game_deck, player_deck, take_player_flag, take_dealer_flag);
        Print_decks(player_deck, dealer_deck);
        if (!take_player_flag && game_deck.size()!=0)
        {
            Dealer_turn(game_deck, dealer_deck, take_player_flag, take_dealer_flag);
            this_thread::sleep_for(chrono::milliseconds(2000));
            Print_decks(player_deck, dealer_deck);
        }
    }
    else if (take_player_flag || !take_dealer_flag)
    {
        Dealer_turn(game_deck, dealer_deck, take_player_flag, take_dealer_flag);
        this_thread::sleep_for(chrono::milliseconds(2000));
        Print_decks(player_deck, dealer_deck);
        if (!take_dealer_flag && game_deck.size()!=0)
        {
            Player_turn(game_deck, player_deck, take_player_flag, take_dealer_flag);
            Print_decks(player_deck, dealer_deck);
        }
    }
}

void Print_totals (vector<int> player_deck, vector<int> dealer_deck)
{
    cout << " Your cards: ";
    copy(player_deck.begin(), player_deck.end(), ostream_iterator<int>(cout, " "));
    cout << " Dealer cards: ";
    copy(dealer_deck.begin(), dealer_deck.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
}

void Game_results (vector<int> player_deck, vector<int> dealer_deck, unsigned long int &player_balance, unsigned long int &player_bet)
{
    int player_score = accumulate(player_deck.begin(), player_deck.end(), 0);
    int dealer_score = accumulate(dealer_deck.begin(), dealer_deck.end(), 0);
    if (player_score > dealer_score && player_score < 22 || player_score == 21 && dealer_score != 21)
    {
        cout << "You win!";
        Print_totals(player_deck, dealer_deck);
        player_balance+= player_bet;
    }
    else if (player_score < dealer_score && dealer_score < 22 || dealer_score == 21 && player_score != 21)
    {
        cout << "You lost :(";
        Print_totals(player_deck, dealer_deck);
        player_balance-= player_bet;
    }
    else if (player_score == dealer_score && player_score < 22 || player_score == 21 && dealer_score == 21)
    {
        cout << "No one won";
        Print_totals(player_deck, dealer_deck);
    }
    else if (player_score > 21 || dealer_score > 21)
    {
        if (player_score < dealer_score)
        {
            cout << "You win!";
            Print_totals(player_deck, dealer_deck);
            player_balance+= player_bet;
        }
        else if (player_score > dealer_score)
        {
            cout << "You lost :(";
            Print_totals(player_deck, dealer_deck);
            player_balance-= player_bet;
        }
        else
        {
            cout << "No one won";
            Print_totals(player_deck, dealer_deck);
        }
    }
}

int main ()
{
    unsigned long int balance = 0, bet = 0;
    bool take_player_flag, take_dealer_flag;
    vector <int> game_deck(11), player_deck, dealer_deck;
    player_deck.reserve(2), dealer_deck.reserve(2);
    Filling_deck(game_deck);
    srand(time(NULL));

    cout << "Welcome to 21.\n" << "Want to familiarize yourself with the rules? (Y/N)" << endl;
    if (yes_or_no())
    {
        Rules();
    }
    cout << "Enter your starting balance: ";
    Input_int(balance);
    do
    {
        cout << "Enter your bet: ";
        for(;;)
        {
            Input_int(bet);
            if (bet > balance)
                cout << "The bet exceeds your balance! Reenter your bet: " << endl;
            else
                break;
        }
        
        for (int i = 0; i < 2; i++)
        {
            Take(game_deck, player_deck);
            Take(game_deck, dealer_deck);
        }
        Print_decks(player_deck, dealer_deck);

        do
        {
            Main_turn(game_deck, player_deck, dealer_deck, take_player_flag, take_dealer_flag);
        } while ((take_player_flag || take_dealer_flag) && game_deck.size()!=0);
        if (game_deck.size()==0)
            cout << "The game deck has run out" << endl;
        Game_results(player_deck, dealer_deck, balance, bet);

        cout << "Your current balance: " << balance << endl;
        game_deck.resize(11), player_deck.clear(), dealer_deck.clear();
        Filling_deck(game_deck);
        take_player_flag = false, take_dealer_flag = false;
        if (balance == 0)
        {
            cout << "Your balance is 0. No further play is possible." << endl;
            break;
        }
        cout << "Do you want to play again? (Y/N)" << endl;
    } while (yes_or_no());
    cout << "Thanks for playing the game. To close the program, press Enter" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}