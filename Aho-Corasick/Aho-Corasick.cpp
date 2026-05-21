#include <bits/stdc++.h>
using namespace std;

constexpr int K = 26;

struct Node {
    int next[K];
    int id = -1;
    int par;
    char parChar;
    int link;
    int go[K];

    Node(int par = -1, char parChar = -1) : par(par), parChar(parChar) {
        fill(begin(next), end(next), -1);
    }
};

vector<Node> trie;
constexpr int ROOT = 0;

void insertWord(const string& word, int wordId) {
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
    trie[cur].id = wordId;
}

void buildLinks() {
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
            int parLink = trie[trie[cur].par].link;
            trie[cur].link = trie[parLink].go[trie[cur].parChar-'a'];
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
        insertWord(words[i], i);
    buildLinks();
}

void findWordsSlow(const string& text, const vector<string>& words) {
    int m = words.size();
    vector<bool> inText(m, false);

    int cur = 0;
    for (char c : text) {
        cur = trie[cur].go[c-'a'];
        for (int u = cur; u != ROOT; u = trie[u].link) {
            if (trie[u].id != -1)
                inText[trie[u].id] = true;
        }
    }

    for (int i = 0; i < m; i++) {
        if (inText[i])
            cout << words[i] << '\n';
    }
}

void findWords(const string& text, const vector<string>& words) {
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
        vector<int> nextDepth;
        for (int node : depths.back()) {
            for (int i = 0; i < K; i++) {
                int nxt = trie[node].next[i];
                if (nxt != -1) {
                    nextDepth.push_back(nxt);
                }
            }
        }
        if (nextDepth.empty()) break;
        depths.push_back(std::move(nextDepth));
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
    vector<bool> inText(m, false);

    // Find all terminal nodes that are visited
    for (int u = 0; u < trie.size(); u++) {
        if (visited[u] && trie[u].id != -1)
            inText[trie[u].id] = true;
    }

    for (int i = 0; i < m; i++) {
        if (inText[i])
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
    findWords(text, words);
}
