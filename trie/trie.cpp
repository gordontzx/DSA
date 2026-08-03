#include <bits/stdc++.h>
#include <cassert>
using namespace std;

class Trie {
private:
    struct TrieNode {
        int next[26] = {};
        bool is_terminal = false;
    };

    static constexpr int ROOT = 0;
    int next_id = 1;
    vector<TrieNode> trie;

public:
    Trie() {
        trie.emplace_back();
    }

    Trie(int num_chars) {
        trie.reserve(1 + num_chars);
        trie.emplace_back();
    }

    void addWord(const string& word) {
        int cur = ROOT;
        for (const char c : word) {
            int next = trie[cur].next[c - 'a'];
            if (next == 0) { 
                trie.emplace_back();
                trie[cur].next[c - 'a'] = next_id;
                next = next_id++;
            }
            cur = next;
        }
        trie[cur].is_terminal = true;
    }

    bool has_word(const string& word) {
        int cur = ROOT;
        for (const char c : word) {
            int next = trie[cur].next[c - 'a'];
            if (next == 0)
                return false;
            cur = next;
        }
        return trie[cur].is_terminal;
    }

    bool has_prefix(const string& word) {
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

    assert(t.has_word("asldfjl") && "trie does not contain word in dict");
    assert(t.has_word("aword") && "trie does not contain word in dict");
    assert(t.has_word("boo") && "trie does not contain word in dict");
    assert(t.has_word("hohoho") && "trie does not contain word in dict");

    assert(!t.has_word("hohoh"));
    assert(!t.has_word("no"));

    assert(t.has_prefix("asl"));
    assert(t.has_prefix("aw"));
    assert(!t.has_prefix("z"));
    assert(!t.has_prefix("booo"));
    assert(t.has_prefix("bo"));
    assert(t.has_prefix("boo"));

    cout << "All tests pass.\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_trie();
}
