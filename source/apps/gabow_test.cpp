
#include <arbok/gabow/gabow.h>

int main() {
    arbok::Gabow gabow(3);
    gabow.create_edge(0,1,1);
    gabow.create_edge(1,2,2);
    gabow.run(0);
    gabow.debug();

    return 0;
}
