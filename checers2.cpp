#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
const char WHITE_CHECKER = '2';
const char BLACK_CHECKER = '1';
const char EMPTY = '0';
const int BOARD_SIZE = 8;
const int DIRECTIONS[4][2] =
{
    {-1, -1},
    {-1, 1},
    {1, -1},
    {1, 1},
};

vector<string> board(BOARD_SIZE);
int whiteChRow = -1;
int whiteChCol = -1;
int maxCaptures = 0;

bool readBoard(const string& input, const string& output)
{
    ifstream f1(input);
    if (!f1.is_open())
    {
        cout << "Не получилось открыть файл " << input << endl;
        return false;
    }

    string line;
    int row = 0;
    whiteChRow = -1;
    whiteChCol = -1;

    while (getline(f1, line) && row < BOARD_SIZE)
    {
        if (line.length() >= BOARD_SIZE)
        {
            board[row] = line.substr(0, BOARD_SIZE);

            if (whiteChRow == -1)
            {
                for (int col = 0; col < BOARD_SIZE; col++)
                {
                    if (board[row][col] == WHITE_CHECKER)
                    {
                        whiteChRow = row;
                        whiteChCol = col;
                        break;
                    }
                }
            }
            row++;
        }
    }
    f1.close();

    if (row != BOARD_SIZE)
    {
        cout << "Неверный размер доски. Строк прочитано: " << row << endl;
        return false;
    }

    if (whiteChRow == -1)
    {
        cout << "Белая шашка не найдена на доске" << endl;
        return false;
    }

    return true;
}

bool findCaptures(int row, int col, int dirRow, int dirCol, const vector<string>& currentBoard)
{
    int blackRow = row + dirRow;
    int blackCol = col + dirCol;
    int newRow = row + 2 * dirRow;
    int newCol = col + 2 * dirCol;

    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE ||
        blackRow < 0 || blackRow >= BOARD_SIZE || blackCol < 0 || blackCol >= BOARD_SIZE ||
        newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE)
    {
        return false;
    }

    if (currentBoard[blackRow][blackCol] == BLACK_CHECKER && currentBoard[newRow][newCol] == EMPTY)
    {
        return true;
    }
    return false;
}

void findMaxCaptures(int row, int col, int captures, const vector<string>& currentBoard)
{
    if (captures > maxCaptures)
    {
        maxCaptures = captures;
    }

    for (int i = 0; i < 4; i++)
    {
        int dirRow = DIRECTIONS[i][0];
        int dirCol = DIRECTIONS[i][1];

        if (findCaptures(row, col, dirRow, dirCol, currentBoard))
        {
            int blackRow = row + dirRow;
            int blackCol = col + dirCol;
            int newRow = row + 2 * dirRow;
            int newCol = col + 2 * dirCol;

            vector<string> newBoard = currentBoard;

            newBoard[row][col] = EMPTY;
            newBoard[blackRow][blackCol] = EMPTY;
            newBoard[newRow][newCol] = WHITE_CHECKER;

            findMaxCaptures(newRow, newCol, captures + 1, newBoard);
        }
    }
}

int solve()
{
    if (!readBoard("input.txt", "output.txt"))
    {
        cout << "Ошибка при чтении доски" << endl;
        return -1;
    }

    maxCaptures = 0;

    findMaxCaptures(whiteChRow, whiteChCol, 0, board);

    ofstream f2("output.txt");
    if (f2.is_open())
    {
        f2 << maxCaptures;
        f2.close();
        cout << "Результат записан в output.txt " << endl;
    }
    else
    {
        cout << "Не удалось открыть output.txt для записи" << endl;
    }

    return maxCaptures;
}

int main()
{
    setlocale(LC_ALL, "RU");
    int result = solve();
    return 0;
}
