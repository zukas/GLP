#ifndef ARRAY_BUFFERS_H
#define ARRAY_BUFFERS_H

#include "mutils.h"

template <typename _Arr, size_t _Num> class array_buffers {
  static_assert(_Num > 1, "Array buffered expects more then one buffer");

public:
  array_buffers() : m_cursor(0) {}

  ~array_buffers() {}
  array_buffers(const array_buffers &) = delete;
  array_buffers(array_buffers &&) = delete;

  _Arr &current() { return m_buffers[m_cursor % _Num]; }

  _Arr &next() { return m_buffers[(m_cursor + 1) % _Num]; }

  void swap() {
    m_cursor = (m_cursor + 1) % _Num;
    m_buffers[m_cursor].clear();
  }

private:
  _Arr m_buffers[_Num];
  long m_cursor;
};

#endif // ARRAY_BUFFERS_H
