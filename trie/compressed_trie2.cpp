#include <bits/stdc++.h>
using namespace std;

class Trie {
private:
    struct TrieNode {
        TrieNode* next[26];
        string edgeLabel[26];
        bool isTerminal;
    };

    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word) {
        TrieNode* cur = root;
        int i = 0;

        while (i < word.size()) {
            int idx = word[i] - 'a';

            // Edge does not exist
            if (cur->next[idx] == nullptr) {
                cur->next[idx] = new TrieNode();
                cur->edgeLabel[idx] = word.substr(i);
                cur = cur->next[idx];
                break;
            }

            const string& label = cur->edgeLabel[idx];
            int j = 0;
            for (; j < label.size() && i < word.size(); i++, j++) {
                if (word[i] != label[j])
                    break;
            }

            // Matches full edge label
            if (j == label.size()) {
                cur = cur->next[idx];
                continue;
            }

            TrieNode* mid = new TrieNode();
            mid->next[label[j] - 'a'] = cur->next[idx];
            mid->edgeLabel[label[j] - 'a'] = label.substr(j);
            cur->next[idx] = mid;
            cur->edgeLabel[idx] = label.substr(0, j);

            cur = mid;
        }

        cur->isTerminal = true;
    }

    bool hasWord(const string& word) {
        TrieNode* cur = root;
        int i = 0;

        while (i < word.size()) {
            int idx = word[i] - 'a';

            if (cur->next[idx] == nullptr)
                return false;

            const string& label = cur->edgeLabel[idx];
            int j = 0;
            for (; j < label.size() && i < word.size(); i++, j++) {
                if (word[i] != label[j])
                    return false;
            }

            if (i == word.size() && j != label.size())
                return false;

            cur = cur->next[idx];
        }

        return cur->isTerminal;
    }

    bool hasPrefix(const string& prefix) {
        TrieNode* cur = root;
        int i = 0;

        while (i < prefix.size()) {
            int idx = prefix[i] - 'a';

            if (cur->next[idx] == nullptr)
                return false;

            const string& label = cur->edgeLabel[idx];
            int j = 0;
            for (; j < label.size() && i < prefix.size(); i++, j++) {
                if (prefix[i] != label[j])
                    return false;
            }

            cur = cur->next[idx];
        }

        return true;
    }
};

void test_trie() {
    const vector<string> words = {
        "apple",
        "banana",
        "app",
        "bob"
    };

    Trie trie;

    // Insert words into trie
    cout << "Inserting words:\n";
    for (const string& word : words) {
        cout << word << '\n';
        trie.insert(word);
    }
    cout << '\n';

    // Check that inserted words are in trie
    for (const string& word : words) {
        if (!trie.hasWord(word)) {
            cout << word << " not in trie!\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_trie();
}
