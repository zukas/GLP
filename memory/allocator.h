#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#ifdef NDEBUG
#define DEBUG_MEMBER_EXPR(expr)
#define DEBUG_CTOR_EXPR(expr)
#define DEBUG_USE(var)
#else
#define DEBUG_MEMBER_EXPR(expr) expr;
#define DEBUG_CTOR_EXPR(expr) expr,
#define DEBUG_USE(var) var
#endif

namespace glp {

struct memblk {
  void *ptr;
  long size;
};

using byte = char;

template <long blk_size> constexpr long align_block(long size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <size_t blk_size> constexpr size_t align_block(size_t size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <long blk_size> constexpr bool is_aligned(const void *__restrict p) {
  return reinterpret_cast<uintptr_t>(p) % blk_size == 0;
}

template <typename T> constexpr T max(T x, T y) {
  return x ^ ((x ^ y) & -(x < y));
}

#define KB(N) 1024 * N
#define MB(N) KB(1024) * N

/** Allocator manager **/

byte *__glp_mem_sys_alloc(long size);
void __glp_mem_sys_free(byte *p);

class mem_buffer_manager {
private:
  byte *_buffer;
  DEBUG_MEMBER_EXPR(long _total)
  long _fetched;

public:
  mem_buffer_manager(byte *buffer, long DEBUG_USE(size))
      : _buffer(buffer), DEBUG_CTOR_EXPR(_total(size)) _fetched(0) {}
  ~mem_buffer_manager() { assert(_fetched == _total); }

  template <class mem_alloc> byte *fetch(const mem_alloc *alloc) {
    byte *ret = nullptr;
    const long heap = alloc->heap_size();
    assert(_fetched + heap <= _total);
    if (heap > 0) {
      ret = _buffer;
      _buffer += heap;
      _fetched += heap;
    }
    return ret;
  }
};

template <class mem_alloc_head, class... mem_alloc_tail>
class mem_alloc_helper : private mem_alloc_head,
                         private mem_alloc_helper<mem_alloc_tail...> {
  using mem_alloc_tail_t = mem_alloc_helper<mem_alloc_tail...>;

public:
  mem_alloc_helper(mem_buffer_manager &&buffer)
      : mem_alloc_head(buffer),
        mem_alloc_tail_t(static_cast<mem_buffer_manager &&>(buffer)) {}
  memblk alloc(long size) {
    return mem_alloc_head::can_alloc(size) ? mem_alloc_head::alloc(size)
                                           : mem_alloc_tail_t::alloc(size);
  }
  void free(memblk blk) {
    mem_alloc_head::owns(blk) ? mem_alloc_head::free(blk)
                              : mem_alloc_tail_t::free(blk);
  }
  bool can_alloc(long size) const {
    return mem_alloc_head::can_alloc(size) || mem_alloc_tail_t::can_alloc(size);
  }
  bool owns(memblk blk) const {
    return mem_alloc_head::owns(blk) || mem_alloc_tail_t::owns(blk);
  }
  constexpr long heap_size() const {
    return mem_alloc_head::heap_size() + mem_alloc_tail_t::heap_size();
  }

  constexpr long stack_size() const {
    return mem_alloc_head::stack_size() + mem_alloc_tail_t::stack_size();
  }
};

template <class mem_alloc_head>
class mem_alloc_helper<mem_alloc_head> : private mem_alloc_head {
public:
  mem_alloc_helper(mem_buffer_manager &&buffer) : mem_alloc_head(buffer) {}
  memblk alloc(long size) { return mem_alloc_head::alloc(size); }
  void free(memblk blk) { mem_alloc_head::free(blk); }
  bool can_alloc(long size) const { return mem_alloc_head::can_alloc(size); }
  bool owns(memblk blk) const { return mem_alloc_head::owns(blk); }
  constexpr long heap_size() const { return mem_alloc_head::heap_size(); }
  constexpr long stack_size() const { return mem_alloc_head::stack_size(); }
};

template <class... mem_alloc_list>
class mem_alloc_manager : private mem_alloc_helper<mem_alloc_list...> {
  using mem_alloc_list_t = mem_alloc_helper<mem_alloc_list...>;

private:
  byte *_buffer;

public:
  mem_alloc_manager()
      : mem_alloc_list_t(mem_buffer_manager(
            _buffer = __glp_mem_sys_alloc(mem_alloc_list_t::heap_size()),
            mem_alloc_list_t::heap_size())) {}
  ~mem_alloc_manager() { __glp_mem_sys_free(_buffer); }

  memblk alloc(long size) {
    assert(size > 0);
    assert(can_alloc(size) == true);
    memblk blk = mem_alloc_list_t::alloc(size);
    assert(is_aligned<8>(blk.ptr));
    return blk;
  }
  void free(memblk blk) {
    assert(owns(blk) == true);
    mem_alloc_list_t::free(blk);
  }
  bool can_alloc(long size) const { return mem_alloc_list_t::can_alloc(size); }
  bool owns(memblk blk) const { return mem_alloc_list_t::owns(blk); }
  constexpr long heap_size() const { return mem_alloc_list_t::heap_size(); }
  constexpr long stack_size() const { return mem_alloc_list_t::stack_size(); }
  constexpr long total_size() const { return heap_size() + stack_size(); }
};

/** END **/

/** System allocator **/

class mem_alloc_sys {
public:
  mem_alloc_sys(mem_buffer_manager &) {}
  memblk alloc(long size);
  void free(memblk blk);
  bool owns(memblk) const { return true; }
  bool can_alloc(long) const { return true; }
  long heap_size() const { return 0; }
  long stack_size() const { return 0; }
};

/** END **/

/** Free-list allocator **/

template <long min_blk_size, long max_blk_size, class mem_alloc>
class mem_alloc_free_list : private mem_alloc {
private:
  struct free_list_node {
    free_list_node *next{nullptr};
  } * _root{nullptr};

public:
  mem_alloc_free_list(mem_buffer_manager &buffer) : mem_alloc(buffer) {}
  memblk alloc(long size) {
    memblk blk;
    bool in_range = size >= min_blk_size && size <= max_blk_size;
    if (in_range && _root != nullptr) {
      blk = {_root, max_blk_size};
      _root = _root->next;
    } else {
      blk = mem_alloc::alloc(in_range ? max_blk_size : size);
    }
    return blk;
  }
  void free(memblk blk) {
    if (mem_alloc::owns(blk)) {
      if (blk.size == max_blk_size) {
        free_list_node *tmp = static_cast<free_list_node *>(blk.ptr);
        tmp->next = _root;
        _root = tmp;
      } else {
        mem_alloc::free(blk);
      }
    }
  }
  bool can_alloc(long size) const {
    return (size >= min_blk_size && size <= max_blk_size && _root != nullptr) ||
           mem_alloc::can_alloc(size);
  }
  bool owns(memblk blk) const { return mem_alloc::owns(blk); }
  long heap_size() const { return mem_alloc::heap_size(); }
  long stack_size() const { return mem_alloc::stack_size(); }
};

/** END **/

/** Fallback allocator **/

template <class mem_alloc_primary, class mem_alloc_secondary>
class mem_alloc_fallback : private mem_alloc_primary,
                           private mem_alloc_secondary {
public:
  mem_alloc_fallback(mem_buffer_manager &buffer)
      : mem_alloc_primary(buffer), mem_alloc_secondary(buffer) {}
  memblk alloc(long size) {
    if (mem_alloc_primary::can_alloc(size)) {
      return mem_alloc_primary::alloc(size);
    } else {
      return mem_alloc_secondary::alloc(size);
    }
  }

  void free(memblk blk) {
    if (mem_alloc_primary::owns(blk)) {
      mem_alloc_primary::free(blk);
    } else {
      mem_alloc_secondary::free(blk);
    }
  }

  bool owns(memblk blk) const {
    return mem_alloc_primary::owns(blk) || mem_alloc_secondary::owns(blk);
  }
  bool can_alloc(long size) const {
    return mem_alloc_primary::can_alloc(size) ||
           mem_alloc_secondary::can_alloc(size);
  }
  long heap_size() const {
    return mem_alloc_primary::heap_size() + mem_alloc_secondary::heap_size();
  }
  long stack_size() const {
    return mem_alloc_primary::stack_size() + mem_alloc_secondary::stack_size();
  }
};

/** END **/

/** Partition allocator **/

template <class mem_alloc_small, long partition, class mem_alloc_large>
class mem_alloc_partition : private mem_alloc_small, private mem_alloc_large {
public:
  mem_alloc_partition(mem_buffer_manager &buffer)
      : mem_alloc_small(buffer), mem_alloc_large(buffer) {}
  memblk alloc(long size) {
    if (size <= partition) {
      return mem_alloc_small::alloc(size);
    } else {
      return mem_alloc_large::alloc(size);
    }
  }
  void free(memblk blk) {
    if (blk.size <= partition) {
      mem_alloc_small::free(blk);
    } else {
      mem_alloc_large::free(blk);
    }
  }
  bool owns(memblk blk) const {
    return mem_alloc_small::owns(blk) || mem_alloc_large::owns(blk);
  }
  bool can_alloc(long size) const {
    return mem_alloc_small::can_alloc(size) || mem_alloc_large::can_alloc(size);
  }
  long heap_size() const {
    return mem_alloc_small::heap_size() + mem_alloc_large::heap_size();
  }
  long stack_size() const {
    return mem_alloc_small::stack_size() + mem_alloc_large::stack_size();
  }
};

/** END **/

/** Stacked allocator using stack **/

template <long blk_size> class mem_allock_stack_s {
  static_assert(align_block<4>(blk_size) == blk_size,
                "Block size must be aligned to 4 bytes");
  static_assert(blk_size > 0 && blk_size <= KB(16), "Incorrect block size");

private:
  byte _buffer[blk_size];
  long _idx{0};

public:
  mem_allock_stack_s(mem_buffer_manager &) {}
  memblk alloc(long size) {
    memblk blk{nullptr, 0};
    long _idx_next = size + _idx;
    if (_idx_next <= blk_size) {
      blk = {&_buffer[_idx], size};
      _idx = _idx_next;
    }
    return blk;
  }
  void free(memblk blk) {
    if (&_buffer[_idx - blk.size] == blk.ptr) {
      _idx -= blk.size;
    }
  }
  bool owns(memblk blk) const {
    return blk.ptr >= _buffer && blk.ptr < _buffer + blk_size;
  }
  bool can_alloc(long size) const { return size + _idx <= blk_size; }
  long heap_size() const { return 0; }
  long stack_size() const { return blk_size; }
};

/** END **/

/** Stacked allocator using heap **/

template <long blk_size> class mem_allock_stack_h {
  static_assert(align_block<4>(blk_size) == blk_size,
                "Block size must be aligned to 4 bytes");
  static_assert(blk_size > 0, "Incorrect block size");

private:
  byte *_buffer{nullptr};
  long _idx{0};

public:
  mem_allock_stack_h(mem_buffer_manager &buffer)
      : _buffer(buffer.fetch(this)) {}
  memblk alloc(long size) {
    memblk blk{nullptr, 0};
    long _idx_next = size + _idx;
    if (_idx_next <= blk_size) {
      blk = {&_buffer[_idx], size};
      _idx = _idx_next;
    }
    return blk;
  }
  void free(memblk blk) {
    if (&_buffer[_idx - blk.size] == blk.ptr) {
      _idx -= blk.size;
    }
  }
  bool owns(memblk blk) const {
    return blk.ptr >= _buffer && blk.ptr < _buffer + blk_size;
  }
  bool can_alloc(long size) const { return size + _idx <= blk_size; }
  long heap_size() const { return blk_size; }
  long stack_size() const { return 0; }
};

template <> class mem_allock_stack_h<0l> {
public:
  mem_allock_stack_h(mem_buffer_manager &) {}
  memblk alloc(long DEBUG_USE(size)) {
    assert(size == 0);
    return memblk{nullptr, 0};
  }
  void free(memblk DEBUG_USE(blk)) { assert(blk.size == 0); }
  bool owns(memblk) const { return false; }
  bool can_alloc(long) const { return false; }
  long heap_size() const { return 0; }
  long stack_size() const { return 0; }
};

/** END **/

/** Pool alocator **/

template <long blk_size, long blk_count> struct mem_allock_pool_distrubution {
  static constexpr long mem_on_stack =
      blk_size * blk_count >= KB(16) ? KB(16) : blk_size * blk_count;
  static constexpr long mem_on_heap = blk_size * blk_count - mem_on_stack;
};

template <long min_blk_size, long max_blk_size, long blk_count>
using mem_alloc_pool_base = mem_alloc_free_list<
    min_blk_size, max_blk_size,
    mem_alloc_fallback<mem_allock_stack_s<mem_allock_pool_distrubution<
                           max_blk_size, blk_count>::mem_on_stack>,
                       mem_allock_stack_h<mem_allock_pool_distrubution<
                           max_blk_size, blk_count>::mem_on_heap>>>;

template <long min_blk_size, long max_blk_size, long blk_count>
class mem_alloc_pool
    : private mem_alloc_pool_base<min_blk_size, max_blk_size, blk_count> {
  static_assert(align_block<4>(max_blk_size) == max_blk_size,
                "Block size must be aligned to 4 bytes");
  static_assert(min_blk_size > 0, "Incorrect block size");
  static_assert(max_blk_size > 0, "Incorrect block size");
  using mem_alloc_pool_base_t =
      mem_alloc_pool_base<min_blk_size, max_blk_size, blk_count>;

public:
  mem_alloc_pool(mem_buffer_manager &buffer) : mem_alloc_pool_base_t(buffer) {}
  memblk alloc(long DEBUG_USE(size)) {
    assert(size >= min_blk_size && size <= max_blk_size);
    return mem_alloc_pool_base_t::alloc(max_blk_size);
  }
  void free(memblk blk) {
    assert(blk.size <= max_blk_size);
    if (mem_alloc_pool_base_t::owns(blk)) {
      mem_alloc_pool_base_t::free(blk);
    }
  }
  bool owns(memblk blk) const { return mem_alloc_pool_base_t::owns(blk); }
  bool can_alloc(long size) const {
    return size >= min_blk_size && size <= max_blk_size &&
           mem_alloc_pool_base_t::can_alloc(size);
  }
  long heap_size() const { return mem_alloc_pool_base_t::heap_size(); }
  long stack_size() const { return mem_alloc_pool_base_t::stack_size(); }
};

/** END **/

/** Waste calculator **/

template <class mem_alloc> class mem_alloc_waste : private mem_alloc {
private:
  long long int _waste{0};

public:
  mem_alloc_waste(mem_buffer_manager &buffer) : mem_alloc(buffer) {}
  ~mem_alloc_waste() { printf("Wasted memory: %lld (b)\n", _waste); }
  memblk alloc(long size) {
    memblk blk = mem_alloc::alloc(size);
    _waste += (blk.size - size);
    return blk;
  }
  void free(memblk blk) { mem_alloc::free(blk); }
  bool owns(memblk blk) const { return mem_alloc::owns(blk); }
  bool can_alloc(long size) const { return mem_alloc::can_alloc(size); }
  long heap_size() const { return mem_alloc::heap_size(); }
  long stack_size() const { return mem_alloc::stack_size(); }
};

/** END **/

/** Total calculator **/

template <class mem_alloc> class mem_alloc_total : private mem_alloc {
private:
  long long int _total{0};

public:
  mem_alloc_total(mem_buffer_manager &buffer) : mem_alloc(buffer) {}
  ~mem_alloc_total() { printf("Total allocated memory: %lld (b)\n", _total); }
  memblk alloc(long size) {
    memblk blk = mem_alloc::alloc(size);
    _total += blk.size;
    return blk;
  }
  void free(memblk blk) { mem_alloc::free(blk); }
  bool owns(memblk blk) const { return mem_alloc::owns(blk); }
  bool can_alloc(long size) const { return mem_alloc::can_alloc(size); }
  long heap_size() const { return mem_alloc::heap_size(); }
  long stack_size() const { return mem_alloc::stack_size(); }
};

/** END **/

/** Composite Allocator **/

using mem_alloc_composit_small = mem_alloc_partition<
    mem_alloc_partition<mem_alloc_partition<mem_alloc_pool<4, 16, MB(29)>, 16,
                                            mem_alloc_pool<17, 32, MB(10)>>,
                        32, mem_alloc_pool<33, 64, KB(1)>>,
    64, mem_alloc_pool<65, 256, MB(10)>>;

using mem_alloc_composit_large =
    mem_alloc_partition<mem_alloc_pool<257, KB(1), KB(2)>, KB(1),
                        mem_alloc_pool<KB(1) + 1, KB(4), KB(4)>>;

using mem_alloc_composit = mem_alloc_partition<mem_alloc_composit_small, 256,
                                               mem_alloc_composit_large>;

using mem_alloc_testing = mem_alloc_manager<mem_alloc_composit, mem_alloc_sys>;

/** END **/

template <long max_size, typename object_counter_next>
class object_counter : object_counter_next {
private:
  long _counter{0};
  long _max{0};

public:
  void inc(long size) {
    if (size > max_size) {
      object_counter_next::inc(size);
    } else {
      _max = glp::max(_max, ++_counter);
    }
  }
  void dec(long size) {
    if (size > max_size) {
      object_counter_next::dec(size);
    } else {
      --_counter;
    }
  }
  ~object_counter() { printf("Allocated %ld of size %ld\n", _max, max_size); }
};

class object_counter_end {
public:
  void inc(long DEBUG_USE(size)) { assert(size == 0); }
  void dec(long DEBUG_USE(size)) { assert(size == 0); }
};

class mem_alloc_null {
public:
  memblk allocate(long) { return {nullptr, 0}; }
  void deallocate(memblk DEBUG_USE(blk)) {
    assert(blk.ptr == nullptr);
    assert(blk.size == 0);
  }
};

using instance_counter = object_counter<
    16,
    object_counter<
        32, object_counter<
                64, object_counter<
                        256, object_counter<
                                 KB(1),
                                 object_counter<KB(4), object_counter_end>>>>>>;

template <typename mem_alloc>
class mem_alloc_counter : private mem_alloc, private instance_counter {

public:
  mem_alloc_counter(mem_buffer_manager &buffer) : mem_alloc(buffer) {}
  memblk alloc(long size) {
    memblk blk = mem_alloc::alloc(size);
    if (blk.ptr != nullptr) {
      instance_counter::inc(blk.size);
    }
    return blk;
  }
  void free(memblk blk) {
    if (mem_alloc::owns(blk)) {
      mem_alloc::free(blk);
      instance_counter::dec(blk.size);
    }
  }
  bool can_alloc(long size) const { return mem_alloc::can_alloc(size); }
  bool owns(memblk blk) const { return mem_alloc::owns(blk); }
  long heap_size() const { return mem_alloc::heap_size(); }
  long stack_size() const { return mem_alloc::stack_size(); }
};

using mem_alloc_stat = mem_alloc_manager<mem_alloc_counter<mem_alloc_sys>>;
}
#endif // ALLOCATOR_H
