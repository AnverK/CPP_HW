#ifndef PERSISTENT_SET_H
#define PERSISTENT_SET_H

#include "smart_pointers/shared_ptr.h"

template <typename T, template<typename> class U = shared_ptr>
class persistent_set
{
private:
    struct node;
    struct base_node
    {
        base_node() noexcept:
            left(nullptr)
        {}

        base_node(U<node> const& l) noexcept:
            left(l)
        {}

        base_node(base_node const& other) noexcept:
            left(other.left)
        {}

        base_node(base_node&& other) noexcept:
            left(std::move(other.left))
        {}

        base_node(U<base_node> l) noexcept:
            left(std::move(l))
        {}

        U<node> left;
    };

    struct node: base_node
    {
        node() = delete;

        node(T a):
            base_node(),
            right(nullptr),
            data(std::move(a))
        {}

        node(U<node> l, U<node> r, T a):
            base_node(std::move(l)),
            right(std::move(r)),
            data(std::move(a))
        {}

        U<node> right;
        T data;
    };

public:
    struct iterator
    {
        friend struct persistent_set<T, U>;
        iterator():
            root(nullptr),
            ptr(nullptr)
        {
        }

        iterator(const iterator& it):
            root(it.root),
            ptr(it.ptr)
        {

        }

        iterator& operator++();
        iterator& operator--();

        iterator operator++(int);
        iterator operator--(int);

        friend bool operator==(iterator a, iterator b)
        {
            return ((a.ptr == b.ptr) && (a.root== b.root));
        }

        friend bool operator!=(iterator a, iterator b)
        {
            return ((a.ptr != b.ptr) || (a.root != b.root));
        }

        T const& operator*() const
        {
            return static_cast<node const*>(ptr)->data;
        }

    private:
        iterator(base_node const* r, base_node const* p) :
            root(r), ptr(p)
        {
        }

        base_node const* root;
        base_node const* ptr;
    };

    persistent_set();
    persistent_set(persistent_set const&);
    persistent_set(persistent_set&&);
    persistent_set& operator=(persistent_set const&);
    persistent_set& operator=(persistent_set&&);

    iterator find(const T& el);
    std::pair<iterator, bool> insert(const T& el);
    void erase(iterator);

    iterator begin() const;
    iterator end() const;

private:
    base_node fake_node;

    node* insert(const U<node> &, const T &el);
    iterator find(U<node>, const T &el);

    base_node *find_parent_of_erasable(const T &el);
    node* build_tree_instead_of_erasable(node*);
    node* build_tree_until_prev(node*, const T &);
    node* build_tree_until_next(node*, const T &);

};

template <typename T, template<typename> class U>
void swap(persistent_set<T, U>& a, persistent_set<T, U>& b) noexcept
{
    std::swap(a.fake_node, b.fake_node);
}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set():
    fake_node()
{
}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set(const persistent_set &other):
    fake_node(other.fake_node)
{
}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set(persistent_set&& other):
    fake_node(std::move(other.fake_node))
{
}

template <typename T, template<typename> class U>
persistent_set<T, U>& persistent_set<T, U>::operator =(persistent_set const& other)
{
    fake_node.left = other.fake_node.left;
    return *this;
}

template <typename T, template<typename> class U>
persistent_set<T, U>& persistent_set<T, U>::operator =(persistent_set&& other)
{
    fake_node.left = std::move(other.fake_node.left);
    return *this;
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::node* persistent_set<T, U>::insert(const U<node>& root, const T& el){
    T cur_key = root.get()->data;
    if(el < cur_key){
        if(root.get()->left.get()){
            node *l = root.get()->left.get();
            root.get()->left= U<node>(new node(l->left, l->right, l->data));
            return insert(root.get()->left, el);
        }
        else{
            node *tmp = new node(el);
            root.get()->left= U<node>(tmp);
            return tmp;
        }
    }
    if(root.get()->right.get()){
        node *r = root.get()->right.get();
        root.get()->right= U<node>(new node(r->left, r->right, r->data));
        return insert(root.get()->right, el);
    }
    else{
        node *tmp = new node(el);
        root.get()->right= U<node>(tmp);
        return tmp;
    }

}

template <typename T, template<typename> class U>

std::pair<typename persistent_set<T, U>::iterator, bool> persistent_set<T, U>::insert(const T& el){

    if(fake_node.left.get() == nullptr){
        fake_node.left= U<node>(new node(el));
        return {iterator(&fake_node, fake_node.left.get()), true};
    }

    iterator it = find(el);
    if(it != end()){
        return {it, false};
    }
    node *l = fake_node.left.get();
    fake_node.left= U<node>(new node(l->left, l->right, l->data));
    return {iterator(&fake_node, insert(fake_node.left, el)), true};

}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::find(U<node> root, const T& el){
    node *cur = root.get();
    while (cur != nullptr && cur->data != el) {
        if(el < cur->data){
            cur = cur->left.get();
        }
        else{
            cur = cur->right.get();
        }
    }
    if(cur){
        return iterator(&fake_node, cur);
    }
    return iterator(&fake_node, &fake_node);
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::find(const T& el){
    return fake_node.left.get() ? find(fake_node.left, el) : iterator();
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::base_node* persistent_set<T, U>::find_parent_of_erasable(T const& el){
    if(fake_node.left.get()->data == el){
        return &fake_node;
    }
    node* par = new node(fake_node.left.get()->left, fake_node.left.get()->right, fake_node.left.get()->data);
    fake_node.left= U<node>(par);
    node* cur;
    if(el < par->data){
        cur = par->left.get();
    }
    else{
        cur = par->right.get();
    }
    while (cur->data != el) {
        if(cur->data < el){
            if(par->right.get() == cur){
                par->right= U<node>(new node(cur->left, cur->right, cur->data));
                par = par->right.get();
            }
            else{
                par->left= U<node>(new node(cur->left, cur->right, cur->data));
                par = par->left.get();
            }
            cur = par->right.get();
        }
        else{
            if(par->right.get() == cur){
                par->right= U<node>(new node(cur->left, cur->right, cur->data));
                par = par->right.get();
            }
            else{
                par->left= U<node>(new node(cur->left, cur->right, cur->data));
                par = par->left.get();
            }
            cur = par->left.get();
        }
    }
    return par;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::node* persistent_set<T, U>::build_tree_until_prev(node *st, const T& el){
    if(st == nullptr){
        return nullptr;
    }

    if(st->data != el){
        U<node> r(build_tree_until_prev(st->right.get(), el));
        return new node(st->left, r, st->data);
    }
    if(st->left.get()){
        node *l = st->left.get();
        return new node(l->left, l->right, l->data);
    }
    return nullptr;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::node* persistent_set<T, U>::build_tree_until_next(node *st, const T& el){
    if(st == nullptr){
        return nullptr;
    }
    if(st->data != el){
        return new node(U<node>(build_tree_until_next(st->left.get(), el)), st->right, st->data);
    }
    if(st->right.get()){
        node *r = st->right.get();
        return new node(r->left, r->right, r->data);
    }
    return nullptr;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::node* persistent_set<T, U>::build_tree_instead_of_erasable(node *erasable){
    node* res = nullptr;
    if(erasable->left.get()){
        res = erasable->left.get();
        while (res->right.get()) {
            res = res->right.get();
        }
        res = new node(res->data);
        res->right = erasable->right;
        res->left= U<node>(build_tree_until_prev(erasable->left.get(), res->data));
        if(res->left.get() == nullptr){
            res->left = erasable->left.get()->left;
        }
    }
    else if(erasable->right.get()){
        res = erasable->right.get();
        while (res->left.get()) {
            res = res->left.get();
        }
        res = new node(res->data);
        res->left = erasable->left;
        res->right = U<node>(build_tree_until_next(erasable->right.get(), res->data));
        if(res->right.get() == nullptr){
            res->right = erasable->right.get()->right;
        }
    }
    return res;
}


template <typename T, template<typename> class U>

void persistent_set<T, U>::erase(iterator it){
    if(begin() == end() || it == end() || (find((*it)) == end())){
        return;
    }
    base_node *par = find_parent_of_erasable((*it));
    if(par->left.get() && par->left.get()->data == (*it)){
        par->left= U<node>(build_tree_instead_of_erasable(par->left.get()));
        return;
    }
    static_cast<node*>(par)->right= U<node>(build_tree_instead_of_erasable(static_cast<node*>(par)->right.get()));
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::begin() const{
    base_node const* tmp = &fake_node;
    while (tmp->left.get() != nullptr) {
        tmp = tmp->left.get();
    }
    return iterator(&fake_node, tmp);
}


template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::end() const{
    return iterator(&fake_node, &fake_node);
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator& persistent_set<T, U>::iterator::operator++() {
    node const* tmp = static_cast<node const*>(this->ptr);
    if(tmp->right.get() != nullptr){
        tmp = tmp->right.get();
        while (tmp->left.get()) {
            tmp = tmp->left.get();
        }
        this->ptr = tmp;
        return *this;
    }

    const T& key = tmp->data;
    node const* cur = static_cast<node const*>(this->root->left.get());
    base_node const* tmp2 = this->root;

    while (cur) {
        if(cur->data > key){
            tmp2 = cur;
            cur = cur->left.get();
        }
        else{
            cur = cur->right.get();
        }
    }
    this->ptr = tmp2;
    return *this;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::iterator::operator++(int) {
    iterator tmp(*this);
    ++(*this);
    return tmp;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator& persistent_set<T, U>::iterator::operator--() {
    node const* tmp = static_cast<node const*>(this->ptr);
    if(tmp->left.get() != nullptr){
        tmp = tmp->left.get();
        while (tmp->right.get()) {
            tmp = tmp->right.get();
        }
        this->ptr = tmp;
        return *this;
    }
    const T& key = tmp->data;
    node const* cur = static_cast<node const*>(this->root->left.get());
    base_node const* tmp2 = nullptr;
    while (cur) {
        if(cur->data < key){
            tmp2 = cur;
            cur = cur->right.get();
        }
        else{
            cur = cur->left.get();
        }
    }
    this->ptr = tmp2;
    return *this;
}

template <typename T, template<typename> class U>

typename persistent_set<T, U>::iterator persistent_set<T, U>::iterator::operator--(int) {
    iterator tmp(*this);
    --(*this);
    return tmp;
}

#endif // PERSISTENT_SET_H
