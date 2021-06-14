#pragma once
#include <iterator>

template <typename T>
struct list
{
    template <typename V>
    struct basic_iterator;

    using iterator = basic_iterator<T>;
    using const_iterator = basic_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list();
    list(list const&);
    list& operator=(list);

    bool empty() const;

    T& front();
    T const& front() const;

    void push_front(T const&);
    void pop_front();

    T& back();
    T const& back() const;

    void push_back(T const&);
    void pop_back();

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    void clear();

    iterator insert(const_iterator pos, T const& val);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);

    friend void swap(list& a, list& b)
    {
        using std::swap;
        swap(a.fake.next, b.fake.next);
        swap(a.fake.prev, b.fake.prev);

        a.fake.next->prev = &a.fake;
        a.fake.prev->next = &a.fake;

        b.fake.next->prev = &b.fake;
        b.fake.prev->next = &b.fake;
    }

private:
    struct node
    {
        node(node* next, node* prev);

        node* next;
        node* prev;
    };

    struct vnode : node
    {
        vnode(node* next, node* prev, T const& value);
        T value;
    };

private:
    mutable node fake;
};

template <typename T>
list<T>::list()
    : fake(&fake, &fake)
{}

template <typename T>
list<T>::list(list const& other)
    : list()
{
    for (auto const& e : other)
        push_back(e);
}

template <typename T>
list<T>& list<T>::operator=(list other)
{
    swap(other, *this);
    other.clear();
    return *this;
}

template <typename T>
bool list<T>::empty() const
{
    return fake.next == &fake;
}

template <typename T>
T& list<T>::front()
{
    return *begin();
}

template <typename T>
T const& list<T>::front() const
{
    return *begin();
}

template <typename T>
void list<T>::push_front(T const& val)
{
    insert(begin(), val);
}

template <typename T>
void list<T>::pop_front()
{
    erase(begin());
}

template <typename T>
T& list<T>::back()
{
    return *std::prev(end());
}

template <typename T>
T const& list<T>::back() const
{
    return *std::prev(end());
}

template <typename T>
void list<T>::push_back(T const& val)
{
    insert(end(), val);
}

template <typename T>
void list<T>::pop_back()
{
    erase(std::prev(end()));
}

template <typename T>
typename list<T>::iterator list<T>::begin()
{
    return iterator(fake.next);
}

template <typename T>
typename list<T>::const_iterator list<T>::begin() const
{
    return const_iterator(fake.next);
}

template <typename T>
typename list<T>::iterator list<T>::end()
{
    return iterator(&fake);
}

template <typename T>
typename list<T>::const_iterator list<T>::end() const
{
    return const_iterator(&fake);
}

template <typename T>
typename list<T>::reverse_iterator list<T>::rbegin()
{
    return reverse_iterator(end());
}

template <typename T>
typename list<T>::const_reverse_iterator list<T>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <typename T>
typename list<T>::reverse_iterator list<T>::rend()
{
    return reverse_iterator(begin());
}

template <typename T>
typename list<T>::const_reverse_iterator list<T>::rend() const
{
    return const_reverse_iterator(begin());
}

template <typename T>
void list<T>::clear()
{
    erase(begin(), end());
}

template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, T const& val)
{
    vnode* n = new vnode(pos.p, pos.p->prev, val);
    n->next->prev = n;
    n->prev->next = n;

    return iterator(n);
}

template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator pos)
{
    node* old = pos.p;
    node* next = old->next;
    old->prev->next = old->next;
    old->next->prev = old->prev;

    delete static_cast<vnode*>(old);
    return iterator(next);
}

template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last)
{
    while (first != last)
        first = erase(first);

    return iterator(last.p);
}

template <typename T>
void list<T>::splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
{
    auto triswap = [](node*& a, node*& b, node*& c)
    {
        node* tmp = a;
        a = b;
        b = c;
        c = tmp;
    };

    triswap(pos.p->prev->next, first.p->prev->next, last.p->prev->next);
    triswap(pos.p->prev, last.p->prev, first.p->prev);
}

template <typename T>
list<T>::node::node(node* next, node* prev)
    : next(next)
    , prev(prev)
{}

template <typename T>
list<T>::vnode::vnode(node* next, node* prev, T const& value)
    : node(next, prev)
    , value(value)
{}

template <typename T>
template <typename U>
struct list<T>::basic_iterator
{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = U;
    using difference_type = std::ptrdiff_t;
    using pointer = U*;
    using reference = U&;

    basic_iterator() = default;
    basic_iterator(basic_iterator const&) = default;

    template <typename V, typename = std::enable_if_t<std::is_const_v<U> && !std::is_const_v<V>>>
    basic_iterator(basic_iterator<V> const&);

    U& operator*() const;
    U* operator->() const;

    iterator& operator++() &;
    iterator operator++(int) &;

    iterator& operator--() &;
    iterator operator--(int) &;

    friend bool operator==(basic_iterator const& lhs, basic_iterator const& rhs)
    {
        return lhs.p == rhs.p;
    }

    friend bool operator!=(basic_iterator const& lhs, basic_iterator const& rhs)
    {
        return !(lhs == rhs);
    }

private:
    explicit basic_iterator(node* p);

private:
    node* p;

    friend struct list<T>;
};

template <typename T>
template <typename U>
template <typename V, typename>
list<T>::basic_iterator<U>::basic_iterator(basic_iterator<V> const& other)
    : p(other.p)
{}

template <typename T>
template <typename U>
U& list<T>::basic_iterator<U>::operator*() const
{
    return static_cast<vnode*>(p)->value;
}

template <typename T>
template <typename U>
U* list<T>::basic_iterator<U>::operator->() const
{
    return &static_cast<vnode*>(p)->value;
}

template <typename T>
template <typename U>
typename list<T>::iterator& list<T>::basic_iterator<U>::operator++() &
{
    p = p->next;
}

template <typename T>
template <typename U>
typename list<T>::iterator list<T>::basic_iterator<U>::operator++(int) &
{
    iterator res(*this);
    ++*this;
    return res;
}

template <typename T>
template <typename U>
typename list<T>::iterator& list<T>::basic_iterator<U>::operator--() &
{
    p = p->prev;
}

template <typename T>
template <typename U>
typename list<T>::iterator list<T>::basic_iterator<U>::operator--(int) &
{
    iterator res(*this);
    --*this;
    return res;
}

template <typename T>
template <typename U>
list<T>::basic_iterator<U>::basic_iterator(node* p)
    : p(p)
{}
