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
        string edgeLabel;
        TrieNode* next[27]; // next[26] denotes sentinel character
        TrieNode* parent;
        TrieNode* suffixLink;
        bool isLeaf = true;

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

        TrieNode* firstLeaf = new TrieNode;
        firstLeaf->edgeLabel = s[0];
        firstLeaf->parent = root;
        root->next[s[0]-'a'] = firstLeaf;

        for (int i = 1; i < m; i++) {
            firstLeaf->edgeLabel += s[i];
            TrieNode* cur = firstLeaf->parent;
            TrieNode* lastInserted = nullptr;

            int l = i - firstLeaf->edgeLabel.size() + 1;
            for (int j = 1; j <= i; j++) {
                // Follow suffix link
                if (cur->suffixLink == nullptr) {
                    l++;
                } else {
                    cur = cur->suffixLink;
                }

                // down-walks
                while (l < i) {
                    TrieNode* nextNode = cur->next[s[l]-'a'];
                    int edgeLen = nextNode->edgeLabel.size();
                    if (l + edgeLen > i)
                        break;
                    l += edgeLen;
                    cur = nextNode;
                }

                if (cur->isLeaf) {
                    cur->edgeLabel += s[i];
                    l -= cur->edgeLabel.size() - 1;
                    cur = cur->parent;
                    continue;
                }

                TrieNode* nextNode = cur->next[s[l]-'a'];
                if (nextNode == nullptr) {
                    TrieNode* newLeaf = new TrieNode;
                    newLeaf->edgeLabel = s[l];
                    newLeaf->parent = cur;
                    cur->next[s[l]-'a'] = newLeaf;

                    if (lastInserted != nullptr) {
                        lastInserted->suffixLink = cur;
                    }
                    lastInserted = nullptr;
                    continue;
                }

                const string& label = nextNode->edgeLabel;
                if (label[i-l] == s[i]) {
                    // Rule 3
                    if (lastInserted != nullptr) {
                        lastInserted->suffixLink = cur;
                    }
                    lastInserted = nullptr;
                    continue;
                }

                TrieNode* midNode = splitEdge(cur, s[l]-'a', i-l, s[i]);
                if (lastInserted != nullptr) {
                    lastInserted->suffixLink = midNode;
                }
                lastInserted = midNode;
            }
        }
    }

    TrieNode* splitEdge(TrieNode* node, int edgeId, int idx, char leafLabel) {
        TrieNode* nextNode = node->next[edgeId];

        const string& label = nextNode->edgeLabel;

        TrieNode* midNode = new TrieNode;
        midNode->isLeaf = false;
        midNode->edgeLabel = label.substr(0, idx);
        midNode->next[label[idx]-'a'] = nextNode;
        midNode->parent = node;
        node->next[edgeId] = midNode;

        nextNode->edgeLabel = label.substr(idx);
        nextNode->parent = midNode;

        TrieNode* newLeaf = new TrieNode;
        midNode->next[leafLabel-'a'] = newLeaf;
        newLeaf->edgeLabel = leafLabel;
        newLeaf->parent = midNode;

        return midNode;
    }

    void print(TrieNode* node, string prefix) {
        // │ ─ ├ └
        static constexpr char NODE_SYM = '*';

        cout << NODE_SYM << '\n';

        int lastChild = -1;
        int longestEdge = 0;
        for (int i = 0; i < 27; i++) {
            if (node->next[i] != nullptr) {
                lastChild = i;
                longestEdge = max(longestEdge, (int) node->next[i]->edgeLabel.size());
            }
        }

        if (lastChild == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (node->next[i] == nullptr) continue;

            const string& label = node->next[i]->edgeLabel;
            const string& rightPadding = repeat("─", longestEdge - label.size() + 1);

            cout << prefix << (i == lastChild ? "└─" : "├─") << convertLabel(label) << rightPadding;

            string nextPrefix = prefix + (i == lastChild ? " " : "│") + repeat(" ", longestEdge + 2);
            print(node->next[i], nextPrefix);
        }
    }

    /* Converts the sentinel character(s) to '$' */
    string convertLabel(const string& label) {
        string res = "";
        for (char c : label) {
            res += c == SENTINEL ? '$' : c;
        }
        return res;
    }

public:
    SuffixTree(string& s) : s(s + SENTINEL) {
        root = new TrieNode();
        root->isLeaf = s.empty();
        build();
        print();
    }

    void print() {
        print(root, "");
    }

    bool containsWord(const string& word) {
        // Checks if suffix tree contains full word
        string toSearch = word + SENTINEL;
        int i = 0;
        TrieNode* cur = root;

        while (i < toSearch.size()) {
            TrieNode* nextNode = cur->next[toSearch[i]-'a'];
            if (nextNode == nullptr)
                return false;

            const string& label = cur->next[toSearch[i]-'a']->edgeLabel;
            int j = 0;
            for (; j < label.size() && i < toSearch.size(); i++, j++) {
                if (toSearch[i] != label[j])
                    return false;
            }
            if (i == toSearch.size())
                return true;

            cur = nextNode;
        }

        return true;
    }

    bool contains(const string& word) {
        // check if word is a substring of s
        int i = 0;
        TrieNode* cur = root;

        while (i < word.size()) {
            TrieNode* nextNode = cur->next[word[i]-'a'];
            if (nextNode == nullptr)
                return false;

            const string& label = cur->next[word[i]-'a']->edgeLabel;
            int j = 0;
            for (; j < label.size() && i < word.size(); i++, j++) {
                if (label[j] != word[i])
                    return false;
            }
            if (i == word.size())
                return true;

            cur = nextNode;
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
            string toSearch = s.substr(i, j-i+1);
            if (!st.contains(toSearch)) {
                cout << toSearch << " not in tree!\n";
            }
        }
    }
}
