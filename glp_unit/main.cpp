#include "engine/memory/memory.h"
#include "memory/pointer.h"
#include <atomic>
#include <cstdio>
#include <math.h>
#include <mutex>
#include <thread>
#include <time.h>

void escape(void *p) { asm volatile("" ::"g"(p) : "memory"); }
void clobber() { asm volatile("" : : : "memory"); }

long c_atol(const char *str) {
  long res = 0;
  for (auto c = unsigned(*str - 48); *str != '\0';
       c = unsigned(*(++str) - 48)) {
    if (c <= 9) {
      res = res * 10 + c;
    } else {
      return 0;
    }
  }
  return res;
}

long long get_time() {
  timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return static_cast<long long>(t.tv_sec) * 1000000000 +
         static_cast<long long>(t.tv_nsec);
}

template <typename _Tp, typename... _Args>
inline static void perf(int iterations, const char *func_name, _Tp func,
                        _Args... args) {
  auto t = get_time();
  for (int i = 0; i < iterations; i++)
    func(args...);
  t = get_time() - t;
  printf("%-15s%20f\n", func_name, double(t) / 1000000.0);
}

template <typename _Tp, typename... _Args>
inline static void perf_thread(int threads, int iterations,
                               const char *func_name, _Tp func, _Args... args) {
  std::thread th[64];
  auto t = get_time();
  for (int j = 0; j < threads; j++) {
    th[j] = std::thread([=]() {
      for (int i = 0; i < iterations; i++)
        func(args...);
    });
  }
  for (int j = 0; j < threads; j++) {
    th[j].join();
  }
  t = get_time() - t;
  printf("%-15s%20.2f\n", func_name, double(t) / 1000000.0);
}

void test_malloc() {

  void *b1 = malloc(MB(8));
  *static_cast<int *>(b1) = 10;
  escape(b1);
  void *b2 = malloc(MB(4));
  *static_cast<int *>(b2) = 20;
  escape(b2);
  void *b3 = malloc(MB(16));
  *static_cast<int *>(b3) = 30;
  escape(b3);
  ::free(b1);
  void *b4 = malloc(MB(4));
  *static_cast<int *>(b4) = 40;
  escape(b4);
  void *b5 = malloc(MB(4));
  *static_cast<int *>(b5) = 50;
  escape(b5);
  ::free(b2);
  void *b6 = malloc(MB(4));
  *static_cast<int *>(b6) = 60;
  escape(b6);
  void *b7 = malloc(MB(16));
  *static_cast<int *>(b7) = 70;
  escape(b7);
  ::free(b3);
  ::free(b5);
  ::free(b4);
  ::free(b7);
  ::free(b6);
  clobber();
}

void test_custom() {

  auto b1 = mem_alloc(MB(8));
  *static_cast<int *>(b1.ptr) = 10;
  escape(b1.ptr);
  auto b2 = mem_alloc(MB(4));
  *static_cast<int *>(b2.ptr) = 20;
  escape(b2.ptr);
  auto b3 = mem_alloc(MB(16));
  *static_cast<int *>(b1.ptr) = 30;
  escape(b3.ptr);
  mem_free(b1);
  auto b4 = mem_alloc(MB(4));
  *static_cast<int *>(b4.ptr) = 40;
  escape(b4.ptr);
  auto b5 = mem_alloc(MB(4));
  *static_cast<int *>(b5.ptr) = 50;
  escape(b5.ptr);
  mem_free(b2);
  auto b6 = mem_alloc(MB(4));
  *static_cast<int *>(b6.ptr) = 60;
  escape(b6.ptr);
  auto b7 = mem_alloc(MB(16));
  *static_cast<int *>(b7.ptr) = 70;
  escape(b7.ptr);
  mem_free(b3);
  mem_free(b5);
  mem_free(b4);
  mem_free(b7);
  mem_free(b6);
  clobber();
}

int main() {

  //  test_custom(2);

  //  for (int i = 0; i < 100; ++i) {
  perf_thread(32, 10000, "test_malloc", test_malloc);
  {
    Memory m{MB(1200u)};
    perf_thread(32, 100000, "test_custom", test_custom);
  }
  //  }
  return 0;
}
