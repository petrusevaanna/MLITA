/* 11. Имеется  текстовый  файл  записей  с некоторым  ключевым
полем.  Выполнить в оперативной  памяти  турнирную  сортировку.
Показать этапы сортировки (9).*/
//Петрюсева Анна 
//https://habr.com/ru/companies/edison/articles/508646/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Record {
    string key;
    string data;
    bool isMax;

    Record(const string& line) {
        size_t spacePos = line.find(' ');
        if (spacePos != string::npos) {
            key = line.substr(0, spacePos);
            data = line;
        }
        else {
            key = line;
            data = line;
        }
        isMax = false;
    }

    Record(bool maxFlag = false) {
        if (maxFlag) {
            key = "MAX";
            data = "MAX_VALUE";
            isMax = true;
        }
    }

    bool operator<(const Record& other) const {
        if (isMax) return false;
        if (other.isMax) return true;
        return key < other.key;
    }
};

vector<Record> readRecordsFromFile(const string& filename) {
    vector<Record> records;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return records;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            records.push_back(Record(line));
        }
    }

    file.close();
    return records;
}

void printTableHeader(ofstream& outFile) {
    outFile << "+-------+----------------------+---------------------------------------------------+----------------------+" << endl;
    outFile << "| Шаг   | Победитель (ключ)   | Состояние дерева                                 | Действие             |" << endl;
    outFile << "+-------+----------------------+---------------------------------------------------+----------------------+" << endl;
}

void printTableRow(ofstream& outFile, int step, const string& winner, const string& treeState, const string& action) {
    outFile << "| " << setw(5) << step << " | "
        << setw(20) << left << winner.substr(0, 20) << right << " | "
        << setw(50) << left << treeState.substr(0, 49) << right << " | "
        << setw(20) << left << action.substr(0, 20) << right << " |" << endl;
}

void printTableFooter(ofstream& outFile) {
    outFile << "+-------+----------------------+---------------------------------------------------+----------------------+" << endl;
}

vector<Record> tournamentSort(const vector<Record>& arr, ofstream& outFile) { 
    int n = arr.size();
    if (n == 0) return {};

    vector<Record> workArr = arr;
    vector<int> tree(2 * n, -1);
    vector<Record> result;
    result.reserve(n);

    outFile << "\n" << string(70, '=') << endl;
    outFile << "ТУРНИРНАЯ СОРТИРОВКА - ЭТАПЫ ВЫПОЛНЕНИЯ" << endl;
    outFile << string(70, '=') << endl;

    outFile << "\nЭТАП 1: ИНИЦИАЛИЗАЦИЯ ЛИСТЬЕВ ДЕРЕВА" << endl;
    outFile << string(50, '-') << endl;
    outFile << left << setw(10) << "Индекс" << setw(20) << "Ключ" << "Полная запись" << endl;
    outFile << string(50, '-') << endl;

    for (int i = 0; i < n; i++) {
        tree[n + i] = i;
        outFile << setw(10) << i << setw(20) << workArr[i].key 
            << workArr[i].data << endl;
    }
    outFile << string(50, '-') << endl;

    outFile << "\nЭТАП 2: ПОСТРОЕНИЕ ДЕРЕВА ПОБЕДИТЕЛЕЙ" << endl;
    outFile << string(70, '-') << endl;
    outFile << left << setw(10) << "Узел" << setw(15) << "Левый потомок"
        << setw(15) << "Правый потомок" << setw(15) << "Победитель"
        << setw(20) << "Ключ победителя" << endl;
    outFile << string(70, '-') << endl;

    for (int i = n - 1; i > 0; i--) {
        int leftIdx = tree[2 * i];
        int rightIdx = tree[2 * i + 1];
        int winnerIdx = -1;
        string leftKey = (leftIdx != -1) ? workArr[leftIdx].key : "-";
        string rightKey = (rightIdx != -1) ? workArr[rightIdx].key : "-";

        if (leftIdx != -1 && rightIdx != -1) {
            winnerIdx = (workArr[leftIdx] < workArr[rightIdx]) ? leftIdx : rightIdx;
        }
        else if (leftIdx != -1) {
            winnerIdx = leftIdx;
        }
        else if (rightIdx != -1) {
            winnerIdx = rightIdx;
        }

        tree[i] = winnerIdx;
        string winnerKey = (winnerIdx != -1) ? workArr[winnerIdx].key : "-";

        outFile << setw(10) << i 
            << setw(15) << leftKey.substr(0, 12)
            << setw(15) << rightKey.substr(0, 12)
            << setw(15) << ((winnerIdx != -1) ? to_string(winnerIdx) : "-")
            << setw(20) << winnerKey.substr(0, 18) << endl;
    }
    outFile << string(70, '-') << endl;

    outFile << "\nЭТАП 3: ИЗВЛЕЧЕНИЕ ПОБЕДИТЕЛЕЙ" << endl;
    outFile << "Корень дерева: узел 1, индекс " << tree[1]
        << ", ключ '" << workArr[tree[1]].key << "'" << endl;
    outFile << string(70, '=') << endl;

    printTableHeader(outFile); 

    for (int step = 1; step <= n; step++) {
        int winnerIdx = tree[1];
        if (winnerIdx == -1) break;

        Record winner = workArr[winnerIdx];
        result.push_back(winner);

        string treeState = "Корень: " + workArr[tree[1]].key.substr(0, 15);
        if (tree.size() > 2) {
            int leftChild = tree[2];
            int rightChild = tree[3];
            if (leftChild != -1) treeState += " Л: " + workArr[leftChild].key.substr(0, 12);
            if (rightChild != -1) treeState += " П: " + workArr[rightChild].key.substr(0, 12);
        }

        string action = "Извлекли '" + winner.key + "' (" + to_string(winnerIdx) + ")";

        printTableRow(outFile, step, winner.key, treeState, action); 

        workArr[winnerIdx] = Record(true);

        int leafIdx = n + winnerIdx;
        for (int node = leafIdx; node > 1; node /= 2) {
            int parent = node / 2;
            int leftChild = 2 * parent;
            int rightChild = 2 * parent + 1;

            int leftIdx = tree[leftChild];
            int rightIdx = tree[rightChild];

            if (leftIdx != -1 && rightIdx != -1) {
                tree[parent] = (workArr[leftIdx] < workArr[rightIdx]) ? leftIdx : rightIdx;
            }
            else if (leftIdx != -1) {
                tree[parent] = leftIdx;
            }
            else if (rightIdx != -1) {
                tree[parent] = rightIdx;
            }
            else {
                tree[parent] = -1;
            }
        }

        if (step < n) {
            string nextTreeState = "Корень: " + workArr[tree[1]].key.substr(0, 15);
            if (tree.size() > 2) {
                int leftChild = tree[2];
                int rightChild = tree[3];
                if (leftChild != -1) nextTreeState += " Л: " + workArr[leftChild].key.substr(0, 12);
                if (rightChild != -1) nextTreeState += " П: " + workArr[rightChild].key.substr(0, 12);
            }

            printTableRow(outFile, step, "-> заменен на MAX", nextTreeState, "Обновили дерево"); 

            if (step < n - 1) {
                outFile << "|" << string(98, '-') << "|" << endl; 
            }
        }
    }

    printTableFooter(outFile);  

    return result;
}

int main() {
    setlocale(LC_ALL, "RU");
    string filename;
    cout << "Введите имя файла с данными: ";  
    cin >> filename;

    vector<Record> records = readRecordsFromFile(filename);

    if (records.empty()) {
        cout << "Файл пуст или не найден. Программа завершена." << endl; 
        return 1;
    }

    string outputFilename = "out_" + filename;
    ofstream outFile(outputFilename);

    if (!outFile.is_open()) {
        cerr << "Ошибка создания файла вывода: " << outputFilename << endl;
        return 1;
    }

    cout << "\nСортировка запущена. Результаты будут сохранены в файл: " << outputFilename << endl; 

    outFile << "\n" << string(60, '=') << endl;  
    outFile << "ИСХОДНЫЕ ДАННЫЕ (" << records.size() << " записей)" << endl;
    outFile << string(60, '=') << endl;
    outFile << left << setw(7) << "номер" << setw(20) << "Ключ" << "Полная запись" << endl;
    outFile << string(60, '-') << endl;

    for (size_t i = 0; i < records.size(); i++) {
        outFile << setw(5) << i << setw(20) << records[i].key 
            << records[i].data << endl;
    }
    outFile << string(60, '=') << endl << endl;

    vector<Record> sorted = tournamentSort(records, outFile); 

    outFile << "\n\n" << string(60, '=') << endl;  
    outFile << "РЕЗУЛЬТАТ СОРТИРОВКИ" << endl;
    outFile << string(60, '=') << endl;
    outFile << left << setw(7) << "номер" << setw(30) << "Ключ" << "Полная запись" << endl;
    outFile << string(60, '-') << endl;

    for (size_t i = 0; i < sorted.size(); i++) {
        outFile << setw(7) << i + 1 << setw(30) << sorted[i].key  
            << sorted[i].data << endl;
    }
    outFile << string(60, '=') << endl;

    bool isSorted = true;
    for (size_t i = 1; i < sorted.size(); i++) {
        if (sorted[i].key < sorted[i - 1].key) {
            isSorted = false;
            break;
        }
    }

    if (isSorted) {
        outFile << "\n Сортировка завершена успешно" << endl;
        cout << "\n Сортировка завершена успешно" << endl; 
    }
    else {
        outFile << "\n Ошибка сортировки" << endl;
        cout << "\n Ошибка сортировки" << endl; 
    }

    outFile.close();

    cout << "Результаты сохранены в файл: " << outputFilename << endl;  
    cout << "Отсортировано записей: " << sorted.size() << endl;

    return 0;
}