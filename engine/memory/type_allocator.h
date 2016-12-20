#ifndef TYPE_ALLOCATOR_H
#define TYPE_ALLOCATOR_H

#include "mutils.h"

#include <memory>
#include <type_traits>

template <typename _Tp, typename _Ma> class type_allocator : _Ma {
public:
  template <typename... _Args> _Tp *make_new(_Args... args) {
    if (std::is_trivially_constructible<_Tp>::value) {
      return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp)));
    } else {
      return new (_Ma::alloc(sizeof(_Tp))) _Tp(std::forward<_Args>(args)...);
    }
  }
  void destroy(_Tp *ptr) {
    if (!std::is_trivially_destructible<_Tp>::value) {
      ptr->~_Tp();
    }
    _Ma::free(ptr, sizeof(_Tp));
  }
};

template <typename _Tp, typename _Ma, typename... _Args>
_Tp *make_new(_Args... args) {
  if (std::is_trivially_constructible<_Tp>::value) {
    return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp)));
  } else {
    return new (_Ma::alloc(sizeof(_Tp))) _Tp(std::forward<_Args>(args)...);
  }
}

template <typename _Tp, typename _Ma> void destroy(_Tp *ptr) {
  if (!std::is_trivially_destructible<_Tp>::value) {
    ptr->~_Tp();
  }
  _Ma::free(ptr, sizeof(_Tp));
}

#endif // TYPE_ALLOCATOR_H
