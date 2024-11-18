#include <iostream>
#include <vector>
#include <algorithm>

struct Location {
    int x; // Координата X
    int y; // Координата Y

    Location(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
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

int main() {
    PriorityQueue pq;

    // Пример использования
    Location loc1(1, 2);
    Location loc2(3, 4);
    Location loc3(5, 6);

    pq.put(&loc1, 2);
    pq.put(&loc2, 1);
    pq.put(&loc3, 3);

    while (!pq.isEmpty()) {
        Location* loc = pq.get();
        if (loc) {
            std::cout << "Location: (" << loc->x << ", " << loc->y << ")\n";
        }
    }

    return 0;
}
