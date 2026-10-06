#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

string formatTime(double sec) {
    int total = (int)sec;
    int h = (total / 3600) % 24;
    int m = (total % 3600) / 60;
    int s = total % 60;
    ostringstream oss;
    oss << setw(2) << setfill('0') << h << ":"
        << setw(2) << setfill('0') << m << ":"
        << setw(2) << setfill('0') << s;
    return oss.str();
}

void simulate(int n, double a, double b, int M) {
    mt19937 rng(random_device{}());
    uniform_real_distribution<double> intervalDist(a, b);
    uniform_real_distribution<double> startDist(0.0, (a + b) / 2.0);
    vector<pair<int, double>> events;
    events.reserve((size_t)n * M);
    vector<double> streamEnd(n);

    for (int i = 0; i < n; ++i) {
        double t = startDist(rng);
        for (int k = 0; k < M; ++k) {
            events.emplace_back(i + 1, t);
            t += intervalDist(rng);
        }
        streamEnd[i] = t;
    }

    sort(events.begin(), events.end(),
        [](const pair<int, double>& p1, const pair<int, double>& p2) {
            return p1.second < p2.second;
        });

    double cutoff = *min_element(streamEnd.begin(), streamEnd.end());
    vector<double> intervals;
    intervals.reserve(events.size());
    for (size_t i = 1; i < events.size(); ++i) {
        if (events[i].second > cutoff) break;
        double dt = events[i].second - events[i - 1].second;
        if (dt > 1e-12) intervals.push_back(dt);
    }

    double sum = 0.0, sum2 = 0.0;
    for (double dt : intervals) {
        sum += dt;
        sum2 += dt * dt;
    }
    double mean = sum / intervals.size();
    double var = sum2 / intervals.size() - mean * mean;
    if (var < 0.0) var = 0.0;
    double sigma = sqrt(var);
    double cv = sigma / mean;

    double lambda = n * 2.0 / (a + b);
    double theo_mean = 1.0 / lambda;
    double theo_var = 1.0 / (lambda * lambda);
    double cv_uniform = 2.0 * (b - a) / (sqrt(12.0) * (a + b));

    cout << fixed << setprecision(6);

    cout << "n=" << n << " a=" << a << " b=" << b << " M=" << M << "\n";
    cout << "событий=" << intervals.size() + 1
        << " интервалов=" << intervals.size()
        << " cutoff=" << cutoff << "\n";
    cout << "mean=" << mean << " (теор " << theo_mean << ")\n";
    cout << "var=" << var << " (теор " << theo_var << ")\n";
    cout << "lambda=" << lambda << "\n";
    cout << "CV=" << cv << " (эксп 1.0, uniform " << cv_uniform << ")\n\n";
}

int main() {
    setlocale(LC_ALL, "RUS");
    int nMax, M;
    double a, b;

    cout << "nMax a b M: ";
    cin >> nMax >> a >> b >> M;

    for (int n = 1; n <= nMax; n *= 2) {
        simulate(n, a, b, M);
    }
    if (nMax > 1 && (nMax & (nMax - 1)) != 0) {
        simulate(nMax, a, b, M);
    }

    return 0;
}