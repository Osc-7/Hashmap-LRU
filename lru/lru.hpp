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
/******************************/
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
  /**
   * elements
   * add whatever you want
   */

  // --------------------------
  /**
   * the follows are constructors and destructors
   * you can also add some if needed.
   */
  double_list(int size, node *head, node *tail)
      : size(size), head(head), tail(tail) {}
  double_list(const double_list<T> &other) {
    if (this == &other)
      return;
    size = other.size;
    head = other.head;
    tail = other.tail;
  }
  ~double_list(){} = default;

  class iterator {
  public:
    double_list *dl;
    node *ptr;
    /**
     * elements
     * add whatever you want
     */
    // --------------------------
    /**
     * the follows are constructors and destructors
     * you can also add some if needed.
     */
    iterator(double_list *dl = nullptr, node *ptr = nullptr)
        : dl(dl), ptr(ptr) {}
    iterator(const iterator &t) {
      if (this == &t)
        return;
      dl = t.dl;
      ptr = t.ptr;
    }
    ~iterator() {
      for (node *p = dl->head; p != nullptr; p = p->next) {
        delete p;
      }
      dl = nullptr;
      ptr = nullptr;
    }
    /**
     * iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      if (ptr == nullptr)
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
      ptr = ptr->prev;
      return *this;
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
    T *operator->() const noexcept {
      if (ptr == nullptr)
        throw "invalid";
      return &(ptr->val);
    }
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
  /**
   * return an iterator to the beginning
   */
  iterator begin() {
    if (head == nullptr)
      return iterator(this, nullptr);
    return iterator(this, head);
  }
  /**
   * return an iterator to the ending
   * in fact, it returns the iterator point to nothing,
   * just after the last element.
   */
  iterator end() { return iterator(this, nullptr); }
  /**
   * if the iter didn't point to anything, do nothing,
   * otherwise, delete the element pointed by the iter
   * and return the iterator point at the same "index"
   * e.g.
   * 	if the origin iterator point at the 2nd element
   * 	the returned iterator also point at the
   *  2nd element of the list after the operation
   *  or nothing if the list after the operation
   *  don't contain 2nd elememt.
   */
  iterator erase(iterator pos) {
    if (pos.ptr == nullptr)
      return end();
    node *p = pos.ptr;
    node *prev = p->prev;
    node *next = p->next;
    if (prev != nullptr)
      prev->next = next;
    if (next != nullptr) {
      next->prev = prev;
      return iterator(this, next);
    }
    return end();
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
  /**
   * if didn't contain anything, return true,
   * otherwise false.
   */
  bool empty() {
    if (size == 0)
      return true;
    return false;
  }
};
/******************************/
template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key>>
class hashmap {
  const size_t INITIAL_CAPACITY = 8;
  const double LOAD_FACTOR = 0.75;

public:
  using value_type = pair<const Key, T>;

  struct node {
    value_type kv; // 存储键值对
    int next;      // 指向下一个节点的索引（-1 表示链表结束）
    node(const value_type &kv, int next) : kv(kv), next(next) {}
  };

  std::vector<int> hash_table;
  std::vector<node> data;
  int size;
  int capacity;
  // --------------------------

  /**
   * the follows are constructors and destructors
   * you can also add some if needed.
   */
  hashmap() : size(0), capacity(INITIAL_CAPACITY) {
    hash_table.assign(capacity, -1);
  }
  hashmap(const hashmap &other) {
    if (this == &other)
      return;
    size = other.size;
    capacity = other.capacity;
    hash_table = other.hash_table;
    data = other.data;
  }
  ~hashmap() { clear(); }

  hashmap &operator=(const hashmap &other) {
    if (this == &other)
      return *this;
    size = other.size;
    capacity = other.capacity;
    hash_table = other.hash_table;
    data = other.data;
    return *this;
  }

  class iterator {
    hashmap *hm;
    int idx = -1;

  public:
    /**
     * elements
     * add whatever you want
     */
    // --------------------------
    /**
     * the follows are constructors and destructors
     * you can also add some if needed.
     */
    iterator(hashmap *hm = nullptr, idx = -1) : hashmap(hm), idx(idx) {}
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

    /**
     * other operation
     */
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
    std::vector<int> new_data;

    // 重新分配节点
    for (const node &n : data) {
      size_t new_idx = Hash()(n.kv.first) % capacity;
      new_data.push_back(node(n.kv, new_table[new_idx]));
      new_table[new_idx] = new_data.size() - 1;
    }

    // 更新哈希表
    capacity = capacity;
    hash_table = std::move(new_hash_table);
    data = std::move(new_data);
  }

  /**
   * the iterator point at nothing
   */
  iterator end() const { return iterator(this, -1); }
  /**
   * find, return a pointer point to the value
   * not find, return the end (point to nothing)
   */
  iterator find(const Key &key) const {
    size_t idx = get_index(key);
    for (int i = hash_table[idx]; i != -1; i = data[i].next) {
      if (Equal()(data[i].kv.first, key))
        return iterator(this, i);
    }
    return end();
  }
  /**
   * already have a value_pair with the same key
   * -> just update the value, return false
   * not find a value_pair with the same key
   * -> insert the value_pair, return true
   */
  sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
    size_t idx = get_index(value_pair.first);
    for (int i = hash_table[idx]; i != -1; i = data[i].next) {
      if (Equal()(data[i].kv.first, value_pair.first)) {
        data[i].kv.second = value_pair.second;
        return sjtu::pair<iterator, bool>(iterator(i), false);
      }

      data.push_back(node(value_pair, hash_table[idx]));
      hash_table[idx] = data.size() - 1;
      size++;

      if (size >= capacity * LOAD_FACTOR)
        expand();

      return sjtu::pair<iterator, bool>(iterator(data.size() - 1), true);
    }
  }
  /**
   * the value_pair exists, remove and return true
   * otherwise, return false
   */
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
/******************************/
template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key>>
class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
public:
  typedef pair<const Key, T> value_type;
  double_list<value_type> dl;
  /**
   * elements
   * add whatever you want
   */
  // --------------------------
  class const_iterator;
  class iterator {
  public:
    linked_hashmap *lhm;
    int idx;
    /**
     * elements
     * add whatever you want
     */
    // --------------------------
    iterator(linked_hashmap *lhm = nullptr, int idx = -1)
        : lhm(lhm), idx(idx) {}
    iterator(const iterator &other) {
      if (this == &other)
        return;
      lhm = other.lhm;
      idx = other.idx;
    }
    ~iterator(){} = default;

    /**
     * iter++
     */
    iterator operator++(int) {
      iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].next;
      return temp;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].next;
      return *this;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].prev;
      return temp;
    }
    /**
     * --iter
     */
    iterator &operator--() {
      iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].prev;
    }

    /**
     * if the iter didn't point to a value
     * throw "star invalid"
     */
    value_type &operator*() const {
      if (this == nullptr)
        throw "star invalid";
      return lhm->data[idx].kv;
    }
    value_type *operator->() const noexcept {
      if (this == nullptr)
        throw "star invalid";
      return &(lhm->data[idx].kv);
    }

    /**
     * operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (lhm != rhs.lhm)
        return false;
      return idx == rhs.idx;
    }
    bool operator!=(const iterator &rhs) const {
      if (this == &rhs)
        return false;
      if (lhm != rhs.lhm)
        return true;
      return idx != rhs.idx;
    }
    bool operator==(const const_iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (lhm != rhs.lhm)
        return false;
      return idx == rhs.idx;
    }
    bool operator!=(const const_iterator &rhs) const {
      if (this == &rhs)
        return false;
      if (lhm != rhs.lhm)
        return true;
      return idx != rhs.idx;
    }
  };

  class const_iterator {
  public:
    linked_hashmap *lhm;
    int idx;

    /**
     * elements
     * add whatever you want
     */
    // --------------------------
    const_iterator(linked_hashmap *lhm, int idx) : lhm(lhm), idx(idx) {}
    const_iterator(const iterator &other) {
      if (this == &other)
        return;
      lhm = other.lhm;
      idx = other.idx;
    }

    /**
     * iter++
     */
    const_iterator operator++(int) {
      const_iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].next;
      return temp;
    }
    /**
     * ++iter
     */
    const_iterator &operator++() {
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].next;
      return *this;
    }
    /**
     * iter--
     */
    const_iterator operator--(int) {
      const_iterator temp = *this;
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].prev;
      return temp;
    }
    /**
     * --iter
     */
    const_iterator &operator--() {
      if (idx == -1)
        throw exception("invalid");
      idx = lhm->data[idx].prev;
      return *this;
    }

    /**
     * if the iter didn't point to a value
     * throw
     */
    const value_type &operator*() const {
      if (this == nullptr)
        throw "star invalid";
      return lhm->data[idx].kv;
    }
    const value_type *operator->() const noexcept {
      if (this == nullptr)
        throw "star invalid";
      return &(lhm->data[idx].kv);
    }

    /**
     * operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (lhm != rhs.lhm)
        return false;
      return idx == rhs.idx;
    }
    bool operator!=(const iterator &rhs) const {
      if (this == &rhs)
        return false;
      if (lhm != rhs.lhm)
        return true;
      return idx != rhs.idx;
    }
    bool operator==(const const_iterator &rhs) const {
      if (this == &rhs)
        return true;
      if (lhm != rhs.lhm)
        return false;
      return idx == rhs.idx;
    }
    bool operator!=(const const_iterator &rhs) const {
      if (this == &rhs)
        return false;
      if (lhm != rhs.lhm)
        return true;
      return idx != rhs.idx;
    }
  };

  linked_hashmap(){} = default;
  linked_hashmap(const linked_hashmap &other) {
    if (this == &other)
      return;
    dl = other.dl;
  }
  ~linked_hashmap(){} = default;
  linked_hashmap &operator=(const linked_hashmap &other) {
    if (this == &other)
      return *this;
    dl = other.dl;
    return *this;
  }

  /**
   * return the value connected with the Key(O(1))
   * if the key not found, throw
   */
  T &at(const Key &key) {
    auto it = this->find(key);
    if (it == -1)
      throw "invalid";
    return dl.data[it].kv.second;
  }
  const T &at(const Key &key) const {
    auto it = this->find(key);
    if (it == -1)
      throw "invalid";
    return dl.data[it].kv.second;
  }
  T &operator[](const Key &key) {
    auto it = hashmap<Key, T, Hash, Equal>::find(key);
    if (it == hashmap<Key, T, Hash, Equal>::end()) {
      auto res = hashmap<Key, T, Hash, Equal>::insert(value_type(key, T()));
      dl.insert_head(*res.first);
      return res.first->second;
    }
    return it->second;
  }
  const T &operator[](const Key &key) const {
    auto it = this->find(key);
    if (it == this->end())
      throw "invalid";
    return it->second;
  }

  /**
   * return an iterator point to the first
   * inserted and existed element
   */
  iterator begin() {
    if (dl.head == nullptr)
      return iterator(this, -1);
    return iterator(this, dl.head);
  }
  const_iterator cbegin() const {
    if (dl.head == nullptr)
      return const_iterator(this, -1);
    return const_iterator(this, dl.head);
  }
  /**
   * return an iterator after the last inserted element
   */
  iterator end() {
    if (dl.tail == nullptr)
      return iterator(this, -1);
    return iterator(this, dl.tail);
  }
  const_iterator cend() const {
    if (dl.tail == nullptr)
      return const_iterator(this, -1);
    return const_iterator(this, dl.tail);
  }
  /**
   * if didn't contain anything, return true,
   * otherwise false.
   */
  bool empty() const {
    if (dl.size == 0)
      return true;
    return false;
  }

  void clear() {
    dl.clear();
    this->clear();
  }

  size_t size() const { return dl.size; }
  /**
   * insert the value_pair
   * if the key of the value_pair exists in the map
   * update the value instead of adding a new element，
   * then the order of the element moved from inner of the
   * list to the head of the list
   * and return false
   * if the key of the value_pair doesn't exist in the map
   * add a new element and return true
   */
  pair<iterator, bool> insert(const value_type &value) {
    auto it = this->find(value.first);
    if (it != this->end()) {
      dl.erase(it);
      dl.insert_head(value);
      return {iterator(this, dl.head), false};
    }
    auto res = hashmap<Key, T, Hash, Equal>::insert(value);
    dl.insert_head(value);
    return {iterator(this, dl.head), true};
  }
  /**
   * erase the value_pair pointed by the iterator
   * if the iterator points to nothing
   * throw
   */
  void remove(iterator pos) {
    if (pos.idx == -1)
      throw "invalid";
    hashmap<Key, T, Hash, Equal>::erase(pos->first);
    dl.erase(pos);
  }
  /**
   * return how many value_pairs consist of key
   * this should only return 0 or 1
   */
  size_t count(const Key &key) const {
    auto it = hashmap<Key, T, Hash, Equal>::find(key);
    if (it == this->end())
      return 0;
    return 1;
  }
  /**
   * find the iterator points at the value_pair
   * which consist of key
   * if not find, return the iterator
   * point at nothing
   */
  iterator find(const Key &key) {
    auto it = hashmap<Key, T, Hash, Equal>::find(key);
    if (it == this->end())
      return iterator(this, -1);
    return iterator(this, it);
  }
};
/******************************/
class lru {
  using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
  using value_type = sjtu::pair<const Integer, Matrix<int>>;

public:
  lru(int size) {}
  ~lru() {}
  /**
   * save the value_pair in the memory
   * delete something in the memory if necessary
   */
  void save(const value_type &v) const {}
  /**
   * return a pointer contain the value
   */
  Matrix<int> *get(const Integer &v) const {}
  /**
   * just print everything in the memory
   * to debug or test.
   * this operation follows the order, but don't
   * change the order.
   */
  void print() {}
};
} // namespace sjtu

#endif