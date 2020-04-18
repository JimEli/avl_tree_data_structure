// avl-tree
#include <iostream>

template <typename T, typename A = std::allocator<T> >
class Tree
{
    struct node {
        T data;
        short depth = 1;
        size_t n = 1;
        std::shared_ptr<node> parent = nullptr;
        std::shared_ptr<node> left = nullptr;
        std::shared_ptr<node> right = nullptr;

        node() noexcept { }
        node(const T& t) noexcept : data(t) { }
        node(T&& t) noexcept : data(std::move(t)) { }

        void updateDepth() { depth = 1 + std::max(left ? left->depth : 0, right ? right->depth : 0); }
        void updateN() { n = 1 + (left ? left->n : 0) + (right ? right->n : 0); }
        short imbalance() { return (right ? right->depth : 0) - (left ? left->depth : 0); }
    };

public:
    class iterator {
        friend class Tree;

    public:
        iterator() { p = nullptr; }
        iterator(std::shared_ptr<node> p) { p = p; }
        iterator(const iterator& it) { p = it.p; }

        iterator& operator= (const iterator& it)
        {
            p = it.p;
            return *this;
        }

        bool operator== (const iterator& it) const { return p == it.p; }
        bool operator!= (const iterator& it) const { return p != it.p; }
        bool operator< (const iterator& it) const { return **this < *it; }
        bool operator> (const iterator& it) const { return **this > * it; }
        bool operator<= (const iterator& it) const { return **this <= *it; }
        bool operator>= (const iterator& it) const { return **this >= *it; }

        // pre-increment
        iterator& operator++ ()
        {
            if (p->right)
            {
                p = p->right;
                while (p->left)
                    p = p->left;
            }
            else {
                std::shared_ptr<node> before;
                do {
                    before = p;
                    p = p->parent;
                } while (p && before == p->right);
            }
            return *this;
        }
        // post-increment
        iterator operator++ (int)
        {
            iterator old(*this);
            ++(*this);
            return old;
        }

        // pre-decrement
        iterator& operator-- ()
        {
            if (p->left)
            {
                p = p->left;
                while (p->right) {
                    p = p->right;
                }
            }
            else
            {
                std::shared_ptr<node> before;
                do {
                    before = p;
                    p = p->parent;
                } while (p && before == p->left);
            }
            return *this;
        }
        // post-decrement
        iterator operator-- (int)
        {
            iterator old(*this);
            --(*this);
            return old;
        }

        T& operator* () const { return p->data; }
        T* operator-> () const { return &(p->data); }

    private:
        std::shared_ptr<node> p;
    };

    class const_iterator {
    public:
        const_iterator() { p = nullptr; }
        const_iterator(const std::shared_ptr<node> p) { p = p; }
        const_iterator(const const_iterator& it) { p = it.p; }
        const_iterator(const iterator& it) { p = it.p; }

        const_iterator& operator= (const const_iterator& it)
        {
            p = it.p;
            return *this;
        }

        bool operator== (const const_iterator& it) const { return p == it.p; }
        bool operator!= (const const_iterator& it) const { return p != it.p; }
        bool operator< (const const_iterator& it) const { return **this < *it; }
        bool operator> (const const_iterator& it) const { return **this > * it; }
        bool operator<= (const const_iterator& it) const { return **this <= *it; }
        bool operator>= (const const_iterator& it) const { return **this >= *it; }

        // pre-increment
        const_iterator& operator++ ()
        {
            if (p->right)
            {
                p = p->right;
                while (p->left) {
                    p = p->left;
                }
            }
            else
            {
                const std::shared_ptr<node> before;
                do {
                    before = p;
                    p = p->parent;
                } while (p && before == p->right);
            }
            return *this;
        }
        // post-increment
        const_iterator operator++ (int)
        {
            const_iterator old(*this);
            ++(*this);
            return old;
        }

        // pre-decrement
        const_iterator& operator-- ()
        {
            if (p->left)
            {
                p = p->left;
                while (p->right)
                    p = p->right;
            }
            else
            {
                const std::shared_ptr<node> before;

                do {
                    before = p;
                    p = p->parent;
                } while (p && before == p->left);
            }
            return *this;
        }
        // post-decrement
        const_iterator operator-- (int)
        {
            const_iterator old(*this);
            --(*this);
            return old;
        }

        const T& operator* () const { return (const T&)(p->data); }
        const T* operator-> () const { return &(p->data); }

    private:
        node const* p;
    };

    Tree() noexcept
    {
        root = std::make_shared<node>();
        root->n = 0;
    }
    Tree(const Tree& t) noexcept { *this = t; }
    Tree(Tree&& t) noexcept
    {
        root = t.root;
        t.root = alloc.allocate(1);
        alloc.construct(t.root);
        t.root->n = 0;
    }
    Tree& operator= (const Tree& t) noexcept
    {
        root = deep_copy_node(t.root);
        return *this;
    }
    Tree& operator= (Tree&& t) noexcept
    {
        clear();
        std::swap(root, t.root);
    }

    ~Tree() noexcept
    {
        clearNode(root);
        //alloc.destroy(root);
        //alloc.deallocate(root, 1);
        root.reset();
    }

    bool operator== (const Tree& t) const
    {
        const_iterator it1, it2;
        for (it1 = cbegin(), it2 = t.cbegin(); it1 != cend() && it2 != t.cend(); ++it1, ++it2)
        {
            if (*it1 != *it2)
                return false;
        }
        if (it1 == cend() && it2 == t.cend())
            return true;
        else
            return false;
    }
    bool operator!= (const Tree& t) const { return !(*this == t); }

    iterator begin()
    {
        std::shared_ptr<node> p = root;
        while (p->left)
            p = p->left;
        return iterator(p);
    }
    const_iterator begin() const
    {
        const std::shared_ptr<node> p = root;
        while (p->left)
            p = p->left;
        return const_iterator(p);
    }
    const_iterator cbegin() const
    {
        const std::shared_ptr<node> p = root;
        while (p->left)
            p = p->left;
        return const_iterator(p);
    }

    iterator end() { return iterator(root); }
    const_iterator end() const { return const_iterator(root); }
    const_iterator cend() const { return const_iterator(root); }

    T& front()
    {
        iterator b = begin();
        return *b;
    }
    const T& front() const
    {
        const_iterator b = begin();
        return *b;
    }

    T& back()
    {
        iterator b = end();
        return *(--b);
    }
    const T& back() const
    {
        const_iterator b = end();
        return *(--b);
    }

    iterator insert(const T& t)
    {
        iterator res;
        // descent the search Tree
        std::shared_ptr<node> parent = root;
        while (true)
        {
            ++parent->n;
            if (parent == root || t < parent->data)
            {
                if (parent->left)
                {
                    parent = parent->left;
                }
                else
                {
                    parent->left = std::make_shared<node>(t);
                    parent->left->parent = parent;
                    res = iterator(parent->left);
                    break;
                }
            }
            else
            {
                if (parent->right)
                {
                    parent = parent->right;
                }
                else
                {
                    parent->right = std::make_shared<node>(t);
                    parent->right->parent = parent;
                    res = iterator(parent->right);
                    break;
                }
            }
        }
        short branch_depth = 1;
        do
        {
            if (parent->depth > branch_depth)
                break;
            parent->depth = 1 + branch_depth;
            if (parent == root)
                break;
            if (parent->imbalance() < -1)
            {
                // check for double-rotation case
                if (parent->left->imbalance() > 0)
                    rotateLeft(parent->left);
                rotateRight(parent);
                break;
            }
            else if (parent->imbalance() > 1)
            {
                // check for double-rotation case
                if (parent->right->imbalance() < 0)
                    rotateRight(parent->right);
                rotateLeft(parent);
                break;
            }

            branch_depth = parent->depth;
            parent = parent->parent;
        } while (parent);
        return res;
    }

    iterator insert(T&& t)
    {
        iterator res;
        // descent the search Tree
        std::shared_ptr<node> parent = root;
        while (true)
        {
            ++parent->n;
            if (parent == root || t < parent->data)
            {
                if (parent->left)
                {
                    parent = parent->left;
                }
                else
                {
                    parent->left = alloc.allocate(1);
                    alloc.construct(parent->left, std::move(t));
                    parent->left->parent = parent;
                    res = iterator(parent->left);
                    break;
                }
            }
            else
            {
                if (parent->right)
                {
                    parent = parent->right;
                }
                else
                {
                    parent->right = alloc.allocate(1);
                    alloc.construct(parent->right, std::move(t));
                    parent->right->parent = parent;
                    res = iterator(parent->right);
                    break;
                }
            }
        }

        short branch_depth = 1;
        do
        {
            if (parent->depth > branch_depth)
                break;

            parent->depth = 1 + branch_depth;

            if (parent == root)
                break;

            if (parent->imbalance() < -1)
            {
                // check for double-rotation case
                if (parent->left->imbalance() > 0)
                    rotateLeft(parent->left);
                rotateRight(parent);
                break;
            }
            else if (parent->imbalance() > 1)
            {
                // check for double-rotation case
                if (parent->right->imbalance() < 0)
                    rotateRight(parent->right);
                rotateLeft(parent);
                break;
            }

            branch_depth = parent->depth;
            parent = parent->parent;
        } while (parent);
        return res;
    }

    iterator at(size_t i)
    {
        // bounds checking
        if (i >= size())
            throw std::out_of_range("Tree::at out-of-range");
        size_t j = i;
        std::shared_ptr<node> p = root->left;
        while (true)
        {
            if (p->left)
            {
                if (j == p->left->n)
                {
                    break;
                }
                else if (j < p->left->n)
                {
                    p = p->left;
                }
                else {
                    j -= 1 + p->left->n;
                    p = p->right;
                }
            }
            else
            {
                if (j == 0)
                {
                    break;
                }
                else
                {
                    --j;
                    p = p->right;
                }
            }
        }
        return iterator(p);
    }

    const_iterator at(size_t i) const
    {
        // bounds checking
        if (i >= size())
            throw std::out_of_range("Tree[] out-of-range");
        size_t j = i;
        const std::shared_ptr<node> p = root->left;
        while (true)
        {
            if (p->left)
            {
                if (j == p->left->n)
                {
                    break;
                }
                else if (j < p->left->n)
                {
                    p = p->left;
                }
                else
                {
                    j -= 1 + p->left->n;
                    p = p->right;
                }
            }
            else
            {
                if (j == 0)
                {
                    break;
                }
                else
                {
                    --j;
                    p = p->right;
                }
            }
        }
        return const_iterator(p);
    }

    T& operator[] (size_t i) { return *at(i); }
    const T& operator[] (size_t i) const { return *at(i); }

    iterator erase(iterator it)
    {
        iterator itn(it);
        ++itn;
        std::shared_ptr<node> p = it.p;
        std::shared_ptr<node> q;
        if (!p->left || !p->right)
            q = p;
        else
            q = itn.p;
        std::shared_ptr<node> s;
        if (q->left)
        {
            s = q->left;
            q->left = nullptr;
        }
        else
        {
            s = q->right;
            q->right = nullptr;
        }
        if (s)
            s->parent = q->parent;
        if (q == q->parent->left)
            q->parent->left = s;
        else
            q->parent->right = s;
        std::shared_ptr<node> q_parent = q->parent;
        if (q != p)
        {
            q->parent = p->parent;
            if (q->parent->left == p)
                q->parent->left = q;
            else
                q->parent->right = q;
            q->left = p->left;

            if (q->left)
                q->left->parent = q;
            q->right = p->right;
            if (q->right)
                q->right->parent = q;
            q->n = p->n;
            q->depth = p->depth;
            p->left = nullptr;
            p->right = nullptr;
        }
        if (q_parent == p)
            q_parent = q;
        std::shared_ptr<node> parent;
        for (parent = q_parent; parent; parent = parent->parent)
            --parent->n;
        for (parent = q_parent; parent; parent = parent->parent)
        {
            parent->update_depth();
            if (parent == root)
                break;
            if (parent->imbalance() < -1)
            {
                // check for double-rotation case
                if (parent->left->imbalance() > 0)
                {
                    rotateLeft(parent->left);
                }
                rotateRight(parent);
                break;
            }
            else if (parent->imbalance() > 1)
            {
                // check for double-rotation case
                if (parent->right->imbalance() < 0)
                {
                    rotateRight(parent->right);
                }
                rotateLeft(parent);
                break;
            }
        }
        alloc.destroy(p);
        alloc.deallocate(p, 1);
        return itn;
    }

    iterator find(const T& t)
    {
        std::shared_ptr<node> p = root->left;
        while (p)
        {
            if (t == p->data)
            {
                return iterator(p);
            }
            else if (t < p->data)
            {
                p = p->left;
            }
            else
            {
                p = p->right;
            }
        }
        return end();
    }

    void remove(const T& t)
    {
        iterator it = find(t);
        if (it == end())
            return;
        do {
            it = erase(it);
        } while (*it == t);
    }

    void clear() noexcept
    {
        clearNode(root);
        root->left = nullptr;
        root->n = 0;
        root->depth = 1;
    }

    void swap(Tree& t) { std::swap(root, t.root); }

    size_t size() const { return root->n; }
    bool empty() const { return root->left == nullptr; }

    A getAllocator() { return alloc; }

private:
    void rotateLeft(std::shared_ptr<node> n)
    {
        std::shared_ptr<node> tmp = n->right->left;
        if (n == n->parent->left)
        {
            n->parent->left = n->right;
        }
        else
        {
            n->parent->right = n->right;
        }

        n->right->parent = n->parent;
        n->right->left = n;
        n->parent = n->right;
        n->right = tmp;
        if (tmp)
            tmp->parent = n;
        // update ns
        n->updateN();
        n->parent->updateN();
        // update depths
        do {
            n->updateDepth();
            n = n->parent;
        } while (n);
    }

    void rotateRight(std::shared_ptr<node> n)
    {
        std::shared_ptr<node> tmp = n->left->right;
        if (n == n->parent->left)
        {
            n->parent->left = n->left;
        }
        else
        {
            n->parent->right = n->left;
        }
        n->left->parent = n->parent;
        n->left->right = n;
        n->parent = n->left;
        n->left = tmp;
        if (tmp)
            tmp->parent = n;
        // update ns
        n->updateN();
        n->parent->updateN();
        // update depths
        do {
            n->updateDepth();
            n = n->parent;
        } while (n);
    }

    std::shared_ptr<node> deepCopyNode(const std::shared_ptr<node> nd)
    {
        std::shared_ptr<node> cp_nd = alloc.allocate(1);
        alloc.construct(cp_nd, nd->data);
        cp_nd->n = nd->n;
        cp_nd->depth = nd->depth;
        if (nd->left)
        {
            cp_nd->left = deep_copy_node(nd->left);
            cp_nd->left->parent = cp_nd;
        }
        if (nd->right)
        {
            cp_nd->right = deepCopyNode(nd->right);
            cp_nd->right->parent = cp_nd;
        }
        return cp_nd;
    }

    void clearNode(std::shared_ptr<node> nd) noexcept
    {
        if (nd->left)
        {
            clearNode(nd->left);
            nd->left.reset();
        }
        if (nd->right)
        {
            clearNode(nd->right);
            nd->right.reset();
        }
    }

    typename std::allocator_traits<A>::template rebind_alloc<node> alloc;
    std::shared_ptr<node> root;
};

template <typename T, typename A = std::allocator<T> >
void swap(Tree<T, A>& t1, Tree<T, A>& t2) { t1.swap(t2); }
