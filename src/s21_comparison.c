#include "s21_decimal.h"
// сравнение 1 - Истина , 0 - ложь, -2 невозможно сравнить scale > 28
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int scale_value_1 = 0, scale_value_2 = 0;
  int return_value = -1;
  int sign_value_1 = get_sign(value_1);
  int sign_value_2 = get_sign(value_2);
  return_value = comparison_to_bits(value_1, value_2, &return_value,
                                    &scale_value_1, &scale_value_2);

  if (return_value != -2 && return_value == -1) {
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    alignment_scale_overflow(&scale_value_1, &scale_value_2, &value_1,
                             &value_2);
    int bit_comp = bit_comparison_overflow(value_1, value_2);
    if ((sign_value_1 + sign_value_2) == 0) {
      return_value = (bit_comp == 0 ? 1 : 0);
    } else {
      if (bit_comp != -1)
        return_value = (bit_comp == 0 ? 0 : 1);
      else
        return_value = 0;
    }
  } else if (return_value == 0)
    return_value = 1;
  else if (return_value == 1)
    return_value = 0;
  return return_value;
}

// 1 - числа равны истина, 0 - ложь, -2 - невозможно сравнить
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int return_value = -1;
  int scale_value_1 = 0, scale_value_2 = 0;
  return_value = comparison_to_bits(value_1, value_2, &return_value,
                                    &scale_value_1, &scale_value_2);
  if (return_value == -1) {
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    alignment_scale_overflow(&scale_value_1, &scale_value_2, &value_1,
                             &value_2);
    return_value = bit_comparison_overflow(value_1, value_2) == -1 ? 1 : 0;
  } else
    return_value = 0;
  return return_value;
}
// 1 - числа равны истина, 0 - ложь, -2 - невозможно сравнить
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int return_value = 0;
  int equal = s21_is_equal(value_1, value_2);
  int greater = s21_is_greater(value_1, value_2);
  int res = equal + greater;
  if (res == 1)
    return_value = 1;
  else if (res == 0)
    return_value = 0;
  else
    return_value = -2;
  return return_value;
}
// 1 - числа равны истина, 0 - ложь, -2 - невозможно сравнить
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_greater(value_1, value_2) + s21_is_equal(value_1, value_2) == 1
             ? 0
             : 1;
}
// 1 - числа равны истина, 0 - ложь, -2 - невозможно сравнить
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_equal(value_1, value_2) + s21_is_less(value_1, value_2) == 1
             ? 1
             : 0;
}
// 1 - числа равны истина, 0 - ложь, -2 - невозможно сравнить
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_equal(value_1, value_2);
}