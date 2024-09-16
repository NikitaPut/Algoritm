#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <map>
#include <functional>

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

        // Проверка на прямые углы
        bool right_angles = is_right_angle(points[0], points[1], points[2]) &&
            is_right_angle(points[1], points[2], points[3]) &&
            is_right_angle(points[2], points[3], points[0]);

        // Дополнительная проверка: длины противоположных сторон должны быть равны
        auto distance_squared = [](const Point& p1, const Point& p2) {
            int dx = p2.x - p1.x;
            int dy = p2.y - p1.y;
            return dx * dx + dy * dy; // Квадрат длины отрезка
            };

        bool equal_sides = (distance_squared(points[0], points[1]) == distance_squared(points[2], points[3])) &&
            (distance_squared(points[1], points[2]) == distance_squared(points[3], points[0]));

        return right_angles && equal_sides;
    }
};

// Функция для определения ориентации трёх точек (p, q, r)
int orientation(const Point& p, const Point& q, const Point& r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;  // Коллинеарны
    return (val > 0) ? 1 : 2; // По часовой или против часовой
}

// Функция для проверки, находится ли точка q на отрезке pr
bool onSegment(const Point& p, const Point& q, const Point& r) {
    return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
}

// Проверка пересечения двух отрезков p1q1 и p2q2
bool doIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) return true;

    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;
}

// Основная функция для проверки пересечения двух многоугольников
bool checkIntersection(const Polygon& poly1, const Polygon& poly2) {
    int n1 = poly1.points.size();
    int n2 = poly2.points.size();

    // Используем std::any_of для проверки пересечений ребер
    return std::any_of(poly1.points.begin(), poly1.points.end(), [&](const Point& p1) {
        int i = &p1 - &poly1.points[0];  // Индекс точки
        Point q1 = poly1.points[(i + 1) % n1];

        // Проверяем пересечение с каждым ребром второго многоугольника
        return std::any_of(poly2.points.begin(), poly2.points.end(), [&](const Point& p2) {
            int j = &p2 - &poly2.points[0];  // Индекс точки
            Point q2 = poly2.points[(j + 1) % n2];

            return doIntersect(p1, q1, p2, q2);  // Проверка пересечения двух ребер
            });
        });
}

// Функция для чтения многоугольника из ввода
Polygon readPolygonFromInput() {
    Polygon polygon;
    int num_points;
    std::cin >> num_points; // Считываем количество вершин многоугольника

    // Используем std::generate_n для генерации точек и добавления их в polygon.points
    std::generate_n(std::back_inserter(polygon.points), num_points, []() {
        Point p;
        char ch;
        std::cin >> ch >> p.x >> ch >> p.y >> ch; // Чтение координат в формате (x;y)
        return p;
        });

    return polygon;
}

std::vector<Polygon> readPolygonsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Polygon> polygons;

    std::istream_iterator<std::string> file_iter(file), end;

    // Преобразуем каждую строку в файл в многоугольник
    std::transform(file_iter, end, std::back_inserter(polygons), [](const std::string& line) {
        std::istringstream iss(line);
        Polygon poly;
        int num_points;
        iss >> num_points;

        // Преобразуем каждую пару координат в точку и добавляем в полигон
        std::generate_n(std::back_inserter(poly.points), num_points, [&iss]() {
            Point p;
            char ch;
            iss >> ch >> p.x >> ch >> p.y >> ch;
            return p;
            });

        return poly;
        });

    return polygons;
}

void calculateArea(const std::vector<Polygon>& polygons, const std::string& type) {
    double total_area = 0.0;
    int count = 0;

    if (type == "EVEN" || type == "ODD") {
        bool is_even = (type == "EVEN");

        // Используем std::accumulate для вычисления суммы площадей нужных многоугольников
        total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [is_even, &count](double sum, const Polygon& poly) {
                if ((poly.points.size() % 2 == 0) == is_even) {
                    ++count;
                    return sum + poly.area();
                }
                return sum;
            });
    }
    else if (type == "MEAN") {
        if (polygons.empty()) {
            std::cerr << "No polygons available\n";
            return;
        }

        // Используем std::accumulate для вычисления суммы площадей всех многоугольников
        total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double sum, const Polygon& poly) {
                return sum + poly.area();
            });

        std::cout << total_area / polygons.size() << "\n";
        return;
    }
    else {
        try {
            int num_vertices = std::stoi(type);

            // Используем std::accumulate для вычисления суммы площадей многоугольников с заданным количеством вершин
            total_area = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                [num_vertices, &count](double sum, const Polygon& poly) {
                    if (poly.points.size() == num_vertices) {
                        ++count;
                        return sum + poly.area();
                    }
                    return sum;
                });
        }
        catch (const std::invalid_argument&) {
            std::cerr << "Invalid type argument\n";
            return;
        }
    }

    // Вывод результата для случаев "EVEN", "ODD" или по числу вершин
    if (count > 0) {
        std::cout << total_area << "\n";
    }
    else {
        std::cout << "No matching polygons found\n";
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
    // Используем std::unique для удаления всех идущих подряд дубликатов target
    auto new_end = std::unique(polygons.begin(), polygons.end(), [&](const Polygon& a, const Polygon& b) {
        return a == target && b == target;
        });

    // Удаляем лишние элементы из вектора
    int removed_count = std::distance(new_end, polygons.end());
    polygons.erase(new_end, polygons.end());

    // Выводим количество удаленных элементов
    std::cout << removed_count << "\n";
}

void echoPolygon(std::vector<Polygon>& polygons, const Polygon& target) {
    // Используем std::find для поиска первого вхождения target
    auto it = std::find(polygons.begin(), polygons.end(), target);
    int added_count = 0;

    // Пока мы находим элементы target
    while (it != polygons.end()) {
        // Вставляем копию target после текущего вхождения
        it = polygons.insert(it + 1, target);
        ++added_count;
        // Ищем следующее вхождение после вставленного элемента
        it = std::find(it + 1, polygons.end(), target);
    }

    // Выводим количество добавленных элементов
    std::cout << added_count << "\n";
}

void lessArea(const std::vector<Polygon>& polygons, const Polygon& target) {
    int count = std::count_if(polygons.begin(), polygons.end(), [&target](const Polygon& poly) {
        return poly.area() < target.area();
        });
    std::cout << count << "\n";
}

void inFrame(const std::vector<Polygon>& polygons, const Polygon& target) {
    // Используем std::minmax_element для нахождения минимальных и максимальных координат
    auto x_minmax = std::minmax_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
        return std::min_element(a.points.begin(), a.points.end(), [](const Point& p1, const Point& p2) {
            return p1.x < p2.x;
            })->x < std::min_element(b.points.begin(), b.points.end(), [](const Point& p1, const Point& p2) {
                return p1.x < p2.x;
                })->x;
        });

    auto y_minmax = std::minmax_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
        return std::min_element(a.points.begin(), a.points.end(), [](const Point& p1, const Point& p2) {
            return p1.y < p2.y;
            })->y < std::min_element(b.points.begin(), b.points.end(), [](const Point& p1, const Point& p2) {
                return p1.y < p2.y;
                })->y;
        });

    int min_x = std::min_element((*x_minmax.first).points.begin(), (*x_minmax.first).points.end(), [](const Point& p1, const Point& p2) {
        return p1.x < p2.x;
        })->x;

    int max_x = std::max_element((*x_minmax.second).points.begin(), (*x_minmax.second).points.end(), [](const Point& p1, const Point& p2) {
        return p1.x < p2.x;
        })->x;

    int min_y = std::min_element((*y_minmax.first).points.begin(), (*y_minmax.first).points.end(), [](const Point& p1, const Point& p2) {
        return p1.y < p2.y;
        })->y;

    int max_y = std::max_element((*y_minmax.second).points.begin(), (*y_minmax.second).points.end(), [](const Point& p1, const Point& p2) {
        return p1.y < p2.y;
        })->y;

    // Используем std::all_of для проверки, находятся ли все точки целевого многоугольника внутри ограничивающего прямоугольника
    bool inside = std::all_of(target.points.begin(), target.points.end(), [&](const Point& p) {
        return p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y;
        });

    std::cout << (inside ? "<TRUE>" : "<FALSE>") << "\n";
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

    // Определяем мапу для команд и соответствующих функций
    std::map<std::string, std::function<void()>> command_map = {
        {"AREA", [&]() {
            std::string type;
            std::cin >> type;
            calculateArea(polygons, type);
        }},
        {"MAX", [&]() {
            std::string type;
            std::cin >> type;
            calculateMax(polygons, type);
        }},
        {"MIN", [&]() {
            std::string type;
            std::cin >> type;
            calculateMin(polygons, type);
        }},
        {"COUNT", [&]() {
            std::string type;
            std::cin >> type;
            countPolygons(polygons, type);
        }},
        {"RMECHO", [&]() {
            Polygon target = readPolygonFromInput();
            removeConsecutiveDuplicates(polygons, target);
        }},
        {"ECHO", [&]() {
            Polygon target = readPolygonFromInput();
            echoPolygon(polygons, target);
        }},
        {"LESSAREA", [&]() {
            Polygon target = readPolygonFromInput();
            lessArea(polygons, target);
        }},
        {"INFRAME", [&]() {
            Polygon target = readPolygonFromInput();
            inFrame(polygons, target);
        }},
        {"INTERSECTIONS", [&]() {
            Polygon target = readPolygonFromInput();
            intersections(polygons, target);
        }},
        {"SAME", [&]() {
            Polygon target = readPolygonFromInput();
            samePolygons(polygons, target);
        }},
        {"RECTS", [&]() {
            countRectangles(polygons);
        }},
        {"RIGHTSHAPES", [&]() {
            countRightShapes(polygons);
        }}
    };

    std::string command;
    while (std::cin >> command) {
        auto it = command_map.find(command);
        if (it != command_map.end()) {
            it->second();  // Выполняем соответствующую функцию
        }
        else {
            std::cout << "<INVALID COMMAND>" << "\n";
        }
    }

    return 0;
}