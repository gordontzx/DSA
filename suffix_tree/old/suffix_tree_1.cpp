#include <bits/stdc++.h>
using namespace std;

string repeat(const string& str, int count) {
    string res = "";
    for (int i = 0; i < count; i++)
        res += str;
    return res;
}

/*
 * Trivial O(m^3) algorithm
 */
class SuffixTree {
private:
    struct TrieNode {
        TrieNode* next[27]; // next[26] denotes sentinel character
        string edge_label[27];
        bool is_leaf = true;
        int start = -1;

        ~TrieNode() {
            for (int i = 0; i < 27; i++)
                delete next[i];
        }
    };

    static constexpr char SENTINEL = 'z' + 1;
    string s;
    TrieNode* root;

    /* Constructs an implicit suffix tree of `s`. */
    void build() {
        int m = s.size();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j <= i; j++) {
                extend(j, i);
            }
        }
    }

    void extend(int l, int r) {
        if (l == r) {
            if (root->next[s[r]-'a'] == nullptr) {
                root->next[s[r]-'a'] = new TrieNode;
                root->edge_label[s[r]-'a'] = s[r];
            }
            return;
        }

        TrieNode* cur = root;
        int i = l;
        while (i < r) {
            const string& label = cur->edge_label[s[i]-'a'];
            if (i + label.size() >= r)
                break;
            cur = cur->next[s[i]-'a'];
            i += label.size();
        }

        if (i + cur->edge_label[s[i]-'a'].size() == r) {
            if (cur->next[s[i]-'a']->is_leaf) {
                cur->edge_label[s[i]-'a'] += s[r];
                return;
            }
            cur = cur->next[s[i]-'a'];
            i = r;

            if (cur->next[s[r]-'a'] == nullptr) {
                cur->next[s[r]-'a'] = new TrieNode;
                cur->edge_label[s[r]-'a'] = s[r];
                return;
            }
        }

        const string& label = cur->edge_label[s[i]-'a'];
        if (label[r-i] == s[r])
            return;

        split_edge(cur, s[i]-'a', r-i, s[r]);
    }

    void split_edge(TrieNode* node, int edge_id, int idx, char leaf_label) {
        // Insert new node right before idx on the edge

        const string& label = node->edge_label[edge_id];

        TrieNode* next_node = node->next[edge_id];
        TrieNode* mid_node = new TrieNode;
        mid_node->is_leaf = false;
        TrieNode* new_node = new TrieNode;
        mid_node->next[label[idx] - 'a'] = next_node;
        mid_node->edge_label[label[idx] - 'a'] = label.substr(idx);
        mid_node->next[leaf_label - 'a'] = new_node;
        mid_node->edge_label[leaf_label - 'a'] = leaf_label;

        node->next[edge_id] = mid_node;
        node->edge_label[edge_id] = label.substr(0, idx);
    }

    void print(TrieNode* node, string prefix) {
        // │ ─ ├ └
        static constexpr char NODE_SYM = '*';

        cout << NODE_SYM << '\n';

        int last_child = -1;
        int longest_edge = 0;
        for (int i = 0; i < 27; i++) {
            if (node->next[i] != nullptr) {
                last_child = i;
                longest_edge = max(longest_edge, (int) node->edge_label[i].size());
            }
        }

        if (last_child == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (node->next[i] == nullptr) continue;

            const string& label = node->edge_label[i];
            const string& right_padding = repeat("─", longest_edge - label.size() + 1);

            cout << prefix << (i == last_child ? "└─" : "├─") << convert_label(label) << right_padding;

            string next_prefix = prefix + (i == last_child ? " " : "│") + repeat(" ", longest_edge + 2);
            print(node->next[i], next_prefix);
        }
    }

    /* Converts the sentinel character(s) to '$' */
    string convert_label(const string& label) {
        string res = "";
        for (char c : label) {
            res += c == SENTINEL ? '$' : c;
        }
        return res;
    }

public:
    SuffixTree(string& s) : s(s + SENTINEL) {
        root = new TrieNode();
        root->is_leaf = s.empty();
        build();
        print();
    }

    void print() {
        print(root, "");
    }

    bool contains_word(const string& word) {
        // Checks if suffix tree contains full word
        string to_search = word + SENTINEL;
        int i = 0;
        TrieNode* cur = root;

        while (i < to_search.size()) {
            TrieNode* next_node = cur->next[to_search[i]-'a'];
            if (next_node == nullptr)
                return false;

            const string& label = cur->edge_label[to_search[i]-'a'];
            int j = 0;
            for (; j < label.size() && i < to_search.size(); i++, j++) {
                if (to_search[i] != label[j])
                    return false;
            }
            if (i == to_search.size())
                return true;

            cur = next_node;
        }

        return true;
    }

    bool contains(const string& word) {
        // check if word is a substring of s
        int i = 0;
        TrieNode* cur = root;

        while (i < word.size()) {
            TrieNode* next_node = cur->next[word[i]-'a'];
            if (next_node == nullptr)
                return false;

            const string& label = cur->edge_label[word[i]-'a'];
            int j = 0;
            for (; j < label.size() && i < word.size(); i++, j++) {
                if (label[j] != word[i])
                    return false;
            }
            if (i == word.size())
                return true;

            cur = next_node;
        }

        return false;
    }
};

int main() {
    string s = "banana";
    SuffixTree st(s);
}
