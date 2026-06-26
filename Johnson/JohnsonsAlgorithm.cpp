/*
 * I did not actually test the code so don't blame me if it is buggy.
 */

#include <algorithm>
#include <limits>
#include <vector>
#include <utility>
#include <queue>

constexpr long long INF = std::numeric_limits<long long>::max() / 2;

struct Edge {
    int from;
    int to;
    long long weight;
};

struct ReweightResult {
    std::vector<std::vector<Edge>> adj;
    std::vector<long long> h;
    bool is_success;
};

std::pair<std::vector<long long>, bool> bellman_ford(const int start, const std::vector<std::vector<Edge>>& adj) {
    /* Returns {dist, has_negative_cycle} */

    int N = adj.size();

    std::vector<long long> dist(N, INF);
    dist[start] = 0;
    for (int i = 1; i < N; i++) {
        for (int u = 0; u < N; u++) {
            if (dist[u] == INF) continue;
            for (Edge e : adj[u]) {
                dist[e.to] = std::min(dist[e.to], dist[u] + e.weight);
            }
        }
    }

    // Check for negative cycle
    for (int u = 0; u < N; u++) {
        for (Edge e : adj[u]) {
            if (dist[u] + e.weight < dist[e.to])
                return {{}, true};
        }
    }

    return {dist, false};
}

ReweightResult reweight(const std::vector<std::vector<Edge>>& adj) {
    const int N = adj.size();   // number of vertices

    // node N is a dummy node for reweighting
    std::vector<std::vector<Edge>> dummy_adj(N+1);
    for (int i = 0; i < N; i++) {
        dummy_adj[i] = adj[i];
    }

    // Add edge from N to all other nodes with edge weight 0
    dummy_adj[N].reserve(N);
    for (int u = 0; u < N; u++) {
        dummy_adj[N].push_back(Edge{N, u, 0});
    }

    auto [h, has_negative_cycle] = bellman_ford(N, dummy_adj);

    if (has_negative_cycle)
        return {{}, {}, false};

    std::vector<std::vector<Edge>> reweighted_adj = adj;
    for (int u = 0; u < N; u++) {
        for (int i = 0; i < adj[u].size(); i++) {
            Edge e = adj[u][i];
            reweighted_adj[u][i].weight += h[u] - h[e.to];
        }
    }

    return {reweighted_adj, h, true};
}

std::vector<long long> dijkstra(const int start, const std::vector<std::vector<Edge>>& adj) {
    int N = adj.size();

    std::vector<long long> dist(N, INF);
    std::vector<bool> visited(N, false);
    std::priority_queue<
        std::pair<long long, int>,
        std::vector<std::pair<long long, int>>,
        std::greater<std::pair<long long, int>>
    > pq;

    pq.emplace(0, start);
    dist[start] = 0;

    while (!pq.empty()) {
        auto [d, cur] = pq.top(); pq.pop();

        if (visited[cur]) continue;
        visited[cur] = true;

        for (Edge edge : adj[cur]) {
            long long new_dist = d + edge.weight;
            if (new_dist < dist[edge.to]) {
                dist[edge.to] = new_dist;
                pq.emplace(new_dist, edge.to);
            }
        }
    }

    return dist;
}

std::vector<std::vector<long long>> johnsons(const std::vector<std::vector<Edge>>& adj) {
    int N = adj.size();
    auto [reweighted_adj, h, is_success] = reweight(adj);

    if (!is_success)
        return {};

    // Dijkstra from every node
    std::vector<std::vector<long long>> all_pairs(N);
    for (int u = 0; u < N; u++) {
        std::vector<long long> dist = dijkstra(u, reweighted_adj);

        for (int v = 0; v < N; v++) {
            if (dist[v] != INF) {
                dist[v] -= h[u] - h[v];
            }
        }

        all_pairs[u] = dist;
    }

    return all_pairs;
}
