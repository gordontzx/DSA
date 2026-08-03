#include <bits/stdc++.h>
using namespace std;

string repeat(const string& str, int count) {
    string res = "";
    for (int i = 0; i < count; i++)
        res += str;
    return res;
}

/*
 * O(m^2) algorithm
 * Implements suffix links with skip/count down-walks
 */
class SuffixTree {
private:
    struct TrieNode {
        string edge_label;
        TrieNode* next[27]; // next[26] denotes sentinel character
        TrieNode* parent;
        TrieNode* suffix_link;
        bool is_leaf = true;

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
        if (m == 0)
            return;

        TrieNode* first_leaf = new TrieNode;
        first_leaf->edge_label = s[0];
        first_leaf->parent = root;
        root->next[s[0]-'a'] = first_leaf;

        for (int i = 1; i < m; i++) {
            first_leaf->edge_label += s[i];
            TrieNode* cur = first_leaf->parent;
            TrieNode* last_inserted = nullptr;

            int l = i - first_leaf->edge_label.size() + 1;
            for (int j = 1; j <= i; j++) {
                // Follow suffix link
                if (cur->suffix_link == nullptr) {
                    l++;
                } else {
                    cur = cur->suffix_link;
                }

                // down-walks
                while (l < i) {
                    TrieNode* next_node = cur->next[s[l]-'a'];
                    int edge_len = next_node->edge_label.size();
                    if (l + edge_len > i)
                        break;
                    l += edge_len;
                    cur = next_node;
                }

                if (cur->is_leaf) {
                    cur->edge_label += s[i];
                    l -= cur->edge_label.size() - 1;
                    cur = cur->parent;
                    continue;
                }

                TrieNode* next_node = cur->next[s[l]-'a'];
                if (next_node == nullptr) {
                    TrieNode* new_leaf = new TrieNode;
                    new_leaf->edge_label = s[l];
                    new_leaf->parent = cur;
                    cur->next[s[l]-'a'] = new_leaf;

                    if (last_inserted != nullptr) {
                        last_inserted->suffix_link = cur;
                    }
                    last_inserted = nullptr;
                    continue;
                }

                const string& label = next_node->edge_label;
                if (label[i-l] == s[i]) {
                    // Rule 3
                    if (last_inserted != nullptr) {
                        last_inserted->suffix_link = cur;
                    }
                    last_inserted = nullptr;
                    continue;
                }

                TrieNode* mid_node = split_edge(cur, s[l]-'a', i-l, s[i]);
                if (last_inserted != nullptr) {
                    last_inserted->suffix_link = mid_node;
                }
                last_inserted = mid_node;
            }
        }
    }

    TrieNode* split_edge(TrieNode* node, int edge_id, int idx, char leaf_label) {
        TrieNode* next_node = node->next[edge_id];

        const string& label = next_node->edge_label;

        TrieNode* mid_node = new TrieNode;
        mid_node->is_leaf = false;
        mid_node->edge_label = label.substr(0, idx);
        mid_node->next[label[idx]-'a'] = next_node;
        mid_node->parent = node;
        node->next[edge_id] = mid_node;

        next_node->edge_label = label.substr(idx);
        next_node->parent = mid_node;

        TrieNode* new_leaf = new TrieNode;
        mid_node->next[leaf_label-'a'] = new_leaf;
        new_leaf->edge_label = leaf_label;
        new_leaf->parent = mid_node;

        return mid_node;
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
                longest_edge = max(longest_edge, (int) node->next[i]->edge_label.size());
            }
        }

        if (last_child == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (node->next[i] == nullptr) continue;

            const string& label = node->next[i]->edge_label;
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

            const string& label = cur->next[to_search[i]-'a']->edge_label;
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

            const string& label = cur->next[word[i]-'a']->edge_label;
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
    string s = "applepie";
    SuffixTree st(s);

    int m = s.size();
    for (int i = 0; i < m; i++) {
        for (int j = i; j < m; j++) {
            string to_search = s.substr(i, j-i+1);
            if (!st.contains(to_search)) {
                cout << to_search << " not in tree!\n";
            }
        }
    }
}
