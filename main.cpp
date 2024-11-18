#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>

using namespace std;

struct Location{
    int x;
    int y;
};

// Элемент списка
struct Open_set {
    Location* point = NULL;
    Open_set* next = NULL;
};


void fillUp(int* array, int Size, int value)
{
    for (int i = 0; i < Size; i++)
    {
        array[i] = value;
    } // for i
} // for fillUp
void print_arr(int* arr, int N) {
    for (int i = 0; i < N; i++) {
        cout << "\t"<< arr[i] << " ";
    }
    cout << endl;
}



// Чтение графа из бинарного файла
int** readGraphFromFile(const char *filename, int &size) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        perror("Не удалось открыть файл");
        exit(EXIT_FAILURE);
    }

    bool isBinary = false;
    string filenameStr(filename);
    if (filenameStr.find(".bl") != string::npos) {
        isBinary = true;
    }

    int width, height;
    if (isBinary) {
        file.read(reinterpret_cast<char *>(&width), sizeof(int));
        file.read(reinterpret_cast<char *>(&height), sizeof(int));
    } else {
        file >> width;
        file >> height;
    }
    size = width;
    cout << "width x height = %d x %d" << width << height << endl;

    int** graph = new int*[height];
    
    int capacity;
    for (int i = 0; i < width; i++) {
        graph[i] = new int[height];
        for (int j = 0; j < height; j++) {
            if (isBinary && file.read(reinterpret_cast<char *>(&capacity), sizeof(int))) {
                graph[i][j] = capacity; // Add edge
                
            } else {
                file >> capacity;
                graph[i][j] = capacity; // Add edge
                
            }
        }
    }

    file.close();
    return graph; // Return the created graph
}

// Ф У Н К Ц И И   Д Л Я   С П И С К А
void make(int a, Open_set*& end, Open_set*& head) //  Добавление нового элемента
{
    Open_set* ptr = new Open_set; // Новый элемент
    if (!head) head = ptr; // Если это первый элемент
    else { end->next = ptr; } // У конца появилось продолжение
    ptr->data = a;
    end = ptr; // Делаем этот элемент последним
    ptr->next = NULL; // Следующих элементов списка нет
} // make

int findElem(int a, Open_set* head) // Найти элемент в списке
{
    Open_set* ptr = head;
    int counter = 0;
    while (ptr)
    {
        if (ptr->data == a) return counter;
        ptr = ptr->next;
        counter++;
    }
    return -1;
} // findElem
void print(Open_set* head)  // Печать всего списка
{
    Open_set* ptr = head;
    if (!ptr) cout << "Список пуст";
    while (ptr)
    {
        cout << "\t" << ptr->data;  // Считывание данного
        ptr = ptr->next; // Переход к следующему
    }
    cout << endl;
} // print

int count_size(Open_set* head) // Посчитать размер списка
{
    int counter = 0;
    while (head)
    {
        counter++;
        head = head->next;
    } // while
    return counter;
} // counter_size

void Delete(Open_set*& head, int index) { // Удаление элемента по индексу
    Open_set* ptr = head;
    for (int i = 0; i < index - 1; i++) {
        ptr = ptr->next;
    } // for i
    if (index == 0) {
        head = ptr->next;
        delete ptr;
    } // if
    else
    {
        Open_set* ptr2 = ptr->next;
        ptr->next = ptr->next->next;
        delete ptr2;
    } // else
} // Delete

void delList(Open_set*& head) // Удаление всего списка
{
    Open_set* ptr;
    while (head) // Удаление, начинающееся с "головы"
    {
        ptr = head->next;
        delete head;
        head = ptr;
    } // while
    head = NULL;
} // delList

void diykstra(int *shortest, int *pred, int **Graph, Open_set*& head, Location* start, Location* dest,
 int size) 
{    
    Location* w = start; // Вершина, путь к которой самый короткий
    // Ставим дефолтные значения
    fillUp(pred, size, -1);
    fillUp(shortest, size, INT_MAX);
    shortest[start->x + start->y] = 0;

    while (head) // пока список Q не пуст
    {
        int min = INT_MAX;
        for (int j = 0; j < size; j++) { // Поиск вершины, имеющей самый короткий путь от последней, удаленной из Q
            if (shortest[j] < min && findElem(j, head)!=-1) { // Если путь до j самый короткий и вершина j есть в Q
                    min = shortest[j];
                    w = j;
            } // if
        } // for j

        for (int j = 0; j < size; j++) { // Ищем вершины, смежные w-й 
            if (Graph[w][j] > 0) {
                cout << w << " -> " << j << " ; ";
                //смежная вершина v и вес ребра (w, v)
                int v = j, weight = Graph[w][j];
                //Relax
                //путь (w, v) короче, чем записанный ранее в shortest -> обновить shortest и pred
                if (shortest[w] + weight < shortest[v]) {
                    shortest[v] = shortest[w] + weight;
                    pred[v] = w;
                    cout << "\nshortest: " << endl;
                    print_arr(shortest, size);
                    cout << "pred: " << endl;
                    print_arr(pred, size);
                    cout << "\n\n";
                } // if
            } // if
        } // for j
        int i = findElem(w, head);
        Delete(head, i);         // Удаление вершины w из Q
    } // while
} // void deikstra
/*
void a_star(double *g_array, double *f_array, int *pred, int **Graph, Open_set*& head, int start, int dest, int size,
 double (*heuristic)(int, int, int, int)) 
{    
    int w = start; // Вершина, путь к которой самый короткий
    // Ставим дефолтные значения
    fillUp(pred, size, -1);
    fillUp(g_array, size, INT_MAX);
    g_array[start] = 0;
    f_array[start] = g_array[start] + heuristic(start, start, dest, dest);

    while (head) // пока список Q не пуст
    {
        int min = INT_MAX;
        for (int j = 0; j < size; j++) { // Поиск вершины, имеющей самый короткий путь от последней, удаленной из Q
            if (g_array[j] < min && findElem(j, head)!=-1) { // Если путь до j самый короткий и вершина j есть в Q
                    min = g_array[j];
                    w = j;
            } // if
        } // for j

        for (int j = 0; j < size; j++) { // Ищем вершины, смежные w-й 
            if (Graph[w][j] > 0) {
                cout << w << " -> " << j << " ; ";
                //смежная вершина v и вес ребра (w, v)
                int v = j, weight = Graph[w][j];
                //Relax
                //путь (w, v) короче, чем записанный ранее в shortest -> обновить shortest и pred
                if (g_array[w] + weight < g_array[v]) {
                    g_array[v] = g_array[w] + weight;
                    pred[v] = w;
                    cout << "\nshortest: " << endl;
                    print_arr(g_array, size);
                    cout << "pred: " << endl;
                    print_arr(pred, size);
                    cout << "\n\n";
                } // if
            } // if
        } // for j
        int i = findElem(w, head);
        Delete(head, i);         // Удаление вершины w из Q
    } // while
} // void deikstra
*/

//
int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    if (argc < 2)
    {
        fprintf(stderr, "Использование: %s <имя входного файла> [-o <имя выходного файла>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *inputFilename = argv[1];
    const char *outputFilename = "output.txt";

    // Проверка на наличие параметра -o для имени выходного файла
    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            outputFilename = argv[i + 1];
            break;
        }
    }

    int source, dest, size;
    int** graph = readGraphFromFile(inputFilename, size);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            cout << graph[i][j] << " ";
        }
        cout << endl;
    }
    
    int* shortest = new int[size*size];
    int* pred = new int[size*size];
    source = 0;     // Начала ставим сами
    dest = size-1;

    Open_set* Qhead = NULL; Open_set* Qend = NULL;
    for (int i = 0; i < size; i++) make(i, Qend, Qhead);

    diykstra(shortest, pred, graph, Qhead, source, size);

    print_arr(shortest, size);
    cout<<endl;
    print_arr(pred, size);

    return EXIT_SUCCESS;
}



