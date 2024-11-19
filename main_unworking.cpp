#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

enum class HeuristicType{Manhattan, Chebyshev, Euclidean};

struct Location {
    int x; // Координата X
    int y; // Координата Y

    Location(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
    // Оператор сравнения для упрощения работы с приоритетами
    bool operator==(const Location& other) const {
        return x == other.x && y==other.y; 
    }
};

struct Node {
    Location* location; // Указатель на структуру Location
    int priority;       // Приоритет (вес ребра)

    Node(Location* loc, int prio) : location(loc), priority(prio) {}

    // Оператор сравнения для упрощения работы с приоритетами
    bool operator<(const Node& other) const {
        return priority > other.priority; // Чем меньше вес - тем выше приоритет
    }
};

class PriorityQueue {
private:
    std::vector<Node> heap; // Вектор для хранения элементов очереди

public:
    // Добавить новую вершину с заданным приоритетом
    void put(Location* loc, int priority) {
        Node newNode(loc, priority);
        heap.push_back(newNode); // Добавляем элемент в конец
        std::push_heap(heap.begin(), heap.end()); // Перестраиваем кучу
    }

    // Достать первую вершину (элемент с наивысшим приоритетом)
    Location* get() {
        if (heap.empty()) {
            return nullptr; // Если очередь пуста, возвращаем nullptr
        }
        std::pop_heap(heap.begin(), heap.end()); // Перемещаем наивысший элемент в конец
        Node topNode = heap.back(); // Сохраняем его
        heap.pop_back(); // Удаляем его из кучи
        return topNode.location; // Возвращаем указатель на Location
    }

    // Проверка на пустоту очереди
    bool isEmpty() const {
        return heap.empty();
    }
};

// Функция для получения соседей вершины current
std::vector<Location*> neighbours(int** Graph, int rows, int cols, Location* current, HeuristicType heuristic) {
    std::vector<Location*> result;
    // Определяем возможные направления для всех 8 соседей
    int directions[8][2] = {
        {-1, 0},  // вверх
        {1, 0},   // вниз
        {0, -1},  // влево
        {0, 1},   // вправо
        {-1, -1}, // верхний левый угол (диагональ)
        {-1, 1},  // верхний правый угол (диагональ)
        {1, -1},  // нижний левый угол (диагональ)
        {1, 1}    // нижний правый угол (диагональ)
    };

    // Определяем соседей в зависимости от выбранной эвристики
    if (heuristic == HeuristicType::Manhattan) {
        // Добавить соседей в соответствии с манхеттенской эвристикой
        
    } else if (heuristic == HeuristicType::Chebyshev || heuristic == HeuristicType::Euclidean) {
        // Добавить соседей в соответствии с чебышевской эвристикой || евклидовой
        // Проходим по всем направлениям
        for (int i = 0; i < 8; ++i) {
            int newX = current->x + directions[i][0];
            int newY = current->y + directions[i][1];

            // Проверяем границы
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) {
                result.emplace_back(newX, newY); // Добавляем соседа в результат
            }
        }
    }

    return result; // Возвращаем массив соседей
}

int cost(int** Graph, Location* current, Location* neighbor, HeuristicType heuristic) {
    if (heuristic == HeuristicType::Manhattan) {
        // Вычислить стоимость для манхеттенской эвристики
        return abs(current->x - neighbor->x) + abs(current->y - neighbor->y);
    } else if (heuristic == HeuristicType::Chebyshev) {
        // Вычислить стоимость для чебышевской эвристики
        return max(abs(current->x - neighbor->x), abs(current->y - neighbor->y));
    } else if (heuristic == HeuristicType::Euclidean) {
        // Вычислить стоимость для евклидовой эвристики
        return sqrt(pow(current->x - neighbor->x, 2) + pow(current->y - neighbor->y, 2));
    }
    return 1; // или другая стоимость по умолчанию
}


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

void diykstra(int **shortest, Location*** pred, int **Graph, Location* start, Location* dest,
 int size, HeuristicType heuristic) 
{    
    Location* w = start; // Вершина, путь к которой самый короткий
    // создать очерередь
    PriorityQueue frontier = PriorityQueue();
    frontier.put(start, Graph[start->x][start->y]);

    shortest[start->x][start->y] = 0;

    while (!frontier.isEmpty()) // не все 
    {
        Location* current = frontier.get();

        if (current == dest)
            break;

        vector<Location*> neighArray = neighbours(Graph, size, size, current, heuristic);
        for (const auto& neighbor : neighArray){
           
            int new_cost = shortest[current->x][current->y] + cost(Graph, current, neighbor, heuristic);
            if (new_cost < shortest[neighbor->x][neighbor->y]){

                shortest[neighbor->x][neighbor->y] = new_cost;
                frontier.put(neighbor, new_cost);
                pred[neighbor->x][neighbor->y] = current;
            
            }
        }
        
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
int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Russian");
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <имя входного файла> [-o <имя выходного файла>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *inputFilename = argv[1];
    const char *outputFilename = "output.txt";

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputFilename = argv[i + 1];
            break;
        }
    }

    int size;
    int** graph = readGraphFromFile(inputFilename, size);
    
    // Теперь size корректно инициализировано
    int** shortest = new int*[size];
    Location*** pred = new Location**[size];

    for (int i = 0; i < size; i++) {
        shortest[i] = new int[size];
        pred[i] = new Location*[size];

        for (int j = 0; j < size; j++) {
            cout << graph[i][j] << " ";
            shortest[i][j] = INT_MAX;
            pred[i][j] = new Location(-1, -1);
        }
        cout << endl;
    }
    
    Location source(0, 0);  // Начало
    Location dest(size - 1, size - 1); // Конечная точка

    diykstra(shortest, pred, graph, &source, &dest, size, HeuristicType::Manhattan); // Замените 0 на ваш тип эвристики

    cout << "shortest: " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << shortest[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "pred: " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (pred[i][j] != NULL) {
                cout << "(" << pred[i][j]->x << ", " << pred[i][j]->y << ") ";
            } else {
                cout << "NULL ";
            }
        }
        cout << endl;
    }

    // Освободите память (не забудьте сделать это)

    return EXIT_SUCCESS;
}