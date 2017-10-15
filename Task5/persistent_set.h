#ifndef PERSISTENT_SET_H
#define PERSISTENT_SET_H

#include <iostream>
#include <iterator>
#include <cassert>
#include <memory>
using std::cout;
using  std::endl;


template <typename T>
class persistent_set
{
private:
    friend void ps_swap(persistent_set<T>& a, persistent_set<T>& b) noexcept        //просто swap подчёркивается в редакторе, и мне не нрав
    {
        std::swap(a.fake_node, b.fake_node);
    }

    struct node;
    struct base_node
    {
        base_node():
            left(nullptr)
        {}

        base_node(const std::shared_ptr<base_node>& other):
            left(other.left)
        {}

        base_node(const std::shared_ptr<node>& l):
            left(l)
        {}

        base_node(const base_node& other):
            left(other.left)
        {}


        base_node(std::shared_ptr<base_node> l):
            left(l)
        {}

        std::shared_ptr<node> left;
    };

    struct node: base_node
    {
        node() = delete;

        node(const T& a):
            base_node(),
            right(nullptr),
            data(a)
        {}
        node(const std::shared_ptr<node> &l, const std::shared_ptr<node>& r, const T& a):
            base_node(l),
            right(r),
            data(a)
        {}

        std::shared_ptr<node> right;
        T data;
    };

public:
    struct iterator
    {
        friend struct persistent_set<T>;
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
    persistent_set& operator=(persistent_set const& rhs);

    iterator find(const T& el);
    std::pair<iterator, bool> insert(const T& el);
    void erase(iterator);

    iterator begin() const;
    iterator end() const;
private:
    base_node fake_node;   //всегда существует, всегда прям самая-самая первая (последняя) нода

    iterator insert(const std::shared_ptr<node> &, const T &el);
    iterator find(std::shared_ptr<node>, const T &el);

    base_node *find_parent_of_erasable(const T &el);
    node* build_tree_instead_of_erasable(node*);
    node* build_tree_until_prev(node*, const T &);
    node* build_tree_until_next(node*, const T &);

    const T& get_data(const std::shared_ptr<base_node> &from);
};

template <typename T>
persistent_set<T>::persistent_set():
    fake_node()
{
}

template <typename T>
persistent_set<T>::persistent_set(const persistent_set &other):
    fake_node(other.fake_node)
{
}

template <typename T>
persistent_set<T>& persistent_set<T>::operator =(const persistent_set &other)
{
    fake_node.left = other.fake_node.left;
    return *this;
}

template <typename T>
const T& persistent_set<T>::get_data(const std::shared_ptr<base_node>& from){
    return static_cast<node*>(from.get())->data;
}

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::insert(const std::shared_ptr<node>& root, const T& el){
    T cur_key = root.get()->data;
    assert(el != cur_key);

    if(el < cur_key){
        if(root.get()->left){
            node *l = root.get()->left.get();
            root.get()->left.reset(new node(l->left, l->right, l->data));
            return insert(root.get()->left, el);
        }
        else{
            node *tmp = new node(el);
            root.get()->left.reset(tmp);
            return iterator(&fake_node, tmp);
        }
    }

    if(root.get()->right){
        node *r = root.get()->right.get();
        root.get()->right.reset(new node(r->left, r->right, r->data));
        return insert(root.get()->right, el);
    }
    else{
        node *tmp = new node(el);
        root.get()->right.reset(tmp);
        return iterator(&fake_node, tmp);
    }

}

template <typename T>
std::pair<typename persistent_set<T>::iterator, bool> persistent_set<T>::insert(const T& el){

    if(fake_node.left == nullptr){
        fake_node.left.reset(new node(el));
        return {iterator(&fake_node, fake_node.left.get()), true};
    }
    iterator it = find(el);

    if(it != end()){
        return {it, false};
    }
    node *l = fake_node.left.get();
    fake_node.left.reset(new node(l->left, l->right, l->data));
    return {insert(fake_node.left, el), true};

}

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::find(std::shared_ptr<node> root, const T& el){
    node *cur = root.get();
    while (cur && cur->data != el) {
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

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::find(const T& el){
    return fake_node.left ? find(fake_node.left, el) : iterator();
}

template <typename T>
typename persistent_set<T>::base_node* persistent_set<T>::find_parent_of_erasable(T const& el){
    if(fake_node.left.get()->data == el){
        return &fake_node;
    }
    node* par = new node(fake_node.left.get()->left, fake_node.left.get()->right, fake_node.left.get()->data);
    fake_node.left.reset(par);
    node* cur;
    if(el < par->data){
        cur = par->left.get();
    }
    else{
        cur = par->right.get();
    }
    while (cur->data != el) {
//        cout << cur->data << " " << par->data << endl;
        if(cur->data < el){
            par->right.reset(new node(cur->left, cur->right, cur->data));
            par = par->right.get();
            cur = par->right.get();
        }
        else{
            par->left.reset(new node(cur->left, cur->right, cur->data));
            par = par->left.get();
            cur = par->left.get();
        }

    }

    return par;
}

template <typename T>
typename persistent_set<T>::node* persistent_set<T>::build_tree_until_prev(node *st, const T& el){
    if(st == nullptr){
        return nullptr;
    }
    if(st->data != el){
        return new node(st->left, std::shared_ptr<node>(build_tree_until_next(st->right.get(), el)), st->data);
    }
    if(st->left.get()){
        node *l = st->left.get();
        return new node(l->left, l->right, l->data);
    }
    return nullptr;
}

template <typename T>
typename persistent_set<T>::node* persistent_set<T>::build_tree_until_next(node *st, const T& el){
    if(st == nullptr){
        return nullptr;
    }
    if(st->data != el){
        return new node(std::shared_ptr<node>(build_tree_until_next(st->left.get(), el)), st->right, st->data);
    }
    if(st->right.get()){
        node *r = st->right.get();
        return new node(r->left, r->right, r->data);
    }
    return nullptr;
}

template <typename T>
typename persistent_set<T>::node* persistent_set<T>::build_tree_instead_of_erasable(node *erasable){
    node* res = nullptr;
    if(erasable->left.get()){
        res= erasable->left.get();
        while (res->right.get()) {
            res = res->right.get();
        }
        res = new node(res->data);
        res->right = erasable->right;
        res->left = std::shared_ptr<node>(build_tree_until_prev(erasable->left.get(), res->data));
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
        res->right = std::shared_ptr<node>(build_tree_until_next(erasable->right.get(), res->data));
        if(res->right.get() == nullptr){
            res->right = erasable->right.get()->right;
        }
    }
    return res;
}


template <typename T>
void persistent_set<T>::erase(iterator it){
    if((find((*it)) == end())){
        return;
    }
    base_node* par = find_parent_of_erasable((*it));
    if(par->left.get() && par->left.get()->data == (*it)){
        par->left.reset(build_tree_instead_of_erasable(par->left.get()));
//        par->left.reset();

        return;
    }
    static_cast<node*>(par)->right.reset(build_tree_instead_of_erasable(static_cast<node*>(par)->right.get()));
}

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::begin() const{
    base_node const* tmp = &fake_node;
    while (tmp->left.get() != nullptr) {
        tmp = tmp->left.get();
    }
    return iterator(&fake_node, tmp);
}


template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::end() const{
    return iterator(&fake_node, &fake_node);
}

template <typename T>
typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator++() {
    node const* tmp = static_cast<node const*>(this->ptr);
    if(tmp->right.get() != nullptr){
        tmp = tmp->right.get();
        while (tmp->left) {
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

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::iterator::operator++(int) {
    iterator tmp(*this);
    ++(*this);
    return tmp;
}

template <typename T>
typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator--() {
    node const* tmp = static_cast<node const*>(this->ptr);
    if(tmp->left.get() != nullptr){
        tmp = tmp->left.get();
        while (tmp->right) {
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

template <typename T>
typename persistent_set<T>::iterator persistent_set<T>::iterator::operator--(int) {
    iterator tmp(*this);
    --(*this);
    return tmp;
}

#endif // PERSISTENT_SET_H
