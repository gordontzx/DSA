#include <bits/stdc++.h>
using namespace std;

constexpr int K = 26;

struct Node {
    int next[K];
    int id = -1;
    int par;
    char par_char;
    int link;
    int go[K];

    Node(int par = -1, char par_char = -1) : par(par), par_char(par_char) {
        fill(begin(next), end(next), -1);
    }
};

vector<Node> trie;
constexpr int ROOT = 0;

void insert_word(const string& word, int word_id) {
    int cur = ROOT;
    for (char c : word) {
        int nxt = trie[cur].next[c-'a'];
        if (nxt == -1) {
            nxt = trie.size();
            trie.emplace_back(cur, c);
            trie[cur].next[c-'a'] = nxt;
        }
        cur = nxt;
    }
    trie[cur].id = word_id;
}

void build_links() {
    queue<int> q;
    trie[ROOT].link = ROOT;
    for (int i = 0; i < K; i++) {
        int nxt = trie[ROOT].next[i];
        if (nxt == -1) continue;
        q.push(nxt);
        trie[ROOT].go[i] = nxt;
    }

    while (!q.empty()) {
        int cur = q.front(); q.pop(); 

        if (trie[cur].par == ROOT) {
            trie[cur].link = ROOT;
        } else {
            int par_link = trie[trie[cur].par].link;
            trie[cur].link = trie[par_link].go[trie[cur].par_char-'a'];
        }

        for (int i = 0; i < K; i++) {
            int nxt = trie[cur].next[i];
            if (nxt != -1) {
                q.push(nxt);
                trie[cur].go[i] = nxt;
            } else {
                trie[cur].go[i] = trie[trie[cur].link].go[i];
            }
        }
    }
}

void build(const vector<string>& words) {
    trie.emplace_back();
    for (int i = 0; i < words.size(); i++)
        insert_word(words[i], i);
    build_links();
}

void find_words_slow(const string& text, const vector<string>& words) {
    int m = words.size();
    vector<bool> in_text(m, false);

    int cur = 0;
    for (char c : text) {
        cur = trie[cur].go[c-'a'];
        for (int u = cur; u != ROOT; u = trie[u].link) {
            if (trie[u].id != -1)
                in_text[trie[u].id] = true;
        }
    }

    for (int i = 0; i < m; i++) {
        if (in_text[i])
            cout << words[i] << '\n';
    }
}

void find_words(const string& text, const vector<string>& words) {
    vector<bool> visited(trie.size());

    // Mark visited nodes
    int cur = ROOT;
    for (char c : text) {
        cur = trie[cur].go[c-'a'];
        visited[cur] = true;
    }

    // BFS from deepest nodes
    vector<vector<int>> depths({{ROOT}});
    while (true) {
        vector<int> next_depth;
        for (int node : depths.back()) {
            for (int i = 0; i < K; i++) {
                int nxt = trie[node].next[i];
                if (nxt != -1) {
                    next_depth.push_back(nxt);
                }
            }
        }
        if (next_depth.empty()) break;
        depths.push_back(std::move(next_depth));
    }

    while (!depths.empty()) {
        const vector<int> last = std::move(depths.back()); depths.pop_back();
        for (int node : last) {
            if (visited[node]) {
                visited[trie[node].link] = true;
            }
        }
    }

    int m = words.size();
    vector<bool> in_text(m, false);

    // Find all terminal nodes that are visited
    for (int u = 0; u < trie.size(); u++) {
        if (visited[u] && trie[u].id != -1)
            in_text[trie[u].id] = true;
    }

    for (int i = 0; i < m; i++) {
        if (in_text[i])
            cout << words[i] << '\n';
    }
}

int main() {
    vector<string> words = {
        "bab", "abc", "ayba"
    };
    vector<string> words2 = {
        "aabab", "bab"
    };
    build(words);

    const string text = "aybabtu";
    const string text2 = "aabab";
    find_words(text, words);
}
