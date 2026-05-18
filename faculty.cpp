/* Структура  некоторого института задана деревом. Сыновьям
корневой вершины соответствуют факультеты,  факультеты в  свою
очередь  делятся  на  кафедры,  которые  могут  иметь филиалы.
Листьям   дерева    соответствуют    преподаватели.    Выявить
преподавателей, ведущих занятия на трех и более кафедрах (9).
*/
/*1) вводить   исходное   дерево  из  файла  в  понятной  для
пользователя форме, а не с клавиатуры;
   2) по требованию прользователя показывать дерево на экране;
   3) обрабатывать  дерево  в  оперативной памяти,  а не путем
многократного обращения к файлу;
   4) обеспечить   возможность   многократных   запросов   без
повторного запуска программы.*/
// Петрюсева Анна ПС23
#include <iostream> 
#include <fstream>
#include <vector>  
#include <cstring>
#include <map>
#include <set>
#include <limits>

using namespace std;

const int DL = 100;

struct Tree
{
    char name[DL];
    int urov;
    Tree* fath;
    vector<Tree*> sons;
};

Tree* root = NULL;

int read_from_file(ifstream& F, Tree*& r)
{
    char buf[DL];
    int i, j, k, m, len;
    Tree* p, * q, * t;
    m = -1;
    t = NULL;
    r = NULL;
    bool has_root = false;
    bool skip_children = false;
    int skip_level = -1;

    while (F.getline(buf, DL))
    {
        len = strlen(buf);
        if (len == 0) continue;

        k = 0;
        while (buf[k] == '.') {
            k++;
        }

        if (t != NULL && k > m + 1) {
            skip_children = true;
            skip_level = k - 1;
            continue;
        }

        j = k;
        while (j < len && buf[j] == ' ') j++;

        if (j >= len) {
            skip_children = true;
            skip_level = k;
            continue;
        }

        bool has_valid_content = false;
        for (int idx = j; idx < len; idx++) {
            if (buf[idx] != ' ' && buf[idx] != '\t' && buf[idx] != '\r') {
                has_valid_content = true;
                break;
            }
        }

        if (!has_valid_content) {
            skip_children = true;
            skip_level = k;
            continue;
        }

        if (skip_children && k > skip_level) {
            continue;
        }
        else if (skip_children && k <= skip_level) {
            skip_children = false;
            skip_level = -1;
        }

        p = new Tree;
        i = 0;

        while (j < len && i < DL - 1) {
            p->name[i] = buf[j];
            i++;
            j++;
        }
        p->name[i] = '\0';

        if (strlen(p->name) == 0) {
            delete p;
            skip_children = true;
            skip_level = k;
            continue;
        }

        p->urov = k;
        p->fath = NULL;
        p->sons.clear();

        if (k == 0)
        {
            if (r != NULL) {
                delete p;
                return -1;
            }
            r = p;
            t = r;
            m = 0;
            has_root = true;
            continue;
        }

        if (!has_root) {
            delete p;
            return -2;
        }

        if (k > m)
        {
            if (t != NULL) {
                t->sons.push_back(p);
                p->fath = t;
            }
        }
        else if (k == m)
        {
            if (t != NULL && t->fath != NULL) {
                q = t->fath;
                q->sons.push_back(p);
                p->fath = q;
            }
        }
        else
        {
            if (t != NULL) {
                q = t;
                for (i = 0; i <= m - k; i++) {
                    if (q == NULL) break;
                    q = q->fath;
                }
                if (q != NULL) {
                    q->sons.push_back(p);
                    p->fath = q;
                }
            }
        }

        if (p->fath == NULL && k != 0) {
            delete p;
            return -1;
        }

        m = k;
        t = p;
        skip_children = false;
        skip_level = -1;
    }

    if (!has_root) {
        return -2;
    }

    return 0;
}

void delete_tree(Tree* node) {
    if (node == NULL) return;
    for (Tree* son : node->sons) {
        delete_tree(son);
    }
    delete node;
}

void print_tree(Tree* node, int level = 0) {
    if (node == NULL) return;

    for (int i = 0; i < level; i++) {
        cout << ".";
    }
    cout << node->name << endl;

    for (Tree* son : node->sons) {
        print_tree(son, level + 1);
    }
}

void collect_teachers_and_cathedras(Tree* node, map<string, set<string>>& teacher_cathedras) {
    if (node == NULL) return;
    if (node->sons.empty()) {
        string teacher_name = node->name;

        Tree* current = node->fath;
        while (current != NULL && current != root) {
            if (current->urov == 2) {
                teacher_cathedras[teacher_name].insert(current->name);
                break;
            }
            current = current->fath;
        }
    }

    for (Tree* son : node->sons) {
        collect_teachers_and_cathedras(son, teacher_cathedras);
    }
}

void find_teachers_on_3_cathedras(Tree* node, set<string>& result) {
    map<string, set<string>> teacher_cathedras;

    collect_teachers_and_cathedras(node, teacher_cathedras);

    for (const auto& pair : teacher_cathedras) {
        const string& teacher = pair.first;
        const set<string>& cathedras = pair.second;

        if (cathedras.size() >= 3) {
            result.insert(teacher);
        }
    }
}

void show_menu() {
    cout << "\nМЕНЮ" << endl;
    cout << "1. Показать структуру института" << endl;
    cout << "2. Найти преподавателей на 3+ кафедрах" << endl;
    cout << "3. Загрузить новую структуру из файла" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

void clear_input_buffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool load_new_structure() {
    string filename;
    cout << "Введите имя файла со структурой института: ";
    cin >> filename;
    clear_input_buffer();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return false;
    }
    if (root != NULL) {
        delete_tree(root);
        root = NULL;
    }

    int result = read_from_file(file, root);
    file.close();

    if (result == 0) {
        cout << "Структура института успешно загружена!" << endl;
        return true;
    }
    else if (result == -2) {
        cout << "Ошибка: в файле отсутствует корневой узел (уровень 0)!" << endl;
        cout << "Структура должна начинаться с элемента самого верхнего уровня." << endl;
        root = NULL;
        return false;
    }
    else {
        cout << "Ошибка при загрузке структуры!" << endl;
        root = NULL;
        return false;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    if (!load_new_structure()) {
        cout << "Программа завершена из-за ошибки загрузки структуры." << endl;
        return -1;
    }

    int choice;
    do {
        show_menu();

        if (!(cin >> choice)) {
            cout << "Ошибка! Пожалуйста, введите число от 1 до 4." << endl;
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

        switch (choice) {
        case 1: {
            cout << "\nСтруктура института:" << endl;
            if (root == NULL) {
                cout << "Структура не загружена или пуста!" << endl;
            }
            else {
                print_tree(root);
            }
            break;
        }
        case 2: {
            if (root == NULL) {
                cout << "Структура не загружена!" << endl;
                break;
            }
            set<string> teachers;
            find_teachers_on_3_cathedras(root, teachers);

            cout << "\nПреподаватели, работающие на 3+ кафедрах:" << endl;
            if (teachers.empty()) {
                cout << "Таких преподавателей не найдено." << endl;
            }
            else {
                int count = 1;
                for (const string& teacher : teachers) {
                    cout << count++ << ". " << teacher << endl;
                }
            }
            break;
        }
        case 3: {
            load_new_structure();
            break;
        }
        case 4:
            cout << "Выход из программы." << endl;
            break;
        default:
            cout << "Неверный выбор! Пожалуйста, введите число от 1 до 4." << endl;
        }
    } while (choice != 4);

    if (root != NULL) {
        delete_tree(root);
        root = NULL;
    }

    return 0;
}