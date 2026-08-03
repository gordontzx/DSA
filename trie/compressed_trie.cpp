#include <bits/stdc++.h>
using namespace std;

class Trie {
private:
    struct TrieNode {
        int next[26] = {};
        string edge_label[26];
        bool is_terminal = false;
    };

    static constexpr int ROOT = 0;
    vector<TrieNode> trie;
    int next_id = 1;

    void print(int node_id, const string prefix) {
        // │ ─ ├ └
        cout << node_id;

        TrieNode& node = trie[node_id];
        if (node.is_terminal)
            cout << "$";
        cout << '\n';

        int last_child = -1;
        int longest_len = 0;
        for (int i = 0; i < 26; i++) {
            if (node.next[i]) {
                last_child = i;
                longest_len = max(longest_len, (int) node.edge_label[i].size());
            }
        }

        if (last_child == -1)
            return;

        string next_prefix = "│" + string(longest_len+2, ' ');
        string last_prefix(longest_len+3, ' ');
        for (int i = 0; i < 26; i++) {
            if (node.next[i] == 0) continue;

            cout << prefix << (i == last_child ? "└─" : "├─") << node.edge_label[i];
            int dashes = longest_len - node.edge_label[i].size() + 1;
            for (int j = 0; j < dashes; j++)
                cout << "─";

            print(node.next[i], prefix + (i == last_child ? last_prefix : next_prefix));
        }
    }

public:
    Trie() {
        trie.emplace_back();
    }

    Trie(int num_words) {
        trie.reserve(1 + 2 * num_words);
        trie.emplace_back();
    }

    void insert(const string& word) {
        int len = word.size();

        int cur = ROOT;
        int i = 0;
        while (i < len) {
            int idx = word[i] - 'a';

            // edge does not exist
            if (trie[cur].next[idx] == 0) {
                trie.emplace_back();
                int next = next_id++;
                trie[cur].next[idx] = next;
                string label = word.substr(i);
                trie[cur].edge_label[idx] = label;
                cur = next;
                break;
            }

            const string edge_label = trie[cur].edge_label[idx];
            int j = 0;
            for (; j < edge_label.size() && i < len; i++, j++) {
                if (word[i] != edge_label[j])
                    break;
            }

            // Matched full edgeLabel
            if (j == edge_label.size()) {
                cur = trie[cur].next[idx];
                continue;
            }

            // Matched partial edgeLabel, split edge
            trie.emplace_back();
            int mid = next_id++;
            trie[mid].next[edge_label[j] - 'a'] = trie[cur].next[idx];
            trie[mid].edge_label[edge_label[j] - 'a'] = edge_label.substr(j);
            trie[cur].next[idx] = mid;
            trie[cur].edge_label[idx] = edge_label.substr(0, j);

            cur = mid;
        }

        trie[cur].is_terminal = true;
    }

    bool has_word(const string& word) {
        int len = word.size();

        int cur = ROOT;
        int i = 0;
        while (i < len) {
            int idx = word[i] - 'a';
            TrieNode& node = trie[cur];
            if (node.next[idx] == 0)
                return false;
            const string& edge_label = node.edge_label[idx];
            int j = 0;
            for (; j < edge_label.size() && i < len; j++, i++) {
                if (word[i] != edge_label[j])
                    return false;
            }
            if (i == len && j != edge_label.size())
                return false;
            cur = node.next[idx];
        }

        return trie[cur].is_terminal;
    }

    bool has_prefix(const string& word) {
        int len = word.size();

        int cur = ROOT;
        int i = 0;
        while (i < len) {
            int idx = word[i] - 'a';
            TrieNode& node = trie[cur];
            if (node.next[idx] == 0)
                return false;

            const string& edge_label = node.edge_label[idx];
            for (int j = 0; j < edge_label.size() && i < len; j++, i++) {
                if (word[i] != edge_label[j])
                    return false;
            }
            cur = node.next[idx];
        }

        return true;
    }

    void print() {
        print(ROOT, "");
    }
};

void test_trie() {
    const vector<string> words = {
        "apple",
        "banana",
        "app",
        "bob",
        "askljdfl",
        "ask",
        "word",
        "wordle"
    };

    Trie trie;

    // Insert words into trie
    cout << "Inserting words:\n";
    for (const string& word : words) {
        cout << word << '\n';
        trie.insert(word);
        trie.print();
        cout << '\n';
    }

    // cout << "\nPrinting trie:\n";
    // trie.print();
    // cout << '\n';

    // Check that inserted words are in trie
    for (const string& word : words) {
        if (!trie.has_word(word) || !trie.has_prefix(word)) {
            cout << word << " not in trie!\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_trie();
}
