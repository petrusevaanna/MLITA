/*14. Проект  научно-технической  программы  задан  с помощью
ориентированного графа. Вершина графа соответствует отдельному
исследованию,   а   дуги   показывают  очередность  выполнения
исследований (каждое  исследование  может  начаться  не  ранее
окончания   предшествующих   исследований).  Продолжительность
каждого исследования известна.  Задан конечный срок выполнения
проекта.  Требуется  определить  максимальный интервал времени
для проведения каждого исследования,  чтобы  проект  мог  быть
завершен к назначенному сроку (9).*/
// Петрюсева Анна ПС23
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

const int MAX_TASKS = 100;
int taskDurations[MAX_TASKS];
string taskNames[MAX_TASKS];
int taskDependencies[MAX_TASKS][MAX_TASKS];
int earliestStart[MAX_TASKS];
int latestStart[MAX_TASKS];
int maxPathToVertex[MAX_TASKS];
int maxPathFromVertex[MAX_TASKS];
int projectDeadline;
int taskCount;
bool projectValid = false;

void computeMaxPathToEachVertex() {
    for (int i = 0; i < taskCount; i++) {
        bool hasPredecessors = false;
        for (int j = 0; j < taskCount; j++) {
            if (taskDependencies[j][i]) {
                hasPredecessors = true;
                break;
            }
        }
        if (!hasPredecessors) {
            maxPathToVertex[i] = taskDurations[i];
        }
        else {
            maxPathToVertex[i] = 0;
        }
    }

    bool changed;
    do {
        changed = false;
        for (int i = 0; i < taskCount; i++) {
            int maxPredPath = -1;
            for (int j = 0; j < taskCount; j++) {
                if (taskDependencies[j][i] && maxPathToVertex[j] > maxPredPath) {
                    maxPredPath = maxPathToVertex[j];
                }
            }
            if (maxPredPath != -1) {
                int newPath = maxPredPath + taskDurations[i];
                if (newPath > maxPathToVertex[i]) {
                    maxPathToVertex[i] = newPath;
                    changed = true;
                }
            }
        }
    } while (changed);

    for (int i = 0; i < taskCount; i++) {
        earliestStart[i] = maxPathToVertex[i] - taskDurations[i];
    }
}

void computeMaxPathFromEachVertex() {
    for (int i = 0; i < taskCount; i++) {
        bool hasSuccessors = false;
        for (int j = 0; j < taskCount; j++) {
            if (taskDependencies[i][j]) {
                hasSuccessors = true;
                break;
            }
        }
        if (!hasSuccessors) {
            maxPathFromVertex[i] = taskDurations[i];
        }
        else {
            maxPathFromVertex[i] = 0;
        }
    }

    bool changed;
    do {
        changed = false;
        for (int i = taskCount - 1; i >= 0; i--) {
            int maxSuccPath = -1;
            for (int j = 0; j < taskCount; j++) {
                if (taskDependencies[i][j] && maxPathFromVertex[j] > maxSuccPath) {
                    maxSuccPath = maxPathFromVertex[j];
                }
            }
            if (maxSuccPath != -1) {
                int newPath = taskDurations[i] + maxSuccPath;
                if (newPath > maxPathFromVertex[i]) {
                    maxPathFromVertex[i] = newPath;
                    changed = true;
                }
            }
        }
    } while (changed);
}

int findMaximumProjectDuration() {
    int maxDuration = 0;
    for (int i = 0; i < taskCount; i++) {
        bool hasSuccessors = false;
        for (int j = 0; j < taskCount; j++) {
            if (taskDependencies[i][j]) {
                hasSuccessors = true;
                break;
            }
        }
        if (!hasSuccessors && maxPathToVertex[i] > maxDuration) {
            maxDuration = maxPathToVertex[i];
        }
    }
    return maxDuration;
}

void computeLatestStartTimes() {
    int maxDuration = findMaximumProjectDuration();
    int effectiveDeadline = max(projectDeadline, maxDuration);

    for (int i = 0; i < taskCount; i++) {
        latestStart[i] = effectiveDeadline - maxPathFromVertex[i];
        if (latestStart[i] < earliestStart[i]) {
            latestStart[i] = earliestStart[i];
        }
        if (latestStart[i] < 0) {
            latestStart[i] = 0;
        }
    }
}

void loadProjectData() {
    string filename;
    cout << "Введите имя файла с описанием проекта: ";
    getline(cin, filename);

    ifstream inputFile(filename);
    if (!inputFile) {
        cout << "Не удалось открыть файл\n";
        return;
    }

    taskCount = 0;
    projectValid = false;

    for (int i = 0; i < MAX_TASKS; i++) {
        taskDurations[i] = 0;
        taskNames[i] = "Исследование " + to_string(i + 1);
        earliestStart[i] = 0;
        latestStart[i] = 0;
        maxPathToVertex[i] = 0;
        maxPathFromVertex[i] = 0;
        for (int j = 0; j < MAX_TASKS; j++) {
            taskDependencies[i][j] = 0;
        }
    }

    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "TASK") {
            int id, duration;
            string name = "";
            if (ss >> id >> duration) {
                getline(ss, name);
                if (!name.empty()) name = name.substr(name.find_first_not_of(" \t"));
                if (name.empty()) name = "Исследование " + to_string(id);

                if (id > 0 && id <= MAX_TASKS) {
                    taskDurations[id - 1] = duration;
                    taskNames[id - 1] = name;
                    if (id > taskCount) taskCount = id;
                }
            }
        }
        else if (command == "PRECEDENCE") {
            int predecessor, successor;
            if (ss >> predecessor >> successor) {
                if (predecessor > 0 && predecessor <= MAX_TASKS &&
                    successor > 0 && successor <= MAX_TASKS) {
                    taskDependencies[predecessor - 1][successor - 1] = 1;
                }
            }
        }
        else if (command == "DEADLINE") {
            ss >> projectDeadline;
        }
    }

    inputFile.close();
    cout << "Загружено " << taskCount << " задач\n";
    cout << "Срок выполнения: " << projectDeadline << "\n";

    if (taskCount > 0) {
        computeMaxPathToEachVertex();
        computeMaxPathFromEachVertex();
        int maxDuration = findMaximumProjectDuration();

        if (projectDeadline < maxDuration) {
            cout << "\n!!! ОШИБКА: Заданный срок (" << projectDeadline
                << ") меньше максимально необходимого (" << maxDuration << ") !!!\n";
            cout << "Проект не может быть выполнен в заданный срок.\n";
            projectValid = false;
        }
        else {
            computeLatestStartTimes();
            cout << "Проект выполним. Максимальная продолжительность: " << maxDuration << "\n";
            projectValid = true;
        }
    }
}

void displayAllTasks() {
    if (taskCount == 0) {
        cout << "Данные не загружены.\n";
        return;
    }

    if (!projectValid) {
        cout << "\nНевозможно отобразить интервалы: проект невыполним в заданный срок!\n";
        return;
    }

    int maxDuration = findMaximumProjectDuration();

    cout << "\n--- МАКСИМАЛЬНЫЕ ИНТЕРВАЛЫ ВЫПОЛНЕНИЯ ИССЛЕДОВАНИЙ ---\n";
    cout << "Максимальная продолжительность проекта: " << maxDuration << "\n";
    cout << "Срок выполнения: " << projectDeadline << "\n";

    if (projectDeadline >= maxDuration) {
        cout << "Резерв времени: " << projectDeadline - maxDuration << " единиц\n";
    }

    cout << "\n----------------------------------------------------------------\n";
    cout << "номер\tНазвание\t\tНачало интервала\tКонец интервала\n";
    cout << "----------------------------------------------------------------\n";

    for (int i = 0; i < taskCount; i++) {
        cout << i + 1 << "\t";

        string displayName = taskNames[i];
        if (displayName.length() > 18) {
            displayName = displayName.substr(0, 15) + "...";
        }
        cout << displayName;

        if (displayName.length() < 8) cout << "\t\t\t";
        else if (displayName.length() < 16) cout << "\t\t";
        else cout << "\t";

        int lateFinish = latestStart[i] + taskDurations[i];
        cout << earliestStart[i] << "\t\t\t" << lateFinish << "\n";
    }

}

int main() {
    setlocale(LC_ALL, "Russian");

    while (true) {
        cout << "\n--- СИСТЕМА ПЛАНИРОВАНИЯ НАУЧНО-ТЕХНИЧЕСКОЙ ПРОГРАММЫ ---\n";
        cout << "1. Загрузить данные из файла\n";
        cout << "2. Показать временные интервалы\n";
        cout << "3. Выход\n";
        cout << "Выберите действие: ";

        string input;
        getline(cin, input);

        int choice;
        try {
            choice = stoi(input);
        }
        catch (...) {
            cout << "Ошибка: введите число от 1 до 3!\n";
            continue;
        }

        switch (choice) {
        case 1:
            loadProjectData();
            break;
        case 2:
            displayAllTasks();
            break;
        case 3:
            cout << "Работа завершена.\n";
            return 0;
        default:
            cout << "Неверный выбор. Введите число от 1 до 3.\n";
        }
    }

    return 0;
}