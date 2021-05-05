
#include <iostream>
#include <fstream>
#include <random>

#include <arbok/paths.h>
#include <arbok/tarjan.h>
#include <cassert>

using namespace std;
using namespace arbok;

int main() {

    auto testcase = DATA_DIR + "konect/convote.soap"s;
    ifstream inf(testcase);

    int n,m;
    inf>>n>>m;

    mt19937 gen(1337);
    uniform_int_distribution dist(1, 20);

    Tarjan alg(n+1,TarjanVariant::SET);
    for(int i=0; i<m; ++i) {
        int from, to;
        inf>>from>>to;
        assert(0<=from && from<n);
        assert(0<=to   && to<n);
        alg.create_edge(from, to, dist(gen));
    }

    for(int i=0; i<n; ++i)
        alg.create_edge(n, i, 1e9);

    auto res = alg.run(n);

    cout << "n=" << n << endl;
    cout << "m=" << m << endl;
    cout << "w=" << res << endl;
    cout << "w/1e9=" << res/1'000'000'000 << endl;

    return 0;
}

