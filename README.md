[![Build Status](https://github.com/chistopher/arbok/actions/workflows/test.yml/badge.svg)](https://github.com/chistopher/arbok/actions/workflows/test.yml)

# Build

This repository contains efficient implementations of directed minimum spanning tree algorithms.
Read our [corresponding paper]() for descriptions.

The project uses CMake and can be built like any other cmake project.
For example, the following commands create a release build in the `./build/` folder and compile it.

```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build/
```

The lemon algorithm is only available if you unpack the archive into a folder called `./lemon` or use the `./dependencies.sh` script to do it BEFORE compiling.
The atofigh algorithm is only available if you have Boost installed.
Test will only be built only if you have [GTest](https://github.com/google/googletest).

# Usage in C++

The algorithms are implemented in a shared library (the `arbok` target in CMake).
Our implementations are in the `<arbok/tarjan/tarjan-[version].h>` and `<arbok/gabow/gabow.h>` headers where `[version]` can be `hollow`, `pq`, `set`, `matrix`, and `treap`.
The other implementations are in `<arbok/others/...>`.
Lemon and atofig have their own shared libraries `lemon_adapter` and `atofigh_adapter` so that their dependencies do not carry over to the `arbok` lib.
Their implementations, however, are also in `<arbok/others/...>` headers.

Each algorithm is represented by a class. It has a constructor, an `create_edge` method, a `run` method, and a `reconstuct` method. 
The constructor takes the number of nodes and an upper bound for the number of edges (used for optimizations).
`create_edge` takes from, to and weight of an edge as ints and the other two methods require the root as an int.
Indices are zero-based.
Useage might look as follows.

```cpp
#include <iostream>
#include <vector>
#include <array>

#include <arbok/tarjan/tarjan_pq.h>

int main() {

    int n,m;
    std::cin>>n>>m;
    
    arbok::TarjanPQ alg(n,m);
    std::vector<std::array<int,3>> edges;

    for(auto& [u,v,w] : edges) {
        std::cin>>u>>v>>w;
        alg.create_edge(u,v,w);
    }

    int root;
    std::cin>>root;

    alg.run(root);
    auto res = alg.reconstruct(root);

    for(int e_id : res) {
        auto [u, v, w] = edges[e_id];
        std::cout << u << ' ' << v << ' ' <<  w << '\n';
    }

    return 0;
}

```

# Command Line Interface

This builds a command line interface `./build/arbok-cli`, which accepts the following options.
```
./build/arbok-cli -input ../data/konect/slashdot-zoo.soap -algo pq -giantCC 0 -check 1 
```
Order does not matter and all arguments are optional.
The above command contains their defaults if omitted. 
Additionally one can specify `csv`, `root` and `outfile`.

| Option | Argument  | Description |
|---|---|---|
| input     | a file path | the input graph |
| algo      | see below  | the algorithm to run |
| giantCC   | 0 or 1  | reduces the graph to the largest connected component (not strongly connected) |
| check     | 0 or 1  | run assertions to verify the result |
| csv       | a file path | if given, writes run statistics to file |
| root      | a vertex $r$ | if given, finds an arborescence rooted at $r$, otherwise the arboresence will be rooted at a supernode with $INF$ weight edges to all original nodes |
| outfile   | a file path | if given, writes the solution to file. |

Possible algorithms are `matrix pq treap hollow gabow lemon atofigh felerius spaghetti yosupo`.
They are explained in the paper.
The default algorithm (`pq`) has a consistently good performance.
Spaghetti does not implement the reconstruction phase and thus will not write anything if used with `outfile`.

### Input Format
The input file must start with a line that contains the number of vertices and edges.
Then, for each edge there follows a line with the two endpoints and (if weighted) an integer weight.
The cli recognizes weighted graphs by their file ending in `.wsoap` :)
If the graph is not weighted, it gets uniform random weights in the range [1,20].
Indices should be zero-based.
```
6 5
1 2 3
1 3 2
2 3 4
2 5 1
3 4 2
5 3 8
```

### Output Format
For the output format (see `outfile` option), each line contains the index of an edge that is part of the solution. 
Edge indices are determined by the order of edges in the input.
Indices are zero-based.
Note that `giantCC` changes edge indices.
If you want more control over this use the C++ library directly.
```
3      --input[3] = (2,5)
4      --input[4] = (3,4)
1      --input[1] = (1,3)
0      --input[0] = (1,2)
5      --input[5] = (5,3)
```

# Data for Experiments

The `./data` folder contains a bunch of scripts to download, generate, and convert the datasets.
There is a makefile (`./data/Makefile`) which has one target per dataset.
Since our initial crawl of networkrepository, they changed their query limit and scripts will probably not work for this dataset anymore. 
Howerever, there is an archive with the resulting graphs.
The girgs dataset requires the [generator by Bläsius et al](https://github.com/chistopher/girgs). 
Depending on your installation you might need to adjust the path to the generator in the `data/girgs_generate.py` script.
