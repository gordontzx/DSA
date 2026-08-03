#include <bits/stdc++.h>
using namespace std;

string repeat(string s, int count) {
    string res;
    for (int i = 0; i < count; i++) {
        res += s;
    }
    return res;
}

/*
 * Suffix Tree with O(m) construction time. Uses heap-allocated nodes.
 */
class SuffixTree {
private:
    struct TrieNode {
        int left, right;
        TrieNode* next[27] = {};     // next[26] denotes sentinel character
        TrieNode* suffix_link = nullptr;

        TrieNode(int left, int right) : left(left), right(right) {
        }

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

        TrieNode* active_node = root;
        int active_edge = -1;
        int active_length = 0;
        int remainder = 0;

        for (int i = 0; i < m; i++) {
            remainder++;
            TrieNode* last_inserted = nullptr;

            while (remainder > 0) {
                if (active_length == 0)
                    active_edge = i;

                TrieNode* next_node = active_node->next[s[active_edge] - 'a'];

                // Check for rule 3
                if (next_node != nullptr && s[next_node->left + active_length] == s[i]) {
                    if (last_inserted != nullptr)
                        last_inserted->suffix_link = active_node;
                    last_inserted = nullptr;
                    active_length++;
                    canonize(active_node, active_edge, active_length);
                    break;
                }

                if (next_node == nullptr) {
                    active_node->next[s[i] - 'a'] = new TrieNode(i, m - 1);
                    if (last_inserted != nullptr)
                        last_inserted->suffix_link = active_node;
                    last_inserted = nullptr;
                } else {
                    TrieNode* midNode = split_edge(active_node, s[active_edge] - 'a', active_length, i);
                    if (last_inserted != nullptr)
                        last_inserted->suffix_link = midNode;
                    last_inserted = midNode;
                }

                remainder--;
                // Follow suffix link
                if (active_node == root && active_length > 0) {
                    active_edge++;
                    active_length--;
                } else if (active_node != root) {
                    active_node = active_node->suffix_link;
                }
                canonize(active_node, active_edge, active_length);
            }
        }
    }

    TrieNode* split_edge(TrieNode* node, int edge_id, int offset, int leaf_start) {
        TrieNode* next_node = node->next[edge_id];

        int l = next_node->left, r = next_node->right;
        TrieNode* mid_node = new TrieNode(l, l + offset - 1);
        mid_node->next[s[l + offset] - 'a'] = next_node;
        node->next[edge_id] = mid_node;

        next_node->left += offset;

        TrieNode* new_leaf = new TrieNode(leaf_start, s.size() - 1);
        mid_node->next[s[leaf_start] - 'a'] = new_leaf;

        return mid_node;
    }

    void canonize(TrieNode*& active_node, int& active_edge, int& active_length) {
        while (true) {
            TrieNode* next_node = active_node->next[s[active_edge] - 'a'];
            if (next_node == nullptr) break;

            int edge_len = next_node->right - next_node->left + 1;
            if (active_length < edge_len) break;

            active_edge += edge_len;
            active_length -= edge_len;
            active_node = next_node;
        }
    }

    void print(TrieNode* node, string prefix) {
        // │ ─ ├ └
        static constexpr char NODE_SYM = '*';
        cout << NODE_SYM << '\n';

        int last_child = -1;
        int longest_len = 0;
        for (int i = 0; i < 27; i++) {
            TrieNode* next_node = node->next[i];
            if (next_node != nullptr) {
                last_child = i;
                longest_len = max(longest_len, (int) next_node->right - next_node->left + 1);
            }
        }

        if (last_child == -1) return;

        for (int i = 0; i < 27; i++) {
            TrieNode* next_node = node->next[i];
            if (next_node == nullptr) continue;

            int edge_len = next_node->right - next_node->left + 1;
            string right_padding = repeat("─", longest_len - edge_len + 1);
            cout << prefix << (i == last_child ? "└─" : "├─") << convert_label(next_node->left, next_node->right) << right_padding;
            string next_prefix = prefix + (i == last_child ? " " : "│") + string(longest_len + 2, ' ');

            print(next_node, next_prefix);
        }
    }

    string convert_label(int left, int right) {
        string res; res.reserve(right - left + 1);
        for (int i = left; i <= right; i++) {
            res += s[i] == SENTINEL ? '$' : s[i];
        }
        return res;
    }

public:
    SuffixTree(const string& s) : s(s + SENTINEL) {
        root = new TrieNode(-1, -1);
        root->suffix_link = root;
        build();
        print();
    }

    void print() {
        print(root, "");
    }

    bool contains(const string& word) {
        int i = 0;
        TrieNode* cur = root;

        while (i < word.size()) {
            TrieNode* next_node = cur->next[word[i] - 'a'];
            if (next_node == nullptr)
                return false;

            int j = next_node->left;
            while (j <= next_node->right && i < word.size()) {
                if (s[j] != word[i])
                    return false;
                j++;
                i++;
            }
            if (i == word.size())
                return true;

            cur = next_node;
        }

        return false;
    }
};

int main() {
    string s = "abcbcdabcd";
    SuffixTree st(s);
}
