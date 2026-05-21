#include <bits/stdc++.h>
using namespace std;

class Trie {
private:
    struct TrieNode {
        int next[26] = {};
        string edgeLabel[26];
        bool isTerminal = false;
    };

    static constexpr int ROOT = 0;
    vector<TrieNode> trie;
    int nextId = 1;

    void print(int nodeId, const string prefix) {
        // │ ─ ├ └
        cout << nodeId;

        TrieNode& node = trie[nodeId];
        if (node.isTerminal)
            cout << "$";
        cout << '\n';

        int lastChild = -1;
        int longestLen = 0;
        for (int i = 0; i < 26; i++) {
            if (node.next[i]) {
                lastChild = i;
                longestLen = max(longestLen, (int) node.edgeLabel[i].size());
            }
        }

        if (lastChild == -1)
            return;

        string nextPrefix = "│" + string(longestLen+2, ' ');
        string lastPrefix(longestLen+3, ' ');
        for (int i = 0; i < 26; i++) {
            if (node.next[i] == 0) continue;

            cout << prefix << (i == lastChild ? "└─" : "├─") << node.edgeLabel[i];
            int dashes = longestLen - node.edgeLabel[i].size() + 1;
            for (int j = 0; j < dashes; j++)
                cout << "─";

            print(node.next[i], prefix + (i == lastChild ? lastPrefix : nextPrefix));
        }
    }

public:
    Trie() {
        trie.emplace_back();
    }

    Trie(int numWords) {
        trie.reserve(1 + 2 * numWords);
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
                int next = nextId++;
                trie[cur].next[idx] = next;
                string label = word.substr(i);
                trie[cur].edgeLabel[idx] = label;
                cur = next;
                break;
            }

            const string edgeLabel = trie[cur].edgeLabel[idx];
            int j = 0;
            for (; j < edgeLabel.size() && i < len; i++, j++) {
                if (word[i] != edgeLabel[j])
                    break;
            }

            // Matched full edgeLabel
            if (j == edgeLabel.size()) {
                cur = trie[cur].next[idx];
                continue;
            }

            // Matched partial edgeLabel, split edge
            trie.emplace_back();
            int mid = nextId++;
            trie[mid].next[edgeLabel[j] - 'a'] = trie[cur].next[idx];
            trie[mid].edgeLabel[edgeLabel[j] - 'a'] = edgeLabel.substr(j);
            trie[cur].next[idx] = mid;
            trie[cur].edgeLabel[idx] = edgeLabel.substr(0, j);

            cur = mid;
        }

        trie[cur].isTerminal = true;
    }

    bool hasWord(const string& word) {
        int len = word.size();

        int cur = ROOT;
        int i = 0;
        while (i < len) {
            int idx = word[i] - 'a';
            TrieNode& node = trie[cur];
            if (node.next[idx] == 0)
                return false;
            const string& edgeLabel = node.edgeLabel[idx];
            int j = 0;
            for (; j < edgeLabel.size() && i < len; j++, i++) {
                if (word[i] != edgeLabel[j])
                    return false;
            }
            if (i == len && j != edgeLabel.size())
                return false;
            cur = node.next[idx];
        }

        return trie[cur].isTerminal;
    }

    bool hasPrefix(const string& word) {
        int len = word.size();

        int cur = ROOT;
        int i = 0;
        while (i < len) {
            int idx = word[i] - 'a';
            TrieNode& node = trie[cur];
            if (node.next[idx] == 0)
                return false;

            const string& edgeLabel = node.edgeLabel[idx];
            for (int j = 0; j < edgeLabel.size() && i < len; j++, i++) {
                if (word[i] != edgeLabel[j])
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
        if (!trie.hasWord(word) || !trie.hasPrefix(word)) {
            cout << word << " not in trie!\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_trie();
}
