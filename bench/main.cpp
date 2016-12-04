#include "engine/memory/memory.h"
#include "memory_helper.h"
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <memory/allocator.h>
#include <memory/pool_allocator.h>
#include <memory>

// static void GLM_dot(benchmark::State &state) {
//  glm::vec3 v1{0.5f, 0.3f, 0.2f};
//  glm::vec3 v2{0.3f, 0.7f, 0.5f};
//  while (state.KeepRunning()) {

//    for (int i = 0; i < 100; ++i) {
//      float res = glm::dot(v1, v2);
//      escape(&res);
//      res = glm::dot(v2, v1);
//      escape(&res);
//    }
//  }
//  clobber();
//}

// BENCHMARK(GLM_dot);

// struct A {
//  int data[1]{0};
//};

// struct B {
//  int data[2]{0, 1};
//};

// struct C {
//  int data[4]{0, 1, 2, 3};
//};

// struct D {
//  int data[8]{0, 1, 2, 3, 4, 5, 6, 7};
//};

// struct E {
//  int data[64]{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
//               16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
//               32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
//               48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
//               63};
//};

// template <typename __T> class glp_ptr {
// private:
//  __T *_data;
//  long _size;

// public:
//  glp_ptr(glp::memblk blk) : _data(new (blk.ptr) __T), _size(blk.size) {}
//  __T *data() { return _data; }
//  long size() const { return _size; }
//  glp::memblk block() const { return {_data, _size}; }
//  __T *operator->() { return _data; }
//};

// static void Memory_custom(benchmark::State &state) {
//  glp::mem_alloc_testing alloc;
//  std::vector<glp_ptr<A>> _a;
//  std::vector<glp_ptr<B>> _b;
//  std::vector<glp_ptr<C>> _c;
//  std::vector<glp_ptr<D>> _d;
//  std::vector<glp_ptr<E>> _e;

//  _a.reserve(4096);
//  _b.reserve(4096);
//  _c.reserve(4096);
//  _d.reserve(4096);
//  _e.reserve(4096);

//  int v = 0;
//  while (state.KeepRunning()) {
//    _a.push_back(glp_ptr<A>(alloc.alloc(sizeof(A))));
//    _b.push_back(glp_ptr<B>(alloc.alloc(sizeof(B))));
//    _c.push_back(glp_ptr<C>(alloc.alloc(sizeof(C))));
//    _d.push_back(glp_ptr<D>(alloc.alloc(sizeof(D))));
//    _e.push_back(glp_ptr<E>(alloc.alloc(sizeof(E))));

//    _a.back().data()->data[0] = v;
//    _b.back().data()->data[0] = v;
//    _c.back().data()->data[0] = v;
//    _d.back().data()->data[0] = v;
//    _e.back().data()->data[0] = v;
//    ++v;
//  }
//  for (int i = 0; i < v; ++i) {
//    assert(i == _a[i]->data[0]);
//    assert(i == _b[i]->data[0]);
//    assert(i == _c[i]->data[0]);
//    assert(i == _d[i]->data[0]);
//    assert(i == _e[i]->data[0]);
//  }
//  clobber();
//}

// BENCHMARK(Memory_custom);

// static void Memory_malloc(benchmark::State &state) {
//  std::vector<std::unique_ptr<A>> _a;
//  std::vector<std::unique_ptr<B>> _b;
//  std::vector<std::unique_ptr<C>> _c;
//  std::vector<std::unique_ptr<D>> _d;
//  std::vector<std::unique_ptr<E>> _e;

//  _a.reserve(4096);
//  _b.reserve(4096);
//  _c.reserve(4096);
//  _d.reserve(4096);
//  _e.reserve(4096);

//  int v = 0;
//  while (state.KeepRunning()) {
//    _a.push_back(std::unique_ptr<A>(new A()));
//    _b.push_back(std::unique_ptr<B>(new B()));
//    _c.push_back(std::unique_ptr<C>(new C()));
//    _d.push_back(std::unique_ptr<D>(new D()));
//    _e.push_back(std::unique_ptr<E>(new E()));

//    _a.back()->data[0] = v;
//    _b.back()->data[0] = v;
//    _c.back()->data[0] = v;
//    _d.back()->data[0] = v;
//    _e.back()->data[0] = v;
//    ++v;
//  }
//  for (int i = 0; i < v; ++i) {
//    assert(i == _a[i]->data[0]);
//    assert(i == _b[i]->data[0]);
//    assert(i == _c[i]->data[0]);
//    assert(i == _d[i]->data[0]);
//    assert(i == _e[i]->data[0]);
//  }

//  clobber();
//}

// BENCHMARK(Memory_malloc);

// static void Memory_malloc_mat4(benchmark::State &state) {
//  std::vector<std::unique_ptr<glm::mat4>> _store;
//  while (state.KeepRunning()) {
//    _store.push_back(std::unique_ptr<glm::mat4>(new glm::mat4()));
//  }
//  escape(_store.data());
//  clobber();
//}

// BENCHMARK(Memory_malloc_mat4);

// static void Memory_custom_mat4(benchmark::State &state) {
//  glp::mem_alloc_testing alloc;
//  std::vector<glp_ptr<glm::mat4>> _store;
//  while (state.KeepRunning()) {
//    auto t = glp_ptr<glm::mat4>(alloc.alloc(sizeof(glm::mat4)));
//    if (t.data() == nullptr) {
//      printf("Memory_custom_mat4: Failed to allocate\n");
//    }
//    _store.push_back(t);
//  }
//  escape(_store.data());
//  clobber();
//}

// BENCHMARK(Memory_custom_mat4);

// static void Memory_custom2_mat4(benchmark::State &state) {
//  glp::mem::pool_allocator_64<1024 * 1024 * 64> alloc;
//  std::vector<glp_ptr<glm::mat4>> _store;
//  while (state.KeepRunning()) {
//    _store.push_back(glp_ptr<glm::mat4>({alloc.alloc(), sizeof(glm::mat4)}));
//  }
//  escape(_store.data());
//  clobber();
//}

// BENCHMARK(Memory_custom2_mat4);

// static void DIV_long(benchmark::State &state) {
//  long l = std::numeric_limits<long>::max();
//  while (state.KeepRunning()) {
//    long t = l / 7;
//    escape(&t);
//    l -= t;
//  }
//  clobber();
//}

// BENCHMARK(DIV_long);

// static void DIV_float(benchmark::State &state) {
//  float l = std::numeric_limits<float>::max();
//  while (state.KeepRunning()) {
//    float t = l / 7.0f;
//    escape(&t);
//    l -= t;
//  }
//  clobber();
//}

// BENCHMARK(DIV_float);

// static void DIV_double(benchmark::State &state) {
//  double l = std::numeric_limits<double>::max();
//  while (state.KeepRunning()) {
//    double t = l / 7.0;
//    escape(&t);
//    l -= t;
//  }
//  clobber();
//}

// BENCHMARK(DIV_double);

// static void std_atol(benchmark::State &state) {
//  const char _data[] = {"48664684"};
//  while (state.KeepRunning()) {
//    long res = std::atol(_data);
//    escape(&res);
//  }
//  clobber();
//}

// BENCHMARK(std_atol);

// long c_atol(const char *str) {
//  long res = 0;
//  for (auto c = unsigned(*str - 48); *str != '\0';
//       c = unsigned(*(++str) - 48)) {
//    if (c <= 9) {
//      res = res * 10 + c;
//    } else {
//      return 0;
//    }
//  }
//  return res;
//}

// static void custom_atol(benchmark::State &state) {
//  const char _data[] = {"48664684"};
//  while (state.KeepRunning()) {
//    long res = c_atol(_data);
//    escape(&res);
//  }
//  clobber();
//}

// BENCHMARK(custom_atol);

// static void flip_mod(benchmark::State &state) {
//  long l = 0;
//  while (state.KeepRunning()) {
//    l++;
//    l = l % 2;
//    escape(&l);
//  }
//  clobber();
//}

// BENCHMARK(flip_mod);

// static void flip_add_abs(benchmark::State &state) {
//  long l = 0;
//  while (state.KeepRunning()) {
//    l++;
//    l = abs(l - 1);
//    escape(&l);
//  }
//  clobber();
//}

// BENCHMARK(flip_add_abs);

static void memory_control_malloc(benchmark::State &state) {
  size_t size = 2;
  while (state.KeepRunning()) {
    void *b1 = malloc(MB(size * 8));
    *static_cast<int *>(b1) = 10;
    escape(b1);
    void *b2 = malloc(MB(size * 4));
    *static_cast<int *>(b2) = 20;
    escape(b2);
    void *b3 = malloc(MB(size * 16));
    *static_cast<int *>(b3) = 30;
    escape(b3);
    ::free(b1);
    void *b4 = malloc(MB(size * 4));
    *static_cast<int *>(b4) = 40;
    escape(b4);
    void *b5 = malloc(MB(size * 4));
    *static_cast<int *>(b5) = 50;
    escape(b5);
    ::free(b2);
    void *b6 = malloc(MB(size * 4));
    *static_cast<int *>(b6) = 60;
    escape(b6);
    void *b7 = malloc(MB(size * 16));
    *static_cast<int *>(b7) = 70;
    escape(b7);
    ::free(b3);
    ::free(b5);
    ::free(b4);
    ::free(b7);
    ::free(b6);
  }
  clobber();
}

BENCHMARK(memory_control_malloc);
BENCHMARK(memory_control_malloc)->Threads(2);
BENCHMARK(memory_control_malloc)->Threads(4);
BENCHMARK(memory_control_malloc)->Threads(8);

static Memory &init_allocator() {
  static Memory m{MB(800u)};
  return m;
}

static void memory_control_custom(benchmark::State &state) {
  size_t size = 1;
  Memory &m = init_allocator();
  while (state.KeepRunning()) {
    auto b1 = mem_alloc(MB(size * 8));
    *static_cast<int *>(b1.ptr) = 10;
    escape(b1.ptr);
    auto b2 = mem_alloc(MB(size * 4));
    *static_cast<int *>(b2.ptr) = 20;
    escape(b2.ptr);
    auto b3 = mem_alloc(MB(size * 16));
    *static_cast<int *>(b1.ptr) = 30;
    escape(b3.ptr);
    mem_free(b1);
    auto b4 = mem_alloc(MB(size * 4));
    *static_cast<int *>(b4.ptr) = 40;
    escape(b4.ptr);
    auto b5 = mem_alloc(MB(size * 4));
    *static_cast<int *>(b5.ptr) = 50;
    escape(b5.ptr);
    mem_free(b2);
    auto b6 = mem_alloc(MB(size * 4));
    *static_cast<int *>(b6.ptr) = 60;
    escape(b6.ptr);
    auto b7 = mem_alloc(MB(size * 16));
    *static_cast<int *>(b7.ptr) = 70;
    escape(b7.ptr);
    mem_free(b3);
    mem_free(b5);
    mem_free(b4);
    mem_free(b7);
    mem_free(b6);
  }
  clobber();
}

BENCHMARK(memory_control_custom);
BENCHMARK(memory_control_custom)->Threads(2);
BENCHMARK(memory_control_custom)->Threads(4);
BENCHMARK(memory_control_custom)->Threads(8);

BENCHMARK_MAIN();
