#ifndef TYPE_ALLOCATOR_H
#define TYPE_ALLOCATOR_H

#include "mutils.h"

#include <memory>
#include <type_traits>

template <typename _Tp, typename _Ma> class type_allocator : public _Ma {
public:
  _Tp *make_new() { return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp))); }
  void destroy(_Tp *ptr) {
    if (!std::is_trivially_destructible<_Tp>::value) {
      ptr->~_Tp();
    }
    _Ma::free(ptr, sizeof(_Tp));
  }
};

template <typename _Tp, typename _Ma> _Tp *make_new() {
  return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp)));
}

template <typename _Tp, typename _Ma> void destroy(_Tp *ptr) {
  if (!std::is_trivially_destructible<_Tp>::value) {
    ptr->~_Tp();
  }
  _Ma::free(ptr, sizeof(_Tp));
}

#endif // TYPE_ALLOCATOR_H
