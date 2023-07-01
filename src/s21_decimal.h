#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DECIMAL powl(2.0, 96) - 1
#define MIN_DECIMAL -1 * MAX_DECIMAL
#define MAX_POW 28
#define FLOAT_ACCURACY 7
#define START_INFO 96

typedef struct s21_decimal {
  unsigned bits[4];
} s21_decimal;
void set_sign(s21_decimal *s21_decimal, int sign);
void invert_sign(s21_decimal *s21_decimal);
void copy_bits(s21_decimal *src, s21_decimal dest);
void null_decimal(s21_decimal *src, int index);
int s21_get_bit(s21_decimal s21_decimal, int index);
void s21_set_bit(s21_decimal *s21_decimal, int index, int bit);
int add_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int bit_comparison(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int comparison_to_bits(s21_decimal value_1, s21_decimal value_2,
                       int *return_value, int *scale_value_1,
                       int *scale_value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);
int s21_negate(s21_decimal value, s21_decimal *result);
int check_full_zero_bits(s21_decimal src);
int check_scale_less_28(int scale_value_1, int scale_value_2, int sign_value_1,
                        int sign_value_2);
void shift_bits_number(s21_decimal *src, char c, int shift);
int check_overflow(s21_decimal src);
void bits_mul_10(s21_decimal *scr);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div_bits(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                 s21_decimal *remains);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_floor(s21_decimal value, s21_decimal *result);
void add_bits_with_overflow(s21_decimal value_1, s21_decimal value_2,
                            s21_decimal *result);
void print_decimal(s21_decimal s21_decimal);
int get_sign(s21_decimal src);
void s21_set_scale(s21_decimal *decl, int scale);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
void print_32_bits(int x);
int get_scale(s21_decimal src);
float pow_2_index(int index);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
unsigned float_binary_exp(float f);
void alignment_scale_overflow(int *scale_value_1, int *scale_value_2,
                              s21_decimal *value_1, s21_decimal *value_2);
int bit_comparison_overflow(s21_decimal value_1, s21_decimal value_2);
int s21_sub_bits_with_overflow(s21_decimal value_1, s21_decimal value_2,
                               s21_decimal *result);

int s21_div_bits_overflow(s21_decimal value_1, s21_decimal value_2,
                          s21_decimal *result, s21_decimal *remains);
void shift_bits_number_overflow(s21_decimal *src, char c, int shift);
void bank_rounding(s21_decimal *result, int *n, s21_decimal rem);
#endif