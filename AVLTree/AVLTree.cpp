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

    int getHeight(Node* node) {
        return node == nullptr ? -1 : node->height;
    }

    void updateHeight(Node* node) {
        if (node == nullptr) return;
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    int getWeight(Node* node) {
        return node == nullptr ? 0 : node->weight;
    }

    void updateWeight(Node* node) {
        if (node == nullptr) return;
        node->weight = 1 + getWeight(node->left) + getWeight(node->right);
    }

    void updateNode(Node* node) {
        updateHeight(node);
        updateWeight(node);
    }

    Node* rotateLeft(Node* node) {
        Node* rightChild = node->right;

        // rotate
        node->right = rightChild->left;
        rightChild->left = node;

        // Update heights and weights
        updateNode(node);
        updateNode(rightChild);

        return rightChild;
    }

    Node* rotateRight(Node* node) {
        Node* leftChild = node->left;

        // rotate
        node->left = leftChild->right;
        leftChild->right = node;

        // Update heights and weights
        updateNode(node);
        updateNode(leftChild);

        return leftChild;
    }

    int balanceFactor(Node* node) {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* getMin(Node* node) {
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

        int bf = balanceFactor(node);
        if (bf >= 2) {
            int leftBf = balanceFactor(node->left);
            if (balanceFactor(node->left) <= -1)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        } else if (bf <= -2) {
            int rightBf = balanceFactor(node->right);
            if (balanceFactor(node->right) >= 1)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        updateNode(node);
        return node;
    }

    Node* erase(Node* node, Key key) {
        if (node == nullptr) return node;

        if (comp(key, node->key)) {
            node->left = erase(node->left, key);
        } else if (comp(node->key, key)) {
            node->right = erase(node->right, key);
        } else {
            int numChildren = (node->left != nullptr) + (node->right != nullptr);
            if (node->left == nullptr || node->right == nullptr) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                node = temp;
            } else {
                Node* successor = getMin(node->right);
                node->key = successor->key;
                node->val = successor->val;
                node->right = erase(node->right, successor->key);
            }
        }

        if (node == nullptr) return node;

        int bf = balanceFactor(node);
        if (bf >= 2) {
            if (balanceFactor(node->left) <= -1)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        } else if (bf <= -2) {
            if (balanceFactor(node->right) >= 1)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        updateNode(node);
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

    AVLTree(const AVLTree& toCopy) : comp(toCopy.comp) {
        if (toCopy.root == nullptr) return;

        root = new Node(*(toCopy.root));
        std::stack<std::pair<Node*, Node*>> st;
        st.emplace(root, toCopy.root);
        while (!st.empty()) {
            auto [newNode, oldNode] = st.top(); st.pop();
            if (oldNode->left) {
                newNode->left = new Node(*(oldNode->left));
                st.emplace(newNode->left, oldNode->left);
            }
            if (oldNode->right) {
                newNode->right = new Node(*(oldNode->right));
                st.emplace(newNode->right, oldNode->right);
            }
        }
    }

    AVLTree& operator=(const AVLTree& toCopy) {
        if (this == &toCopy) return *this;

        AVLTree temp(toCopy);
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

    std::pair<Key, Value> getKth(int k) {
        k++; // Change to 1-indexed

        assert(root != nullptr && k >= 0 && k <= root->weight);

        Node* cur = root;
        while (k > 0) {
            if (k == getWeight(cur->left) + 1) {
                return {cur->key, cur->val};
            } else if (k <= getWeight(cur->left)) {
                cur = cur->left;
            } else {
                k -= getWeight(cur->left) + 1;
                cur = cur->right;
            }
        }

        return {cur->key, cur->val};
    }

    void print() {
        std::function<void(Node*)> printFunc = [=](Node* node) {
            std::cout << '(' << node->key << ": " << node->val << ")\n";
        };
        print(root, printFunc);
    }

    std::pair<Key, Value> operator[](int index) {
        return getKth(index);
    }
};

int main() {
    AVLTree<int, int> tree;
    std::vector<int> toInsert = {10, 5, 2, 7, 3, 50, 25};
    for (int num : toInsert) {
        tree.insert(num, num);
    }
    tree.print();
    std::cout << '\n';

    for (int i = 0; i < toInsert.size(); i++) {
        std::cout << tree.getKth(i) << '\n';
    }
    std::cout << '\n';

    std::vector<int> toErase = {-1, 2, 7};
    for (int num : toErase) {
        std::cout << "Erasing: " << num << '\n';
        tree.erase(num);
        tree.print();
        std::cout << '\n';
    }
}
