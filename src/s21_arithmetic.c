#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  null_decimal(result, 4);
  int return_value = 0;
  int scale_value_1 = get_scale(value_1);
  int scale_value_2 = get_scale(value_2);
  int sign_value_1 = get_sign(value_1);
  int sign_value_2 = get_sign(value_2);
  s21_decimal div_10 = {0};
  s21_decimal rem = {0};
  s21_from_int_to_decimal(10, &div_10);
  if (scale_value_1 > 28) {
    if (sign_value_1 == 1)
      return_value = 2;
    else
      return_value = 1;
  } else if (scale_value_2 > 28) {
    if (sign_value_2 == 1)
      return_value = 2;
    else
      return_value = 1;
  } else {
    value_1.bits[3] = 0;
    value_2.bits[3] = 0;
    alignment_scale_overflow(&scale_value_1, &scale_value_2, &value_1,
                             &value_2);
    if (!(sign_value_1 | sign_value_2)) {  // + + +
      add_bits_with_overflow(value_1, value_2, result);
      int n = 0;
      while (!check_overflow(*result) && scale_value_1-- != 0) {
        s21_div_bits_overflow(*result, div_10, result, &rem);
        n++;
      }
      bank_rounding(result, &n, rem);
      if (!check_overflow(*result)) {
        return_value = 1;
        null_decimal(result, 4);
      }
    } else if (((sign_value_1 + sign_value_2) & 0b10) == 2) {  // - + -
      add_bits_with_overflow(value_1, value_2, result);
      int n = 0;
      while (!check_overflow(*result) && scale_value_1-- != 0) {
        s21_div_bits_overflow(*result, div_10, result, &rem);
        n++;
      }
      bank_rounding(result, &n, rem);
      set_sign(result, 1);
      if (!check_overflow(*result)) {
        return_value = 2;
        null_decimal(result, 4);
      }
    } else if ((sign_value_1 | sign_value_2) == 0b1) {
      int sign = bit_comparison_overflow(value_1, value_2);
      s21_sub_bits_with_overflow(sign == 1 ? value_2 : value_1,
                                 sign == 1 ? value_1 : value_2, result);
      int n = 0;
      while (!check_overflow(*result) && scale_value_1-- != 0) {
        s21_div_bits_overflow(*result, div_10, result, &rem);
        n++;
      }
      bank_rounding(result, &n, rem);
      if (!check_overflow(*result)) {
        if (sign_value_1 == 1 && sign == 0)
          return_value = 1;
        else if (sign_value_2 == 1 && sign == 1)
          return_value = 2;
      }
      if (sign_value_1 == 1 && sign == 0)
        set_sign(result, 1);
      else if (sign_value_2 == 1 && sign == 1)
        set_sign(result, 1);
    }
    s21_set_scale(result, scale_value_1);
  }
  return return_value;
}

// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal copy_value_2 = {0};
  copy_bits(&copy_value_2, value_2);
  invert_sign(&copy_value_2);
  int return_value = s21_add(value_1, copy_value_2, result);
  return return_value;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_value = 0;
  null_decimal(result, 4);
  if (check_full_zero_bits(value_2)) {
    if (check_full_zero_bits(value_1)) {
      int sign_value_1 = get_sign(value_1);
      int sign_value_2 = get_sign(value_2);
      int scale_value_1 = get_scale(value_1);
      int scale_value_2 = get_scale(value_2);
      int error = 0;
      if (check_scale_less_28(scale_value_1, scale_value_2, sign_value_1,
                              sign_value_2) == 0) {
        int new_scale = (scale_value_1 - scale_value_2);
        value_1.bits[3] = 0;
        value_2.bits[3] = 0;
        while (bit_comparison(value_1, value_2) == 1) {
          bits_mul_10(&value_1);
          new_scale++;
        }
        if (check_overflow(value_1)) {
          s21_decimal remains = {0};
          s21_decimal integer_part = {0};
          s21_from_int_to_decimal(new_scale, &remains);
          return_value =
              s21_div_bits(value_1, value_2, &integer_part, &remains);
          if (!return_value) {
            error = add_bits(integer_part, *result, result);
            // пока у нас остаток не будет равен нулю 1) находит результат
            // деления 2) находим остаток 3) если остаток не равен нулю мы
            // увеличиваем его на 10 и результат деления тоже умножаем на 10
            // скеил тоже увеличваем 3) находим результат деления присваиваем
            // его предыдущему и так в цикле
            while (check_full_zero_bits(remains) && error == 0 &&
                   new_scale != 28) {
              bits_mul_10(&remains);
              s21_div_bits(remains, value_2, &integer_part, &remains);
              s21_decimal over = {0};
              copy_bits(&over, *result);
              bits_mul_10(&over);
              if (check_overflow(over))
                copy_bits(result, over);
              else
                break;
              new_scale++;
              error = add_bits(integer_part, *result, result);
            }
            if (((sign_value_1 + sign_value_2) & 0b1) == 1) set_sign(result, 1);
            s21_set_scale(result, new_scale);
          } else {
            if (((sign_value_1 + sign_value_2) & 0b1) == 0)
              return_value = 1;
            else
              return_value = 2;
          }
        } else {
          if (((sign_value_1 + sign_value_2) & 0b1) == 0)
            return_value = 1;
          else
            return_value = 2;
        }
      } else
        return_value = check_scale_less_28(scale_value_1, scale_value_2,
                                           sign_value_1, sign_value_2);
    } else {
      return_value = 0;
      null_decimal(result, 4);
    }
  } else
    return_value = 3;
  return return_value;
}

// 0 - OK, 1 - переполнение;
// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_value = 0;
  null_decimal(result, 4);
  int sign_value_1 = get_sign(value_1);
  int sign_value_2 = get_sign(value_2);
  int scale_value_1 = get_scale(value_1);
  int scale_value_2 = get_scale(value_2);
  int new_scale = scale_value_1 + scale_value_2;
  for (int i = 0; i < 96; i++) {
    s21_decimal copy_value_1 = {0};
    copy_bits(&copy_value_1, value_1);
    s21_set_scale(&copy_value_1, 0);

    if (s21_get_bit(value_2, i)) {
      shift_bits_number_overflow(&copy_value_1, '<', i);
      add_bits_with_overflow(*result, copy_value_1, result);
      while (!check_overflow(*result) && new_scale > 0) {
        new_scale--;
        s21_decimal div_10 = {0};
        s21_from_int_to_decimal(10, &div_10);
        s21_div(*result, div_10, result);
      }
    }
  }
  if (!check_overflow(*result) || new_scale > MAX_POW) {
    if (((sign_value_1 | sign_value_2) & 0b1) == 0)
      return_value = 1;
    else {
      return_value = 2;
    }
    null_decimal(result, 4);
  }
  if (return_value == 0) {
    if (result->bits[0] != 0) s21_set_scale(result, new_scale);
    set_sign(result, 0);
    if (((sign_value_1 + sign_value_2) & 0b1) == 0) {
      set_sign(result, 0);
    } else {
      if (result->bits[0] != 0) set_sign(result, 1);
    }
  }
  return return_value;
}