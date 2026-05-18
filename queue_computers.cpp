// Петрюсева Анна ПС23

/*16 В   некотором   институте   приобретаемые    компьютеры
выделяются   различным   факультетам  поочередно.  В  пределах
факультетов имеются очереди из кафедр.  Факультет,  получивший
компьютер,  перемещается  в  конец очереди,  а соответствующая
кафедра   исключается   из   факультетской   очереди.    Вновь
организованные факультеты и кафедры занимают последние места в
соответствующих очередях.  Составить программу ведения очереди
на компьютеры (9)*/

/*1) организовать  ввод  данных  из  файла  в   понятной  для
пользователя форме;
   2) обеспечить   возможность   многократных   запросов   без
повторного запуска программы;
   3) при реализации в С++ не использовать контейнерные классы
для работы с линейными списками типа stack, queue и т. п.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
using namespace std;

struct QueueNode {
    string data;
    QueueNode* next;
};

struct FacultyNode {
    string name;
    QueueNode* departmentsFront;
    QueueNode* departmentsRear;
    FacultyNode* next;
};

struct SystemState {
    FacultyNode* facultiesFront;
    FacultyNode* facultiesRear;
};

void enqueue(QueueNode*& front, QueueNode*& rear, string value) {
    QueueNode* newNode = new QueueNode{ value, nullptr };
    if (rear == nullptr) {
        front = rear = newNode;
    }
    else {
        rear->next = newNode;
        rear = newNode;
    }
}

void dequeue(QueueNode*& front, QueueNode*& rear) {
    if (front == nullptr) return;
    QueueNode* temp = front;
    front = front->next;
    if (front == nullptr) rear = nullptr;
    delete temp;
}

string peek(QueueNode* front) {
    return (front == nullptr) ? "" : front->data;
}

bool isQueueEmpty(QueueNode* front) {
    return front == nullptr;
}

int countQueue(QueueNode* front) {
    int count = 0;
    QueueNode* current = front;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

void clearQueue(QueueNode*& front, QueueNode*& rear) {
    while (front != nullptr) {
        dequeue(front, rear);
    }
}

void addFaculty(SystemState& system, string facultyName) {
    FacultyNode* newFaculty = new FacultyNode{ facultyName, nullptr, nullptr, nullptr };

    if (system.facultiesRear == nullptr) {
        system.facultiesFront = system.facultiesRear = newFaculty;
    }
    else {
        system.facultiesRear->next = newFaculty;
        system.facultiesRear = newFaculty;
    }
}

FacultyNode* findFaculty(SystemState& system, string facultyName) {
    FacultyNode* current = system.facultiesFront;
    while (current != nullptr) {
        if (current->name == facultyName) return current;
        current = current->next;
    }
    return nullptr;
}

bool departmentExists(FacultyNode* faculty, string departmentName) {
    QueueNode* current = faculty->departmentsFront;
    while (current != nullptr) {
        if (current->data == departmentName) return true;
        current = current->next;
    }
    return false;
}

void addDepartmentToFaculty(FacultyNode* faculty, string departmentName) {
    enqueue(faculty->departmentsFront, faculty->departmentsRear, departmentName);
}

bool facultyHasDepartments(FacultyNode* faculty) {
    return faculty->departmentsFront != nullptr;
}

int countFacultyDepartments(FacultyNode* faculty) {
    return countQueue(faculty->departmentsFront);
}

void showFacultyDepartments(FacultyNode* faculty) {
    if (faculty->departmentsFront == nullptr) {
        cout << "Нет кафедр" << endl;
        return;
    }

    cout << "Кафедры: ";
    QueueNode* current = faculty->departmentsFront;
    while (current != nullptr) {
        cout << current->data << " ";
        current = current->next;
    }
    cout << endl;
}

void initializeSystem(SystemState& system) {
    system.facultiesFront = system.facultiesRear = nullptr;
}

void clearSystem(SystemState& system) {
    FacultyNode* current = system.facultiesFront;
    while (current != nullptr) {
        FacultyNode* next = current->next;
        clearQueue(current->departmentsFront, current->departmentsRear);
        delete current;
        current = next;
    }
    system.facultiesFront = system.facultiesRear = nullptr;
}

bool loadFromFile(SystemState& system, string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла: " << filename << endl;
        return false;
    }

    clearSystem(system);

    string line;
    FacultyNode* currentFaculty = nullptr;
    int loadedFaculties = 0;
    int loadedDepartments = 0;

    while (getline(file, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) continue;
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        if (line.empty()) continue;

        if (line.find("Факультет:") == 0) {
            string facultyName = line.substr(10);
            facultyName.erase(0, facultyName.find_first_not_of(" "));
            facultyName.erase(facultyName.find_last_not_of(" ") + 1);
            if (facultyName.empty()) {
                cout << "Пропущен факультет с пустым именем" << endl;
                continue;
            }
            if (findFaculty(system, facultyName) != nullptr) {
                cout << "Факультет '" << facultyName << "' уже существует, пропуск" << endl;
                continue;
            }

            addFaculty(system, facultyName);
            currentFaculty = system.facultiesRear;
            loadedFaculties++;
            cout << "Добавлен факультет: " << facultyName << endl;
        }
        else if (line.find("Кафедра:") == 0 && currentFaculty != nullptr) {
            string departmentName = line.substr(8);
            departmentName.erase(0, departmentName.find_first_not_of(" "));
            departmentName.erase(departmentName.find_last_not_of(" ") + 1);
            if (departmentName.empty()) {
                cout << "  Пропущена кафедра с пустым именем" << endl;
                continue;
            }
            if (departmentExists(currentFaculty, departmentName)) {
                cout << "  Кафедра '" << departmentName << "' уже существует на факультете '"
                    << currentFaculty->name << "', пропуск" << endl;
                continue;
            }

            addDepartmentToFaculty(currentFaculty, departmentName);
            loadedDepartments++;
            cout << "  Добавлена кафедра: " << departmentName << endl;
        }
    }

    file.close();
    cout << "Загружено: " << loadedFaculties << " факультетов, "
        << loadedDepartments << " кафедр" << endl;
    return true;
}

void addFacultyWithDepartments(SystemState& system) {
    string facultyName;
    while (true) {
        cout << "Название факультета: ";
        getline(cin, facultyName);
        size_t start = facultyName.find_first_not_of(" \t");
        if (start == string::npos) {
            cout << "Имя факультета не может быть пустым!" << endl;
            continue;
        }
        size_t end = facultyName.find_last_not_of(" \t");
        facultyName = facultyName.substr(start, end - start + 1);

        if (facultyName.empty()) {
            cout << "Имя факультета не может быть пустым!" << endl;
            continue;
        }
        if (findFaculty(system, facultyName) != nullptr) {
            cout << "Факультет с таким именем уже существует!" << endl;
            continue;
        }

        break;
    }

    addFaculty(system, facultyName);
    FacultyNode* newFaculty = system.facultiesRear;

    cout << "Вводите названия кафедр (пустая строка для завершения):" << endl;
    int departmentsAdded = 0;

    while (true) {
        string departmentName;
        cout << "Кафедра: ";
        getline(cin, departmentName);
        size_t start = departmentName.find_first_not_of(" \t");
        if (start == string::npos) {
            break;
        }
        size_t end = departmentName.find_last_not_of(" \t");
        departmentName = departmentName.substr(start, end - start + 1);

        if (departmentName.empty()) {
            break;
        }
        if (departmentExists(newFaculty, departmentName)) {
            cout << "Кафедра с таким именем уже существует на этом факультете!" << endl;
            continue;
        }

        addDepartmentToFaculty(newFaculty, departmentName);
        departmentsAdded++;
        cout << "Добавлена кафедра: " << departmentName << endl;
    }

    if (!facultyHasDepartments(newFaculty)) {
        cout << "Факультет не добавлен (нет кафедр)" << endl;
        if (system.facultiesFront == newFaculty) {
            system.facultiesFront = newFaculty->next;
        }
        else {
            FacultyNode* prev = system.facultiesFront;
            while (prev != nullptr && prev->next != newFaculty) {
                prev = prev->next;
            }
            if (prev != nullptr) prev->next = newFaculty->next;
        }
        if (system.facultiesRear == newFaculty) {
            system.facultiesRear = nullptr;
        }
        delete newFaculty;
    }
    else {
        cout << "Факультет '" << facultyName << "' добавлен в очередь ("
            << countFacultyDepartments(newFaculty) << " кафедр)" << endl;
    }
}

void addDepartmentToExistingFaculty(SystemState& system) {
    if (system.facultiesFront == nullptr) {
        cout << "Нет факультетов в системе!" << endl;
        return;
    }
    string facultyName;
    cout << "Введите название факультета: ";
    getline(cin, facultyName);

    size_t start = facultyName.find_first_not_of(" \t");
    if (start == string::npos) {
        cout << "Имя факультета не может быть пустым!" << endl;
        return;
    }
    size_t end = facultyName.find_last_not_of(" \t");
    facultyName = facultyName.substr(start, end - start + 1);

    FacultyNode* faculty = findFaculty(system, facultyName);
    if (faculty == nullptr) {
        cout << "Факультет '" << facultyName << "' не найден!" << endl;
        return;
    }

    cout << "Выбран факультет: " << faculty->name << endl;
    cout << "Текущие кафедры: ";
    showFacultyDepartments(faculty);

    cout << "Вводите названия кафедр для добавления (пустая строка для завершения):" << endl;
    int departmentsAdded = 0;

    while (true) {
        string departmentName;
        cout << "Новая кафедра: ";
        getline(cin, departmentName);

        size_t startDept = departmentName.find_first_not_of(" \t");
        if (startDept == string::npos) {
            break;
        }
        size_t endDept = departmentName.find_last_not_of(" \t");
        departmentName = departmentName.substr(startDept, endDept - startDept + 1);

        if (departmentName.empty()) {
            break;
        }
        if (departmentExists(faculty, departmentName)) {
            cout << "Кафедра '" << departmentName << "' уже существует на этом факультете!" << endl;
            continue;
        }

        addDepartmentToFaculty(faculty, departmentName);
        departmentsAdded++;
        cout << "Добавлена кафедра: " << departmentName << endl;
    }

    cout << "Добавлено " << departmentsAdded << " кафедр к факультету '"
        << faculty->name << "'. Всего кафедр: " << countFacultyDepartments(faculty) << endl;
}

void giveComputer(SystemState& system) {
    if (system.facultiesFront == nullptr) {
        cout << "Нет факультетов в очереди!" << endl;
        return;
    }

    FacultyNode* faculty = system.facultiesFront;
    string department = peek(faculty->departmentsFront);

    cout << "Выдан компьютер:" << endl;
    cout << "  Факультет: " << faculty->name << endl;
    cout << "  Кафедра: " << department << endl;

    dequeue(faculty->departmentsFront, faculty->departmentsRear);

    system.facultiesFront = faculty->next;
    if (system.facultiesFront == nullptr) {
        system.facultiesRear = nullptr;
    }

    if (facultyHasDepartments(faculty)) {
        faculty->next = nullptr;
        if (system.facultiesRear == nullptr) {
            system.facultiesFront = system.facultiesRear = faculty;
        }
        else {
            system.facultiesRear->next = faculty;
            system.facultiesRear = faculty;
        }
        cout << "Факультет перемещен в конец очереди" << endl;
    }
    else {
        cout << "Факультет удален из очереди" << endl;
        delete faculty;
    }
}

void showStatus(SystemState& system) {
    if (system.facultiesFront == nullptr) {
        cout << "Очередь пуста!" << endl;
        return;
    }

    cout << "\nТекущая очередь:" << endl;
    FacultyNode* current = system.facultiesFront;
    int number = 1;

    while (current != nullptr) {
        cout << number << ". " << current->name
            << " (" << countFacultyDepartments(current) << " кафедр)" << endl;
        showFacultyDepartments(current);
        current = current->next;
        number++;
    }
}

void saveToFile(SystemState& system, string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка создания файла: " << filename << endl;
        return;
    }

    FacultyNode* currentFaculty = system.facultiesFront;
    while (currentFaculty != nullptr) {
        file << "Факультет: " << currentFaculty->name << endl;

        QueueNode* currentDept = currentFaculty->departmentsFront;
        while (currentDept != nullptr) {
            file << "Кафедра: " << currentDept->data << endl;
            currentDept = currentDept->next;
        }

        currentFaculty = currentFaculty->next;
    }

    file.close();
    cout << "Данные сохранены в " << filename << endl;
}

void showMenu() {
    cout << "Система распределения компьютеров:" << endl;
    cout << "1. Загрузить из файла (с кодировкой ANSI)" << endl;
    cout << "2. Показать очередь" << endl;
    cout << "3. Выдать компьютер" << endl;
    cout << "4. Добавить факультет" << endl;
    cout << "5. Добавить кафедру к существующему факультету" << endl;  
    cout << "6. Сохранить в файл" << endl;
    cout << "7. Выход" << endl;
    cout << "Выбор: ";
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    SystemState system;
    initializeSystem(system);
    int choice;
    string filename;

    do {
        showMenu();

        if (!(cin >> choice)) {
            cout << "Ошибка ввода!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "Имя файла: ";
            getline(cin, filename);
            loadFromFile(system, filename);
            break;
        case 2:
            showStatus(system);
            break;
        case 3:
            giveComputer(system);
            break;
        case 4:
            addFacultyWithDepartments(system);
            break;
        case 5:
            addDepartmentToExistingFaculty(system);
            break;
        case 6:
            cout << "Имя файла: ";
            getline(cin, filename);
            saveToFile(system, filename);
            break;
        case 7:
            cout << "Выход..." << endl;
            break;
        default:
            cout << "Неверный выбор!" << endl;
        }

    } while (choice != 7);

    clearSystem(system);
    return 0;
}