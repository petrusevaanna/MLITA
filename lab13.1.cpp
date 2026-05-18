//13.1.Алгоритм Рабина - Карпа(6)
//Требуется найти все вхождения образца в текстовом файле методом Рабина - Карпа.
// В файле нет переноса слов.Образец может включать пробелы и переходить с одной строки файла на другую.
// Конец строки файла может интерпретироваться как пробел.Результаты поиска не должны зависеть от регистра букв, то есть каждая буква в образце и файле может быть как строчной, 
// так  и прописной.Файл не должен полностью загружаться в оперативную память.
//Ввод из файла INPUT.TXT.Первая строка файла является образцом и имеет длину от 1 до 255. Вторая строка задает имя текстового файла.
//Вывод в файл OUTPUT.TXT.Вывести в каждой строке через пробел последовательность номеров строк и позиций в строке, начиная с которых образец входит в текст.
// Нумерация строк и позиций в строке начинается с 1. Если вхождений нет, вывести No.
//Пример
//Ввод
//Мама мыла раму
//Bukvar.txt
//Вывод
//1 1
//1 23
//Пояснение.Файл Bukvar.txt состоит из следующих двух строк :
//Мама мыла раму вчера, мама
//мыла раму сегодня.

//Источники  https://habr.com/ru/articles/662678/ - Хабр Алгоритм Рабина — Карпа
//deepseak
// промт:unsigned char NormalizeChar(unsigned char c)
//код не может считать конец строки в файле с кодировкой ANSI что можно сделать
// {
//    if (c == '\n' || c == '\r') return (unsigned char)' ';
//    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
//    if (c >= 192 && c <= 223) return c + 32;
//    if (c >= 224 && c <= 255) return c;
//    return c;
//}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

const int BASE = 257;
const int MOD = 9973;

unsigned char NormalizeChar(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 'a';

    if (c >= 'a' && c <= 'z')
        return c;

    if (c >= 192 && c <= 223)
        return c + 32;

    if (c >= 224 && c <= 255)
        return c;

    if (c == 168)
        return 184;

    if (c == 184)
        return 184;

    return c;
}

vector<long long> PrecomputePowers(int maxLen)
{
    vector<long long> powers(maxLen + 1);
    powers[0] = 1;
    for (int i = 1; i <= maxLen; i++)
    {
        powers[i] = (powers[i - 1] * BASE) % MOD;
    }
    return powers;
}

char GetCharWithNewlineAsSpace(ifstream& file)
{
    char c;
    if (!file.get(c))
        return 0;

    if (c == '\r')
    {
        if (file.peek() == '\n')
            file.get();
        return ' ';
    }
    if (c == '\n')
        return ' ';
    return c;
}

void BuildLineInfo(const string& filename, vector<int>& lineStarts, vector<int>& lineLengths)
{
    ifstream lineFile(filename);
    if (!lineFile.is_open())
    {
        cerr << "Ошибка открытия файла для анализа строк: " << filename << endl;
        exit(1);
    }

    string line;
    int globalPos = 0;

    lineStarts.push_back(0);

    while (getline(lineFile, line))
    {
        lineLengths.push_back(line.length());
        globalPos += line.length() + 1;
        lineStarts.push_back(globalPos);
    }

    lineFile.close();
}

pair<int, int> FindPositionInOriginalFile(const string& filename, int logicalPos)
{
    ifstream file(filename);
    if (!file.is_open()) 
    {
        cerr << "Ошибка открытия файла: " << filename << endl;
        exit(1);

    }

    int lineNum = 1;
    int posInLine = 1;
    int currentLogicalPos = 0;

    while (currentLogicalPos < logicalPos && file.get())
    {
        file.unget();
        char c;
        file.get(c);

        if (c == '\r')
        {
            if (file.peek() == '\n')
                file.get();
            lineNum++;
            posInLine = 1;
            currentLogicalPos++;
        }
        else if (c == '\n')
        {
            lineNum++;
            posInLine = 1;
            currentLogicalPos++;
        }
        else
        {
            posInLine++;
            currentLogicalPos++;
        }
    }

    file.close();
    return { lineNum, posInLine };
}

bool IsMatch(ifstream& file, const string& pattern, int patternLen, int startLogicalPos)
{
    file.clear();
    file.seekg(0);

    int logicalPos = 0;

    while (logicalPos < startLogicalPos)
    {
        char c;
        if (!file.get(c))
            return false;

        if (c == '\r')
        {
            if (file.peek() == '\n')
                file.get();
            logicalPos++;
        }
        else if (c == '\n')
        {
            logicalPos++;
        }
        else
        {
            logicalPos++;
        }
    }

    for (int i = 0; i < patternLen; i++)
    {
        char c = GetCharWithNewlineAsSpace(file);
        if (c == 0)
            return false;

        if (NormalizeChar(c) != NormalizeChar(pattern[i]))
            return false;
    }

    return true;
}

vector<pair<int, int>> FindMatches(const string& filename, const string& pattern)
{
    vector<pair<int, int>> result;
    int m = pattern.length();

    if (m == 0) return result;

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Ошибка открытия файла: " << filename << endl;
        exit(1);
    }

    auto powers = PrecomputePowers(m);

    long long pHash = 0;
    for (int i = 0; i < m; i++)
    {
        pHash = (pHash * BASE + NormalizeChar(pattern[i])) % MOD;
    }

    vector<char> window(m);
    long long hash = 0;

    file.clear();
    file.seekg(0);

    for (int i = 0; i < m; i++)
    {
        char c = GetCharWithNewlineAsSpace(file);
        if (c == 0)
        {
            file.close();
            return result;
        }
        window[i] = c;
        hash = (hash * BASE + NormalizeChar(c)) % MOD;
    }

    long long maxPow = powers[m - 1];
    int logicalPos = 0;

    if (hash == pHash)
    {
        ifstream checkFile(filename);
        if (IsMatch(checkFile, pattern, m, logicalPos))
        {
            result.push_back(FindPositionInOriginalFile(filename, logicalPos));
        }
        checkFile.close();
    }

    while (true)
    {
        char nextChar = GetCharWithNewlineAsSpace(file);
        if (nextChar == 0)
            break;

        unsigned char oldChar = NormalizeChar(window[0]);
        unsigned char newChar = NormalizeChar(nextChar);
        hash = ((hash - (oldChar * maxPow) % MOD + MOD) % MOD * BASE + newChar) % MOD;

        for (int i = 0; i < m - 1; i++)
        {
            window[i] = window[i + 1];
        }
        window[m - 1] = nextChar;

        logicalPos++;

        if (hash == pHash)
        {
            ifstream checkFile(filename);
            if (IsMatch(checkFile, pattern, m, logicalPos))
            {
                result.push_back(FindPositionInOriginalFile(filename, logicalPos));
            }
            checkFile.close();
        }
    }

    file.close();
    return result;
}

void WriteResults(const vector<pair<int, int>>& results)
{
    ofstream outputFile("output.txt");

    if (!outputFile.is_open())
    {
        cerr << "Ошибка открытия output.txt" << endl;
        return;
    }

    if (results.empty())
    {
        outputFile << "No" << endl;
    }
    else
    {
        for (const auto& pos : results)
        {
            outputFile << pos.first << " " << pos.second << endl;
        }
    }

    outputFile.close();
}

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        cerr << "Ошибка открытия input.txt" << endl;
        return 1;
    }

    string pattern;
    getline(inputFile, pattern);

    string textFilename;
    getline(inputFile, textFilename);

    inputFile.close();

    vector<pair<int, int>> results = FindMatches(textFilename, pattern);

    WriteResults(results);

    return 0;
}