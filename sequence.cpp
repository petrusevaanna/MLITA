/*10.2. Последовательность 2 (5)
Каждый член последовательности десятичных цифр d1, d2, d3..., начиная с четвёртого, вычисляется по формуле di  = (di-3 + di-2  + di-1) mod x. 
По заданным d1, d2, d3, x найти N-й член последовательности.
Ввод из файла INPUT.TXT. В первой строке находятся d1, d2, d3, x, разделённые пробелами, во второй - число N.
Вывод в файл OUTPUT.TXT. Вывести одну цифру - dN.
Ограничения: 1 ≤ N ≤ 1018, 2 ≤  x ≤ 10, время 1 с.
*/
//Петрюсева Анна ПС23
// Среда выполнения Visual Studio Community 2022

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <unordered_map>

using namespace std;

bool SearchSquencing(const string& inputFile)
{
    clock_t startTime = clock();

    ifstream inFile(inputFile);

    if (!inFile.is_open())
    {
        cout << "Не получилось открыть файл " << inputFile << endl;
        return false;
    }

    ofstream outFile("OUTPUT.TXT");
    if (!outFile.is_open())
    {
        cout << "Не получилось создать файл OUTPUT.TXT" << endl;
        return false;
    }

    int d1, d2, d3, x;
    long long N;

    inFile >> d1 >> d2 >> d3 >> x;
    inFile >> N;

    if (N <= 3)
    {
        int result = (N == 1) ? d1 : (N == 2) ? d2 : d3;
        outFile << result << endl;
        inFile.close();
        outFile.close();

        clock_t endTime = clock();
        double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;
        cout << elapsedTime << endl;

        return true;
    }

    vector<int> sequence;
    sequence.reserve(10000);
    sequence.push_back(d1);
    sequence.push_back(d2);
    sequence.push_back(d3);

    unordered_map<long long, long long> tripleMap;

    int maxTriples = x * x * x;
    long long cycleStart = -1;
    long long cycleLength = -1;
    int result = 0;

    for (long long i = 3; i <= N; i++)
    {
        int nextElement = (sequence[i - 3] + sequence[i - 2] + sequence[i - 1]) % x;
        sequence.push_back(nextElement);

        if (i == N)
        {
            result = nextElement;
            outFile << result << endl;
            inFile.close();
            outFile.close();

            clock_t endTime = clock();
            double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;
            cout << elapsedTime << endl;

            return true;
        }

        int a = sequence[i - 2];
        int b = sequence[i - 1];
        int c = nextElement;
        long long tripleCode = ((long long)a * x * x + b * x + c);

        auto it = tripleMap.find(tripleCode);
        if (it != tripleMap.end())
        {
            cycleStart = it->second;
            cycleLength = i - cycleStart;
            break;
        }

        tripleMap[tripleCode] = i;
    }

    if (cycleStart == -1)
    {
        result = sequence[N - 1];
        outFile << result << endl;
    }
    else
    {
        if (N - 1 <= cycleStart)
        {
            result = sequence[N - 1];
        }
        else
        {
            long long position = cycleStart + (N - 1 - cycleStart) % cycleLength;
            result = sequence[position];
        }
        outFile << result << endl;
    }

    inFile.close();
    outFile.close();

    clock_t endTime = clock();
    double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;
    cout << elapsedTime << endl;

    return true;
}

int main(int argc, char* argv[])
{
    string inputFile = "input.txt";

    if (argc > 1)
    {
        inputFile = argv[1];
    }

    SearchSquencing(inputFile);

    return 0;
}