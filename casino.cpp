//Казино в день своего юбилея решило сделать подарок клиентам.Составляется список клиентов в порядке их прихода.Каждый клиент имеет общий баланс игры, 
//выраженный положительным или отрицательным целым числом.Казино устраивает общий фуршет, затраты на который устанавливаются следующим образом.
//По списку клиентов формируется соответствующий список балансов.
//Значения двух соседних элементов списка заменяются разностью предыдущего и последующего элементов до тех пор, 
//пока не останется единственное число, определяющее величину затрат на фуршет.Найти такую последовательность операций, 
//чтобы оставшееся число было максимальным.Определить этот максимум.
//Ввод из файла INPUT.TXT.В  первой  строке  указывается  количество  элементов  списка  N(3 ≤ N ≤ 105).
//Во второй строке через пробел задаются N элементов списка балансов A1, A2, …, AN(-104 ≤  Ai  ≤ 104).
//Вывод в файл OUTPUT.TXT.В первой строке выводится итоговое максимальное значение.Во
//второй строке указываются через пробел N - 1 целых чисел B1, B2, …, BN - 1, 
//определяющих последовательность операций.Выражение вычисляется следующим образом.
//Из баланса с номером B1 = k вычитается ближайший следующий баланс, который еще не участвовал в предшествующих операциях в качестве вычитаемого.
//Результат замещает баланс Ak.Затем подобная операция проводится со значением B2  и т.д.Если имеется несколько решений, вывести любое из них.
//Примеры
//Ввод 1          Ввод 2
//3               3
//- 5 3 2          2 - 6 - 3
//Вывод 1         Вывод 2
//- 6              11
//2 1             1 1
//Пояснение.В первом примере сначала производится операция 3 – 2 = 1, а затем(-5) – 1 = -6. 
//Во втором примере сначала производится операция 2 –(-6) = 8, а затем  8 –(-3) = 11.
//Среда выполнения Visual Studio Community 2022
//Петрюсева Анна ПС23

//источники:deepseak 
// промт: обработай третий смешанный случай на основе формул для последовательности положительных чисел и формулы для последовательности отрицательных чисел 
//положительные все
//а1 - а2 + а3 + ... + аn
//все отрицательные
//а1 - а2 - а3 - ... - аn

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

void BalanceAllPositive(vector<long long>& balances, int size, long long& maxCost, vector<int>& operations)
{
    maxCost = balances[0] - balances[1];
    for (int i = 2; i < size; i++)
    {
        maxCost += balances[i];
    }
    for (int i = 2; i < size; i++)
    {
        operations.push_back(2);
    }
    operations.push_back(1);
}

void BalanceAllNegative(vector<long long>& balances, int size, long long& maxCost, vector<int>& operations)
{
    maxCost = balances[0];
    for (int i = 1; i < size; i++)
    {
        maxCost -= balances[i];
    }
    for (int i = 0; i < size - 1; i++)
    {
        operations.push_back(1);
    }
}

void BalanceMixed(vector<long long>& balances, int size, long long& maxCost, vector<int>& operations)
{
    maxCost = balances[0] - balances[1];
    for (int i = 2; i < size; i++)
    {
        maxCost += abs(balances[i]);
    }
    for (int i = 2; i < size; i++)
    {
        if (balances[i] >= 0)
        {
            operations.push_back(2);
        }
        else
        {
            operations.push_back(1);
        }
    }
    operations.push_back(1);
}

void calculate(vector<long long>& balances, int size, long long& maxCost, vector<int>& operations)
{
    bool allPositive = true;
    bool allNegative = true;

    for (int i = 0; i < size; i++)
    {
        if (balances[i] > 0) allNegative = false;
        if (balances[i] < 0) allPositive = false;
    }

    if (allPositive)
    {
        BalanceAllPositive(balances, size, maxCost, operations);
    }
    else if (allNegative)
    {
        BalanceAllNegative(balances, size, maxCost, operations);
    }
    else
    {
        BalanceMixed(balances, size, maxCost, operations);
    }
}

void printExecutionTime(const steady_clock::time_point& start, const steady_clock::time_point& end)
{
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Время выполнения программы: " << duration.count() / 1000.0 << " мс" << endl;
    cout << "или " << duration.count() << " мкс" << endl;
}

int main()
{
    auto start = high_resolution_clock::now();

    ifstream inputFile("input15.txt");
    ofstream outputFile("output.txt");

    int elementCount;
    inputFile >> elementCount;
    vector<long long> balances(elementCount);
    for (int i = 0; i < elementCount; i++)
    {
        inputFile >> balances[i];
    }

    long long maximumCost;
    vector<int> operationSequence;

    calculate(balances, elementCount, maximumCost, operationSequence);

    outputFile << maximumCost << "\n";

    for (int i = 0; i < (int)operationSequence.size(); i++)
    {
        if (i > 0) outputFile << " ";
        outputFile << operationSequence[i];
    }
    outputFile << "\n";

    auto end = high_resolution_clock::now();
    printExecutionTime(start, end);

    return 0;
}