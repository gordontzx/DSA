#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>
#include <utility>

// Allows duplicate keys
template <
    typename Key,
    typename Value,
    typename Compare = std::less<Key>
>
class RedBlackTree {
private:
    static constexpr int8_t RED = 0;
    static constexpr int8_t BLACK = 1;

    static constexpr bool LEFT = 0;
    static constexpr bool RIGHT = 1;

    struct Node {
        Key key;
        Value val;
        int8_t color;
        int weight;
        Node* child[2];

        Node(Key key, Value val, int8_t color = RED, int weight = 1)
            : key(key), val(val), color(color), weight(1) {}

        Node(const Node* to_copy)
            : key(to_copy->key), val(to_copy->val), color(to_copy->color),
              weight(to_copy->weight), child(to_copy->child) {}
    };

    Node* root;
    Compare comp;

    bool is_red(Node* node) {
        return node != nullptr && node->color == RED;
    }

    void flip_color(Node* node) {
        node->color = RED;
        node->child[LEFT]->color = node->child[RIGHT]->color = BLACK;
    }

    int get_weight(Node* node) {
        return node == nullptr ? 0 : node->weight;
    }

    void update_weight(Node* node) {
        if (node == nullptr) return;
        node->weight = 1 + get_weight(node->child[LEFT]) + get_weight(node->child[RIGHT]);
    }

    Node* rotate(Node* node, bool dir) {
        Node* temp = node->child[!dir];
        node->child[!dir] = temp->child[dir];
        temp->child[dir] = node;

        temp->color = node->color;
        node->color = RED;

        update_weight(node);
        update_weight(temp);

        return temp;
    }

    Node* fix_insert(Node* node, bool dir) {
        if (is_red(node->child[dir])) {
            // Both children red
            if (is_red(node->child[!dir])) {
                // Two reds in a row
                if (is_red(node->child[dir]->child[LEFT]) || is_red(node->child[dir]->child[RIGHT]))
                    flip_color(node);
            } else {
                // Inner child is red
                if (is_red(node->child[dir]->child[!dir]))
                    node->child[dir] = rotate(node->child[dir], dir);

                // Outer child is red
                if (is_red(node->child[dir]->child[dir]))
                    node = rotate(node, !dir);
            }
        }

        return node;
    }

    Node* insert(Node* node, Key key, Value val) {
        if (node == nullptr)
            return new Node(key, val);

        bool dir = !comp(key, node->key);   // LEFT = 0, RIGHT = 1
        node->child[dir] = insert(node->child[dir], key, val);
        update_weight(node);
        return fix_insert(node, dir);
    }

    Node* get_min(Node* node) {
        Node* res = node;
        while (res->child[LEFT] != nullptr)
            res = res->child[LEFT];
        return res;
    }

    Node* fix_erase(Node* node, bool dir, bool& fixed) {
        Node* parent = node;
        Node* sibling = node->child[!dir];

        bool is_red_sibling = false;
        if (is_red(sibling)) {
            node = rotate(node, dir);
            sibling = parent->child[!dir];
            is_red_sibling = true;
        }

        // Both children of sibling are black
        if (!is_red(sibling->child[LEFT]) && !is_red(sibling->child[RIGHT])) {
            if (is_red(parent))
                fixed = true;
            sibling->color = RED;
            parent->color = BLACK;
        } else { // At least 1 child of sibling is red
            bool init_parent_color = parent->color;

            // Outer child is black
            if (!is_red(sibling->child[!dir]))
                parent->child[!dir] = rotate(parent->child[!dir], !dir);
            parent = rotate(parent, dir);

            parent->color = init_parent_color;
            parent->child[LEFT]->color = parent->child[RIGHT]->color = BLACK;

            if (is_red_sibling)
                node->child[dir] = parent;
            else
                node = parent;

            fixed = true;
        }

        return node;
    }

    Node* erase(Node* node, Key toDelete, bool& fixed) {
        if (node == nullptr) {
            fixed = true;
            return nullptr;
        }

        bool is_equal = !comp(toDelete, node->key) && !comp(node->key, toDelete);
        if (is_equal) {
            // 0 or 1 child
            if (node->child[LEFT] == nullptr || node->child[RIGHT] == nullptr) {
                Node* child = nullptr;
                if (node->child[LEFT] != nullptr) child = node->child[LEFT];
                if (node->child[RIGHT] != nullptr) child = node->child[RIGHT];

                if (is_red(node)) {
                    fixed = true;
                } else if (is_red(child)) {
                    child->color = BLACK;
                    fixed = true;
                }

                delete node;
                return child;

            } else { // 2 children
                Node* successor = get_min(node->child[RIGHT]);
                node->key = successor->key;
                node->val = successor->val;
                toDelete = successor->key;
            }
        }

        bool dir = !comp(toDelete, node->key);  // LEFT = 0, RIGHT = 1
        node->child[dir] = erase(node->child[dir], toDelete, fixed);
        update_weight(node);
        return fixed ? node : fix_erase(node, dir, fixed);
    }

    void free_tree(Node* node) {
        if (node == nullptr) return;
        free_tree(node->child[LEFT]);
        free_tree(node->child[RIGHT]);
        delete node;
    }

    Node* copy_tree(const Node* node) {
        if (node == nullptr) return nullptr;
        return new Node(node);
    }

public:
    RedBlackTree(Compare comp = Compare{}) : comp(comp), root(nullptr) {}

    ~RedBlackTree() {
        free_tree(root);
    }

    RedBlackTree(const RedBlackTree& to_copy)
        : root(copy_tree(to_copy.root)), comp(to_copy.comp) {}

    RedBlackTree& operator=(const RedBlackTree& toCopy) {
        if (this == &toCopy) return *this;

        free_tree(root);
        root = copy_tree(toCopy.root);
        comp = toCopy.comp;

        return *this;
    }

    RedBlackTree(RedBlackTree&& to_move) noexcept
        : root(to_move.root), comp(std::move(to_move.comp)) {
        to_move.root = nullptr;
    }

    RedBlackTree& operator=(RedBlackTree&& to_move) noexcept {
        if (this == &to_move) return *this;

        free_tree(root);
        root = nullptr;

        std::swap(root, to_move.root);
        std::swap(comp, to_move.comp);

        return *this;
    }

    void insert(Key key, Value val) {
        root = insert(root, key, val);
        root->color = BLACK;
    }

    void erase(Key toDelete) {
        bool fixed = false;
        root = erase(root, toDelete, fixed);
        if (root != nullptr)
            root->color = BLACK;
    }

    std::pair<Key, Value> operator[](std::size_t k) {
        k++; // Change to 1-indexed

        if (root == nullptr || k < 1 || k > root->weight) {
            throw std::out_of_range("Index out of bounds");
        }

        Node* cur = root;
        while (k > 0) {
            int left_weight = get_weight(cur->child[LEFT]);
            if (k == 1 + left_weight) {
                return {cur->key, cur->val};
            } else if (k <= left_weight) {
                cur = cur->child[LEFT];
            } else {
                k -= 1 + left_weight;
                cur = cur->child[RIGHT];
            }
        }

        // Should not reach this line
        assert(false);
    }

    std::size_t size() {
        return root == nullptr ? 0 : root->weight;
    }

    std::size_t rank(Key key) {
        // Returns the number of keys that are strictly smaller than `key`
        // throws an exception if key is not found
        Node* found = nullptr;
        std::size_t ans = 0;

        Node* cur = root;
        std::size_t left_weight = 0;

        while (cur != nullptr) {
            bool is_equal = !comp(key, cur->key) && !comp(cur->key, key);
            if (is_equal) {
                found = cur;
                ans = left_weight + get_weight(cur->child[LEFT]);
            }

            if (comp(cur->key, key)) {
                left_weight += get_weight(cur->child[LEFT]) + 1;
                cur = cur->child[RIGHT];
            } else {
                cur = cur->child[LEFT];
            }
        }

        if (found == nullptr)
            throw std::invalid_argument("Key not found");

        return ans;
    }
};
