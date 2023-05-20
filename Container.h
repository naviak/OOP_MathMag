#pragma once
#include <memory>
#include <stdexcept>
/**
* Doubly linked list
**/
template<class T>
class Container final
{
  struct Node
  {
    std::shared_ptr<Node> _prev, _next;
    explicit Node(const T& iData)
      : _data(iData) {}
    T _data;
  };

  template< bool c = false >
  struct ContainerIterator final
  {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = std::conditional_t<c, const T*, T*>;
    using reference = std::conditional_t<c, const T&, T&>;

    ContainerIterator(std::shared_ptr<Node> node) : current(std::move(node)) {}

    bool operator==(const ContainerIterator& other) const {
      return current == other.current;
    }

    bool operator!=(const ContainerIterator& other) const {
      return !(*this == other);
    }

    reference operator*() const {
      return current->_data;
    }

    ContainerIterator& operator++() {
      current = current->_next;
      return *this;
    }

    ContainerIterator operator++(int) {
      ContainerIterator temp = *this;
      ++(*this);
      return temp;
    }

    ContainerIterator& operator--() {
      current = current->_prev;
      return *this;
    }

    ContainerIterator operator--(int) {
      ContainerIterator temp = *this;
      --(*this);
      return temp;
    }
  private:
    std::shared_ptr<Node> current;
  };

public:
  Container() = default;
  void push_front(const T& iObject);
  void push_back(const T& iObject);
  void pop_front();
  void pop_back();
  T& front();
  T& back();
  size_t size() const { return _size; }
  bool empty() const { return _size == 0; }
  void clear();
  void reverse();
  void swap(Container& iOther) noexcept;

  friend std::ostream& operator<<(std::ostream& os, Container& container) {

    for (const auto& value: container)
      os << value << " ";
    return os;
  }
public:
  using Iterator = ContainerIterator<true>;
  Iterator begin() { return Iterator(_head); }
  Iterator end() { return std::next(begin(), _size); }

  using ConstIterator = ContainerIterator<true>;
  ConstIterator cbegin() const { return ConstIterator(_head); }
  ConstIterator cend() const { return std::next(cbegin(), _size); }
private:
  std::shared_ptr<Node> _head, _tail;
  size_t _size = 0;
};

template <class T>
void Container<T>::push_front(const T& iObject)
{
  auto newNode = std::make_shared<Node>(iObject);
  
  if (empty()) {
    _head = newNode;
    _tail = newNode;
  }
  else {
    newNode->_next = _head;
    _head->_prev = newNode;
    _head = newNode;
  }
  _size++;
}

template <class T>
void Container<T>::push_back(const T& iObject)
{
  std::shared_ptr<Node> newNode = std::make_shared<Node>(iObject);

  if (empty()) {
    _head = newNode;
    _tail = newNode;
  }
  else {
    newNode->_prev = _tail;
    _tail->_next = newNode;
    _tail = newNode;
  }
  ++_size;
}

template <class T>
void Container<T>::pop_front()
{
  if (empty())
    throw std::out_of_range("Can't pop from empty container");
  std::shared_ptr<Node> nodeToRemove = _head;
  _head = _head->_next;

  if (_head == nullptr)
    _tail = nullptr;
  else
    _head->_prev = nullptr;
  _size--;
}

template <class T>
void Container<T>::pop_back()
{
  if (empty())
    throw std::out_of_range("Can't pop from empty container");

  std::shared_ptr<Node> nodeToRemove = _tail;
  _tail = _tail->_prev;

  if (_tail == nullptr)
    _head = nullptr;
  else
    _tail->_next = nullptr;
  _size--;
}

template <class T>
T& Container<T>::front()
{
  return _head->_data;
}

template <class T>
T& Container<T>::back()
{
  return _tail->_data;
}

template <class T>
void Container<T>::clear()
{
  if (empty())
    return;
  while (_head) {
    auto nextNode = _head->_next;
    _head.reset();
    _head = nextNode;
  }
  _tail.reset();
  _size = 0;
  _size = 0;
}

template <class T>
void Container<T>::reverse()
{
  std::shared_ptr<Node> current = _head;
  while (current != nullptr) {
    std::shared_ptr<Node> temp = current->_prev;
    current->_prev = current->_next;
    current->_next = temp;
    current = current->_prev;
  }

  std::shared_ptr<Node> temp = _head;
  _head = _tail;
  _tail = temp;
}

template <class T>
void Container<T>::swap(Container& iOther) noexcept
{
  std::swap(_head, iOther._head);
  std::swap(_tail, iOther._tail);
  std::swap(_size, iOther._size);
}
