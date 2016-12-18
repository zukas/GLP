#ifndef STATIC_INSTANCE_FACTORY_H
#define STATIC_INSTANCE_FACTORY_H

#include <new>

template <typename _Tp> class static_instance_factory {
public:
  template <typename... _Args> static _Tp &init(_Args... args) {
    is_init() = true;
    if (std::is_trivially_constructible<_Tp>::value) {
      return *static_cast<_Tp *>(address());
    } else {
      return *(new (address()) _Tp(args...));
    }
  }
  static void deinit() {
    auto &init = is_init();
    if (init) {
      init = false;
      get().~_Tp();
    }
  }
  static _Tp &get() { return *static_cast<_Tp *>(address()); }

private:
  static bool &is_init() {
    static bool _init = false;
    return _init;
  }

  static void *address() {
    static char _buff[sizeof(_Tp)]{0};
    return _buff;
  }
};

#endif // STATIC_INSTANCE_FACTORY_H
