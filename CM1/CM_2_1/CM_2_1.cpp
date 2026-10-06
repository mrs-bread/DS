#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");

    const int n = 1000000;
    double tau1 = 5.0, tau2 = 7.0, sigma1 = 4.0, sigma2 = 6.0;

    mt19937 mt{ static_cast<mt19937::result_type>(
        chrono::steady_clock::now().time_since_epoch().count()) };
    exponential_distribution<> tau(1.0 / 6);
    exponential_distribution<> sigma(1.0 / 5);

    vector<double> arrival(n), serviceEnd(n), service(n), waitingTime(n);

    double t = 0;
    for (int i = 0; i < n; ++i) {
        t += tau(mt);
        arrival[i] = t;
        service[i] = sigma(mt);
    }

    double timeWaiting = 0;
    int waitCount = 0;
    for (int i = 0; i < n; ++i) {
        double serviceStart = (i == 0) ? arrival[i] : max(serviceEnd[i - 1], arrival[i]);
        waitingTime[i] = serviceStart - arrival[i];
        if (i > 0 && arrival[i] < serviceEnd[i - 1]) {
            waitCount++;
        }
        serviceEnd[i] = serviceStart + service[i];
    }

    struct Event { double time; int delta; };
    vector<Event> events;
    events.reserve(2 * n);
    for (int i = 0; i < n; ++i) {
        events.push_back({ arrival[i],   +1 });
        events.push_back({ serviceEnd[i], -1 });
    }
    sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.time < b.time;
        });

    double prevTime = events.front().time;
    int inSystem = 0;
    double areaInQueue = 0.0;

    for (const auto& e : events) {
        double dt = e.time - prevTime;
        areaInQueue += max(0, inSystem - 1) * dt;
        inSystem += e.delta;
        prevTime = e.time;
    }

    double totalTime = serviceEnd.back();
    double avgTime = 0.0;
    for (int i = 0; i < n; i++) {
        avgTime += waitingTime[i];
    }

    cout << "Сервер занят: "
        << (double)waitCount / n << endl;
    cout << "Средняя длина очереди: "
        << areaInQueue / totalTime << endl;
    cout << "Среднее время ожидания: " << avgTime / n;
}