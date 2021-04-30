#include <iostream>
#include <vector>

#include <arbok/tarjan.h>

using namespace std;

// This application should solve the following codeforces task
// https://codeforces.com/gym/102483/problem/F

int main() {
  ios::sync_with_stdio(false);
  cin.tie(NULL);
  //freopen("../data/fastestspeedrun/035-itsybitsycycles05.in", "r", stdin);

  int n;
  cin >> n;
  arbok::Tarjan tarjan(n+1, arbok::TarjanVariant::MATRIX);
  // G.resize(n + 1);
  for (int level = 1; level <= n; level++) {
    int shortcut_item, time;
    cin >> shortcut_item >> time;
    tarjan.create_edge(shortcut_item, level, time);
    for (int item = 0; item <= n; item++) {
      cin >> time;
      if (item != shortcut_item) {
        tarjan.create_edge(item, level, time);
      }
    }
  }

  auto weight = tarjan.run(0);
  cout << weight << endl;

  return 0;
}