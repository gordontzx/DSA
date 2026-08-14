/*
 * Reference:
 * https://tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
 */

#include <algorithm>
#include <numeric>
#include <vector>

struct DominatorTree {
private:
    // Nodes are numbered from 0..n-1
    const int N; // number of nodes

    std::vector<std::vector<int>> adj;      // Original graph
    std::vector<std::vector<int>> rev_adj;  // Original graph but with edges reversed

    // DFS tree
    int timer;
    std::vector<int> t_in;
    std::vector<int> rev;   // t_in[u] -> u (reverse map of t_in)
    std::vector<int> par;   // parent of node in DFS tree

    // Finding idom
    std::vector<int> sdom;
    std::vector<int> rdom;
    std::vector<int> idom;
    std::vector<std::vector<int>> buckets;  // buckets[u] is the nodes for which u is the sdom for

    // DSU
    std::vector<int> dsu_par;
    std::vector<int> dsu_min;   // node with min sdom entry time in DSU tree

    void build_dfs_tree(int u) {
        t_in[u] = timer;
        rev[timer] = u;
        timer++;

        for (int v : adj[u]) {
            if (t_in[v] != -1) continue;
            par[v] = u;
            build_dfs_tree(v);
        }
    }

    int find(int u) {
        if (dsu_par[u] == -1) {
            return u;
        }

        int p = dsu_par[u];
        if (dsu_par[p] != -1) {
            int p_min = find(p);
            if (t_in[sdom[p_min]] < t_in[sdom[dsu_min[u]]]) {
                dsu_min[u] = p_min;
            }
            dsu_par[u] = dsu_par[p];
        }

        return dsu_min[u];
    }

    void unite(int p, int u) {
        dsu_par[u] = p;
    }

public:
    DominatorTree(const std::vector<std::vector<int>>& adj_list)
        : N(adj_list.size()), adj(adj_list), rev_adj(N),
          t_in(N), rev(N), par(N),
          sdom(N), rdom(N), idom(N), buckets(N),
          dsu_par(N), dsu_min(N)
    {
        // Build reverse graph
        for (int u = 0; u < N; u++) {
            for (int v : adj[u]) {
                rev_adj[v].push_back(u);
            }
        }
    }

    /* Returns an array `par` where `par[u]` is the parent of node `u` in the dominator tree. */
    std::vector<int> build_tree(int source) {
        // Initialize
        std::iota(sdom.begin(), sdom.end(), 0);
        std::fill(idom.begin(), idom.end(), -1);
        std::fill(buckets.begin(), buckets.end(), std::vector<int>());
        std::fill(dsu_par.begin(), dsu_par.end(), -1);
        std::iota(dsu_min.begin(), dsu_min.end(), 0);

        // Build DFS tree
        timer = 0;
        fill(t_in.begin(), t_in.end(), -1);
        build_dfs_tree(source);

        // Find sdoms
        for (int i = timer-1; i >= 0; i--) {
            int u = rev[i];

            for (int v : rev_adj[u]) {
                if (t_in[v] == -1) continue;    // unreachable

                int candidate = find(v);
                if (t_in[sdom[candidate]] < t_in[sdom[u]]) {
                    sdom[u] = sdom[candidate];
                }
            }
            buckets[sdom[u]].push_back(u);

            for (int v : buckets[u]) {
                rdom[v] = find(v);
            }

            if (i > 0) {
                unite(par[u], u);
            }
        }

        // Find idoms
        idom[source] = -1;
        for (int i = 1; i < timer; i++) {
            int u = rev[i];
            idom[u] = sdom[u] == sdom[rdom[u]] ? sdom[u] : idom[rdom[u]];
        }

        return idom;
    }
};
