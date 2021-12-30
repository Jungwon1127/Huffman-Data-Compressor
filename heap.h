#ifndef HEAP_H
#define HEAP_H

#include <iostream>

template <typename T>
class heap {
public:
  typedef T element;
  heap(int (*)(const element &, const element &), size_t capacity = 10);
  void add(const element &);
  element remove();
  size_t size() const;
  void debug_print(void (*print)(const T&, std::ostream&),
                   std::ostream & ostr);

private:
  element * _data;
  size_t _size;
  size_t _capacity;
  int (*_cmp)(const element &, const element &);
  void _grow();
  void _reheap_up(size_t);
  void _reheap_down(size_t);
};


#include "heap.cc"

#endif
