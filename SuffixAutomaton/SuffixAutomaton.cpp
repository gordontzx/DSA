#include <bits/stdc++.h>
using namespace std;

class SuffixAutomaton {
private:
    struct State {
        int next[26];
        int len;
        int link;

        State(int len = 0, int link = -1) : len(len), link(link) {
            fill(begin(next), end(next), -1);
        }
    };

    vector<State> states;
    string s;

    void build() {
        int n = s.size();
        int last = 0;

        for (int i = 0; i < n; i++) {
            int ch = s[i]-'a';

            int cur = states.size();
            states.emplace_back(states[last].len + 1);
            int p = last;

            while (p != -1 && states[p].next[ch] == -1) {
                states[p].next[ch] = cur;
                p = states[p].link;
            }

            if (p == -1) {
                states[cur].link = 0;
            } else {
                int q = states[p].next[ch];
                if (states[q].len == states[p].len + 1) {
                    states[cur].link = q;
                } else {
                    int clone = states.size();
                    states.push_back(states[q]);
                    states[clone].len = states[p].len + 1;

                    while (p != -1 && states[p].next[ch] == q) {
                        states[p].next[ch] = clone;
                        p = states[p].link;
                    }

                    states[q].link = states[cur].link = clone;
                }
            }

            last = cur;
        }
    }

public:
    SuffixAutomaton(const string& s) : s(s) {
        states.reserve(2 * s.size());
        states.emplace_back();
        build();
    }

    bool contains(string_view word) {
        int cur = 0;
        for (char c : word) {
            cur = states[cur].next[c-'a'];
            if (cur == -1)
                return false;
        }

        return true;
    }
};

int main() {
    string s = "abcdabcx";
    SuffixAutomaton sa(s);
    vector<string> words = {
        "abc", "dab", "x", "c", "abcd"
    };

    for (string_view word : words) {
        if (!sa.contains(word)) {
            cout << "Automaton does not contain " << word << '\n';
        }
    }
}
