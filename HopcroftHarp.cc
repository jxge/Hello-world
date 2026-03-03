#include <vector>
#include <queue>
#include <iostream>

using namespace std;

/*
 * Given a current matching M, HK performs phases: 
 *   (1) BFS (layering): Build a level graph from all free (unmatched)
 *       vertices in L as sources, layering by shortest alternating
 *       paths (edges alternate between non-matching and matching edges),
 *       stopping when reaching any free vertex in RRR. Let l be the
 *       length of the discovered shortest augmenting paths (if none
 *       exist, we’re done).
 *   (2) DFS (blocking flow): In the directed version of the level graph
 *       (edges respect alternating directions), find a maximal set of 
 *       vertex-disjoint shortest augmenting paths and augment along
 *       all of them at once (a blocking flow w.r.t. those shortest paths).
 */

class HopcroftKarp {
private:
    int n, m;                     // left size, right size
    vector<vector<int>> adj;      // adjacency list (left -> right)

    // dist[u] = level of u in the BFS graph
    //   dist[u] =  0 : free (unmatched) left vertices (BFS sources)
    //   dist[u] =  k : reachable via alternating path of length k
    //   dist[u] = -1 : not visited in current BFS phase
    // The dist array ensures DFS only follows shortest augmenting paths.
    vector<int> dist;

    vector<int> matchL, matchR;   // matchL[u], matchR[v]

    /*
     * In each phase, HK augments along only shortest augmenting paths (of
     * current length l and saturates all pathways of that length: the DFS
     * computes a blocking set of vertex-disjoint shortest augmenting paths,
     * so no augmenting path of length l is left after the phase.
     * Complexity is O(E)
     */
    bool bfs() {
        queue<int> q;

        for (int u = 0; u < n; ++u) {
            if (matchL[u] == -1) {
                dist[u] = 0;    // find a free node in U
                q.push(u);      // make it the starting node in BFS
            } else {
                dist[u] = -1;   // Mark it as not visisted in the BFS
            }
        }

        bool found = false;

        while (!q.empty()) {
            int u = q.front(); q.pop();

            for (int v : adj[u]) {
                int u2 = matchR[v];
                if (u2 == -1) {                 // free node in V
                    found = true; 
                } else if (dist[u2] == -1) {    // not yet visited in BFS
                    dist[u2] = dist[u] + 1;
                    q.push(u2);                 // do the BFS recursively
                }
            }
        }

        return found;
    }

    /* a DFS that finds a blocking set of shortest augmenting paths within
     * the level graph. The DFS visits each edge at most once (edges that
     * cannot advance are dead-ended and not re-tried), so this is also
     * O(E)
     */

    bool dfs(int u) {
        for (int v : adj[u]) {
            int u2 = matchR[v];
            if (u2 == -1 || (dist[u2] == dist[u] + 1 && dfs(u2))) {
                matchL[u] = v;
                matchR[v] = u;
                return true;
            }
        }

        dist[u] = -1;  // pruning
        return false;
    }

public:
    HopcroftKarp(int leftSize, int rightSize) : n(leftSize), m(rightSize)
    {
        adj.resize(n);
        dist.assign(n, 0);
        matchL.assign(n, -1);
        matchR.assign(m, -1);
    }

    void addEdge(int u, int v) {
        // u in [0, n-1], v in [0, m-1]
        adj[u].push_back(v);
    }

    int maximumMatching() 
    {
        int matching = 0;

        /*
         * There will be at most V^(1/2) phases for:
         *   (1) In every phase, the length of the shortest augmenting path
         *       increases.
         *   (2) Suppose we run the algorithm for the first V^{1/2} phases,
         *       the shortest remaining augmenting path must now have a 
         *       length of at least V^{1/2}.
         *   (3) There can be at most V^{1/2} such augmenting paths left.
         *       Suppose M is the current matching and M' is the maximum
         *       matching, then there's will be at most |M' - M| phases
         *       left, and (M' - M) * V^{1/2} <= V. So it will take at
         *       most V^{1/2} more phases to find those remaining matches.
         */
        while (bfs()) {
            for (int u = 0; u < n; ++u) {
                if (matchL[u] == -1 && dfs(u)) {
                    ++matching;
                }
            }
        }

        return matching;
    }

    vector<pair<int,int>> getMatches() {
        vector<pair<int,int>> result;
        for (int u = 0; u < n; ++u) {
            if (matchL[u] != -1) {
                result.emplace_back(u, matchL[u]);
            }
        }
        return result;
    }
};


int test1() {

    int nL = 6;
    int nR = 6;

    HopcroftKarp hk(nL, nR);
  
    std::vector<std::pair<int, int>> edges = {
        {0, 0},
        {0, 1},
        {0, 3},
        {1, 1},
        {1, 5},
        {2, 1},
        {2, 2},
        {3, 2},
        {3, 4},
        {3, 5},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {5, 1},
        {5, 4}};

    for (auto const &p : edges) {
        hk.addEdge(p.first, p.second);
    }

    int res = hk.maximumMatching();
    cout << "Maximum matching size: " << res << "\n";

    auto pairs = hk.getMatches();
    for (auto [u, v] : pairs) {
        cout << u << " - " << v << "\n";
    }

    return 0;
}

int test2() {

    int nL = 3;
    int nR = 3;

    HopcroftKarp hk(nL, nR);
  
    std::vector<std::pair<int, int>> edges = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 5},
        {2, 1},
        {2, 2}};

    for (auto const &p : edges) {
        hk.addEdge(p.first, p.second);
    }

    int res = hk.maximumMatching();
    cout << "Maximum matching size: " << res << "\n";

    auto pairs = hk.getMatches();
    for (auto [u, v] : pairs) {
        cout << u << " - " << v << "\n";
    }

    return 0;
}

int main()
{
    test1();
    return 0;
}
