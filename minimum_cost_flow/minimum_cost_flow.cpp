/*
 * Minimum cost flow using successive shortest paths.
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

struct MCF {
    static constexpr long long INF = 1e18;

    struct Edge {
        int from, to;
        long long capacity;
        long long cost;
    };

    int n;  // Number of nodes

    std::vector<Edge> edges;            // reverse edges have the 0th bit flipped, i.e. the reverse of u is u^1
    std::vector<std::vector<int>> adj;  // node -> array of edge ids

    // Dijkstra
    std::vector<long long> dist;
    std::vector<long long> pi;      // Potentials
    std::vector<int> parent_edge;   // incoming edge id

    MCF(int n) : n(n) {
        adj.resize(n);

        dist.resize(n);
        pi.resize(n);
        parent_edge.resize(n);
    }

    void add_edge(int from, int to, long long cap, long long cost) {
        adj[from].push_back(edges.size());
        edges.push_back({from, to, cap, cost});

        adj[to].push_back(edges.size());
        edges.push_back({to, from, 0, -cost});
    }

    bool set_potentials() {
        // Returns false if there is negative cycle, true otherwise
        std::fill(pi.begin(), pi.end(), 0);

        bool changed = false;
        for (int i = 0; i < n; i++) {
            changed = false;
            for (Edge e : edges) {
                if (e.capacity > 0 && pi[e.from] + e.cost < pi[e.to]) {
                    pi[e.to] = pi[e.from] + e.cost;
                    changed = true;
                }
            }
        }

        return !changed;
    }

    void dijkstra(int start, int end) {
        std::fill(dist.begin(), dist.end(), INF);
        std::fill(parent_edge.begin(), parent_edge.end(), -1);

        std::priority_queue<
            std::pair<long long, int>,
            std::vector<std::pair<long long, int>>,
            std::greater<std::pair<long long, int>>
        > pq;

        pq.emplace(0, start);
        dist[start] = 0;

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();

            if (d > dist[u]) continue;

            for (int id : adj[u]) {
                if (edges[id].capacity <= 0) continue;

                int v = edges[id].to;
                long long new_dist = d + (edges[id].cost + pi[u] - pi[v]);
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent_edge[v] = id;
                    pq.emplace(new_dist, v);
                }
            }
        }
    }

    long long min_cost_flow(int flow, int start, int end) {
        set_potentials();

        long long cost = 0;
        while (flow > 0) {
            dijkstra(start, end);
            if (parent_edge[end] == -1) break;

            // Get min flow on augmenting path
            long long f = flow;
            for (int cur = end; cur != start; cur = edges[parent_edge[cur]].from)
                f = std::min(f, edges[parent_edge[cur]].capacity);

            cost += f * (dist[end] - pi[start] + pi[end]);
            flow -= f;

            // Update capacities
            for (int cur = end; cur != start; cur = edges[parent_edge[cur]].from) {
                int id = parent_edge[cur];
                edges[id].capacity -= f;
                edges[id^1].capacity += f;
            }

            // Update potentials
            for (int u = 0; u < n; u++)
                pi[u] = std::min(pi[u] + dist[u], INF);
        }

        if (flow > 0) return -1;

        return cost;
    }
};

void test_basic_routing() {
    // Graph:
    // S(0) -> 1 (cap 2, cost 1) -> T(3) (cap 2, cost 2) => Path 1 (max 2, cost 3)
    // S(0) -> 2 (cap 1, cost 5) -> T(3) (cap 1, cost 1) => Path 2 (max 1, cost 6)
    MCF mcf(4);
    mcf.add_edge(0, 1, 2, 1);
    mcf.add_edge(0, 2, 1, 5);
    mcf.add_edge(1, 3, 2, 2);
    mcf.add_edge(2, 3, 1, 1);

    // Requesting 2 flow should entirely take the cheaper path
    long long cost = mcf.min_cost_flow(2, 0, 3);
    assert(cost == 6); // 2 units * 3 cost
    std::cout << "Test 1 (Basic Routing) Passed.\n";
}

void test_capacity_overflow() {
    // Using the same graph structure as above, but requesting 3 units.
    // It should take 2 units from Path 1 (cost 6) and 1 unit from Path 2 (cost 6).
    MCF mcf(4);
    mcf.add_edge(0, 1, 2, 1);
    mcf.add_edge(0, 2, 1, 5);
    mcf.add_edge(1, 3, 2, 2);
    mcf.add_edge(2, 3, 1, 1);

    long long cost = mcf.min_cost_flow(3, 0, 3);
    assert(cost == 12); // (2 * 3) + (1 * 6)
    std::cout << "Test 2 (Capacity Overflow) Passed.\n";
}

void test_negative_edges() {
    // Tests if set_pi() correctly handles initial negative edges.
    // 0 -> 1 (cap 1, cost -5)
    // 1 -> 2 (cap 1, cost 2)   => Path 0->1->2 costs -3
    // 0 -> 2 (cap 1, cost 1)   => Path 0->2 costs 1
    MCF mcf(3);
    mcf.add_edge(0, 1, 1, -5);
    mcf.add_edge(1, 2, 1, 2);
    mcf.add_edge(0, 2, 1, 1);

    long long cost = mcf.min_cost_flow(1, 0, 2);
    assert(cost == -3); 
    std::cout << "Test 3 (Negative Edges) Passed.\n";
}

void test_insufficient_capacity() {
    // 0 -> 1 (cap 5, cost 10). Requesting 10 flow. Should return -1.
    MCF mcf(2);
    mcf.add_edge(0, 1, 5, 10);

    long long cost = mcf.min_cost_flow(10, 0, 1);
    assert(cost == -1);
    std::cout << "Test 4 (Insufficient Capacity) Passed.\n";
}

int main() {
    test_basic_routing();
    test_capacity_overflow();
    test_negative_edges();
    test_insufficient_capacity();
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
