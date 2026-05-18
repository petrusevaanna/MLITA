//12.6.Жизнь на Марсе(7)
//При высадке на Марс было основано N поселений.
// Каждое из них равномерно расширялось во все стороны на L марсианских миль в месяц.
// Постепенно поселения начали сливаться друг с другом, получая общее название.Какое минимальное время с момента высадки потребуется для того, 
// чтобы на Марсе осталось не более K поселений ?
//Ввод из файла INPUT.TXT.В первой строке задаются через пробел  три целых положительных значения : начальное количество поселений N(1 ≤ N ≤ 1000), 
// число K(1 ≤ K ≤ 10, K < N) и скорость роста поселений L(1 ≤ L ≤ 100).
// Далее в следующих N строках содержатся через пробел целые координаты поселений Xi, Yi(-1000 ≤ Xi, Yi ≤ 1000) в марсианских милях.
// Вывод в файл OUTPUT.TXT.В единственной строке вывести с точностью до 3 знаков минимальное время в месяцах с момента высадки, 
// необходимое для того, чтобы в результате слияния осталось не более K поселений.
//    Пример
//    Ввод
//    3 2 1
//    - 1 1
//    2 1
//    2 5
//    Вывод
//    1.50
//Петрюсева Анна ПС23
//Среда выполнения Visual Studio Community 2022
//Источники: https://www.tutorialspoint.com/data_structures_algorithms/path_compression_union_by_rank.htm - сортировка по рангу 

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>

using namespace std;

struct SettlementPair
{
    int settlement1, settlement2;
    double distance;
    double meetingTime;
};

int findRoot(vector<int>& parent, int x)
{
    if (parent[x] != x)
    {
        parent[x] = findRoot(parent, parent[x]);
    }
    return parent[x];
}

bool UnionSets(vector<int>& parent, vector<int>& rank, int settlement1, int settlement2)
{
    int root1 = findRoot(parent, settlement1);
    int root2 = findRoot(parent, settlement2);

    if (root1 == root2)
    {
        return false;
    }

    if (rank[root1] < rank[root2])
    {
        parent[root1] = root2;
    }
    else if (rank[root2] < rank[root1])
    {
        parent[root2] = root1;
    }
    else
    {
        parent[root2] = root1;
        rank[root1]++;
    }
    return true;
}

vector<SettlementPair> CalculateMeetingTimes(const vector<double>& x, const vector<double>& y, double L)
{
    vector<SettlementPair> edges;
    int N = x.size();

    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            double dx = x[i] - x[j];
            double dy = y[i] - y[j];
            double distance = sqrt(dx * dx + dy * dy);
            double time = distance / (2.0 * L);
            edges.push_back({ i, j, distance, time });
        }
    }

    sort(edges.begin(), edges.end(), [](const SettlementPair& a, const SettlementPair& b)
        {
            return a.meetingTime < b.meetingTime;
        });

    return edges;
}

void InitDSU(vector<int>& parent, vector<int>& rank, int N)
{
    for (int i = 0; i < N; i++)
    {
        parent[i] = i;
        rank[i] = 0;
    }
}

double FindMinTimeToReduceClusters(const vector<SettlementPair>& edges, int N, int K)
{
    vector<int> parent(N);
    vector<int> rank(N);
    InitDSU(parent, rank, N);

    int currentClusters = N;
    double resultTime = 0.0;

    for (const SettlementPair& edge : edges)
    {
        if (UnionSets(parent, rank, edge.settlement1, edge.settlement2))
        {
            currentClusters--;
            resultTime = edge.meetingTime;

            if (currentClusters <= K)
            {
                return resultTime;
            }
        }
    }

    return resultTime;
}

int main()
{
    ifstream inputFile("input20.txt");
    ofstream outputFile("output.txt");

    int N, K;
    double L;

    inputFile >> N >> K >> L;

    vector<double> x(N), y(N);

    for (int i = 0; i < N; i++)
    {
        inputFile >> x[i] >> y[i];
    }

    inputFile.close();

    vector<SettlementPair> edges = CalculateMeetingTimes(x, y, L);

    double minTime = FindMinTimeToReduceClusters(edges, N, K);

    outputFile << fixed << setprecision(3) << minTime << endl;
    outputFile.close();

    return 0;
}