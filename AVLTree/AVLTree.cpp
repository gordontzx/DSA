#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <stack>
#include <utility>
#include <vector>

template <typename T, typename U>
std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& p) {
    out << "(" << p.first << ", " << p.second << ")";
    return out;
}

template <
    typename Key,
    typename Value,
    typename Compare = std::less<Key>
>
class AVLTree {
private:
    struct Node {
        Key key;
        Value val;
        Node* left;
        Node* right;
        int height;
        int weight;

        Node(Key key, Value val, int height = 0, int weight = 1)
            : key(key), val(val), height(height), weight(weight) {}

        Node(const Node& copy) {
            key = copy.key;
            val = copy.val;
            left = copy.left;
            right = copy.right;
            height = copy.height;
            weight = copy.weight;
        }

        Node& operator=(const Node& copy) {
            key = copy.key;
            val = copy.val;
            left = copy.left;
            right = copy.right;
            height = copy.height;
            weight = copy.weight;
            return *this;
        }
    };

    Compare comp;
    Node* root = nullptr;

    int get_height(Node* node) {
        return node == nullptr ? -1 : node->height;
    }

    void update_height(Node* node) {
        if (node == nullptr) return;
        node->height = std::max(get_height(node->left), get_height(node->right)) + 1;
    }

    int get_weight(Node* node) {
        return node == nullptr ? 0 : node->weight;
    }

    void update_weight(Node* node) {
        if (node == nullptr) return;
        node->weight = 1 + get_weight(node->left) + get_weight(node->right);
    }

    void update_node(Node* node) {
        update_height(node);
        update_weight(node);
    }

    Node* rotate_left(Node* node) {
        Node* right_child = node->right;

        // rotate
        node->right = right_child->left;
        right_child->left = node;

        // Update heights and weights
        update_node(node);
        update_node(right_child);

        return right_child;
    }

    Node* rotate_right(Node* node) {
        Node* left_child = node->left;

        // rotate
        node->left = left_child->right;
        left_child->right = node;

        // Update heights and weights
        update_node(node);
        update_node(left_child);

        return left_child;
    }

    int balance_factor(Node* node) {
        if (node == nullptr) return 0;
        return get_height(node->left) - get_height(node->right);
    }

    Node* get_min(Node* node) {
        if (node == nullptr) return node;
        Node* res = node;
        while (res->left != nullptr)
            res = res->left;
        return res;
    }

    Node* insert(Node* node, Key key, Value val) {
        if (node == nullptr) return new Node(key, val);

        if (comp(key, node->key))
            node->left = insert(node->left, key, val);
        else
            node->right = insert(node->right, key, val);

        int bf = balance_factor(node);
        if (bf >= 2) {
            int left_bf = balance_factor(node->left);
            if (balance_factor(node->left) <= -1)
                node->left = rotate_left(node->left);
            return rotate_right(node);
        } else if (bf <= -2) {
            int right_bf = balance_factor(node->right);
            if (balance_factor(node->right) >= 1)
                node->right = rotate_right(node->right);
            return rotate_left(node);
        }

        update_node(node);
        return node;
    }

    Node* erase(Node* node, Key key) {
        if (node == nullptr) return node;

        if (comp(key, node->key)) {
            node->left = erase(node->left, key);
        } else if (comp(node->key, key)) {
            node->right = erase(node->right, key);
        } else {
            int num_children = (node->left != nullptr) + (node->right != nullptr);
            if (node->left == nullptr || node->right == nullptr) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                node = temp;
            } else {
                Node* successor = get_min(node->right);
                node->key = successor->key;
                node->val = successor->val;
                node->right = erase(node->right, successor->key);
            }
        }

        if (node == nullptr) return node;

        int bf = balance_factor(node);
        if (bf >= 2) {
            if (balance_factor(node->left) <= -1)
                node->left = rotate_left(node->left);
            return rotate_right(node);
        } else if (bf <= -2) {
            if (balance_factor(node->right) >= 1)
                node->right = rotate_right(node->right);
            return rotate_left(node);
        }

        update_node(node);
        return node;

    }

    void print(Node* node, std::function<void(Node*)> func) {
        if (node == nullptr) return;
        print(node->left, func);
        func(node);
        print(node->right, func);
    }

public:
    AVLTree(Compare comp = Compare{}) : comp(comp) {}

    ~AVLTree() {
        if (root == nullptr) return;

        std::stack<Node*> st;
        st.push(root);
        while (!st.empty()) {
            Node* cur = st.top(); st.pop();
            if (cur->left) st.push(cur->left);
            if (cur->right) st.push(cur->right);
            delete cur;
        }
    }

    AVLTree(const AVLTree& to_copy) : comp(to_copy.comp) {
        if (to_copy.root == nullptr) return;

        root = new Node(*(to_copy.root));
        std::stack<std::pair<Node*, Node*>> st;
        st.emplace(root, to_copy.root);
        while (!st.empty()) {
            auto [new_node, old_node] = st.top(); st.pop();
            if (old_node->left) {
                new_node->left = new Node(*(old_node->left));
                st.emplace(new_node->left, old_node->left);
            }
            if (old_node->right) {
                new_node->right = new Node(*(old_node->right));
                st.emplace(new_node->right, old_node->right);
            }
        }
    }

    AVLTree& operator=(const AVLTree& to_copy) {
        if (this == &to_copy) return *this;

        AVLTree temp(to_copy);
        std::swap(root, temp.root);
        std::swap(comp, temp.comp);
        return *this;
    }

    AVLTree(AVLTree&& other) noexcept : comp(std::move(other.comp)), root(other.root) {
        other.root = nullptr;
    }

    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this == &other) return *this;

        std::swap(root, other.root);
        std::swap(comp, other.comp);
        return *this;
    }

    void insert(Key key, Value val) {
        root = insert(root, key, val);
    }

    void erase(Key key) {
        root = erase(root, key);
    }

    std::pair<Key, Value> get_kth(int k) {
        k++; // Change to 1-indexed

        assert(root != nullptr && k >= 0 && k <= root->weight);

        Node* cur = root;
        while (k > 0) {
            if (k == get_weight(cur->left) + 1) {
                return {cur->key, cur->val};
            } else if (k <= get_weight(cur->left)) {
                cur = cur->left;
            } else {
                k -= get_weight(cur->left) + 1;
                cur = cur->right;
            }
        }

        return {cur->key, cur->val};
    }

    void print() {
        std::function<void(Node*)> print_func = [=](Node* node) {
            std::cout << '(' << node->key << ": " << node->val << ")\n";
        };
        print(root, print_func);
    }

    std::pair<Key, Value> operator[](int index) {
        return get_kth(index);
    }
};

int main() {
    AVLTree<int, int> tree;
    std::vector<int> to_insert = {10, 5, 2, 7, 3, 50, 25};
    for (int num : to_insert) {
        tree.insert(num, num);
    }
    tree.print();
    std::cout << '\n';

    for (int i = 0; i < to_insert.size(); i++) {
        std::cout << tree.get_kth(i) << '\n';
    }
    std::cout << '\n';

    std::vector<int> to_erase = {-1, 2, 7};
    for (int num : to_erase) {
        std::cout << "Erasing: " << num << '\n';
        tree.erase(num);
        tree.print();
        std::cout << '\n';
    }
}
