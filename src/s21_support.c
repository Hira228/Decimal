#include "s21_decimal.h"

int s21_get_bit(s21_decimal s21_decimal, int index) {
  int number_of_bite = index / 32;
  int number_of_bit = index % 32;
  return (s21_decimal.bits[number_of_bite] & (1u << number_of_bit)) >>
         number_of_bit;
}

void s21_set_bit(s21_decimal *s21_decimal, int index, int bit) {
  int number_of_bite = index / 32;
  int number_of_bit = index % 32;
  if (bit == 1)
    s21_decimal->bits[number_of_bite] |= (1u << number_of_bit);
  else
    s21_decimal->bits[number_of_bite] &= (~((1u) << number_of_bit));
}

// void print_decimal(s21_decimal s21_decimal) {
//   for (int i = 127; i >= 0; i--) {
//     printf("%d", s21_get_bit(s21_decimal, i));
//     if (i % 32 == 0) {
//       printf(" ");
//     }
//   }
//   printf("\n");
// }

// 0 - sign = 0,    1 - sign = 1
void set_sign(s21_decimal *s21_decimal, int sign) {
  s21_set_bit(s21_decimal, 127, sign);
}

void invert_sign(s21_decimal *s21_decimal) {
  int sign = s21_get_bit(*s21_decimal, 127);
  s21_set_bit(s21_decimal, 127, sign == 1 ? 0 : 1);
}

int get_sign(s21_decimal src) { return s21_get_bit(src, 127); }

unsigned float_binary_exp(float f) {
  unsigned fbits = *((unsigned *)&f);
  int exp = 0;
  // for (unsigned mask = 0x80000000; mask; mask >>= 1) {
  //     printf("%d",!!(fbits&mask));
  // }
  int i = 8;
  for (unsigned mask = 0x40000000; mask != 0x400000; mask >>= 1, i--) {
    exp |= (!!(fbits & mask)) << (i - 1);
  }
  return exp - 127;
}

void s21_set_scale(s21_decimal *decl, int scale) {
  for (int i = START_INFO + 16; i < START_INFO + 23; i++) {
    s21_set_bit(decl, i, scale & 1);
    scale >>= 1;
  }
}

// void print_32_bits(int x) {
//   for (unsigned mask = 0x80000000; mask; mask >>= 1)
//     printf("%d", !!(x & mask));
//   printf("\n");
// }

int get_scale(s21_decimal src) {
  int scale = 0;
  for (unsigned mask = 0x800000; mask != 0x8000; mask >>= 1) {
    scale <<= 1;
    scale |= !!(src.bits[3] & mask);
  }
  return scale;
}

float pow_2_index(int index) { return powf(2, index); }

void alignment_scale_overflow(int *scale_value_1, int *scale_value_2,
                              s21_decimal *value_1, s21_decimal *value_2) {
  if (*scale_value_1 < *scale_value_2) {
    while (*scale_value_1 != *scale_value_2) {
      bits_mul_10(value_1);
      (*scale_value_1)++;
    }
  } else {
    while (*scale_value_2 != *scale_value_1) {
      bits_mul_10(value_2);
      (*scale_value_2)++;
    }
  }
}

// сложение до 96 бита, если сложение будет переходить за 95 бит, возвращение
// единицы - слишком большее число
int add_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_value = 0;
  s21_decimal before_result = {0};
  before_result.bits[3] = result->bits[3];
  for (int i = 0; i < 100; i++) {
    int bit_value_1 = s21_get_bit(value_1, i);
    int bit_value_2 = s21_get_bit(value_2, i);
    int res_add_bit = bit_value_1 + bit_value_2 + s21_get_bit(before_result, i);
    if (res_add_bit < 0b10) {
      s21_set_bit(&before_result, i, res_add_bit);
    } else if (res_add_bit >= 0b10) {
      s21_set_bit(&before_result, i, res_add_bit & 0b1);
      s21_set_bit(&before_result, i + 1, 1);
    }
  }
  if (check_overflow(before_result) == 0)
    return_value = 1;
  else {
    null_decimal(result, 3);
    *result = before_result;
  }
  return return_value;
}

// 127 бита на операцию сложения.
void add_bits_with_overflow(s21_decimal value_1, s21_decimal value_2,
                            s21_decimal *result) {
  s21_decimal before_result = {0};
  for (int i = 0; i < 127; i++) {
    int bit_value_1 = s21_get_bit(value_1, i);
    int bit_value_2 = s21_get_bit(value_2, i);
    int res_add_bit = bit_value_1 + bit_value_2 + s21_get_bit(before_result, i);
    if (res_add_bit < 0b10) {
      s21_set_bit(&before_result, i, res_add_bit);
    } else if (res_add_bit >= 0b10) {
      s21_set_bit(&before_result, i, res_add_bit & 0b1);
      s21_set_bit(&before_result, i + 1, 1);
    }
  }
  *result = before_result;
}

int s21_sub_bits_with_overflow(s21_decimal value_1, s21_decimal value_2,
                               s21_decimal *result) {
  int return_value = 0;
  null_decimal(result, 4);
  for (int i = 0; i < 128; i++) {
    int bit_value_1 = s21_get_bit(value_1, i);
    int bit_value_2 = s21_get_bit(value_2, i);
    int res_sub_bit = bit_value_1 - bit_value_2;
    if (res_sub_bit < 0b0) {
      s21_set_bit(result, i, 1);
      if (s21_get_bit(value_1, i + 1)) {
        s21_set_bit(&value_1, i + 1, 0);
      } else {
        int j = 0;
        for (j = i + 1; s21_get_bit(value_1, j) == 0 && j != 96; j++) {
          s21_set_bit(&value_1, j, 1);
        }
        s21_set_bit(&value_1, j, 0);
      }
    } else if (res_sub_bit >= 0b0) {
      s21_set_bit(result, i, res_sub_bit & 0b1);
    }
  }
  return return_value;
}

int s21_sub_bits(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int return_value = 0;
  null_decimal(result, 3);
  for (int i = 0; i < 96; i++) {
    int bit_value_1 = s21_get_bit(value_1, i);
    int bit_value_2 = s21_get_bit(value_2, i);
    int res_sub_bit = bit_value_1 - bit_value_2;
    if (res_sub_bit < 0b0) {
      s21_set_bit(result, i, 1);
      if (s21_get_bit(value_1, i + 1)) {
        s21_set_bit(&value_1, i + 1, 0);
      } else {
        int j = 0;
        for (j = i + 1; s21_get_bit(value_1, j) == 0 && j != 96; j++) {
          s21_set_bit(&value_1, j, 1);
        }
        s21_set_bit(&value_1, j, 0);
      }
    } else if (res_sub_bit >= 0b0) {
      s21_set_bit(result, i, res_sub_bit & 0b1);
    }
  }
  return return_value;
}

// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0
// int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
//   int return_value = 0;
//   struct decimal_big
//   {
//     uint64_t[3];
//   };

// }

// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0
// b * 2 ^ q <= a
int s21_div_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                 s21_decimal *remains) {
  int k = 0;
  s21_decimal q = {0};
  s21_decimal del = {0};
  s21_decimal del_sec = {0};
  int flag = 0;
  int error = 0;
  int return_value = 0;
  copy_bits(&q, value_2);
  int num = 0;
  int new_scale = 0;
  s21_from_decimal_to_int(value_2, &num);
  s21_from_decimal_to_int(*remains, &new_scale);
  if (num == 1 && new_scale == 0) {
    *result = value_1;
  } else {
    null_decimal(remains, 4);
    if (bit_comparison(value_1, value_2) != 1) {
      while ((bit_comparison(value_1, q) == 0 ||
              bit_comparison(value_1, q) == -1)) {
        copy_bits(&q, value_2);
        k++;
        shift_bits_number(&q, '<', k);
        flag = 1;
        if (!check_overflow(q)) {
          return_value = 1;
          break;
        }
      }
      if (!return_value) {
        if (flag) {
          k--;
          s21_from_int_to_decimal(1, &del);
        }
        shift_bits_number(&del, '<', k);
        shift_bits_number(&q, '>', 1);
        s21_sub_bits(value_1, q, remains);
        while (bit_comparison(*remains, value_2) == 0 && error == 0) {
          null_decimal(&del_sec, 4);
          if (bit_comparison(*remains, value_2) == 0 ||
              bit_comparison(value_1, q) == -1) {
            int j = 0;
            copy_bits(&q, value_2);
            flag = 0;
            while (bit_comparison(*remains, q) == 0 ||
                   bit_comparison(*remains, q) == -1) {
              copy_bits(&q, value_2);
              j++;
              shift_bits_number(&q, '<', j);
              flag = 1;
            }
            if (flag) {
              j--;
              s21_from_int_to_decimal(1, &del_sec);
            }
            if (j) {
              shift_bits_number(&del_sec, '<', j);
            }
          }
          shift_bits_number(&q, '>', 1);
          // остаток
          s21_sub_bits(*remains, q, remains);
          error = add_bits(del, del_sec, &del);
        }
        *result = del;
      } else
        return_value = 1;
    } else {
      *remains = value_1;
      null_decimal(result, 4);
    }
  }
  return return_value;
}

int s21_div_bits_overflow(s21_decimal value_1, s21_decimal value_2,
                          s21_decimal *result, s21_decimal *remains) {
  int k = 0;
  s21_decimal q = {0};
  s21_decimal del = {0};
  s21_decimal del_sec = {0};
  int flag = 0;
  copy_bits(&q, value_2);
  if (bit_comparison_overflow(value_1, value_2) != 1) {
    while (bit_comparison_overflow(value_1, q) == 0 ||
           bit_comparison_overflow(value_1, q) == -1) {
      copy_bits(&q, value_2);
      k++;
      shift_bits_number_overflow(&q, '<', k);
      flag = 1;
    }
    if (flag) {
      k--;
      s21_from_int_to_decimal(1, &del);
    }
    shift_bits_number_overflow(&del, '<', k);
    shift_bits_number_overflow(&q, '>', 1);
    s21_sub_bits_with_overflow(value_1, q, remains);
    while (bit_comparison_overflow(*remains, value_2) == 0) {
      null_decimal(&del_sec, 4);
      if (bit_comparison_overflow(*remains, value_2) == 0 ||
          bit_comparison_overflow(value_1, q) == -1) {
        int j = 0;
        copy_bits(&q, value_2);
        flag = 0;
        while (bit_comparison_overflow(*remains, q) == 0 ||
               bit_comparison_overflow(*remains, q) == -1) {
          copy_bits(&q, value_2);
          j++;
          shift_bits_number_overflow(&q, '<', j);
          flag = 1;
        }
        if (flag) {
          j--;
          s21_from_int_to_decimal(1, &del_sec);
        }
        if (j) {
          shift_bits_number_overflow(&del_sec, '<', j);
        }
      }
      shift_bits_number_overflow(&q, '>', 1);
      // остаток
      s21_sub_bits_with_overflow(*remains, q, remains);
      add_bits_with_overflow(del, del_sec, &del);
    }
    *result = del;
  } else
    *remains = value_1;
  return 0;
}

// 0 - Overflow, 1 - OK, not scale, not sign
int check_overflow(s21_decimal src) {
  int return_value = 1;
  for (int i = 96; i < 95 + 14; ++i) {
    if (s21_get_bit(src, i)) {
      return_value = 0;
      break;
    }
  }
  return return_value;
}

// 0 - все биты нули, 1 - есть хотя бы одна единица
int check_full_zero_bits(s21_decimal src) {
  int return_value = 0;
  for (int i = 0; i < 96; i++) {
    if (s21_get_bit(src, i)) {
      return_value = 1;
      break;
    }
  }
  return return_value;
}
// 1 - NAN, 2 - -NAN , 0 - OK
int check_scale_less_28(int scale_value_1, int scale_value_2, int sign_value_1,
                        int sign_value_2) {
  int return_value = 0;
  if (scale_value_1 > 28 || scale_value_2 > 28) {
    if (sign_value_1 == 0)
      return_value = 1;
    else if (sign_value_1 == 1)
      return_value = 2;
    else if (sign_value_2 == 0)
      return_value = 1;
    else if (sign_value_2 == 1)
      return_value = 2;
  }
  return return_value;
}

void copy_bits(s21_decimal *src, s21_decimal dest) {
  for (int i = 0; i < 4; i++) {
    src->bits[i] = dest.bits[i];
  }
}

void null_decimal(s21_decimal *src, int index) {
  for (int i = 0; i < index; i++) {
    src->bits[i] = 0;
  }
}

void shift_bits_number(s21_decimal *src, char c, int shift) {
  s21_decimal src_copy = {0};
  copy_bits(&src_copy, *src);
  null_decimal(src, 3);
  switch (c) {
    case '<': {
      for (int i = 0; shift != 127; i++, shift++)
        s21_set_bit(src, shift, s21_get_bit(src_copy, i));
      break;
    }
    case '>': {
      for (int i = 95; i >= shift; i--)
        s21_set_bit(src, i - shift, s21_get_bit(src_copy, i));
      break;
    }
  }
}

void shift_bits_number_overflow(s21_decimal *src, char c, int shift) {
  s21_decimal src_copy = {0};
  copy_bits(&src_copy, *src);
  null_decimal(src, 3);
  switch (c) {
    case '<': {
      for (int i = 0; shift != 128; i++, shift++)
        s21_set_bit(src, shift, s21_get_bit(src_copy, i));
      break;
    }
    case '>': {
      for (int i = 127; i >= shift; i--)
        s21_set_bit(src, i - shift, s21_get_bit(src_copy, i));
      break;
    }
  }
}

// умножаем число на 10, тем самым увеличиваем степень на единицу
// void scale_increase(s21_decimal *scr) {
//   s21_set_scale(scr, get_scale(*scr) + 1);
//   s21_decimal src_left_1, src_left_3;
//   copy_bits(&src_left_1, *scr);
//   copy_bits(&src_left_3, *scr);
//   shift_bits_number(&src_left_1, '<', 1);
//   shift_bits_number(&src_left_3, '<', 3);
//   add_bits(src_left_1, src_left_3, scr);
// }

// умножения числа на 10, без мантиссы
void bits_mul_10(s21_decimal *scr) {
  s21_decimal src_left_1, src_left_3;
  copy_bits(&src_left_1, *scr);
  copy_bits(&src_left_3, *scr);
  shift_bits_number(&src_left_1, '<', 1);
  shift_bits_number(&src_left_3, '<', 3);
  add_bits_with_overflow(src_left_1, src_left_3, scr);
}

// проверяем знак и scale, изначально return_value = -1, что ознает пока все
// равно. 0 - первое число больше, 1 - второе число больше, -2 - невозможно
// сравнить
int comparison_to_bits(s21_decimal value_1, s21_decimal value_2,
                       int *return_value, int *scale_value_1,
                       int *scale_value_2) {
  *scale_value_1 = get_scale(value_1);
  *scale_value_2 = get_scale(value_2);
  if (*scale_value_1 > 28 || *scale_value_2 > 28)
    *return_value = -2;
  else {
    int sign_value_1 = get_sign(value_1);
    int sign_value_2 = get_sign(value_2);
    if (sign_value_1 < sign_value_2)
      *return_value = 0;
    else if (sign_value_2 < sign_value_1)
      *return_value = 1;
  }
  return *return_value;
}

// 0 - биты первого числа больше второго, 1 - биты второго числа больше первого,
// -1 - биты равны;
int bit_comparison(s21_decimal value_1, s21_decimal value_2) {
  int return_value = -1;
  for (int i = 95; i >= 0; --i) {
    if (s21_get_bit(value_1, i) > s21_get_bit(value_2, i)) {
      return_value = 0;
      break;
    } else if (s21_get_bit(value_1, i) < s21_get_bit(value_2, i)) {
      return_value = 1;
      break;
    }
  }
  return return_value;
}

// 0 - биты первого числа больше второго, 1 - биты второго числа больше первого,
// -1 - биты равны;
int bit_comparison_overflow(s21_decimal value_1, s21_decimal value_2) {
  int return_value = -1;
  for (int i = 126; i >= 0; --i) {
    if (s21_get_bit(value_1, i) > s21_get_bit(value_2, i)) {
      return_value = 0;
      break;
    } else if (s21_get_bit(value_1, i) < s21_get_bit(value_2, i)) {
      return_value = 1;
      break;
    }
  }
  return return_value;
}

void bank_rounding(s21_decimal *result, int *n, s21_decimal rem) {
  if (*n != 0) {
    s21_decimal half_div_10 = {{5, 0, 0, 0}};
    while ((*n)-- != 1) {
      bits_mul_10(&half_div_10);
    }
    if (bit_comparison(half_div_10, rem)) {
      s21_decimal one_bits = {{1, 0, 0, 0}};
      add_bits_with_overflow(*result, one_bits, result);
    } else if (bit_comparison(half_div_10, rem) == -1) {
      if ((unsigned)(result->bits[0]) % 2 != 0) {
        s21_decimal one_bits = {{1, 0, 0, 0}};
        add_bits(*result, one_bits, result);
      }
    }
  }
}
