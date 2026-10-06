#include <iostream>
#include <random>
#include <chrono>
#include <queue>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;


struct Task {
    int remaining[3];
    double arrivalTime;
};

struct Event {
    double time;
    int type;
    int phase;
    int taskId;
    bool operator>(const Event& o) const { return time > o.time; }
};

int main() {
    setlocale(LC_ALL, "rus");

    int N = 100000;
    int warmup = N / 10;
    double tau1 = 4.0, tau2 = 6.0, rate = 0.5;

    mt19937 mt{ static_cast<mt19937::result_type>(
        chrono::steady_clock::now().time_since_epoch().count()) };
    uniform_real_distribution<> arrDist(tau1, tau2);
    exponential_distribution<> servDist(rate);
    uniform_int_distribution<> KDist(1, 2);
    uniform_int_distribution<> MLDist(0, 2);

    vector<queue<int>> q(3);
    vector<bool> busy(3, false);
    priority_queue<Event, vector<Event>, greater<Event>> pq;

    vector<Task> tasks(N);
    double t = 0;
    for (int i = 0; i < N; i++) {
        t += arrDist(mt);
        tasks[i] = { {KDist(mt), MLDist(mt), MLDist(mt)}, t };
        pq.push({ t, 0, 0, i });
    }

    double totalService[3] = { 0, 0, 0 };
    int serviceCount[3] = { 0, 0, 0 };
    double areaSystem = 0.0, lastTime = 0.0;
    int inSystem = 0;

    double sumT = 0.0, sumT2 = 0.0;
    int completed = 0;

    while (!pq.empty()) {
        Event e = pq.top(); pq.pop();
        double curTime = e.time;

        areaSystem += inSystem * (curTime - lastTime);
        lastTime = curTime;

        if (e.type == 0) {
            inSystem++;
            q[0].push(e.taskId);
        }
        else {
            int id = e.taskId;
            Task& task = tasks[id];
            task.remaining[e.phase]--;
            busy[e.phase] = false;

            int next = -1;
            if (task.remaining[e.phase] > 0) next = e.phase;
            else {
                for (int p = e.phase + 1; p < 3; p++) {
                    if (task.remaining[p] > 0) { next = p; break; }
                }
            }

            if (next == -1) {
                inSystem--;
                double dt = curTime - task.arrivalTime;
                if (completed >= warmup) {
                    sumT += dt;
                    sumT2 += dt * dt;
                }
                completed++;
            }
            else {
                q[next].push(id);
            }
        }

        for (int p = 0; p < 3; p++) {
            if (!busy[p] && !q[p].empty()) {
                int id = q[p].front(); q[p].pop();
                double s = servDist(mt);
                totalService[p] += s;
                serviceCount[p]++;
                busy[p] = true;
                pq.push({ curTime + s, 1, p, id });
            }
        }
    }

    double totalTime = lastTime;
    int cnt = completed - warmup;
    double meanT = sumT / cnt;
    double varT = sumT2 / cnt - meanT * meanT;
    if (varT < 0) varT = 0;
    double stdT = sqrt(varT);
    cout << fixed << setprecision(6);
    cout << "Загрузка фаз:                 ";
    for (int p = 0; p < 3; p++) cout << totalService[p] / totalTime << " ";
    cout << "\n";
    cout << "Среднее обслуживание на фазах: ";
    for (int p = 0; p < 3; p++) cout << totalService[p] / serviceCount[p] << " ";
    cout << "\n";
    cout << "Среднее время в системе: " << meanT << "\n";
    cout << "СКО времени в системе:   " << stdT << "\n";
    return 0;
}