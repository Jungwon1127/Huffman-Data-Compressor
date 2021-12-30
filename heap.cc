#include <cassert>

template <typename T>
heap<T>::heap(int (*cmp)(const element &, const element &), size_t capacity)
{
  _cmp = cmp;
  _capacity = capacity;
  _data = new element[_capacity];
  _size = 0;
}

template <typename T>
void heap<T>::debug_print(void (*print)(const T &, std::ostream &),
                          std::ostream & ostr)
{
  ostr << "Heap [";
  for (size_t i = 0; i < _size; i++) {
    if (i != 0)
      ostr << ", ";
    print(_data[i], ostr);
  }
  ostr << "]" << std::endl;
}

template <typename T>
void heap<T>::add(const element & item)
{
  if (_size == _capacity)
    _grow();
  _data[_size] = item;
  _reheap_up(_size);
  _size++;
}

template <typename T>
T heap<T>::remove()
{
  assert(_size != 0);
  T result = _data[0];
  _data[0] = _data[_size - 1];
  _size--;
  _reheap_down(0);
  return result;
}

// helper functions
size_t parent(size_t pos)
{
  return (pos - 1) / 2;
}
size_t left_child(size_t pos)
{
  return 2 * pos + 1;
}
size_t right_child(size_t pos)
{
  return 2 * pos + 2;
}

template <typename T>
void heap<T>::_reheap_up(size_t pos)
{
  size_t par;
  while (pos > 0 and _cmp(_data[pos], _data[par = parent(pos)]) < 0) {
    std::swap(_data[pos], _data[par]);
    pos = par;
  }
}


template <typename T>
void heap<T>::_reheap_down(size_t pos)
{
  bool done = false;
  while (!done) {
    size_t child = left_child(pos);
    if (child >= _size)
      done = true;
    else {
      size_t right = right_child(pos);
      if (right < _size and _cmp(_data[right], _data[child]) < 0)
        child = right;
      if (_cmp(_data[child], _data[pos]) < 0) {
        std::swap(_data[child], _data[pos]);
        pos = child;
      } // end if child comes before parent case
      else
        done = true;
    } // end internal node case
  } // end loop
} // end reheap down




template <typename T>
size_t heap<T>::size() const
{
  return _size;
}

template <typename T>
void heap<T>::_grow()
{
  _capacity *= 2;
  element *newdata = new element[_capacity];
  for (size_t i = 0; i < _size; i++)
    newdata[i] = _data[i];
  delete [] _data;
  _data = newdata;
}
