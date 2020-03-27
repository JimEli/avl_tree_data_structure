// AVL, balanced binary search tree.
// uses smart pointers

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include <ostream>   // ostreams
#include <algorithm> // max

template<typename T>
class AVL 
{
private:
    template<typename T>
    class AVLNode 
    {
        T data;                                  // Node data element.
        unsigned height = 0;                     // Depth of node.
        std::shared_ptr<AVLNode> left = nullptr; // Child nodes.
        std::shared_ptr<AVLNode> right = nullptr;

        template<typename T> friend class AVL;
    };

    std::shared_ptr<AVLNode<T>> rootNode, emptyNode;

public:
    AVL() { rootNode = emptyNode = std::make_shared<AVLNode<T>>(); }

    bool search(T data) { return search(rootNode, data); }
    void insert(T data) { rootNode = insert(rootNode, data); }
    // Reject duplicates.
    //void insert(T data) {
      //if (!search(rootNode, data))
        //rootNode = insert(rootNode, data);
    //}
    void remove(T data) { rootNode = remove(rootNode, data); }

    // Min value from AVL is leftmost node, max is rightmost node in the tree.
    T min() 
    {
        std::shared_ptr<AVLNode<T>> node = rootNode;
        while (node->left != emptyNode)
            node = node->left;
        return node->data;
    }

    T max() 
    {
        std::shared_ptr<AVLNode<T>> node = rootNode;
        while (node->right != emptyNode)
            node = node->right;
        return node->data;
    }

    void inOrder(std::ostream& os) { inOrder(os, rootNode); }
    void preOrder(std::ostream& os) { preOrder(os, rootNode); }
    void postOrder(std::ostream& os) { postOrder(os, rootNode); }

private:
    bool search(std::shared_ptr<AVLNode<T>> node, T d) 
    {
        if (node == emptyNode)
            return false;
        if (node->data == d)
            return true;
        if (d < node->data)
            return search(node->left, d);
        else
            return search(node->right, d);
    }

    // Balance tree.
    std::shared_ptr<AVLNode<T>> insert(std::shared_ptr<AVLNode<T>> node, T d) 
    {
        if (node == emptyNode) {
            node = std::make_shared<AVLNode<T>>();
            node->data = d;
            node->left = node->right = emptyNode;
            node->height = 1;
            return node;
        }
        if (d <= node->data)
            node->left = insert(node->left, d);
        else
            node->right = insert(node->right, d);
        return balance(node);
    }

    std::shared_ptr<AVLNode<T>> remove(std::shared_ptr<AVLNode<T>> node, T d) 
    {
        std::shared_ptr<AVLNode<T>> t;

        if (node == emptyNode)
            return node;
        if (node->data == d) {
            if (node->left == emptyNode || node->right == emptyNode) {
                if (node->left == emptyNode)
                    t = node->right;
                else
                    t = node->left;
                node.reset();
                return t;
            }
            else {
                for (t = node->right; t->left != emptyNode; t = t->left);
                node->data = t->data;
                node->right = remove(node->right, t->data);
                return balance(node);
            }
        }
        if (d < node->data)
            node->left = remove(node->left, d);
        else
            node->right = remove(node->right, d);
        return balance(node);
    }

    // Updates the depth of the node in the tree.
    void setNodeHeight(std::shared_ptr<AVLNode<T>> node) 
    {
        node->height = 1 + std::max(node->left->height, node->right->height);
    }

    std::shared_ptr<AVLNode<T>> rotateLeft(std::shared_ptr<AVLNode<T>> node) 
    {
        std::shared_ptr<AVLNode<T>> leftNode = node->left;
        node->left = leftNode->right;
        leftNode->right = node;
        setNodeHeight(node);
        setNodeHeight(leftNode);
        return leftNode;
    }

    std::shared_ptr<AVLNode<T>> rotateRight(std::shared_ptr<AVLNode<T>> node) 
    {
        std::shared_ptr<AVLNode<T>> rightNode = node->right;
        node->right = rightNode->left;
        rightNode->left = node;
        setNodeHeight(node);
        setNodeHeight(rightNode);
        return rightNode;
    }

    // Balance nodes so no 2 subtrees of a node have max depth with a difference greater than 1.
    std::shared_ptr<AVLNode<T>> balance(std::shared_ptr<AVLNode<T>> node) 
    {
        setNodeHeight(node);
        if (node->left->height > node->right->height + 1) {
            if (node->left->right->height > node->left->left->height)
                node->left = rotateRight(node->left);
            node = rotateLeft(node);
        }
        else if (node->right->height > node->left->height + 1) {
            if (node->right->left->height > node->right->right->height)
                node->right = rotateLeft(node->right);
            node = rotateRight(node);
        }
        return node;
    }

    void inOrder(std::ostream& os, std::shared_ptr<AVLNode<T>> node) 
    {
        if (node == emptyNode)
            return;
        inOrder(os, node->left);
        os << node->data << " ";
        inOrder(os, node->right);
    }
    void preOrder(std::ostream& os, std::shared_ptr<AVLNode<T>> node) 
    {
        if (node == emptyNode)
            return;
        os << node->data << " ";
        preOrder(os, node->left);
        preOrder(os, node->right);
    }
    void postOrder(std::ostream& os, std::shared_ptr<AVLNode<T>> node) 
    {
        if (node == emptyNode)
            return;
        postOrder(os, node->left);
        postOrder(os, node->right);
        os << node->data << " ";
    }
};

#endif
