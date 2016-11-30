#ifndef POINTER_H
#define POINTER_H

#include "memory_util.h"
#include <memory>
#include <type_traits>

namespace glp {

template <typename __T> class pointer {
private:
  __T *_ptr;
  long _size;

private:
  pointer(__T *ptr, long size) : _ptr(ptr), _size(size) {}

public:
  template <typename... Args>
  static pointer<__T> construct(memblk blk, Args &&... args) {
    if (std::is_pod<__T>::value) {
      return pointer<__T>(static_cast<__T *>(blk.ptr), blk.size);
    } else {
      return pointer<__T>(new (blk.ptr) __T(std::forward<Args...>(args)...),
                          blk.size);
    }
  }

  __T &operator*() { return *_ptr; }
  const __T &operator*() const { return *_ptr; }

  __T *operator->() { return _ptr; }
  const __T *operator->() const { return _ptr; }

  __T &operator[](size_t n) { return _ptr[n]; }
  const __T &operator[](size_t n) const { return *_ptr[n]; }
};
}

#endif // POINTER_H
