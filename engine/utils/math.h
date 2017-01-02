#ifndef MATH_H
#define MATH_H

template <typename _Tp> constexpr _Tp min(_Tp x, _Tp y) {
  return y ^ ((x ^ y) & -(x < y));
}

template <typename T> constexpr T max(T x, T y) {
  return x ^ ((x ^ y) & -(x < y));
}

#endif // MATH_H
