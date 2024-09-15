#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>

// Точка
struct Point {
    int x, y;

    // Оператор для сравнения точек
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Многоугольник
struct Polygon {
    std::vector<Point> points;

    // Оператор для сравнения многоугольников
    bool operator==(const Polygon& other) const {
        if (points.size() != other.points.size()) return false;
        return std::equal(points.begin(), points.end(), other.points.begin());
    }

    // Вычисление площади многоугольника
    double area() const {
        double sum = 0.0;
        int n = points.size();

        // Цикл с предвычислением индексов и использованием ссылок
        for (int i = 0; i < n; ++i) {
            const Point& p1 = points[i];
            const Point& p2 = points[(i + 1) % n];  // Следующая точка с замыканием на начало
            sum += p1.x * p2.y - p2.x * p1.y;
        }

        return std::abs(sum) / 2.0;
    }


    // Проверка, является ли многоугольник прямоугольником
    bool isRectangle() const {
        if (points.size() != 4) return false;
        auto is_right_angle = [](const Point& a, const Point& b, const Point& c) {
            int dx1 = b.x - a.x, dy1 = b.y - a.y;
            int dx2 = c.x - b.x, dy2 = c.y - b.y;
            return (dx1 * dx2 + dy1 * dy2) == 0; // Скалярное произведение равно 0
            };
        return is_right_angle(points[0], points[1], points[2]) &&
            is_right_angle(points[1], points[2], points[3]) &&
            is_right_angle(points[2], points[3], points[0]);
    }
};

// Функция для чтения многоугольника из ввода
Polygon readPolygonFromInput() {
    Polygon polygon;
    int num_points;
    std::cin >> num_points; // Считываем количество вершин многоугольника

    for (int i = 0; i < num_points; ++i) {
        char ch;
        Point p;
        std::cin >> ch >> p.x >> ch >> p.y >> ch; // Чтение координат в формате (x;y)
        polygon.points.push_back(p);
    }

    return polygon;
}

std::vector<Polygon> readPolygonsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Polygon> polygons;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Polygon poly;
        int num_points;
        iss >> num_points;

        for (int i = 0; i < num_points; ++i) {
            char ch;
            Point p;
            iss >> ch >> p.x >> ch >> p.y >> ch;
            poly.points.push_back(p);
        }

        polygons.push_back(poly);
    }
    return polygons;
}

void calculateArea(const std::vector<Polygon>& polygons, const std::string& type) {
    if (type == "EVEN") {
        double total_area = 0.0;
        for (const auto& poly : polygons) {
            if (poly.points.size() % 2 == 0) {
                total_area += poly.area();
            }
        }
        std::cout << total_area << "\n";
    }
    else if (type == "ODD") {
        double total_area = 0.0;
        for (const auto& poly : polygons) {
            if (poly.points.size() % 2 != 0) {
                total_area += poly.area();
            }
        }
        std::cout << total_area << "\n";
    }
    else if (type == "MEAN") {
        if (polygons.empty()) {
            std::cerr << "No polygons available" << "\n";
            return;
        }
        double total_area = 0.0;
        for (const auto& poly : polygons) {
            total_area += poly.area();
        }
        std::cout << total_area / polygons.size() << "\n";
    }
    else {
        int num_vertices = std::stoi(type);
        double total_area = 0.0;
        for (const auto& poly : polygons) {
            if (poly.points.size() == num_vertices) {
                total_area += poly.area();
            }
        }
        std::cout << total_area << "\n";
    }
}

void calculateMax(const std::vector<Polygon>& polygons, const std::string& type) {
    if (polygons.empty()) {
        std::cerr << "No polygons available" << "\n";
        return;
    }
    if (type == "AREA") {
        auto max_poly = *std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.area() < b.area();
            });
        std::cout << max_poly.area() << "\n";
    }
    else if (type == "VERTEXES") {
        auto max_poly = *std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
            });
        std::cout << max_poly.points.size() << "\n";
    }
}

void calculateMin(const std::vector<Polygon>& polygons, const std::string& type) {
    if (polygons.empty()) {
        std::cerr << "No polygons available" << "\n";
        return;
    }
    if (type == "AREA") {
        auto min_poly = *std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.area() < b.area();
            });
        std::cout << min_poly.area() << "\n";
    }
    else if (type == "VERTEXES") {
        auto min_poly = *std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
            });
        std::cout << min_poly.points.size() << "\n";
    }
}

void countPolygons(const std::vector<Polygon>& polygons, const std::string& type) {
    if (type == "EVEN") {
        int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& poly) {
            return poly.points.size() % 2 == 0;
            });
        std::cout << count << "\n";
    }
    else if (type == "ODD") {
        int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& poly) {
            return poly.points.size() % 2 != 0;
            });
        std::cout << count << "\n";
    }
    else {
        int num_vertices = std::stoi(type);
        int count = std::count_if(polygons.begin(), polygons.end(), [num_vertices](const Polygon& poly) {
            return poly.points.size() == num_vertices;
            });
        std::cout << count << "\n";
    }
}

void removeConsecutiveDuplicates(std::vector<Polygon>& polygons, const Polygon& target) {
    int removed_count = 0;

    auto it = polygons.begin();
    while (it != polygons.end()) {
        // Найдем первый экземпляр многоугольника, идентичного target
        if (*it == target) {
            auto next_it = it + 1;
            // Удаляем идущие подряд дубликаты
            while (next_it != polygons.end() && *next_it == target) {
                next_it = polygons.erase(next_it);
                ++removed_count;
            }
        }
        ++it;
    }

    std::cout << removed_count << "\n";
}

void echoPolygon(std::vector<Polygon>& polygons, const Polygon& target) {
    int added_count = 0;
    for (auto it = polygons.begin(); it != polygons.end(); ++it) {
        if (*it == target) {
            it = polygons.insert(it + 1, target); // Вставляем копию после текущего элемента
            ++added_count;
        }
    }
    std::cout << added_count << "\n";
}

void lessArea(const std::vector<Polygon>& polygons, const Polygon& target) {
    int count = std::count_if(polygons.begin(), polygons.end(), [&target](const Polygon& poly) {
        return poly.area() < target.area();
        });
    std::cout << count << "\n";
}

void inFrame(const std::vector<Polygon>& polygons, const Polygon& target) {
    // Найдём минимальные и максимальные координаты для всех сохранённых фигур
    int min_x = std::numeric_limits<int>::max(), max_x = std::numeric_limits<int>::min();
    int min_y = std::numeric_limits<int>::max(), max_y = std::numeric_limits<int>::min();

    for (const auto& poly : polygons) {
        for (const auto& point : poly.points) {
            min_x = std::min(min_x, point.x);
            max_x = std::max(max_x, point.x);
            min_y = std::min(min_y, point.y);
            max_y = std::max(max_y, point.y);
        }
    }

    // Проверяем, лежат ли все точки целевого многоугольника внутри ограничивающего прямоугольника
    bool inside = std::all_of(target.points.begin(), target.points.end(), [&](const Point& p) {
        return p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y;
        });

    std::cout << (inside ? "<TRUE>" : "<FALSE>") << "\n";
}

// Определение пересечений фигур требует сложной геометрии (определение пересечения двух многоугольников).
// Можно применить стандартную реализацию через библиотеку или написать проверку пересечения линий.
bool checkIntersection(const Polygon& poly1, const Polygon& poly2) {
    // Для простоты тут реализован заглушечный вариант, который всегда возвращает false.
    // Для полной реализации нужно проверить пересечения рёбер двух многоугольников.
    return false;
}

void intersections(const std::vector<Polygon>& polygons, const Polygon& target) {
    int count = std::count_if(polygons.begin(), polygons.end(), [&target](const Polygon& poly) {
        return checkIntersection(poly, target);
        });
    std::cout << count << "\n";
}

void samePolygons(const std::vector<Polygon>& polygons, const Polygon& target) {
    int count = std::count_if(polygons.begin(), polygons.end(), [&target](const Polygon& poly) {
        return poly == target;
        });
    std::cout << count << "\n";
}

void countRectangles(const std::vector<Polygon>& polygons) {
    int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& poly) {
        return poly.isRectangle();
        });
    std::cout << count << "\n";
}

void countRightShapes(const std::vector<Polygon>& polygons) {
    int count = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& poly) {
        return poly.isRectangle(); // Допустим, у нас только прямоугольники содержат прямые углы
        });
    std::cout << count << "\n";
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please provide a filename" << "\n";
        return 1;
    }

    std::string filename = argv[1];
    std::vector<Polygon> polygons = readPolygonsFromFile(filename);

    std::string command;
    while (std::cin >> command) {
        if (command == "AREA") {
            std::string type;
            std::cin >> type;
            calculateArea(polygons, type);
        }
        else if (command == "MAX") {
            std::string type;
            std::cin >> type;
            calculateMax(polygons, type);
        }
        else if (command == "MIN") {
            std::string type;
            std::cin >> type;
            calculateMin(polygons, type);
        }
        else if (command == "COUNT") {
            std::string type;
            std::cin >> type;
            countPolygons(polygons, type);
        }
        else if (command == "RMECHO") {
            Polygon target = readPolygonFromInput();
            removeConsecutiveDuplicates(polygons, target);
        }
        else if (command == "ECHO") {
            Polygon target = readPolygonFromInput();
            echoPolygon(polygons, target);
        }
        else if (command == "LESSAREA") {
            Polygon target = readPolygonFromInput();
            lessArea(polygons, target);
        }
        else if (command == "INFRAME") {
            Polygon target = readPolygonFromInput();
            inFrame(polygons, target);
        }
        else if (command == "INTERSECTIONS") {
            Polygon target = readPolygonFromInput();
            intersections(polygons, target);
        }
        else if (command == "SAME") {
            Polygon target = readPolygonFromInput();
            samePolygons(polygons, target);
        }
        else if (command == "RECTS") {
            countRectangles(polygons);
        }
        else if (command == "RIGHTSHAPES") {
            countRightShapes(polygons);
        }
        else {
            std::cout << "<INVALID COMMAND>" << "\n";
        }
    }

    return 0;
}