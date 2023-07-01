#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int return_value = 0;
  int scale_value_1 = get_scale(value);
  int sign_value = get_sign(value);
  s21_decimal remains = {0};
  s21_decimal value_2 = {0};
  null_decimal(result, 4);
  s21_from_int_to_decimal(10, &value_2);
  if (scale_value_1 != 0) {
    if (scale_value_1 > 28) {
      return_value = 1;
      *result = value;
    } else {
      while (scale_value_1-- != 1) bits_mul_10(&value_2);
    }
    if (!return_value) {
      s21_div_bits(value, value_2, result, &remains);
      shift_bits_number(&value_2, '>', 1);
      if ((bit_comparison(remains, value_2) == 0 ||
           bit_comparison(remains, value_2) == -1) &&
          !sign_value) {
        s21_decimal bit_one = {0};
        s21_from_int_to_decimal(1, &bit_one);
        s21_add(*result, bit_one, result);
      } else if (bit_comparison(remains, value_2) == 0 && sign_value) {
        s21_decimal bit_one = {0};
        s21_from_int_to_decimal(1, &bit_one);
        s21_add(*result, bit_one, result);
      }
      set_sign(result, sign_value);
      s21_set_scale(result, scale_value_1);
    }
  } else {
    return_value = 0;
    *result = value;
  }
  return return_value;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int return_value = 0;
  null_decimal(result, 4);
  int scale_value_1 = get_scale(value);
  s21_decimal remains = {0};
  s21_decimal value_2 = {0};
  s21_from_int_to_decimal(10, &value_2);
  if (scale_value_1 != 0) {
    if (scale_value_1 > 28) {
      return_value = 1;
      *result = value;
    } else {
      while (scale_value_1-- != 1) {
        bits_mul_10(&value_2);
      }
    }
    if (!return_value) {
      s21_div_bits(value, value_2, result, &remains);
      shift_bits_number(&value_2, '>', 1);
      s21_decimal bit_one = {0};
      s21_from_int_to_decimal(1, &bit_one);
      if (get_sign(value)) s21_add(*result, bit_one, result);
      set_sign(result, get_sign(value));
    }
  } else {
    return_value = 0;
    *result = value;
  }
  return return_value;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int return_value = 0;
  int scale_value_1 = get_scale(value);
  int sign_value = get_sign(value);
  s21_decimal remains = {0};
  s21_decimal value_2 = {0};
  null_decimal(result, 4);
  s21_from_int_to_decimal(10, &value_2);
  if (scale_value_1 != 0) {
    if (scale_value_1 > 28) {
      return_value = 1;
      *result = value;
    } else {
      while (scale_value_1-- != 1) {
        bits_mul_10(&value_2);
      }
    }
    if (!return_value) {
      value.bits[3] = 0;
      s21_div_bits_overflow(value, value_2, result, &remains);
      set_sign(result, sign_value);
    }
  } else {
    return_value = 0;
    *result = value;
  }
  return return_value;
}
// 0 - OK, 1 - NOT OK
int s21_negate(s21_decimal value, s21_decimal *result) {
  int return_value = 0;
  s21_decimal copy_value = {0};
  if (get_scale(value) > 28) {
    return_value = 1;
  } else {
    copy_bits(&copy_value, value);
    if (get_sign(value))
      set_sign(&copy_value, -1);
    else
      set_sign(&copy_value, 1);
    copy_bits(result, copy_value);
  }
  return return_value;
}