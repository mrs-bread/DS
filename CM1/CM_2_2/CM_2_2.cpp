#include <chrono>
#include <random>
#include <windows.h>
#include <iostream>
using namespace std;
int main()
{
    setlocale(LC_ALL, "rus");

    double n = 10000.0;
    double events = 0;
    //double tau1 = 5.0, tau2 = 7.0, sigma1 = 4.0, sigma2 = 6.0;
    double lambda = 1.0 / 6.0;
    double mu = 1.0 / 5.0;
    double rho = lambda / mu;

    mt19937 mt{ static_cast<mt19937::result_type>(chrono::steady_clock::now().time_since_epoch().count()) };
    exponential_distribution<> tau(lambda);
    exponential_distribution<> sigma(mu);

    double currentTime = 0.0;
    double busyUntil = 0.0;
    double total = 0.0;
    double totalLength = 0.0;
    int tasksWaited = 0;
    double queue = 0.0;

    for (int i = 0; i < n; i++) {
        double tt = tau(mt);
        double ss = sigma(mt);

        double nextArrival = currentTime + tt;
        while (busyUntil > 0.0 && busyUntil < nextArrival) {
            totalLength += queue * (busyUntil - currentTime);
            currentTime = busyUntil;

            if (queue > 0.0) {
                queue -= 1.0;
                busyUntil = currentTime + sigma(mt);
            }
            else {
                busyUntil = 0.0;
            }
        }

        totalLength += queue * (nextArrival - currentTime);
        currentTime = nextArrival;

        if (busyUntil > 0.0) {
            total += (busyUntil - currentTime);
            queue += 1.0;
            tasksWaited++;
        }
        else {
            busyUntil = currentTime + ss;
        }
    }

    while (busyUntil > 0.0) {
        totalLength += queue * (busyUntil - currentTime);
        currentTime = busyUntil;

        if (queue > 0.0) {
            queue -= 1.0;
            busyUntil = currentTime + sigma(mt);
        }
        else {
            busyUntil = 0.0;
        }
    }

    double totalTime = currentTime;
    double avgWait = totalLength / n;
    double probWaiting = (double)tasksWaited / n;
    double avgLength = totalLength / totalTime;

    cout << "Вероятность ожидания: " << probWaiting << endl;
    cout << "Среднее время ожидания: " << avgWait << endl;
    cout << "Средняя длина очереди: " << avgLength << endl;

    cout << "\nПРОВЕРКА ЗНАЧЕНИЙ\n" << endl;

    cout << "lambda = " << lambda << ", mu = " << mu << ", rho = " << rho << endl;

    double probWaitingLecture = rho;
    double avgWaitLecture = rho / (mu - lambda);
    double avgLengthLecture = lambda * avgWaitLecture;

    cout << "Результаты по лекции:" << endl;
    cout << "Вероятность ожидания: " << probWaitingLecture << endl;
    cout << "Среднее время ожидания: " << avgWaitLecture << endl;
    cout << "Средняя длина очереди: " << avgLengthLecture << endl;

    cout << "\nПогрешность (%) по сравнению полученными значениями:" << endl;
    cout << "Вероятность ожидания: " << fabs(probWaiting - probWaitingLecture) / probWaitingLecture * 100.0 << endl;
    cout << "Среднее время ожидания: " << fabs(avgWait - avgWaitLecture) / avgWaitLecture * 100.0 << endl;
    cout << "Средняя длина очереди: " << fabs(avgLength - avgLengthLecture) / avgLengthLecture * 100.0 << endl;
}