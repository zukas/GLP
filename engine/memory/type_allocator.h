#ifndef TYPE_ALLOCATOR_H
#define TYPE_ALLOCATOR_H

#include "mutils.h"

#include <memory>
#include <type_traits>

template <typename _Tp, typename _Ma, typename _E = void>
class type_allocator {};

template <typename _Tp, typename _Ma>
class type_allocator<
    _Tp, _Ma,
    typename std::enable_if<std::is_pointer<typename _Ma::t_type>::value>::type>
    : _Ma {
public:
  template <typename... _Args> _Tp *type_new() {
    if (std::is_trivially_constructible<_Tp>::value) {
      return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp)));
    } else {
      return new (_Ma::alloc(sizeof(_Tp))) _Tp(std::forward<_Args>...);
    }
  }

  void type_delete(_Tp *ptr) {
    if (!std::is_trivially_destructible<_Tp>::value) {
      ptr->~_Tp();
    }
    _Ma::free(ptr);
  }
};

template <typename _Tp, typename _Ma>
class type_allocator<_Tp, _Ma, typename std::enable_if<!std::is_pointer<
                                   typename _Ma::t_type>::value>::type> : _Ma {
public:
  template <typename... _Args> _Tp *type_new() {
    if (std::is_trivially_constructible<_Tp>::value) {
      return static_cast<_Tp *>(_Ma::alloc(sizeof(_Tp)).ptr);
    } else {
      return new (_Ma::alloc(sizeof(_Tp)).ptr) _Tp(std::forward<_Args>...);
    }
  }

  void type_delete(_Tp *ptr) {
    if (!std::is_trivially_destructible<_Tp>::value) {
      ptr->~_Tp();
    }
    _Ma::free({ptr, _Ma::opt_size(sizeof(_Tp))});
  }
};

#endif // TYPE_ALLOCATOR_H
