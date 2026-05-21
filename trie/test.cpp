#include <bits/stdc++.h>
#include <cassert>
using namespace std;

class Trie {
private:
    struct TrieNode {
        int next[26] = {};
        bool isTerminal = false;
    };

    static constexpr int ROOT = 0;
    int nextId = 1;
    vector<TrieNode> trie;

public:
    Trie() {
        trie.emplace_back();
    }

    Trie(int numChars) {
        trie.reserve(1 + numChars);
        trie.emplace_back();
    }

    void addWord(const string& word) {
        int cur = ROOT;
        for (const char c : word) {
            int next = trie[cur].next[c - 'a'];
            if (next == 0) { 
                trie.emplace_back();
                trie[cur].next[c - 'a'] = nextId;
                next = nextId++;
            }
            cur = next;
        }
        trie[cur].isTerminal = true;
    }

    bool hasWord(const string& word) {
        int cur = ROOT;
        for (const char c : word) {
            int next = trie[cur].next[c - 'a'];
            if (next == 0)
                return false;
            cur = next;
        }
        return trie[cur].isTerminal;
    }

    bool hasPrefix(const string& word) {
        int cur = ROOT;
        for (const char c : word) {
            int next = trie[cur].next[c - 'a'];
            if (next == 0)
                return false;
            cur = next;
        }
        return true;
    }
};

void test_trie() {
    vector<string> dict = {
        "asldfjl",
        "aword",
        "boo",
        "hohoho"
    };

    Trie t;
    for (const string& word : dict) {
        t.addWord(word);
    }

    assert(t.hasWord("asldfjl") && "trie does not contain word in dict");
    assert(t.hasWord("aword") && "trie does not contain word in dict");
    assert(t.hasWord("boo") && "trie does not contain word in dict");
    assert(t.hasWord("hohoho") && "trie does not contain word in dict");

    assert(!t.hasWord("hohoh"));
    assert(!t.hasWord("no"));

    assert(t.hasPrefix("asl"));
    assert(t.hasPrefix("aw"));
    assert(!t.hasPrefix("z"));
    assert(!t.hasPrefix("booo"));
    assert(t.hasPrefix("bo"));
    assert(t.hasPrefix("boo"));

    cout << "All tests pass.\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_trie();
}
