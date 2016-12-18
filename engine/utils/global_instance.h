#ifndef GLOBAL_INSTANCE_H
#define GLOBAL_INSTANCE_H

template <typename _Tp> class global_instance {
public:
  static _Tp *get() { return *__global_ptr(); }

protected:
  global_instance() { *__global_ptr() = static_cast<_Tp *>(this); }
  ~global_instance() { *__global_ptr() = nullptr; }

private:
  static _Tp **__global_ptr() {
    static _Tp *_inst = nullptr;
    return &_inst;
  }
};

#endif // GLOBAL_INSTANCE_H
