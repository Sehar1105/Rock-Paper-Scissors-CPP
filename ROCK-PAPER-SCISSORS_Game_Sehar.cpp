/*
 *       ROCK  PAPER  SCISSORS  —  THE ULTIMATE BATTLE
 *           Coded by: Sehar | FAST NUCES Islamabad
 *  FEATURES:
 *   - Colorful ASCII art for Rock, Paper, Scissors
 *   - 3 AI difficulty levels (Easy / Medium / Hard)
 *   - Best of N rounds mode
 *   - Win/Loss streak tracker with combo messages
 *   - Leaderboard saved to file (top 5 scores)
 *   - Smooth animated countdown before each round
 *   - Full match stats at end (win%, favorite move, etc.)
 *
 *  NOTE: No STL <vector> or <algorithm> used.
 *        Leaderboard uses plain arrays + manual bubble sort.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
// ANSI color codes 
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

//Constants
const string LEADERBOARD_FILE = "leaderboard.txt";
const int    TOP_SCORES       = 5;

//Structs
struct Player {
    string name;
    int wins;
    int losses;
    int draws;
    int currentStreak;
    int bestStreak;
    int moveCount[3]; // 0=Rock, 1=Paper, 2=Scissors
};
struct ScoreEntry {
    string name;
    int    score;
    int    rounds;
};
//ASCII art
void printRock() {
    cout << YELLOW;
    cout << "    _______\n";
    cout << "---'   ____)\n";
    cout << "      (_____)\n";
    cout << "      (_____)\n";
    cout << "      (____)\n";
    cout << "---.__(___)\n";
    cout << RESET;
}

void printPaper() {
    cout << CYAN;
    cout << "    _______\n";
    cout << "---'   ____)____\n";
    cout << "          ______)\n";
    cout << "          _______)\n";
    cout << "         _______)\n";
    cout << "---.__________)\n";
    cout << RESET;
}

void printScissors() {
    cout << MAGENTA;
    cout << "    _______\n";
    cout << "---'   ____)____\n";
    cout << "          ______)\n";
    cout << "       __________)\n";
    cout << "      (____)      \n";
    cout << "---.__(___)       \n";
    cout << RESET;
}
void printMove(int move) {
    if(move == 0)      printRock();
    else if(move == 1) printPaper();
    else               printScissors();
}
string moveName(int move) {
    if(move == 0) return "ROCK";
    if(move == 1) return "PAPER";
    return "SCISSORS";
}

//Utilities
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
void pause(int ms = 400) {
    clock_t start = clock();
    while((clock() - start) < (ms * CLOCKS_PER_SEC / 1000)) {}
}
void printLine(string c = "-", int len = 54) {
    cout << BLUE;
    for(int i = 0; i < len; i++) cout << c;
    cout << RESET << "\n";
}
//Countdown animation 
void countdown() {
    cout << BOLD << YELLOW;
    cout << "\n   3..."; cout.flush(); pause(700);
    cout << " 2...";    cout.flush(); pause(700);
    cout << " 1...";    cout.flush(); pause(700);
    cout << GREEN << " SHOOT!\n" << RESET;
    pause(300);
}
//Banner 
void printBanner() {
    clearScreen();
    cout << BOLD << CYAN;
    cout << "\n";
    cout << "  ██████╗  ██████╗ ███████╗\n";
    cout << "  ██╔══██╗██╔═══╝ ██╔════╝\n";
    cout << "  ██████╔╝██████╗ ███████╗\n";
    cout << "  ██╔══██╗██╔══╝  ╚════██║\n";
    cout << "  ██║  ██║╚██████╗███████║\n";
    cout << "  ╚═╝  ╚═╝ ╚═════╝╚══════╝\n";
    cout << RESET << BOLD << YELLOW;
    cout << "   Rock  X  Paper  O  Scissors  V\n";
    cout << "        THE  ULTIMATE  EDITION\n";
    cout << RESET << "\n";
    printLine("=");
}
int getPlayerMove() {
    int choice;
    while(true) {
        cout << "\n  " << BOLD << "Your move:\n" << RESET;
        cout << "  " << YELLOW  << "[1]" << RESET << " Rock\n";
        cout << "  " << CYAN    << "[2]" << RESET << " Paper\n";
        cout << "  " << MAGENTA << "[3]" << RESET << " Scissors\n";
        cout << "\n  Enter (1/2/3): ";
        cin >> choice;
        if(cin.fail() || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << RED << "  Invalid! Enter 1, 2 or 3.\n" << RESET;
        } else {
            return choice - 1;
        }
    }
}

// AI logic
int getAIMove(int difficulty, int playerLastMove, int playerMoves[3]) {
    if(difficulty == 1) {
        // Easy: fully random
        return rand() % 3;
    }
    if(difficulty == 2) {
        // Medium: 50% random, 50% counter player's most used move
        if(rand() % 2 == 0) return rand() % 3;
        int maxMove = 0;
        for(int i = 1; i < 3; i++)
            if(playerMoves[i] > playerMoves[maxMove]) maxMove = i;
        return (maxMove + 1) % 3;
    }
    // Hard: mostly predicts, occasionally random
    int roll = rand() % 10;
    if(roll < 5) {
        // Counter most-used move
        int maxMove = 0;
        for(int i = 1; i < 3; i++)
            if(playerMoves[i] > playerMoves[maxMove]) maxMove = i;
        return (maxMove + 1) % 3;
    } else if(roll < 8) {
        // Counter last move
        if(playerLastMove >= 0) return (playerLastMove + 1) % 3;
        return rand() % 3;
    }
    return rand() % 3;
}

// ─── Round result: 0=draw, 1=player wins, 2=AI wins ──────────────────────────
int determineWinner(int player, int ai) {
    if(player == ai) return 0;
    if((player == 0 && ai == 2) ||
       (player == 1 && ai == 0) ||
       (player == 2 && ai == 1)) return 1;
    return 2;
}

// ─── Streak messages ──────────────────────────────────────────────────────────
void printStreakMessage(int streak) {
    if(streak == 2)     cout << GREEN  << "  DOUBLE WIN! You're on fire!\n"          << RESET;
    else if(streak == 3) cout << GREEN << BOLD << "  TRIPLE! Unstoppable!!\n"        << RESET;
    else if(streak == 4) cout << YELLOW<< BOLD << "  QUAD COMBO! Are you even human?!\n" << RESET;
    else if(streak >= 5) cout << CYAN  << BOLD << "  " << streak << "x GODLIKE STREAK!! LEGENDARY!\n" << RESET;
}

void printLossStreakMessage(int streak) {
    if(streak == 2)      cout << RED << "  Oops, 2 in a row lost... focus!\n"        << RESET;
    else if(streak == 3) cout << RED << BOLD << "  3 losses! AI is reading you!\n"   << RESET;
    else if(streak >= 4) cout << RED << BOLD << "  " << streak << "x loss streak! Change strategy!\n" << RESET;
}

// ─── Leaderboard: manual array, no vector ────────────────────────────────────

// Read leaderboard from file into a plain array; returns count of entries read
int readLeaderboard(ScoreEntry board[], int maxSize) {
    int count = 0;
    ifstream fin(LEADERBOARD_FILE);
    if(!fin.is_open()) return 0;
    while(count < maxSize && fin >> board[count].name
                                >> board[count].score
                                >> board[count].rounds) {
        count++;
    }
    fin.close();
    return count;
}

// Manual bubble sort — descending by score (no <algorithm> needed)
void sortLeaderboard(ScoreEntry board[], int count) {
    for(int i = 0; i < count - 1; i++) {
        for(int j = 0; j < count - i - 1; j++) {
            if(board[j].score < board[j+1].score) {
                ScoreEntry temp = board[j];
                board[j]        = board[j+1];
                board[j+1]      = temp;
            }
        }
    }
}

// Save top TOP_SCORES entries back to file
void saveLeaderboard(ScoreEntry board[], int count) {
    sortLeaderboard(board, count);
    if(count > TOP_SCORES) count = TOP_SCORES;
    ofstream fout(LEADERBOARD_FILE);
    for(int i = 0; i < count; i++) {
        fout << board[i].name  << " "
             << board[i].score << " "
             << board[i].rounds << "\n";
    }
    fout.close();
}

// Display leaderboard on screen
void showLeaderboard() {
    ScoreEntry board[TOP_SCORES + 1];
    int count = readLeaderboard(board, TOP_SCORES + 1);

    printLine("=");
    cout << BOLD << YELLOW << "       HALL OF FAME - TOP PLAYERS\n" << RESET;
    printLine("=");

    if(count == 0) {
        cout << "  No records yet. Be the first champion!\n";
    } else {
        cout << BOLD;
        cout << "  " << left << setw(4) << "#"
             << setw(16) << "Name"
             << setw(8)  << "Wins"
             << "Rounds\n";
        cout << RESET;
        printLine("-");
        for(int i = 0; i < count; i++) {
            // Medal colors
            if(i == 0)      cout << YELLOW;
            else if(i == 1) cout << "\033[37m";  // white (silver)
            else if(i == 2) cout << "\033[33m";  // dark yellow (bronze)
            cout << "  " << (i+1) << ". " << RESET
                 << left << setw(15) << board[i].name
                 << setw(8)          << board[i].score
                 << board[i].rounds  << " rounds\n";
        }
    }
    printLine("=");
}

//Match stats
void showMatchStats(Player& p, int totalRounds) {
    printLine("=");
    cout << BOLD << CYAN << "         MATCH STATS - " << p.name << "\n" << RESET;
    printLine("=");

    int played   = p.wins + p.losses + p.draws;
    float winPct = (played > 0) ? (p.wins * 100.0f / played) : 0;

    cout << "  Rounds played : " << played << "\n";
    cout << GREEN  << "  Wins          : " << p.wins    << RESET << "\n";
    cout << RED    << "  Losses        : " << p.losses  << RESET << "\n";
    cout << YELLOW << "  Draws         : " << p.draws   << RESET << "\n";
    cout << "  Win rate      : " << fixed << setprecision(1) << winPct << "%\n";
    cout << "  Best streak   : " << p.bestStreak << " win(s) in a row\n";

    // Favorite move (manual max search)
    int fav = 0;
    for(int i = 1; i < 3; i++)
        if(p.moveCount[i] > p.moveCount[fav]) fav = i;
    cout << "  Favorite move : " << moveName(fav)
         << " (used " << p.moveCount[fav] << "x)\n";

    // Performance comment
    cout << "\n  ";
    if(winPct >= 70)      cout << GREEN  << BOLD << "DOMINANT! You absolutely crushed the AI!" << RESET;
    else if(winPct >= 50) cout << CYAN         << "Solid game! You had the upper hand."       << RESET;
    else if(winPct >= 40) cout << YELLOW       << "Close battle! A little more practice."     << RESET;
    else                  cout << RED          << "AI won this time. Rematch?"                << RESET;
    cout << "\n";
    printLine("=");
}

//One full match 
void playMatch(Player& player, int difficulty, int totalRounds) {
    int aiWins        = 0;
    int playerLastMove = -1;
    int lossStreak    = 0;

    string diffName = (difficulty == 1) ? "Easy" : (difficulty == 2) ? "Medium" : "Hard";
    cout << "\n" << BOLD << "  Best of " << totalRounds
         << " | AI difficulty: " << diffName << "\n" << RESET;
    printLine();

    for(int round = 1; round <= totalRounds; round++) {
        cout << "\n" << BOLD << BLUE
             << "  --- Round " << round << " of " << totalRounds << " ---"
             << RESET << "\n";
        cout << "  Score: " << GREEN << "You " << player.wins << RESET
             << " - " << RED << aiWins << " AI" << RESET << "\n";

        int playerMove = getPlayerMove();
        int aiMove     = getAIMove(difficulty, playerLastMove, player.moveCount);

        player.moveCount[playerMove]++;
        playerLastMove = playerMove;

        countdown();

        cout << "\n";
        cout << "   YOU              AI\n";
        cout << "  (" << moveName(playerMove) << ")      ("
             << moveName(aiMove) << ")\n\n";
        printMove(playerMove);
        cout << "\n";
        printMove(aiMove);
        cout << "\n";
        int result = determineWinner(playerMove, aiMove);
        if(result == 0) {
            cout << YELLOW << BOLD << "  == DRAW! ==\n" << RESET;
            player.draws++;
            player.currentStreak = 0;
            lossStreak = 0;
        } else if(result == 1) {
            cout << GREEN << BOLD << "  YOU WIN this round!\n" << RESET;
            player.wins++;
            player.currentStreak++;
            lossStreak = 0;
            if(player.currentStreak > player.bestStreak)
                player.bestStreak = player.currentStreak;
            printStreakMessage(player.currentStreak);
        } else {
            cout << RED << BOLD << "  AI wins this round!\n" << RESET;
            aiWins++;
            player.losses++;
            player.currentStreak = 0;
            lossStreak++;
            printLossStreakMessage(lossStreak);
        }

        pause(500);
    }

    // Final match result
    printLine("=");
    if(player.wins > aiWins)
        cout << GREEN  << BOLD << "\n  YOU WON THE MATCH! " << player.wins << " - " << aiWins << "\n" << RESET;
    else if(aiWins > player.wins)
        cout << RED    << BOLD << "\n  AI WON THE MATCH! "  << player.wins << " - " << aiWins << "\n" << RESET;
    else
        cout << YELLOW << BOLD << "\n  MATCH DRAW! "        << player.wins << " - " << aiWins << "\n" << RESET;
    printLine("=");

    showMatchStats(player, totalRounds);

    // Save to leaderboard using plain arrays
    ScoreEntry board[TOP_SCORES + 1];
    int count = readLeaderboard(board, TOP_SCORES); // read existing (max TOP_SCORES)
    // Append new entry
    board[count].name   = player.name;
    board[count].score  = player.wins;
    board[count].rounds = totalRounds;
    count++;
    saveLeaderboard(board, count);
    cout << GREEN << "  Score saved to leaderboard!\n" << RESET;
}

// Main 
int main() {
    srand((unsigned int)time(0));
    printBanner();

    string name;
    cout << BOLD << "  Enter your name: " << RESET;
    cin >> name;

    Player player;
    player.name         = name;
    player.wins         = 0;
    player.losses       = 0;
    player.draws        = 0;
    player.currentStreak = 0;
    player.bestStreak   = 0;
    player.moveCount[0] = player.moveCount[1] = player.moveCount[2] = 0;

    int choice;
    do {
        printBanner();
        cout << "  " << BOLD << "Welcome, " << CYAN << name << RESET << "!\n\n";
        cout << "  " << YELLOW << "[1]" << RESET << " Play a match\n";
        cout << "  " << YELLOW << "[2]" << RESET << " View leaderboard\n";
        cout << "  " << YELLOW << "[3]" << RESET << " How to play\n";
        cout << "  " << YELLOW << "[4]" << RESET << " Quit\n";
        cout << "\n  Your choice: ";
        cin >> choice;

        if(choice == 1) {
            // Difficulty
            int diff;
            cout << "\n" << BOLD << "  Choose AI difficulty:\n" << RESET;
            cout << "  " << GREEN  << "[1]" << RESET << " Easy   (random AI)\n";
            cout << "  " << YELLOW << "[2]" << RESET << " Medium (learns your pattern)\n";
            cout << "  " << RED    << "[3]" << RESET << " Hard   (predicts you)\n";
            cout << "  Enter (1/2/3): ";
            cin >> diff;
            if(diff < 1 || diff > 3) diff = 1;

            // Rounds
            int rounds;
            cout << "\n  Best of how many rounds? (e.g. 3, 5, 7): ";
            cin >> rounds;
            if(rounds < 1) rounds = 3;
            if(rounds % 2 == 0) {
                rounds++;
                cout << YELLOW << "  Adjusted to " << rounds
                     << " (odd number for a fair match)\n" << RESET;
                pause(800);
            }

            // Reset per-match counters
            player.wins          = 0;
            player.losses        = 0;
            player.draws         = 0;
            player.currentStreak = 0;
            player.moveCount[0]  = player.moveCount[1] = player.moveCount[2] = 0;

            playMatch(player, diff, rounds);

            cout << "\n  Press Enter to return to menu...";
            cin.ignore();
            cin.get();

        } else if(choice == 2) {
            clearScreen();
            showLeaderboard();
            cout << "\n  Press Enter to go back...";
            cin.ignore();
            cin.get();

        } else if(choice == 3) {
            clearScreen();
            printLine("=");
            cout << BOLD << CYAN << "          HOW TO PLAY\n" << RESET;
            printLine("=");
            cout << "  Rules:\n";
            cout << "   Rock     beats  Scissors\n";
            cout << "   Paper    beats  Rock\n";
            cout << "   Scissors beats  Paper\n\n";
            cout << "  Difficulty:\n";
            cout << GREEN  << "   Easy   " << RESET << "- AI picks randomly\n";
            cout << YELLOW << "   Medium " << RESET << "- AI counters your most used move\n";
            cout << RED    << "   Hard   " << RESET << "- AI predicts your pattern\n\n";
            cout << "  Streaks:\n";
            cout << "   Win 2+ in a row to unlock combo messages!\n\n";
            cout << "  Leaderboard:\n";
            cout << "   Top 5 scores are saved after every match.\n";
            printLine("=");
            cout << "\n  Press Enter to go back...";
            cin.ignore();
            cin.get();
        }

    } while(choice != 4);

    cout << "\n" << BOLD << CYAN
         << "  Thanks for playing, " << name << "! GG!\n" << RESET;
    cout << "  Coded with love for FAST NUCES - Sehar\n\n";

    return 0;
}