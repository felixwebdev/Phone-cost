#include <iostream>
#include <vector>
#include <Windows.h>
#include <stdexcept>
using namespace std;

#define X "\033[33mX\033[0m"
#define O "\033[32mO\033[0m"


void Nocursortype()
{
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = FALSE;
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
        board = vector<string>(9, " ");
    }

    void printBoard() const {
        for (int i = 0; i < 9; i += 3) {
            cout << " " << board[i] << " | " << board[i + 1] << " | " << board[i + 2] << endl;
            if (i < 6) {
                cout << "---+---+---" << endl;
            }
        }
    }

    string getCell(int i) const {
        return board[i];
    }

    bool isFull() const {
        for (string cell : board) {
            if (cell == " ") return false;
        }
        return true;
    }

    void placeMark(int position, string mark) {
        if (position < 0 || position >= 9) {
            SET_COLOR(4);
            throw out_of_range("Invalid position!");
        }
        if (board[position] != " ") {
            SET_COLOR(4);
            throw invalid_argument("Position already taken!");
        }
        SET_COLOR(7);
        board[position] = mark;
    }

    string Winner() const {
        // check row
        for (int i = 0; i < 9; i += 3) {
            if (board[i] != " " && board[i] == board[i + 1] && board[i] == board[i + 2]) return board[i];
        }
        //check cell
        for (int i = 0; i < 3; ++i) {
            if (board[i] != " " && board[i] == board[i + 3] && board[i] == board[i + 6]) return board[i];
        }
        //check diagonal row 
        if (board[0] != " " && board[0] == board[4] && board[0] == board[8]) return board[0];
        if (board[2] != " " && board[2] == board[4] && board[2] == board[6]) return board[2];

        return " ";
    }
};

class Player {
private:
    string mark;
public:
    Player(string _mark = " ") { mark = _mark; }
    Player(const Player& p) { mark = p.mark; }
    virtual int Move(const Board& board) = 0;
    virtual string Identity() = 0;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(string m = " ") : Player(m) {}

    int Move(const Board& board) {
        int position;
        cout << "Enter your move (1-9): ";
        cin >> position;
        return position - 1;
    }

    string Identity() { return "You"; }
};

bool Run = true;

class ComputerPlayer : public Player {
private:
    int level;
    string playWith;
public:
    ComputerPlayer(string m = " ", int _level = 1, string _playWith = "Human") : Player(m) { 
        level = _level; 
        playWith = _playWith;
    }

    string Identity() { return "Computer"; }

    void setLevel(int n) { level = n; }

    int Move(const Board& board) {
        if (playWith == "Computer") Sleep(1000);
        //hard level 
        if (level == 3) {
            if (Run) {
                Run = false;
                if (board.getCell(4) == " ") return 4;
                if (board.getCell(0) == " ") return 0;
                if (board.getCell(2) == " ") return 2;
                if (board.getCell(6) == " ") return 6;
                if (board.getCell(8) == " ") return 8;
            }
            for (int i = 0; i < 9; i += 3) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i + 2) && board.getCell(i + 1) == " ") return i + 1;
            }
            for (int i = 0; i < 3; ++i) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i + 3) && board.getCell(i + 6) == " ") return i + 6;
            }
        }
        // normal level
        if (level >= 2) {
            // check row
            for (int i = 0; i < 9; i += 3) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i + 1) && board.getCell(i + 2) == " ") return i + 2;
            }
            // check reverse row
            for (int i = 8; i >= 0; i -= 3) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i - 1) && board.getCell(i - 2) == " ") return i - 2;
            }
            // check collum
            for (int i = 0; i < 3; ++i) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i + 3) && board.getCell(i + 6) == " ") return i + 6;
            }
            //check reverse collum
            for (int i = 8; i >= 6; --i) {
                if (board.getCell(i) != " " && board.getCell(i) == board.getCell(i - 3) && board.getCell(i - 6) == " ") return i - 6;
            }
            //check diagonal row
            if (board.getCell(0) != " " && board.getCell(0) == board.getCell(4) && board.getCell(8) == " ") return 8;
            if (board.getCell(8) != " " && board.getCell(8) == board.getCell(4) && board.getCell(0) == " ") return 0;
            if (board.getCell(2) != " " && board.getCell(2) == board.getCell(4) && board.getCell(6) == " ") return 6;
            if (board.getCell(6) != " " && board.getCell(6) == board.getCell(4) && board.getCell(2) == " ") return 2;
        }
        // easy level
        int tmp = rand() % 9;
        while (tmp < 0 || tmp >= 9 || board.getCell(tmp) != " ") { tmp = rand() % 9;}
        return tmp;
    }
};


class Game {
private:
    Board board;
    Player* player1;
    Player* player2;

public:
    Game(Player* p1, Player* p2) : player1(p1), player2(p2) {}

    void play() {
        Player* currentPlayer = player1;
        string currentMark = X;

        while (true) {
            board.printBoard();
            cout << "Your Turn: " << currentMark << "\n";
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
                    cout << currentPlayer->Identity() << " wins!" << endl;
                }
                else {
                    SET_COLOR(2);
                    cout << "Player " << currentMark;
                    SET_COLOR(2);
                    cout <<" wins!" << endl;
                }

                break;
            }
            else if (board.isFull()) {
                system("cls");
                board.printBoard();
                SET_COLOR(6);
                cout << "It's a draw!" << endl;
                break;
            }
            currentMark = (currentMark == X) ? O : X;
            currentPlayer = (currentPlayer == player1) ? player2 : player1;
        }
    }
};

void mainMenu() {
    cout << "             Welcome to Tic-Tac-Toe\n";
    cout << "-------------------------------------------------\n";
    cout << "1. Play with Computer\n";
    cout << "2. Human vs Human \n";
    cout << "3. Computer vs Computer \n";
    SET_COLOR(4);
    cout << "4. Exit \n";
    SET_COLOR(7);
    cout << "-------------------------------------------------\n";
    cout << "Your choice: ";
}


void subMenu() {
    cout << "             Welcome to Tic-Tac-Toe\n";
    cout << "-------------------------------------------------\n";
    SET_COLOR(2);
    cout << "1. Easy\n";
    SET_COLOR(6);
    cout << "2. Normal\n";
    SET_COLOR(12);
    cout << "3. Hard\n";
    SET_COLOR(7);
    cout << "-------------------------------------------------\n";
    cout << "Your choice: ";
}

int main() {
    int choice;
    int level = 1;
    Player* player1 = nullptr;;
    Player* player2 = nullptr;;

    mainMenu();
    cin >> choice;

    if (choice == 1) {
        system("cls");
        subMenu();
        cin >> level;
        player1 = new HumanPlayer(X);
        player2 = new ComputerPlayer(O, level);
        system("cls");
    }
    else if (choice == 2) {
        system("cls");
        player1 = new HumanPlayer(X);
        player2 = new HumanPlayer(O);
    }
    else if (choice == 3){
        srand(time(NULL));
        int randomNumber = rand() % 3 + 1;
        int randomNumber2 = 1;
        player1 = new ComputerPlayer(O,randomNumber, "Computer");
        while (randomNumber2 == randomNumber) {
            randomNumber2 = rand() % 3 + 1;
        }
        player2 = new ComputerPlayer(O, randomNumber2, "Computer");
        system("cls");
        Nocursortype();
    }
    else {
      return 0;
    }

    Game game(player1, player2);
    game.play();

    return 0;
}