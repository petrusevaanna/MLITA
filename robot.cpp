/* Робот 1 (5). Робот находится на расстоянии X шагов от цели. За один ход он продвигается либо к цели, либо в противоположном направлении. Если он достигает цели, то немедленно останавливается. Найти количество способов достижения цели ровно за Y шагов.
Ввод из файла INPUT.TXT. В единственной строке находятся X и Y (1 ≤ X, Y ≤ 50).
Вывод в файл OUTPUT.TXT. В единственной строке вывести количество способов достижения цели.
Пример
Ввод 1     Ввод 2
3 3        3 5
Вывод 1    Вывод 2
1          3
*/
//Петрюсева Анна ПС23
// Среда выполнения Visual Studio Community 2022
//https://habr.com/ru/articles/777618/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

bool RobotMove(const string& inputFile)
{
    ifstream inFile(inputFile);

    if (!inFile.is_open())
    {
        cout << "Не получилось открыть файл " << inputFile << endl;
        return false;
    }

    int X, Y;
    inFile >> X >> Y;

    if (inFile.fail() || X < 1 || X > 50 || Y < 1 || Y > 50)
    {
        cout << "Ошибка: неверный формат входных данных" << endl;
        inFile.close();
        return false;
    }

    inFile.close();

    int maxPos = X + Y;

    vector<vector<long long>> dp(maxPos + 1, vector<long long>(Y + 1, 0));

    dp[X][0] = 1;

    for (int step = 1; step <= Y; step++)
    {
        for (int pos = 1; pos <= maxPos; pos++)
        {
            if (pos - 1 >= 1)
            {
                dp[pos][step] += dp[pos - 1][step - 1];
            }
            if (pos + 1 <= maxPos)
            {
                dp[pos][step] += dp[pos + 1][step - 1];
            }
        }

        dp[0][step] = dp[1][step - 1];
    }

    ofstream outFile("OUTPUT.TXT");
    if (!outFile.is_open())
    {
        cout << "Не получилось создать файл OUTPUT.TXT" << endl;
        return false;
    }

    if (dp[0][Y] == 0)
    {
        outFile << "0" << endl;
    }
    else
    {
        outFile << dp[0][Y] << endl;
    }

    outFile.close();

    return true;
}

int main(int argc, char* argv[])
{
    string inputFile = "input.txt";

    if (argc > 1)
    {
        inputFile = argv[1];
    }

    if (!RobotMove(inputFile))
        return 1;

    return 0;
}