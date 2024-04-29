#include <iostream>  
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <stdexcept>
using namespace std;

#define X "\033[33mX\033[0m"
#define O "\033[32mO\033[0m"

string COLOR_RESET = "\033[0m";
string COLOR_BACKGROUND_GREEN = "\033[42m";
string COLOR_BACKGROUND_RED = "\033[41m";
string COLOR_BACKGROUND_BLUE = "\033[44m";

void print(string& colorCode, int width) {
    cout << colorCode;
    for (int i = 0; i < width; ++i) {
        cout << " ";
    }
    cout << COLOR_RESET;
}

void hideCursor(bool check = FALSE)
{
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = check;
    Info.dwSize = 20;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

void SET_COLOR(int color)
{
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        wColor = (csbi.wAttributes & 0xF0) + (color & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
}

class Board {
private:
    vector<string> board;

public:
    Board() {
        board = vector<string>(9) = {"1","2","3","4","5","6","7","8","9"};
    }

    ~Board() {} 

    void printBoard() const {
        print(COLOR_BACKGROUND_GREEN, 15);
        cout << endl;
        for (int i = 0; i < 9; i += 3) {
            print(COLOR_BACKGROUND_GREEN, 2);
            cout << " " << board[i] << " ";
            print(COLOR_BACKGROUND_GREEN, 1);
            cout << " " << board[i + 1] << " ";
            print(COLOR_BACKGROUND_GREEN, 1);
            cout << " " << board[i + 2] << " ";
            print(COLOR_BACKGROUND_GREEN, 2);
            cout << endl;
            if (i < 7) {
                print(COLOR_BACKGROUND_GREEN, 15);
                cout << endl;
            }
        }
    }

    bool isEmptyCell(const string s) const {
        return (s >= "1" && s <= "9");
    }

    string getCell(int i) const {
        return board[i];
    }

    bool isFull() const {
        for (string cell : board) {
            if (isEmptyCell(cell)) return false;
        }
        return true;
    }

    void placeMark(int pos, string mark) {
        if (pos < 0 || pos >= 9) {
            SET_COLOR(4);
            throw out_of_range("Invalid position!");
        }
        if (!isEmptyCell(board[pos])) {
            SET_COLOR(4);
            throw invalid_argument("Position already taken!");
        }
        SET_COLOR(7);
        board[pos] = mark;
    }

    string Winner() const {
        for (int i = 0; i < 9; i += 3) {
            if (!isEmptyCell(board[i]) && board[i] == board[i + 1] && board[i] == board[i + 2]) return board[i];
        }

        for (int i = 0; i < 3; ++i) {
            if (!isEmptyCell(board[i]) && board[i] == board[i + 3] && board[i] == board[i + 6]) return board[i];
        }

        if (!isEmptyCell(board[0]) && board[0] == board[4] && board[0] == board[8]) return board[0];
        if (!isEmptyCell(board[2]) && board[2] == board[4] && board[2] == board[6]) return board[2];

        return " ";
    }
};


class Player {
private:
    string mark;
public:
    Player(string _mark = " ") { mark = _mark; }
    Player(const Player& p)    { mark = p.mark; }
    ~Player() {}

    virtual int Move(const Board& board) = 0;
    virtual string Identity() = 0;
};

class Human : public Player {
public:
    Human(string m = " ") : Player(m) {}

    ~Human() {}

    int Move(const Board& board) {
        int pos;
        cout << "Enter your move (1-9): ";
        cin >> pos;
        return pos - 1;
    }
    string Identity() { return "You"; }
};

class Computer : public Player {
private:
    int level;
    string playWith;
public:
    Computer(string m = " ", int _level = 1, string _playWith = "Human") : Player(m) {
        level = _level;
        playWith = _playWith;
    }

    ~Computer() {}

    void setLevel(int _level) { level = _level;}
    int getLevel() { return level;         }
    void setPlayWith(string _playWith) { playWith = _playWith; }
    string setPlayWith(){ return playWith;      }

    string Identity() { return "Computer"; }

    bool Run = true;
    int Move(const Board& board) {
        if (playWith == "Computer") Sleep(1000);
        if (level == 3) {
            if (Run) {
                Run = false;
                if (board.isEmptyCell(board.getCell(4))) return 4;
                if (board.isEmptyCell(board.getCell(0))) return 0;
                if (board.isEmptyCell(board.getCell(2))) return 2;
                if (board.isEmptyCell(board.getCell(6))) return 6;
                if (board.isEmptyCell(board.getCell(8))) return 8;
            }
            for (int i = 0; i < 9; i += 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 2) && board.isEmptyCell(board.getCell(i+1))) return i + 1;
            }
            for (int i = 0; i < 3; ++i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 6) && board.isEmptyCell(board.getCell(i+3))) return i + 3;
            }
        }

        if (level >= 2) {

            for (int i = 0; i < 9; i += 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 1) && board.isEmptyCell(board.getCell(i+2))) return i + 2;
            }
            for (int i = 8; i >= 0; i -= 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i - 1) && board.isEmptyCell(board.getCell(i-2))) return i - 2;
            }

            for (int i = 0; i < 3; ++i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 3) && board.isEmptyCell(board.getCell(i+6))) return i + 6;
            }
            for (int i = 8; i >= 6; --i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i - 3) && board.isEmptyCell(board.getCell(i-6))) return i - 6;
            }

            if (!board.isEmptyCell(board.getCell(0)) && board.getCell(0) == board.getCell(4) && board.isEmptyCell(board.getCell(8))) return 8;
            if (!board.isEmptyCell(board.getCell(8)) && board.getCell(8) == board.getCell(4) && board.isEmptyCell(board.getCell(0))) return 0;
            if (!board.isEmptyCell(board.getCell(2)) && board.getCell(2) == board.getCell(4) && board.isEmptyCell(board.getCell(6))) return 6;
            if (!board.isEmptyCell(board.getCell(6)) && board.getCell(6) == board.getCell(4) && board.isEmptyCell(board.getCell(2))) return 2;
        }

        int tmp = rand() % 9;
        while (tmp < 0 || tmp >= 9 || !board.isEmptyCell(board.getCell(tmp))) {
            tmp = rand() % 9;
        }
        return tmp;
    }
};

class Game {
private:
    Board board;      
    Player* player1;   
    Player* player2;   

public:
    Game(Player* p1 = NULL, Player* p2 = NULL) {
        player1 = p1;
        player2 = p2;
    }

    ~Game() {}

    void mainMenu() {
        SetConsoleTitle(L"Tic Tac Toe");
        cout << "+-----------------------------------------------+\n";
        cout << "|            Welcome to Tic-Tac-Toe             |\n";
        cout << "+-----------------------------------------------+\n";
        cout << "|            1. Play with Computer              |\n";
        cout << "|            2. Human vs Human                  |\n";
        cout << "|            3. Computer vs Computer            |\n";
        SET_COLOR(4);
        cout << "|            4. Exit                            |\n";
        SET_COLOR(7);
        cout << "+-----------------------------------------------+\n";
        cout << "Enter your choice: ";
    }

    void subMenu() {
        system("cls");
        SetConsoleTitle(L"Tic Tac Toe");
        cout << "+-----------------------------------------------+\n";
        cout << "|            Welcome to Tic-Tac-Toe             |\n";
        cout << "+-----------------------------------------------+\n";
        SET_COLOR(2);
        cout << "|            1. Easy                            |\n";
        SET_COLOR(6);
        cout << "|            2. Normal                          |\n";
        SET_COLOR(12);
        cout << "|            3. Hard                            |\n";
        SET_COLOR(7);
        cout << "+-----------------------------------------------+\n";
        cout << "Enter your choice: ";
    }

    void play() {
        Player* currentPlayer = player1;
        string currentMark = X;

        while (true) {
            board.printBoard();
            cout << "Your Turn: " << currentMark << "\n \n";
            int move = currentPlayer->Move(board);
            try {
                system("cls");
                board.placeMark(move, currentMark);
            }
            catch (const exception& e) {
                system("cls");
                cout << e.what() << endl;
                continue;
            }

            string winner = board.Winner();
            if (winner != " ") {
                system("cls");
                SET_COLOR(2);
                board.printBoard();
                if (player1->Identity() != player2->Identity()) {
                    SET_COLOR(2);
                    cout << currentPlayer->Identity() << " wins! \n" << endl;
                }
                else {
                    SET_COLOR(2);
                    cout << "Player " << currentMark;
                    SET_COLOR(2);
                    cout << " wins! \n" << endl;
                }
                SET_COLOR(7);
                system("pause");
                cout << "Press any key to continue.....";
                break;
            }
            else if (board.isFull()) {
                system("cls");
                board.printBoard();
                SET_COLOR(6);
                cout << "It's a draw! \n" << endl;
                SET_COLOR(7);
                system("pause");
                cout << "Press any key to continue.....";
                break;
            }
            currentMark = (currentMark == X) ? O : X;
            currentPlayer = (currentPlayer == player1) ? player2 : player1;
        }
    }

    void startGame() {
        while (true) {
            system("cls");
            mainMenu();
            int choice;
            int level = 1;
            Player* player1 = NULL;
            Player* player2 = NULL;

            cin >> choice;
            if (choice == 1) {
                subMenu();
                cin >> level;
                player1 = new Human(X);
                player2 = new Computer(O, level);
                system("cls");
            }
            else if (choice == 2) {
                system("cls");
                SetConsoleTitle(L"Tic Tac Toe");
                player1 = new Human(X);
                player2 = new Human(O);
            }
            else if (choice == 3) {
                srand(time(NULL));
                player1 = new Computer(O, rand() % 3 + 1, "Computer");
                player2 = new Computer(O, rand() % 3 + 1, "Computer");

                system("cls");
                hideCursor();
                SetConsoleTitle(L"Tic Tac Toe");              
            }
            else {
                break;
            }
            Game game(player1, player2);
            game.play();
            hideCursor(TRUE);
        }
    }
};

int main() {
    Game *game = new Game;
    game->startGame();
    return 0;
}
