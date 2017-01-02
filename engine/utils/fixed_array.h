#ifndef FIXED_ARRAY_H
#define FIXED_ARRAY_H

#include "memory/global_malloc.h"

#include <cassert>

template <typename _Tp> class fixed_array {
public:
  fixed_array() : m_data(nullptr), m_size(0) {}
  fixed_array(size_t size) {
    m_data = static_cast<_Tp *>(glp_malloc(sizeof(_Tp) * size));
    m_size = size;
  }
  fixed_array(fixed_array &&other) {
    auto t_d = m_data;
    auto t_s = m_size;

    m_data = other.m_data;
    m_size = other.m_size;

    other.m_data = t_d;
    other.m_size = t_s;
  }

  fixed_array(const fixed_array &) = delete;

  ~fixed_array() { reset(); }

  template <typename _In = size_t> _In size() const {
    return static_cast<_In>(m_size);
  }

  _Tp *data() { return m_data; }

  template <typename _In> _Tp &operator[](_In i) {
    assert(i >= 0 && i < static_cast<long>(m_size));
    return m_data[i];
  }

  template <typename _In> const _Tp &operator[](_In i) const {
    assert(i >= 0 && i < m_size);
    return m_data[i];
  }

  fixed_array &operator=(fixed_array &&other) {
    auto t_d = m_data;
    auto t_s = m_size;

    m_data = other.m_data;
    m_size = other.m_size;

    other.m_data = t_d;
    other.m_size = t_s;
    return *this;
  }

  fixed_array &operator=(const fixed_array &) = delete;

  void reset() {
    glp_free(m_data);
    m_data = nullptr;
    m_size = 0;
  }

private:
  _Tp *m_data;
  size_t m_size;
};

#endif // FIXED_ARRAY_H
