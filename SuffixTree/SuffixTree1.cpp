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
        string edgeLabel[27];
        bool isLeaf = true;
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
                root->edgeLabel[s[r]-'a'] = s[r];
            }
            return;
        }

        TrieNode* cur = root;
        int i = l;
        while (i < r) {
            const string& label = cur->edgeLabel[s[i]-'a'];
            if (i + label.size() >= r)
                break;
            cur = cur->next[s[i]-'a'];
            i += label.size();
        }

        if (i + cur->edgeLabel[s[i]-'a'].size() == r) {
            if (cur->next[s[i]-'a']->isLeaf) {
                cur->edgeLabel[s[i]-'a'] += s[r];
                return;
            }
            cur = cur->next[s[i]-'a'];
            i = r;

            if (cur->next[s[r]-'a'] == nullptr) {
                cur->next[s[r]-'a'] = new TrieNode;
                cur->edgeLabel[s[r]-'a'] = s[r];
                return;
            }
        }

        const string& label = cur->edgeLabel[s[i]-'a'];
        if (label[r-i] == s[r])
            return;

        splitEdge(cur, s[i]-'a', r-i, s[r]);
    }

    void splitEdge(TrieNode* node, int edgeId, int idx, char leafLabel) {
        // Insert new node right before idx on the edge

        const string& label = node->edgeLabel[edgeId];

        TrieNode* nextNode = node->next[edgeId];
        TrieNode* midNode = new TrieNode;
        midNode->isLeaf = false;
        TrieNode* newNode = new TrieNode;
        midNode->next[label[idx] - 'a'] = nextNode;
        midNode->edgeLabel[label[idx] - 'a'] = label.substr(idx);
        midNode->next[leafLabel - 'a'] = newNode;
        midNode->edgeLabel[leafLabel - 'a'] = leafLabel;

        node->next[edgeId] = midNode;
        node->edgeLabel[edgeId] = label.substr(0, idx);
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
                longestEdge = max(longestEdge, (int) node->edgeLabel[i].size());
            }
        }

        if (lastChild == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (node->next[i] == nullptr) continue;

            const string& label = node->edgeLabel[i];
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

            const string& label = cur->edgeLabel[toSearch[i]-'a'];
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

            const string& label = cur->edgeLabel[word[i]-'a'];
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
    string s = "banana";
    SuffixTree st(s);
}
