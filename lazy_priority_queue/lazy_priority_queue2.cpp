/*
 * Priority queue that supports erasure.
 * Items are deleted lazily.
 * Note that items sent for erasure are assumed to be in container.
 */

#include <cassert>
#include <cstddef>
#include <queue>
#include <vector>

template <
    typename T,
    typename Compare = std::less<T>,
    typename Container = std::vector<T>
>
class LazyPriorityQueue {
private:
    std::priority_queue<T, Container, Compare> pq;
    std::priority_queue<T, Container, Compare> to_erase;
    std::size_t sz;

    void clean_top() {
        while (!pq.empty() && !to_erase.empty() && pq.top() == to_erase.top()) {
            pq.pop();
            to_erase.pop();
        }
    }

public:
    LazyPriorityQueue() : sz(0) {}

    std::size_t size() const { return sz; }

    bool empty() const { return sz == 0; }

    void push(const T& t) {
        pq.push(t);
        sz++;
    }

    const T& top() {
        clean_top();
        assert(!pq.empty());
        return pq.top();
    }

    void pop() {
        clean_top();
        assert(!pq.empty());
        pq.pop();
        sz--;
    }

    void erase(const T& t) {
        // Precondition: t exists in the prioroity queue
        to_erase.push(t);
        sz--;
    }
};

int main() {
    LazyPriorityQueue<int> pq;

    pq.push(10);
    pq.push(20);
    pq.push(30);

    pq.erase(30);
    assert(pq.top() == 20);

    pq.erase(10);
    assert(pq.top() == 20);
}
