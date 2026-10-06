#include <iostream>
#include <random>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <clocale>
#include <chrono>
#include <stack>
#include <cmath>
#include <limits>
#include <windows.h>
#include <queue>
using namespace std;

class RandomGenerator {
private:
    mt19937 rng;
    exponential_distribution<double> expDist;
    uniform_real_distribution<double> servDist;
public:
    RandomGenerator(double lambda, double a, double b)
        : rng(static_cast<mt19937::result_type>(
            chrono::steady_clock::now().time_since_epoch().count())),
        expDist(lambda),
        servDist(a, b) {
    }

    double nextTau() {
        return expDist(rng);
    }

    double nextSigma() {
        return servDist(rng);
    }
};

void RunSimulation(double lambda, double a, double b, int N) {
    RandomGenerator gen(lambda, a, b);

    const double INF = numeric_limits<double>::infinity();

    int arrived = 0;
    int K = 0;
    double systemtime = 0.0;
    double T1 = gen.nextTau();
    double T2 = INF;
    stack<double> buffer;
    double currentArrival = 0.0;

    double sumT = 0.0;
    double sumT2 = 0.0;
    int served = 0;

    while (arrived < N || K == 1 || !buffer.empty()) {
        if (arrived < N && T1 <= T2) {
            systemtime = T1;
            double arrTime = systemtime;
            ++arrived;

            if (K == 0) {
                K = 1;
                currentArrival = arrTime;
                double sigma = gen.nextSigma();
                T2 = systemtime + sigma;
            }
            else {
                buffer.push(arrTime);
            }

            if (arrived < N) {
                T1 = systemtime + gen.nextTau();
            }
            else {
                T1 = INF;
            }
        }
        else {
            systemtime = T2;

            double tPreb = systemtime - currentArrival;
            sumT += tPreb;
            sumT2 += tPreb * tPreb;
            ++served;

            if (!buffer.empty()) {
                currentArrival = buffer.top();
                buffer.pop();

                double sigma = gen.nextSigma();
                T2 = systemtime + sigma;
            }
            else {
                K = 0;
                T2 = INF;
            }
        }
    }

    double mean = sumT / served;
    double variance = sumT2 / served - mean * mean;
    if (variance < 0.0) variance = 0.0;
    double sigma = sqrt(variance);

    cout << "Обслужено заявок: " << served << "\n";
    cout << "E[T_преб]: " << mean << "\n";
    cout << "sigma[T_преб]: " << sigma << "\n";
}

int main() {
    setlocale(LC_ALL, "RUS");

    double lambda, a, b;
    int N;

    cout << "lambda: ";
    cin >> lambda;

    cout << "a b: ";
    cin >> a >> b;

    cout << "N: ";
    cin >> N;

    cout << fixed << setprecision(6);

    RunSimulation(lambda, a, b, N);

    return 0;
}