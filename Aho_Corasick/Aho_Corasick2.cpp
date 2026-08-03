#include <bits/stdc++.h>
using namespace std;

constexpr int N = 5e5;

struct Node {
    int next[26];
    int id = -1;
    int link;
};

int next_id = 1;
Node trie[N+1];
bool vis[N+1];
bool ans[N+1];
vector<int> graph[N+1];

void insert(const string& word, int word_id) {
    int cur = 0;
    for (char c : word) {
        int nxt = trie[cur].next[c-'a'];
        if (nxt == 0) {
            nxt = next_id++;
            trie[cur].next[c-'a'] = nxt;
        }
        cur = nxt;
    }
    trie[cur].id = word_id;
}

void build() {
    queue<int> q;

    for (int i = 0; i < 26; i++) {
        int nxt = trie[0].next[i];
        if (nxt) {
            q.push(nxt);
        }
    }

    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int i = 0; i < 26; i++) {
            int nxt = trie[cur].next[i];
            if (nxt) {
                q.push(nxt);
                trie[nxt].link = trie[trie[cur].link].next[i];
            } else {
                trie[cur].next[i] = trie[trie[cur].link].next[i];
            }
        }
    }

    for (int u = 1; u < next_id; u++) {
        graph[trie[u].link].push_back(u);
    }
}

bool dfs(int u) {
    bool res = vis[u];
    for (int v : graph[u]) {
        res |= dfs(v);
    }

    if (trie[u].id != -1)
        ans[trie[u].id] = res;

    return res;
}

void search_words(const string& text, const vector<string>& words) {
    int cur = 0;
    for (char c : text) {
        cur = trie[cur].next[c-'a'];
        vis[cur] = true;
    }

    dfs(0);

    for (int i = 0; i < words.size(); i++) {
        if (ans[i])
            cout << words[i] << '\n';
    }
}

int main() {
    const vector<string> words = {
        "bab", "abc", "ayba"
    };
    const string text = "aybabtu";

    for (int i = 0; i < words.size(); i++)
        insert(words[i], i);
    build();

    search_words(text, words);
}
