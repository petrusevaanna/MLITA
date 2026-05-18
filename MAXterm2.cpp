/* В некоторых строках текстового файла имеются выражения,
состоящие   из   двух   целых   чисел,   разделенных    знаком
арифметической   операции ('+','-','*','/'). Первое  из  чисел
может быть отрицательным. В строке может содержаться несколько
выражений. Перед  выражением  и  после него  могут  находиться
произвольные символы. Требуется  выделить  строку,  в  которой
значение выражения максимально. Вывести найденное максимальное
значение (7).*/
// Петрюсева Анна ПС23

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int Calculator(int a, int b, char op, int& res) {
    switch (op) {
    case '+':
        res = a + b;
        return 0;
    case '-':
        res = a - b;
        return 0;
    case '*':
        res = a * b;
        return 0;
    case '/':
        if (b == 0) {
            return 1;
        }
        res = a / b;
        return 0;
    default:
        return 1;
    }
}

bool Operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool Digit(char c) {
    return c >= '0' && c <= '9';
}

int getStringLength(const string& str) {
    int length = 0;
    const int MAX_SAFE_LENGTH = 10000;
    while (length < MAX_SAFE_LENGTH) {
        if (str[length] == '\0') break;
        length++;
    }
    return length;
}

int stringPartToInt(const string& str, int start, int end) {
    int result = 0;
    int sign = 1;
    int i = start;

    if (start < end && str[start] == '-') {
        sign = -1;
        i = start + 1;
    }

    for (; i < end; i++) {
        if (Digit(str[i])) {
            result = result * 10 + (str[i] - '0');
        }
        else {
            break;
        }
    }

    return result * sign;
}

int findNumberStart(const string& str, int start, int length) {
    for (int i = start; i < length; i++) {
        if (Digit(str[i]) || (str[i] == '-' && i + 1 < length && Digit(str[i + 1]))) {
            return i;
        }
    }
    return -1;
}

int findNumberEnd(const string& str, int start, int length) {
    int i = start;
    if (i < length && str[i] == '-') {
        i++;
    }
    while (i < length && Digit(str[i])) {
        i++;
    }
    return i;
}

void findExpressionsInLine(const string& line, int& maxResult, string& maxExpression, bool& firstValid, bool& foundValid) {
    int length = getStringLength(line);
    if (length == 0) return;

    int pos = 0;
    while (pos < length) {
        int numStart = findNumberStart(line, pos, length);
        if (numStart == -1) break;

        int numEnd = findNumberEnd(line, numStart, length);
        int num1 = stringPartToInt(line, numStart, numEnd);

        int opPos = numEnd;
        while (opPos < length && line[opPos] == ' ') {
            opPos++;
        }

        if (opPos < length && Operator(line[opPos])) {
            char op = line[opPos];

            int num2Start = findNumberStart(line, opPos + 1, length);
            if (num2Start != -1) {
                int num2End = findNumberEnd(line, num2Start, length);
                int num2 = stringPartToInt(line, num2Start, num2End);

                bool validExpression = true;
                for (int i = numEnd; i < num2Start; i++) {
                    if (i != opPos && line[i] != ' ') {
                        validExpression = false;
                        break;
                    }
                }

                if (validExpression) {
                    int result;
                    if (Calculator(num1, num2, op, result) == 0) {
                        string expression = to_string(num1) + " " + op + " " + to_string(num2);

                        if (firstValid || result > maxResult) {
                            maxResult = result;
                            maxExpression = expression;
                            firstValid = false;
                            foundValid = true;
                        }
                    }
                }
            }
        }
        pos = numEnd + 1;
    }
}

void processFile(const string& inputFilename, int& maxResult, string& maxExpression, bool& foundValid) {
    ifstream file(inputFilename);
    if (!file.is_open()) {
        foundValid = false;
        return;
    }
    string currentLine;
    char c;
    maxResult = 0;
    maxExpression = "";
    foundValid = false;
    bool firstValid = true;

    while (file.get(c)) {
        if (c == '\n' || c == '\r') {
            if (!currentLine.empty()) {
                findExpressionsInLine(currentLine, maxResult, maxExpression, firstValid, foundValid);
            }
            currentLine.clear();
        }
        else {
            currentLine += c;
        }
    }

    if (!currentLine.empty()) {
        findExpressionsInLine(currentLine, maxResult, maxExpression, firstValid, foundValid);
    }

    file.close();
}

void writeResultToFile(const string& outputFilename, int maxResult, const string& maxExpression, bool foundValid) {
    ofstream outFile(outputFilename);
    if (!outFile.is_open()) {
        cout << "Не удалось создать выходной файл." << endl;
        return;
    }

    if (!foundValid) {
        outFile << "Нет корректных выражений" << endl;
    }
    else {
        outFile << "Максимальный результат: " << maxResult << endl;
        outFile << "Выражение: " << maxExpression << " = " << maxResult << endl;
    }

    outFile.close();
    cout << "Результат записан в файл: " << outputFilename << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    string inputFilename, outputFilename;

    cout << "Введите имя входного файла: ";
    cin >> inputFilename;

    cout << "Введите имя выходного файла: ";
    cin >> outputFilename;

    int maxResult = 0;
    string maxExpression;
    bool foundValid = false;

    processFile(inputFilename, maxResult, maxExpression, foundValid);

    if (!foundValid) {
        ifstream testFile(inputFilename);
        if (!testFile.is_open()) {
            cout << "Не удалось открыть входной файл." << endl;
        }
        else {
            testFile.close();
            cout << "Нет корректных выражений" << endl;
        }
    }

    writeResultToFile(outputFilename, maxResult, maxExpression, foundValid);
    if (foundValid) {
        cout << "Максимальный результат: " << maxResult << endl;
        cout << "Выражение: " << maxExpression << " = " << maxResult << endl;
    }

    return 0;
}