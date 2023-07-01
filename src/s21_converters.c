#include "s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int return_val = 0;
  int scale = 0;
  int binaryexp = float_binary_exp(src);
  if (binaryexp == 128)
    return_val = 1;
  else {
    null_decimal(dst, 4);
    if (src < 0) {
      set_sign(dst, 1);
      src *= (-1);
    }
    if ((binaryexp) >= 96) {
      return_val = 1;
    } else {
      // float_binary_exp(src) < 96 && scale < MAX_POW &&
      for (; (((int)(src + 0.5)) == 0) && ((src) < MAX_DECIMAL - 1) &&
             scale < MAX_POW;
           src *= 10, scale++)
        ;
      float example = src;
      scale--;
      for (int i = 0; scale < MAX_POW && fabs(example) < 0.999999e7;
           i++, scale++, example *= 10.0) {
        src = floor(example + 0.5);
      }
      src = floor(src + 0.5f);
      if (float_binary_exp(src) >= 96) {
        src /= 10.0;
        scale--;
      }
      binaryexp = float_binary_exp(src);
      if (binaryexp >= 0) s21_set_bit(dst, binaryexp, 1);
      unsigned fbits = *((unsigned *)&src);
      int k = 0;
      for (unsigned mask = 0x800000; mask; mask >>= 1, binaryexp--) {
        k++;
        if (!!(fbits & mask)) s21_set_bit(dst, binaryexp, 1);
      }
      s21_set_scale(dst, scale);
    }
  }
  return return_val;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  null_decimal(dst, 4);
  int return_val = 0;
  if (abs(src) > INT_MAX)
    return_val = 1;
  else {
    if (src < 0) set_sign(dst, 1);
    dst->bits[0] = abs(src);
  }
  return return_val;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int return_value = 0;
  int scale = get_scale(src);
  s21_decimal result = src;
  long long dec_number = 0;
  if (scale > 28) return_value = 1;
  if (!return_value) {
    s21_decimal div_10 = {0};
    s21_from_int_to_decimal(10, &div_10);
    while (scale > 0) {
      s21_decimal remains = {0};
      s21_div_bits(result, div_10, &result, &remains);
      scale--;
    }
    unsigned bits1 = result.bits[1];
    unsigned bits2 = result.bits[2];
    if ((bits1 > 0) || bits2 > 0) return_value = 1;
    for (int i = 0; i != 32; i++) {
      if (s21_get_bit(result, i)) {
        dec_number += pow(2, i);
      }
    }
    if (get_sign(src)) dec_number *= -1;
  }
  if ((dec_number) <= INT_MAX && return_value == 0) {
    *dst = (int)dec_number;
  } else
    return_value = 1;
  return return_value;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double temp = 0.0f;
  *dst = 0;
  int return_value = 0;
  int scale = get_scale(src);
  if (scale > 28) return_value = 1;
  for (int i = 95; i >= 0; i--) {
    if (s21_get_bit(src, i)) temp += pow_2_index(i);
  }
  *dst = temp / powf(10.0, scale);

  if (get_sign(src)) *dst *= -1;
  return return_value;
}
