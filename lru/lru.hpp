#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

class Hash {
public:
  unsigned int operator()(Integer lhs) const {
    int val = lhs.val;
    return std::hash<int>()(val);
  }
};
class Equal {
public:
  bool operator()(const Integer &lhs, const Integer &rhs) const {
    return lhs.val == rhs.val;
  }
};

namespace sjtu {
template <class T> class double_list {
private:
  struct node {
    T val;
    node *prev;
    node *next;
    node(const T &val, node *prev = nullptr, node *next = nullptr)
        : val(val), prev(prev), next(next) {}
  };

public:
  int size;
  node *head;
  node *tail;
  double_list() : size(0), head(nullptr), tail(nullptr) {}
  double_list(int size, node *head, node *tail)
      : size(size), head(head), tail(tail) {}
  // 深拷贝的拷贝构造函数
  double_list(const double_list<T> &other)
      : size(0), head(nullptr), tail(nullptr) {
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      insert_tail(*it);
    }
  }
  // 赋值运算符重载
  double_list<T> &operator=(const double_list<T> &other) {
    if (this != &other) {
      clear();
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
        insert_tail(*it);
      }
    }
    return *this;
  }
  ~double_list() { clear(); }
  class iterator {
  public:
    double_list *dl;
    node *ptr;

    iterator(double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    iterator(node *node) : ptr(node) {}
    iterator(const iterator &t) {
      if (this == &t)
        return;
      dl = t.dl;
      ptr = t.ptr;
    }
    ~iterator() = default;
    /**
     * iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->end())
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->end())
        throw "invalid";
      ptr = ptr->next;
      return *this;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      iterator temp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->begin())
        throw "invalid";
      ptr = ptr->prev;
      return temp;
    }
    /**
     * --iter
     */
    iterator &operator--() {
      iterator temp = *this;

      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->begin())
        throw "invalid";
      ptr = ptr->prev;
      return *this;
    }

    iterator operator+(int n) {
      iterator temp = *this;
      if (n < 0) {
        for (int i = 0; i < -n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->prev;
        }
      } else {
        for (int i = 0; i < n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->next;
        }
      }
      return temp;
    }
    /**
     * if the iter didn't point to a value
     * throw " invalid"
     */
    T &operator*() const {
      if (ptr == nullptr)
        throw "invalid";
      return ptr->val;
    }
    /**
     * other operation
     */
    T *operator->() const noexcept { return &(ptr->val); }
    bool operator==(const iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (dl != rhs.dl)
        return false;
      return ptr == rhs.ptr;
    }
    bool operator!=(const iterator &rhs) const {
      if (this == &rhs)
        return false;

      if (dl != rhs.dl)
        return true;
      return ptr != rhs.ptr;
    }
  };
  //  double_list also uses const interator, for the linked_hashmap
  class const_iterator {
  public:
    const double_list *dl; // 关联的双向链表
    node *ptr;             // 当前指向的节点

    const_iterator(const double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    const_iterator(const const_iterator &t) = default;
    ~const_iterator() = default;

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cend())
        throw "invalid";
      ptr = ptr->next;
      return tmp;
    }

    const_iterator &operator++() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cend())
        throw "invalid";
      ptr = ptr->next;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator temp = *this;
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cbegin())
        throw "invalid";
      ptr = ptr->prev;
      return temp;
    }

    const_iterator &operator--() {
      if (ptr == nullptr)
        throw "invalid";
      if (*this == dl->cbegin())
        throw "invalid";
      ptr = ptr->prev;
      return *this;
    }

    const_iterator operator+(int n) const {
      const_iterator temp = *this;
      if (n < 0) {
        for (int i = 0; i < -n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->prev;
        }
      } else {
        for (int i = 0; i < n; i++) {
          if (temp.ptr == nullptr)
            throw "invalid";
          temp.ptr = temp.ptr->next;
        }
      }
      return temp;
    }

    const T &operator*() const {
      if (ptr == nullptr)
        throw "invalid";
      return ptr->val;
    }

    const T *operator->() const noexcept {
      if (ptr == nullptr)
        throw "invalid";
      return &(ptr->val);
    }

    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }

    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  // begin() and end()
  const_iterator cbegin() const { return const_iterator(this, head); }

  const_iterator cend() const { return const_iterator(this, nullptr); }

  iterator begin() {
    if (head == nullptr)
      return iterator(this, nullptr);
    return iterator(this, head);
  }
  iterator end() { return iterator(this, nullptr); }

  iterator get_tail() const { return iterator(tail); }
  iterator erase(iterator pos) {
    if (pos.ptr == nullptr)
      return end();
    node *p = pos.ptr;
    node *prev = p->prev;
    node *next = p->next;

    if (prev)
      prev->next = next;
    else
      head = next;
    if (next)
      next->prev = prev;
    else
      tail = prev;

    delete p; // 释放被删除节点的内存
    size--;   // 更新链表长度

    return (next) ? iterator(this, next) : end();
  }
  /**
   * the following are operations of double list
   */
  void insert_head(const T &val) {
    node *new_node = new node(val, nullptr, head);
    if (head != nullptr)
      head->prev = new_node;
    head = new_node;
    if (tail == nullptr)
      tail = head;
    size++;
  }
  void insert_tail(const T &val) {
    node *new_node = new node(val, tail, nullptr);
    if (tail != nullptr)
      tail->next = new_node;
    tail = new_node;
    if (head == nullptr)
      head = tail;
    size++;
  }
  void delete_head() {
    if (head == nullptr)
      return;
    node *temp = head;
    head = head->next;
    if (head != nullptr)
      head->prev = nullptr;
    delete temp;
    size--;
  }
  void delete_tail() {
    if (tail == nullptr)
      return;
    node *temp = tail;
    tail = tail->prev;
    if (tail != nullptr)
      tail->next = nullptr;
    delete temp;
    size--;
  }

  bool empty() const {
    if (size == 0)
      return true;
    return false;
  }
  void clear() {
    node *cur = head;
    while (cur) {
      node *tmp = cur;
      cur = cur->next;
      delete tmp;
    }
    size = 0;
    head = tail = nullptr;
  }
};

template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key>>
class hashmap {
  const size_t INITIAL_CAPACITY = 8;
  const double LOAD_FACTOR = 0.75;

public:
  using value_type = pair<const Key, T>;

  struct node {
    value_type kv;
    int next;
    int prev;

    node(const value_type &kv, int next, int prev = -1)
        : kv(kv), next(next), prev(prev) {}

    node(const node &other) = default;
    node &operator=(const node &other) = delete;
    node &operator=(node &&other) noexcept {
      next = other.next;
      prev = other.prev;
      return *this;
    }
  };

  std::vector<int> hash_table;
  std::vector<node> data;
  int size;
  int capacity;

  hashmap() : size(0), capacity(INITIAL_CAPACITY) {
    hash_table.assign(capacity, -1);
  }
  hashmap(const hashmap &other) {
    if (this == &other)
      return;
    size = other.size;
    capacity = other.capacity;
    hash_table = other.hash_table;
    data.reserve(other.data.size());
    for (const auto &node : other.data) {
      data.emplace_back(node.kv, node.next, node.prev);
    }
  }
  ~hashmap() { clear(); }

  hashmap &operator=(const hashmap &other) {
    if (this == &other)
      return *this;

    size = other.size;
    capacity = other.capacity;
    hash_table = other.hash_table;

    data.clear();
    data.reserve(other.data.size());
    for (const auto &node : other.data) {
      data.emplace_back(node.kv, node.next, node.prev);
    }

    return *this;
  }
  class iterator {
  public:
    hashmap *hm;
    int idx = -1;

    iterator(hashmap *hm = nullptr, int idx = -1) : hm(hm), idx(idx) {}
    iterator(const iterator &t) = default;
    ~iterator() = default;

    /**
     * if point to nothing
     * throw
     */
    value_type &operator*() const {
      if (idx == -1)
        throw "invalid";
      return hm->data[idx].kv;
    }

    value_type *operator->() const noexcept { return &(operator*()); }
    bool operator==(const iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (hm != rhs.hm)
        return false;
      return idx == rhs.idx;
    }
    bool operator!=(const iterator &rhs) const {
      if (this == &rhs)
        return false;
      if (hm != rhs.hm)
        return true;
      return idx != rhs.idx;
    }
  };

  size_t get_index(const Key &key) const { return Hash()(key) % capacity; }

  void clear() {
    size = 0;
    capacity = INITIAL_CAPACITY;
    hash_table.assign(capacity, -1);
    data.clear();
  }
  /**
   * you need to expand the hashmap dynamically
   */
  void expand() {
    capacity *= 2;

    std::vector<int> new_hash_table(capacity, -1);
    std::vector<node> new_data;
    new_data.reserve(data.size()); // 预分配空间，避免不必要的重新分配

    // 重新分配节点
    for (node &n : data) {
      size_t new_idx = Hash()(n.kv.first) % capacity;
      new_data.push_back(std::move(n));
      new_hash_table[new_idx] = new_data.size() - 1;
    }

    // 更新哈希表
    hash_table = std::move(new_hash_table);
    data = std::move(new_data);
  }

  // return the end()
  iterator end() const { return iterator(const_cast<hashmap *>(this), -1); }

  iterator find(const Key &key) const {
    size_t idx = get_index(key);
    for (int i = hash_table[idx]; i != -1; i = data[i].next) {
      if (Equal()(data[i].kv.first, key))
        return iterator(const_cast<hashmap *>(this), i);
    }
    return end();
  }

  // O(1) find, expand when neccesary
  sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
    size_t idx = get_index(value_pair.first);
    for (int i = hash_table[idx]; i != -1; i = data[i].next) {
      if (Equal()(data[i].kv.first, value_pair.first)) {
        data[i].kv.second = value_pair.second;
        return sjtu::pair<iterator, bool>(iterator(this, i), false);
      }
    }

    data.push_back(node(value_pair, hash_table[idx]));
    hash_table[idx] = data.size() - 1;
    size++;

    if (size >= capacity * LOAD_FACTOR)
      expand();

    return sjtu::pair<iterator, bool>(iterator(this, data.size() - 1), true);
  }

  bool remove(const Key &key) {
    size_t idx = get_index(key);

    int *ptr = &hash_table[idx];

    while (*ptr != -1) {
      if (Equal()(data[*ptr].kv.first, key)) {
        *ptr = data[*ptr].next;
        return true;
      }
      ptr = &data[*ptr].next;
    }

    return false;
  }
};

template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key>>
class linked_hashmap {
public:
  using value_type = sjtu::pair<const Key, T>;

  // using iterator of double_list
  using iterator = typename double_list<value_type>::iterator;
  using const_iterator = typename double_list<value_type>::const_iterator;

private:
  double_list<value_type> dl;
  hashmap<Key, iterator, Hash, Equal> mapp; // 存储 {key, 对应链表迭代器}

public:
  linked_hashmap() {};
  linked_hashmap(const linked_hashmap &other) : mapp(other.mapp) {
    for (auto it = other.dl.cbegin(); it != other.dl.cend(); ++it)
      dl.insert_tail(*it);
  }
  linked_hashmap &operator=(const linked_hashmap &other) {
    if (this != &other) {
      clear();
      for (auto it = other.dl.cbegin(); it != other.dl.cend(); ++it)
        insert(*it);
    }
    return *this;
  }
  ~linked_hashmap() { clear(); }

  T &at(const Key &key) {
    auto it = mapp.find(key);
    if (it == mapp.end())
      throw std::out_of_range("Key not found");
    return it->second->second;
  }
  const T &at(const Key &key) const {
    auto it = mapp.find(key);
    if (it == mapp.end())
      throw std::out_of_range("Key not found");
    return it->second->second;
  }
  T &operator[](const Key &key) {
    auto it = mapp.find(key);
    if (it == mapp.end())
      throw std::out_of_range("Key not found");
    return it->second->second;
  }
  const T &operator[](const Key &key) const { return at(key); }

  iterator begin() { return dl.begin(); }
  const_iterator cbegin() const { return dl.cbegin(); }
  iterator end() { return dl.end(); }
  const_iterator cend() const { return dl.cend(); }

  bool empty() const { return dl.empty(); }
  size_t size() const { return dl.size; }

  void clear() {
    dl.clear();
    mapp.clear();
  }
  // similar to previous function
  sjtu::pair<iterator, bool> insert(const value_type &value) {
    auto it = mapp.find(value.first);
    if (it != mapp.end()) {
      dl.erase(it->second);
      dl.insert_tail(value);
      auto new_pos = dl.get_tail();
      it->second = new_pos;
      return {iterator(new_pos), false};
    } else {
      dl.insert_tail(value);
      auto lit = dl.get_tail();
      mapp.insert({value.first, lit});
      return {iterator(lit), true};
    }
  }

  void remove(iterator pos) {
    if (pos == end())
      throw std::out_of_range("Iterator out of range");
    mapp.remove(pos->first);
    dl.erase(pos);
  }

  size_t count(const Key &key) {
    auto it = mapp.find(key);
    if (it == mapp.end())
      return 0;
    return 1;
  }
  iterator find(const Key &key) {
    auto it = mapp.find(key);
    return it != mapp.end() ? it->second : end();
  }
};

class lru {
  using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
  using value_type = sjtu::pair<const Integer, Matrix<int>>;

private:
  int n;
  lmap lhm;

public:
  lru(int size) : n(size) {}
  ~lru() {}
  /**
   * save the value_pair in the memory
   * delete something in the memory if necessary
   */
  void save(const value_type &v) {
    auto it = lhm.find(v.first);
    if (it != lhm.end()) {
      lhm.remove(it); // 先删除旧的，再插入新的
    } else if (lhm.size() >= n) {
      lhm.remove(lhm.begin()); // 容量满了，删除最久未使用的(链表头部)
    }
    lhm.insert(v); // 插入到链表头部
  }

  /**
   * return a pointer contain the value
   */

  Matrix<int> *get(const Integer &v) {
    auto it = lhm.find(v);
    if (it == lhm.end())
      return nullptr;

    auto val = it->second;
    lhm.remove(it);       // 先删除
    lhm.insert({v, val}); // 再插入到链表头部
    return &(lhm.find(v)->second);
  }

  void print() {
    for (auto it = lhm.begin(); it != lhm.end(); ++it) {
      std::cout << it->first.val << " " << it->second << std::endl;
    }
  }
};
}; // namespace sjtu

#endif