#include "memory_helper.h"
#include <benchmark/benchmark.h>
#include <glm/glm.hpp>
#include <memory/allocator.h>
#include <memory>

static void GLM_dot(benchmark::State &state) {
  glm::vec3 v1{0.5f, 0.3f, 0.2f};
  glm::vec3 v2{0.3f, 0.7f, 0.5f};
  while (state.KeepRunning()) {

    for (int i = 0; i < 100; ++i) {
      float res = glm::dot(v1, v2);
      escape(&res);
      res = glm::dot(v2, v1);
      escape(&res);
    }
  }
  clobber();
}

BENCHMARK(GLM_dot);

struct A {
  int data[1]{0};
};

struct B {
  int data[2]{0, 1};
};

struct C {
  int data[4]{0, 1, 2, 3};
};

struct D {
  int data[8]{0, 1, 2, 3, 4, 5, 6, 7};
};

struct E {
  int data[64]{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
               16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
               32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
               48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
};

template <typename __T> class glp_ptr {
private:
  __T *_data;
  long _size;

public:
  glp_ptr(glp::memblk blk) : _data(new (blk.ptr) __T), _size(blk.size) {}
  __T *data() { return _data; }
  long size() const { return _size; }
  glp::memblk block() const { return {_data, _size}; }
  __T *operator->() { return _data; }
};

static void Memory_custom(benchmark::State &state) {
  glp::mem_alloc_testing alloc;
  std::vector<glp_ptr<A>> _a;
  std::vector<glp_ptr<B>> _b;
  std::vector<glp_ptr<C>> _c;
  std::vector<glp_ptr<D>> _d;
  std::vector<glp_ptr<E>> _e;

  _a.reserve(4096);
  _b.reserve(4096);
  _c.reserve(4096);
  _d.reserve(4096);
  _e.reserve(4096);

  int v = 0;
  while (state.KeepRunning()) {
    _a.push_back(glp_ptr<A>(alloc.alloc(sizeof(A))));
    _b.push_back(glp_ptr<B>(alloc.alloc(sizeof(B))));
    _c.push_back(glp_ptr<C>(alloc.alloc(sizeof(C))));
    _d.push_back(glp_ptr<D>(alloc.alloc(sizeof(D))));
    _e.push_back(glp_ptr<E>(alloc.alloc(sizeof(E))));

    _a.back().data()->data[0] = v;
    _b.back().data()->data[0] = v;
    _c.back().data()->data[0] = v;
    _d.back().data()->data[0] = v;
    _e.back().data()->data[0] = v;
    ++v;
  }
  for (int i = 0; i < v; ++i) {
    assert(i == _a[i]->data[0]);
    assert(i == _b[i]->data[0]);
    assert(i == _c[i]->data[0]);
    assert(i == _d[i]->data[0]);
    assert(i == _e[i]->data[0]);
  }
  clobber();
}

BENCHMARK(Memory_custom);

static void Memory_malloc(benchmark::State &state) {
  std::vector<std::unique_ptr<A>> _a;
  std::vector<std::unique_ptr<B>> _b;
  std::vector<std::unique_ptr<C>> _c;
  std::vector<std::unique_ptr<D>> _d;
  std::vector<std::unique_ptr<E>> _e;

  _a.reserve(4096);
  _b.reserve(4096);
  _c.reserve(4096);
  _d.reserve(4096);
  _e.reserve(4096);

  int v = 0;
  while (state.KeepRunning()) {
    _a.push_back(std::unique_ptr<A>(new A()));
    _b.push_back(std::unique_ptr<B>(new B()));
    _c.push_back(std::unique_ptr<C>(new C()));
    _d.push_back(std::unique_ptr<D>(new D()));
    _e.push_back(std::unique_ptr<E>(new E()));

    _a.back()->data[0] = v;
    _b.back()->data[0] = v;
    _c.back()->data[0] = v;
    _d.back()->data[0] = v;
    _e.back()->data[0] = v;
    ++v;
  }
  for (int i = 0; i < v; ++i) {
    assert(i == _a[i]->data[0]);
    assert(i == _b[i]->data[0]);
    assert(i == _c[i]->data[0]);
    assert(i == _d[i]->data[0]);
    assert(i == _e[i]->data[0]);
  }

  clobber();
}

BENCHMARK(Memory_malloc);

static void DIV_long(benchmark::State &state) {
  long l = std::numeric_limits<long>::max();
  while (state.KeepRunning()) {
    long t = l / 7;
    escape(&t);
    l -= t;
  }
  clobber();
}

BENCHMARK(DIV_long);

static void DIV_double(benchmark::State &state) {
  double l = std::numeric_limits<double>::max();
  while (state.KeepRunning()) {
    double t = l / 7.0;
    escape(&t);
    l -= t;
  }
  clobber();
}

BENCHMARK(DIV_double);

BENCHMARK_MAIN();
