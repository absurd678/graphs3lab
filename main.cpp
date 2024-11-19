//  g++ -o pathfinding main.cpp
//  ./pathfinding random_heights.bin -o output.txt

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <ctime>
#include <string>

using namespace std;

// Структура для хранения координат
struct Node {
    int x, y;
    int cost; // Стоимость пути до этой клетки
    int heuristic; // Эвристическая оценка
    int totalCost; // Общая стоимость (cost + heuristic)

    // Оператор сравнения для приоритетной очереди
    bool operator>(const Node& other) const {
        return totalCost > other.totalCost;
    }
};


// Функция для чтения высот из бинарного файла
/*
vector<vector<int>> readHeights(const string& filename, int& width, int& height) {
    cout << "Зашли в readHeights" << endl;

    ifstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Cannot open file");
    }

    // Читаем ширину и высоту
    if (!file.read(reinterpret_cast<char*>(&width), sizeof(int))) {
        throw runtime_error("Error reading width");
    }
    if (!file.read(reinterpret_cast<char*>(&height), sizeof(int))) {
        throw runtime_error("Error reading height");
    }

    // Проверяем корректность значений
    if (width <= 0 || height <= 0) {
        throw runtime_error("Invalid width or height");
    }

    vector<vector<int>> heights(height, vector<int>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (!file.read(reinterpret_cast<char*>(&heights[i][j]), sizeof(int))) {
                throw runtime_error("Error reading height data");
            }
        }
    }

    cout << "end of reading" << endl;
    return heights;
}
*/
vector<vector<int>> readHeights(const string& filename, int& width, int& height) {
    cout << "Зашли в readHeights" << endl;

    ifstream file(filename);
    if (!file) {
        throw runtime_error("Cannot open file");
    }

    // Читаем ширину и высоту
    if (!(file >> width >> height)) {
        throw runtime_error("Error reading width and height");
    }

    // Проверяем корректность значений
    if (width <= 0 || height <= 0) {
        throw runtime_error("Invalid width or height");
    }

    vector<vector<int>> heights(height, vector<int>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (!(file >> heights[i][j])) {
                throw runtime_error("Error reading height data");
            }
        }
    }

    cout << "end of reading" << endl;
    return heights;
}

// Функция для записи результатов в текстовый файл
void writeResults(const string& filename, const string& algorithm, int pathLength, double percentVisited, double timeTaken, const vector<Node>& path) {
    
    cout << "Зашли в writeResults" << endl;
    
    ofstream outFile(filename, ios::app);
    if (!outFile) {
        throw runtime_error("Cannot open output file");
    }
    
    outFile << "Algorithm: " << algorithm << "\n";
    outFile << "Path Length: " << pathLength << "\n";
    outFile << "Percent Visited: " << percentVisited << "%\n";
    outFile << "Time Taken: " << timeTaken << " seconds \n";
    outFile << "Path: ";
    for (const auto& node : path) {
        outFile << "(" << node.x << ", " << node.y << ") ";
    }
    outFile << "\n\n";
}

// Эвристики
int chebyshevHeuristic(int x1, int y1, int x2, int y2) {
    return max(abs(x1 - x2), abs(y1 - y2));
}

int euclideanHeuristic(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

int manhattanHeuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Алгоритм Дейкстры
void dijkstra(const vector<vector<int>>& heights, int startX, int startY, int goalX, int goalY, vector<Node>& path) {

    cout << "Зашли в dijkstra" << endl;

    int height = heights.size();
    int width = heights[0].size();
    
    vector<vector<int>> cost(height, vector<int>(width, numeric_limits<int>::max()));
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    
    pq.push({startX, startY, 0, 0, 0});
    cost[startY][startX] = 0;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (visited[current.y][current.x]) continue;
        visited[current.y][current.x] = true;

        // Если достигли цели
        if (current.x == goalX && current.y == goalY) {
            // Восстановление пути
            path.push_back(current);
            break;
        }

        // Проверка соседей
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (abs(dy) == abs(dx)) continue; // Исключаем диагонали

                int newX = current.x + dx;
                int newY = current.y + dy;

                if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                    int newCost = current.cost + heights[newY][newX];
                    if (newCost < cost[newY][newX]) {
                        cost[newY][newX] = newCost;
                        pq.push({newX, newY, newCost, 0});
                    }
                }
            }
        }
        
        // Добавление текущего узла в путь
        path.push_back(current);
    }
}

// Алгоритм A*
void aStar(const vector<vector<int>>& heights, int startX, int startY, int goalX, int goalY, vector<Node>& path, int (*heuristic)(int, int, int, int)) {
    
    cout << "Зашли в aStar" << endl;
    
    int height = heights.size();
    int width = heights[0].size();

    vector<vector<int>> cost(height, vector<int>(width, numeric_limits<int>::max()));
    vector<vector<bool>> visited(height, vector<bool>(width, false));

    priority_queue<Node, vector<Node>, greater<Node>> pq;

    pq.push({startX, startY, 0, heuristic(startX, startY, goalX, goalY), 0});
    cost[startY][startX] = 0;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        if (visited[current.y][current.x]) continue;
        visited[current.y][current.x] = true;

        // Если достигли цели
        if (current.x == goalX && current.y == goalY) {
            // Восстановление пути
            path.push_back(current);
            break;
        }

        // Проверка соседей
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (abs(dy) == abs(dx)) continue; // Исключаем диагонали

                int newX = current.x + dx;
                int newY = current.y + dy;
                
                

                if (newX >= 0 && newX < width && newY >= 0 && newY < height && !visited[newY][newX]) {
                    int newCost = current.cost + heights[newY][newX];
                    if (newCost < cost[newY][newX]) {
                        cost[newY][newX] = newCost;
                        pq.push({newX, newY, newCost, heuristic(newX, newY, goalX, goalY)});
                    }
                }
            }
        }

        // Добавление текущего узла в путь
        path.push_back(current);
    }
}

// Функция для заполнения файла случайными значениями
void fillRandom(const string& filename, int width, int height) {

    cout << "Зашли в fillRandom" << endl;

    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Cannot open file for writing \n");
    }

    file.write(reinterpret_cast<const char*>(&width), sizeof(int));
    file.write(reinterpret_cast<const char*>(&height), sizeof(int));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int value = rand() % 100; // Случайные значения от 0 до 99
            file.write(reinterpret_cast<const char*>(&value), sizeof(int));
        }
    }
}

// Функция для ручного ввода значений
void fillManual(const string& filename, int width, int height) {

    cout << "Зашли в fillManual" << endl;

    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Cannot open file for writing \n");
    }

    file.write(reinterpret_cast<const char*>(&width), sizeof(int));
    file.write(reinterpret_cast<const char*>(&height), sizeof(int));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int value;
            cout << "Enter height for cell (" << i << ", " << j << "): ";
            cin >> value;
            file.write(reinterpret_cast<const char*>(&value), sizeof(int));
        }
    }
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file> [-o output_file]" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = "output.txt"; // По умолчанию

    // Обработка ключа -o
    for (int i = 2; i < argc; ++i) {
        if (string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }
    /*
    // Выбор метода заполнения
    cout << "Выберите метод заполнения:\n1. Случайные значения\n2. Ручной ввод\nВведите номер: ";
    int choice;
    cin >> choice;

    int width, height;
    if (choice == 1) {
        cout << "Введите ширину и высоту: ";
        cin >> width >> height;
        fillRandom(inputFile, width, height);
    } else if (choice == 2) {
        cout << "Введите ширину и высоту: ";
        cin >> width >> height;
        fillManual(inputFile, width, height);
    } else {
        cerr << "Некорректный выбор!" << endl;
        return 1;
    }
    */

    try {
        // Чтение высот из файла
        int width, height;
        vector<vector<int>> heights = readHeights(inputFile, width, height);

        // Пример вызова алгоритмов
        vector<Node> path;

        clock_t start = clock();
        dijkstra(heights, 0, 0, width - 1, height - 1, path);
        clock_t end = clock();

        double timeTakenDijkstra = double(end - start) / CLOCKS_PER_SEC;

        // Подсчет длины пути и процент просмотренных вершин
        int pathLengthDijkstra = path.size();
        double percentVisitedDijkstra = (pathLengthDijkstra / static_cast<double>(width * height)) * 100;

        // Запись результатов в файл
                writeResults(outputFile, "Dijkstra \n", pathLengthDijkstra, percentVisitedDijkstra, timeTakenDijkstra, path);

      // Очистка пути для A*
        path.clear();

        // A* с Манхэттенской эвристикой
        start = clock();
        aStar(heights, 0, 0, width - 1, height - 1, path, manhattanHeuristic);
        end = clock();

        double timeTakenAStarManhattan = double(end - start) / CLOCKS_PER_SEC;

        // Подсчет длины пути и процент просмотренных вершин для A*
        int pathLengthAStarManhattan = path.size();
        double percentVisitedAStarManhattan = (pathLengthAStarManhattan / static_cast<double>(width * height)) * 100;

        // Запись результатов в файл для A* с Манхэттенской эвристикой
        writeResults(outputFile, "A* (Manhattan)", pathLengthAStarManhattan, percentVisitedAStarManhattan, timeTakenAStarManhattan, path);

        // Очистка пути для A* с Евклидовой эвристикой
        path.clear();
        
        start = clock();
        aStar(heights, 0, 0, width - 1, height - 1, path, euclideanHeuristic);
        end = clock();

        double timeTakenAStarEuclidean = double(end - start) / CLOCKS_PER_SEC;

        int pathLengthAStarEuclidean = path.size();
        double percentVisitedAStarEuclidean = (pathLengthAStarEuclidean / static_cast<double>(width * height)) * 100;

        writeResults(outputFile, "A* (Euclidean)", pathLengthAStarEuclidean, percentVisitedAStarEuclidean, timeTakenAStarEuclidean, path);

        // Очистка пути для A* с Чебышёвой эвристикой
        path.clear();

        start = clock();
        aStar(heights, 0, 0, width - 1, height - 1, path, chebyshevHeuristic);
        end = clock();

        double timeTakenAStarChebyshev = double(end - start) / CLOCKS_PER_SEC;

        int pathLengthAStarChebyshev = path.size();
        double percentVisitedAStarChebyshev = (pathLengthAStarChebyshev / static_cast<double>(width * height)) * 100;

        writeResults(outputFile, "A* (Chebyshev)", pathLengthAStarChebyshev, percentVisitedAStarChebyshev, timeTakenAStarChebyshev, path);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
