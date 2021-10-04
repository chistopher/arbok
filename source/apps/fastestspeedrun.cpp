#include <iostream>
#include <vector>

#include <arbok/utils/cf_solution.h>
#include <arbok/tarjan/tarjan.h>

using namespace std;

// This application should solve the following codeforces task
// https://codeforces.com/gym/102483/problem/F

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    //freopen("../data/fastestspeedrun/035-itsybitsycycles05.in", "r", stdin);
    arbok::cf_solution::solve(cin, cout, arbok::TarjanVariant::MATRIX);
    return 0;
}
