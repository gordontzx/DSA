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
 * Suffix Tree with O(m) construction time.
 */
class SuffixTree {
private:
    struct TrieNode {
        int left, right;
        TrieNode* next[27] = {};     // next[26] denotes sentinel character
        TrieNode* suffixLink = nullptr;

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
            TrieNode* lastInserted = nullptr;

            while (remainder > 0) {
                if (active_length == 0)
                    active_edge = i;

                TrieNode* nextNode = active_node->next[s[active_edge] - 'a'];

                // Check for rule 3
                if (nextNode != nullptr && s[nextNode->left + active_length] == s[i]) {
                    if (lastInserted != nullptr)
                        lastInserted->suffixLink = active_node;
                    lastInserted = nullptr;
                    active_length++;
                    canonize(active_node, active_edge, active_length);
                    break;
                }

                if (nextNode == nullptr) {
                    active_node->next[s[i] - 'a'] = new TrieNode(i, m - 1);
                    if (lastInserted != nullptr)
                        lastInserted->suffixLink = active_node;
                    lastInserted = nullptr;
                } else {
                    TrieNode* midNode = splitEdge(active_node, s[active_edge] - 'a', active_length, i);
                    if (lastInserted != nullptr)
                        lastInserted->suffixLink = midNode;
                    lastInserted = midNode;
                }

                remainder--;
                // Follow suffix link
                if (active_node == root && active_length > 0) {
                    active_edge++;
                    active_length--;
                } else if (active_node != root) {
                    active_node = active_node->suffixLink;
                }
                canonize(active_node, active_edge, active_length);
            }
        }
    }

    TrieNode* splitEdge(TrieNode* node, int edgeId, int offset, int leafStart) {
        TrieNode* nextNode = node->next[edgeId];

        int l = nextNode->left, r = nextNode->right;
        TrieNode* midNode = new TrieNode(l, l + offset - 1);
        midNode->next[s[l + offset] - 'a'] = nextNode;
        node->next[edgeId] = midNode;

        nextNode->left += offset;

        TrieNode* newLeaf = new TrieNode(leafStart, s.size() - 1);
        midNode->next[s[leafStart] - 'a'] = newLeaf;

        return midNode;
    }

    void canonize(TrieNode*& active_node, int& active_edge, int& active_length) {
        while (true) {
            TrieNode* nextNode = active_node->next[s[active_edge] - 'a'];
            if (nextNode == nullptr) break;

            int edgeLen = nextNode->right - nextNode->left + 1;
            if (active_length < edgeLen) break;

            active_edge += edgeLen;
            active_length -= edgeLen;
            active_node = nextNode;
        }
    }

    void print(TrieNode* node, string prefix) {
        // │ ─ ├ └
        static constexpr char NODE_SYM = '*';
        cout << NODE_SYM << '\n';

        int lastChild = -1;
        int longestLen = 0;
        for (int i = 0; i < 27; i++) {
            TrieNode* nextNode = node->next[i];
            if (nextNode != nullptr) {
                lastChild = i;
                longestLen = max(longestLen, (int) nextNode->right - nextNode->left + 1);
            }
        }

        if (lastChild == -1) return;

        for (int i = 0; i < 27; i++) {
            TrieNode* nextNode = node->next[i];
            if (nextNode == nullptr) continue;

            int edgeLen = nextNode->right - nextNode->left + 1;
            string rightPadding = repeat("─", longestLen - edgeLen + 1);
            cout << prefix << (i == lastChild ? "└─" : "├─") << convertLabel(nextNode->left, nextNode->right) << rightPadding;
            string nextPrefix = prefix + (i == lastChild ? " " : "│") + string(longestLen + 2, ' ');

            print(nextNode, nextPrefix);
        }
    }

    string convertLabel(int left, int right) {
        string res; res.reserve(right - left + 1);
        for (int i = left; i <= right; i++) {
            res += s[i] == SENTINEL ? '$' : s[i];
        }
        return res;
    }

public:
    SuffixTree(const string& s) : s(s + SENTINEL) {
        root = new TrieNode(-1, -1);
        root->suffixLink = root;
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
            TrieNode* nextNode = cur->next[word[i] - 'a'];
            if (nextNode == nullptr)
                return false;

            int j = nextNode->left;
            while (j <= nextNode->right && i < word.size()) {
                if (s[j] != word[i])
                    return false;
                j++;
                i++;
            }
            if (i == word.size())
                return true;

            cur = nextNode;
        }

        return false;
    }
};

int main() {
    string s = "abcbcdabcd";
    SuffixTree st(s);
}
