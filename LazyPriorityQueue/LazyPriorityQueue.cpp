/*
 * Priority queue that supports erasure.
 * Items are erased lazily.
 */

#include <cassert>
#include <cstddef>
#include <queue>
#include <unordered_set>
#include <vector>

template <
    typename T,
    typename Compare = std::less<T>,
    typename Container = std::vector<T>,
    typename Hash = std::hash<T>,
    typename KeyEqual = std::equal_to<T>
>
class lazy_priority_queue {
private:
    std::priority_queue<T, Container, Compare> pq;
    std::unordered_multiset<T, Hash, KeyEqual> contains;    // If hashing leads to slow runtime just use a multiset.
    std::size_t sz;

    void clean_top() {
        while (!pq.empty() && contains.find(pq.top()) == contains.end()) {
            pq.pop();
        }
    }

public:
    lazy_priority_queue() : sz(0) {}

    std::size_t size() const { return sz; }

    bool empty() const { return sz == 0; }

    void push(const T& t) {
        contains.insert(t);
        pq.push(t);
        sz++;
    }

    const T& top() {
        assert(sz > 0);
        clean_top();
        return pq.top();
    }

    void pop() {
        assert(sz > 0);
        clean_top();
        contains.erase(contains.find(pq.top()));
        pq.pop();
        sz--;
    }

    void erase(const T& t) {
        auto it = contains.find(t);
        if (it != contains.end()) {
            contains.erase(it);
            sz--;
        }
    }
};

int main() {
    lazy_priority_queue<int> pq;

    for (int i = 0; i < 10; i++) {
        pq.push(i);
    }

    assert(pq.size() == 10);

    pq.erase(1);
    assert(pq.size() == 9);

    pq.erase(-100);
    assert(pq.size() == 9);

    pq.erase(9);
    assert(pq.size() == 8);
    assert(pq.top() == 8);
}
