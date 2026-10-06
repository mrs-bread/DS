#include <iostream>
#include <random>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <clocale>
using namespace std;

class TimeGenerator {
private:
    mt19937 rng;
    uniform_int_distribution<int> hourDist;
    uniform_int_distribution<int> minuteDist;
    uniform_int_distribution<int> secondDist;
public:
    TimeGenerator()
        : rng(random_device{}()),
        hourDist(0, 23),
        minuteDist(0, 59),
        secondDist(0, 59) {
    }

    string randomTime24() {
        int hour = hourDist(rng);
        int minute = minuteDist(rng);
        int second = secondDist(rng);
        ostringstream oss;
        oss << setw(2) << setfill('0') << hour << ":"
            << setw(2) << setfill('0') << minute << ":"
            << setw(2) << setfill('0') << second;
        return oss.str();
    }
};

int String_To_Int(string t) {
    int sec = 0;
    size_t pos = t.find(":");
    string hour = t.substr(0, pos);
    sec += stoi(hour) * 3600;
    t = t.substr(pos + 1, t.length());
    pos = t.find(":");
    string min = t.substr(0, pos);
    sec += stoi(min) * 60;
    t = t.substr(pos + 1, t.length());
    sec += stoi(t);
    return sec;
}

int Gen_1_5() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 5);
    int random = dist(gen);
    return random;
}

void BubbleSort(vector<pair<int, string>>& mass) {
    int listLength = mass.size();
    while (listLength--) {
        bool swapped = false;
        for (int i = 0; i < listLength; i++) {
            if (String_To_Int(mass[i].second) > String_To_Int(mass[i + 1].second)) {
                swap(mass[i], mass[i + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}

void printArray(const vector<pair<int, string>>& mass) {
    for (const auto& x : mass) {
        cout << x.first << " " << x.second << "\n";
    }
}

int main() {
    setlocale(LC_ALL, "RUS");

    int n = 30;
    TimeGenerator generator;
    vector<pair<int, string>> mass;

    for (int i = 0; i < n; i++) {
        mass.push_back(make_pair(Gen_1_5(), generator.randomTime24()));
    }

    cout << "Исходный массив:\n";
    printArray(mass);

    BubbleSort(mass);

    cout << "\nОтсортированный массив:\n";
    printArray(mass);

    return 0;
}