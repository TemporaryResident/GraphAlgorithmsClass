#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <climits>
#include <cstdlib> 
#include <ctime> 

using namespace std;

#define NIL 0
#define INF INT_MAX 

struct BiGraph {
    int M, N; // M for left and N for right sides of Bipartite Graph.
    vector<int>* adj; // adj list only for left vertices.
    int* pairU;
    int* pairV;
    int* dist;
} typedef BiGraph;

void deleteBiGraph(BiGraph* G) {
    for (int i = 0; i <= G -> N; i++) {
        G -> adj[i].clear();
    }
    delete G -> adj;
    delete G -> pairU;
    delete G -> pairV;
    delete G -> dist;

    delete G;
}

struct Edge {
    int to;
    int u;
    int l;
} typedef Edge;

Edge* edge(int to, int l, int u) {
    Edge* edge = new Edge;
    
    edge -> to = to;
    edge -> u = u;
    edge -> l = l;

    return edge;
}

void addEdge(BiGraph* G, int u, int v) {
    G -> adj[u].push_back(v);
}

BiGraph* specialBiGraph(int V) {
    BiGraph* G = new BiGraph;
    G -> N = V;
    G -> M = V;
    G -> adj = new vector<int>[V + 1];

    G -> pairU = new int[G -> M + 1];
    G -> pairV = new int[G -> N + 1];
    G -> dist = new int [G -> M + 1];

    return G;
}

BiGraph* createBiGraph(int M, int N) {
    BiGraph* G = new BiGraph;
    
    G -> M = M;
    G -> N = N;
    G -> adj = new vector<int>[M + 1];

    return G;
}

void printBiGraph(BiGraph* G) {
    cout << endl << "----- BiGraph ----- " << endl << "V = " << G -> M << endl; 
    for (int i = 1; i <= G -> M; i++ ) {
        for (auto v : G -> adj[i]) {
            cout << i << " <-> " << v << endl;
        }
    }
    cout << "-------------------";
}

bool dfs(BiGraph* G, int u) {
    if (u != NIL) {
        for(auto v : G -> adj[u]) {
            if (G -> dist[G -> pairV[v]] == G -> dist[u] + 1) {
                if (dfs(G, G -> pairV[v]) == true) {
                    G -> pairV[v] = u;
                    G -> pairU[u] = v;

                    return true;
                }
            }
        }
        G -> dist[u] = INF;
        
        return false;
    }

    return true;
}

bool bfs(BiGraph* G) {
    queue<int> Q;

    for (int u = 1; u <= G -> M; u++) {
        if (G -> pairU[u] == NIL){ 
            G -> dist[u] = 0; 
            Q.push(u);
        } else {
            G -> dist[u] = INF;
        }
    }
    
    G -> dist[NIL] = INF;

    while (!Q.empty()) {
        int u = Q.front();
        Q.pop();

        if (G -> dist[u] < G -> dist[NIL]) {
            for(auto v : G -> adj[u]) {
                if (G -> dist[G -> pairV[v]] == INF) {
                    G -> dist[G -> pairV[v]] = G -> dist[u] + 1;
                    Q.push(G -> pairV[v]);
                }
            }

        }
    }

    return (G -> dist[NIL] != INF);
}

void clean(BiGraph* G) {
    for (int u = 0; u <= G -> M; u++) {
        G -> pairU[u] = NIL;
        G -> dist[u] = NIL;
    }

    for (int v = 0; v <= G -> N; v++) {
        G -> pairV[v] = NIL;
    }
}

int random(int from, int to){
    return rand() % (to - from + 1) + from;
}

int randomMatching(BiGraph* G) {   
    clean(G);

    int maximumMatching = 0;
    int ksi = random(1, G -> N);

    while (ksi-- > 0) {
        int u = random(1, G -> N);
        int len = G -> adj[u].size();

        if (len > 0) {
            int rand = random(0, len - 1);
            int v = G -> adj[u][rand];

            if (G -> dist[u] == NIL && G -> pairV[v] == NIL && G -> pairU[u] == NIL) {
                G -> dist[u] = INF;
                G -> pairU[u] = v;
                G -> pairV[v] = u;

                maximumMatching += 1;
            }
        }
    }

    while(bfs(G)) {
        for (int u = 1; u <= G -> M; u++) {
            if (G -> pairU[u] == NIL && dfs(G, u)) {
                maximumMatching += 1;
            }
        }
    }

    return maximumMatching;
}

int hopccroftKarp(BiGraph* G) {
    clean(G);

    int maxiumMatching = 0;

    while(bfs(G)) {
        for (int u = 1; u <= G -> M; u++) {
            if (G -> pairU[u] == NIL && dfs(G, u)) {
                maxiumMatching += 1;
            }
        }
    }

    return maxiumMatching;
}

int checkMin(int currentMin, int l) {
    if (currentMin < l) return l;
    else                return currentMin;
        
}

int checkMax(int currentMax, int u) {
    if (currentMax < u) return currentMax;
    else                return u;
}

// ---- Notes ---- 
/*
    - Vertices now are numerated from 1 not 0 as it was previosuly!
    - We store adj list for left side!
    - It solves cases where matching found hopcroft carp is the one where there exists correct cost x,
    - Hard --> We'd have to find all perfect matchings and check them one by one to find correct cost x.
    - Humbug --> The idea is to find some random matching and then run hopcroft. 
        We could repeat this process phi times hoping that we will find correct cost x ;)
        It doesn't work that well :/ ..
*/

void solveTest() {
    static int phi = 100;

    int n; // number of vertices.
    cin >> n;    

    int m; // number of edges;
    cin >> m;
    int dummy = m;

    BiGraph* G = specialBiGraph(n);
    vector<Edge*>* edges = new vector<Edge*>[n + 1];

    while (dummy-- > 0) {
        int from, to, l, u;
        Edge* e;

        cin >> from >> to >> l >> u;
        e = edge(to, l, u);

        addEdge(G, from, to);
        edges[from].push_back(e);
    }

    int maxiumMatching = hopccroftKarp(G);

    int x = INF;
    int min = INT_MIN;
    int max = INT_MAX;

    for(int u = 1; u <= G -> M; u++) {
        // If there's no out that means it cannot be part of any cycle!
        if (edges[u].size() == 0) {
            cout << -1 << endl;
            return;
        }

        if (G -> pairU[u] != NIL) {
            for (auto e: edges[u]) {
                if (e -> to == G -> pairU[u]) {
                    min = checkMin(min, e -> l);
                    max = checkMax(max, e -> u);
                    break;
                }
            }
        }
    }

    if (maxiumMatching != n) {
        cout << -1 << endl;
        return;
    }

    // We've finally found correct matching! ;)
    if (!(min > max || maxiumMatching != n)) {            
        cout << min << endl;
        for(int u = 1; u <= G -> M; u++) {
            if (G -> pairU[u] != NIL) {
                cout << u << " " << G -> pairU[u] << endl;
            }
        }
        return;
    }

    // randomized matchins!
    for (int dummy = 0; dummy < phi; dummy++) {
        int maxiumMatching = randomMatching(G);

        int x = INF;
        int min = INT_MIN;
        int max = INT_MAX;

        for(int u = 1; u <= G -> M; u++) {
            // If there's no out that means it cannot be part of any cycle!
            if (edges[u].size() == 0) {
                cout << -1 << endl;
                return;
            }

            if (G -> pairU[u] != NIL) {
                for (auto e: edges[u]) {
                    if (e -> to == G -> pairU[u]) {
                        min = checkMin(min, e -> l);
                        max = checkMax(max, e -> u);
                        break;
                    }
                }
            }
        }

        if (maxiumMatching != n) {
            cout << - 1 << endl;
            return;
        }

         // We've finally found correct matching! ;)
        if (min <= max) {                     
            cout << min << endl;
            for(int u = 1; u <= G -> M; u++) {
                if (G -> pairU[u] != NIL) {
                    cout << u << " " << G -> pairU[u] << endl;
                }
            }
            return;
        }
    }

    // We couldn't find correct matching.. :C
    cout << -1 << endl;
}

int main(int argc, char const *argv[]) {
    int tests;
    cin >> tests;
    srand(11);

    while (tests-- > 0) {
        solveTest();   
    }

    return 0;
}