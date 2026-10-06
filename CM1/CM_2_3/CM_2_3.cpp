#include <chrono>
#include <random>
#include <windows.h>
#include <iostream>

using namespace std;
int main()
{
    setlocale(LC_ALL, "ru");

    double a = 5.0, b = 7.0;
    int N = 1000000;
    int n = 10;
    double p = 0.01;
    double tau = (b - a) / N;

    mt19937 mt{ static_cast<mt19937::result_type>(chrono::steady_clock::now().time_since_epoch().count()) };
    uniform_real_distribution<> urd(0.0, 1.0);

    vector<double> events;
    double t = a;
    for (int i = 0; i < N; i++) {
        if (urd(mt) <= p) events.push_back(t);
        t += tau;
    }
    cout << "P = " << p << "\n";
    cout << "Размер: " << events.size() << "\n";

    vector<double> dist;
    for (size_t i = 1; i < events.size(); i++)
        dist.push_back(events[i] - events[i - 1]);

    double A = 0.0;
    double B = *max_element(dist.begin(), dist.end());

    vector<int> count(n, 0);
    vector<double> limits(n + 1);

    for (int i = 0; i <= n; i++)
        limits[i] = A + ((B - A) / n) * i;
    limits[n] += 1e-9;

    for (double x : dist) {
        for (int j = 0; j < n; j++) {
            if (x >= limits[j] && x < limits[j + 1]) {
                count[j]++;
            }
        }
    }

    cout << "\nГраницы:";
    cout << "A = " << A << " B = " << B << "\n";

    cout << "\nРаспредление: ";
    for (int i = 0; i < n; i++)
        cout << count[i] << " ";
    cout << "\n";
}