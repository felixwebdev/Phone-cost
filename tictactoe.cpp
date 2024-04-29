#include <iostream>  
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <stdexcept>
using namespace std;

// Xác đinh hằng số X và O với kí tự X - Màu vàng và O - Màu xanh ( ANSI escape code)
#define X "\033[33mX\033[0m"
#define O "\033[32mO\033[0m"

// Khai báo các màu nền tương ứng sử dụng ANSI escape code
string COLOR_RESET = "\033[0m";
string COLOR_BACKGROUND_GREEN = "\033[42m";
string COLOR_BACKGROUND_RED = "\033[41m";
string COLOR_BACKGROUND_BLUE = "\033[44m";
// In ra dải màu nền
void print(string& colorCode, int width) {
    cout << colorCode;
    for (int i = 0; i < width; ++i) {
        cout << " ";
    }
    cout << COLOR_RESET;
}

// ẨN con trỏ trên màn hình console
void hideCursor(bool check = FALSE)
{
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = check;
    Info.dwSize = 20;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

// Đặt màu cho các kí tự xuất hiện trên màn hình console
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

// Lớp cơ sở Board của game
class Board {
private:
    vector<string> board;

public:
    // Hàm khởi tạo 
    Board() {
        board = vector<string>(9) = {"1","2","3","4","5","6","7","8","9"};
    }

    // Hàm hủy
    ~Board() {} 

    // Hàm in bảng
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

    // Hàm kiểm tra ô còn trống hay không
    bool isEmptyCell(const string s) const {
        return (s >= "1" && s <= "9");
    }

    // Hàm lấy giá trị từ một ô trong bảng
    string getCell(int i) const {
        return board[i];
    }

    // Hàm kiểm tra bảng đã hết ô trống hay chưa 
    bool isFull() const {
        for (string cell : board) {
            if (isEmptyCell(cell)) return false;
        }
        return true;
    }

    // Hàm điền kí tự vào bảng và xử lí ngoại lệ 
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

    //Hàm kiểm tra và trả về giá trị là dấu X hoặc O tuơng ứng với người chiến thắng
    string Winner() const {
        // Kiểm tra hàng
        for (int i = 0; i < 9; i += 3) {
            if (!isEmptyCell(board[i]) && board[i] == board[i + 1] && board[i] == board[i + 2]) return board[i];
        }
        // Kiểm tra cột 
        for (int i = 0; i < 3; ++i) {
            if (!isEmptyCell(board[i]) && board[i] == board[i + 3] && board[i] == board[i + 6]) return board[i];
        }
        // Kiểm tra đường chéo 
        if (!isEmptyCell(board[0]) && board[0] == board[4] && board[0] == board[8]) return board[0];
        if (!isEmptyCell(board[2]) && board[2] == board[4] && board[2] == board[6]) return board[2];

        // Nếu chưa có người chiến thắng thì trả về dấu khoảng trắng
        return " ";
    }
};

// Lớp cơ sở Player 
class Player {
private:
    string mark;
public:
    // Hàm khởi tạo
    Player(string _mark = " ") { mark = _mark; }
    Player(const Player& p)    { mark = p.mark; }

    // Hàm hủy 
    ~Player() {}

    // Hàm thuần ảo Move để trả về vị trí mà người chơi muốn đi
    virtual int Move(const Board& board) = 0;
    // Hàm thuần ảo Identity trả về danh tính người chơi
    virtual string Identity() = 0;
};

// Lớp Human thể hiện đối tượng là con người kế thừa từ lớp cơ sở Player 
class Human : public Player {
public:
    // Hàm khởi tạo
    Human(string m = " ") : Player(m) {}

    // Hàm hủy
    ~Human() {}
    // Định nghĩa lại hàm thuần ảo kế thừa từ lớp cơ sở
    int Move(const Board& board) {
        int pos;
        cout << "Enter your move (1-9): ";
        cin >> pos;
        return pos - 1;
    }
    // Định nghĩa lại hàm thuần ảo kế thừa từ lớp cơ sở
    string Identity() { return "You"; }
};

// lớp Computer thể hiện người chơi là máy tính kế thừa từ lớp Player
class Computer : public Player {
private:
    int level;
    string playWith;
public:
    // Hàm khởi tạo
    Computer(string m = " ", int _level = 1, string _playWith = "Human") : Player(m) {
        level = _level;
        playWith = _playWith;
    }

    // Hàm hủy
    ~Computer() {}

    // Phương thức getter - setter
    void setLevel(int _level) { level = _level;}
    int getLevel() { return level;         }
    void setPlayWith(string _playWith) { playWith = _playWith; }
    string setPlayWith(){ return playWith;      }

    // Định nghĩa lại hàm kế thừa từ lớp Player
    string Identity() { return "Computer"; }

    bool Run = true;
    // Định nghĩa lại hàm kế thừa từ lớp cơ sở
    int Move(const Board& board) {
        // Nếu đối thủ là máy tính thì đặt độ trễ là 1 giây để người dùng dễ quan sát trận đấu
        if (playWith == "Computer") Sleep(1000);
        // Độ khó cao nhất
        if (level == 3) {
            // Chặn giữa đường chéo
            if (Run) {
                Run = false;
                if (board.isEmptyCell(board.getCell(4))) return 4;
                if (board.isEmptyCell(board.getCell(0))) return 0;
                if (board.isEmptyCell(board.getCell(2))) return 2;
                if (board.isEmptyCell(board.getCell(6))) return 6;
                if (board.isEmptyCell(board.getCell(8))) return 8;
            }
            // Chặn giữa hàng 
            for (int i = 0; i < 9; i += 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 2) && board.isEmptyCell(board.getCell(i+1))) return i + 1;
            }
            // Chặn giữa cột
            for (int i = 0; i < 3; ++i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 6) && board.isEmptyCell(board.getCell(i+3))) return i + 3;
            }
        }

        // Độ khó bình thường
        if (level >= 2) {

            // Chặn hàng
            for (int i = 0; i < 9; i += 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 1) && board.isEmptyCell(board.getCell(i+2))) return i + 2;
            }
            for (int i = 8; i >= 0; i -= 3) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i - 1) && board.isEmptyCell(board.getCell(i-2))) return i - 2;
            }

            // Chặn cột
            for (int i = 0; i < 3; ++i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i + 3) && board.isEmptyCell(board.getCell(i+6))) return i + 6;
            }
            for (int i = 8; i >= 6; --i) {
                if (!board.isEmptyCell(board.getCell(i)) && board.getCell(i) == board.getCell(i - 3) && board.isEmptyCell(board.getCell(i-6))) return i - 6;
            }

            // Chặn đường chéo
            if (!board.isEmptyCell(board.getCell(0)) && board.getCell(0) == board.getCell(4) && board.isEmptyCell(board.getCell(8))) return 8;
            if (!board.isEmptyCell(board.getCell(8)) && board.getCell(8) == board.getCell(4) && board.isEmptyCell(board.getCell(0))) return 0;
            if (!board.isEmptyCell(board.getCell(2)) && board.getCell(2) == board.getCell(4) && board.isEmptyCell(board.getCell(6))) return 6;
            if (!board.isEmptyCell(board.getCell(6)) && board.getCell(6) == board.getCell(4) && board.isEmptyCell(board.getCell(2))) return 2;
        }

        // Cấp độ dễ nhất - Các nước đi đều là ngẫu nhiên
        int tmp = rand() % 9;
        while (tmp < 0 || tmp >= 9 || !board.isEmptyCell(board.getCell(tmp))) {
            tmp = rand() % 9;
        }
        return tmp;
    }
};

// Lớp cơ sở Game
class Game {
private:
    Board board;       // đại diện cho bảng trò chơi
    Player* player1;   // đại diện cho người chơi thứ nhất 
    Player* player2;   // đại diện cho người chơi thứ hai 

public:
    // Hàm khởi tạo
    Game(Player* p1 = NULL, Player* p2 = NULL) {
        player1 = p1;
        player2 = p2;
    }

    // Hàm hủy
    ~Game() {}

    // In Menu chính
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

    // In Menu phụ
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

    // Hàm thực thi trò chơi
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

            // Thực hiện kiểm tra và in ra kết quả của trò chơi
            string winner = board.Winner();
            // Chiến thắng 
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
            // Hòa
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

    // Hàm bắt đầu trò chơi
    void startGame() {
        while (true) {
            // In ra menu chính để chọn chế độ chơi
            system("cls");
            mainMenu();
            int choice;
            int level = 1;
            Player* player1 = NULL;
            Player* player2 = NULL;

            cin >> choice;
            // Người với máy tính
            if (choice == 1) {
                // In ra menu phụ và cho phép ngừời chơi chọn cấp độ
                subMenu();
                cin >> level;
                player1 = new Human(X);
                player2 = new Computer(O, level);
                system("cls");
            }
            // Hai người
            else if (choice == 2) {
                system("cls");
                SetConsoleTitle(L"Tic Tac Toe");
                player1 = new Human(X);
                player2 = new Human(O);
            }
            // Máy tính với máy tính
            else if (choice == 3) {
                // Thực hiện set cấp độ ngẫu nhiên cho hai máy
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
