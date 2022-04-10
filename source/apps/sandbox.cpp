
#include <iostream>

#include <arbok/utils/paths.h>
#include <arbok/others/lemon.h>
#include <arbok/utils//utils.h>

using namespace std;
using namespace arbok;

int main() {
    ios::sync_with_stdio(false);

    auto graph = fromFile(DATA_DIR + "konect/flickr-growth.soap"s);

    return 0;
}

