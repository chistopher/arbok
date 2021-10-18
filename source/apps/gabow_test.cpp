
#include <arbok/gabow/gabow.h>
#include <iostream>

/*
int main() {
    arbok::Gabow gabow(3);
    gabow.create_edge(0,1,1);
    gabow.create_edge(1,2,2);
    long long answer = gabow.run(0);

    std::cout << "gabow sagt " << answer << std::endl;

    return 0;
}
*/

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n;
    std::cin >> n;
    arbok::Gabow gabow(n+1);
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        std::cin >> shortcut_item >> time;
        gabow.create_edge(shortcut_item, level, time);
        for (int item = 0; item <= n; item++) {
            std::cin >> time;
            if (item != shortcut_item) {
                gabow.create_edge(item, level, time);
            }
        }
    }
    std::cout << "starting gabow" << std::endl;
    auto weight = gabow.run(0);
    std::cout << weight << std::endl;

    return 0;
}