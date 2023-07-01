#include <check.h>

#include "../s21_decimal.h"
#define FALSE 0
#define TRUE 1
#define SIGN 31
#define  START_SCALE 16
START_TEST(tests_additional) {
  s21_decimal dst1, dst2;

  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  ck_assert_int_eq(s21_is_equal(dst1, dst2), 0);
}
END_TEST

START_TEST(test_is_equal) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;
  int mask_del3 = 3 << START_SCALE;

  // 1,2345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  ck_assert_int_eq(s21_is_equal(dst1, dst2), FALSE);  // 1,2345678 == 1,2

  // 1,2
  dst1.bits[0] = 12;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del2;
  ck_assert_int_eq(s21_is_equal(dst1, dst2), TRUE);  // 1,2 == 1,2

  dst1.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_is_equal(dst1, dst2), FALSE);  // -1,2 == 1,2

  dst1.bits[3] = mask_minus | mask_del3;
  dst2.bits[3] = mask_minus | mask_del3;
  ck_assert_int_eq(s21_is_equal(dst1, dst2), TRUE);  // -0,012 == -0,012

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 1,0
  dst2.bits[0] = 10;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;
  ck_assert_int_eq(s21_is_equal(dst1, dst2), TRUE);  // 1 == 1.0
}
END_TEST

START_TEST(test_is_not_equal) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;
  int mask_del3 = 3 << START_SCALE;

  // 12345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  // 1,2345678
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;
  ck_assert_int_eq(s21_is_not_equal(dst1, dst2), TRUE);  // 1,2345678 == 1,2

  // 1,2
  dst1.bits[0] = 12;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del2;
  ck_assert_int_eq(s21_is_not_equal(dst1, dst2), FALSE);  // 1,2 == 1,2

  dst1.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_is_not_equal(dst1, dst2), TRUE);  // -1,2 == 1,2

  dst1.bits[3] = mask_minus | mask_del3;
  dst2.bits[3] = mask_minus | mask_del3;
  ck_assert_int_eq(s21_is_not_equal(dst1, dst2), FALSE);  // -0,012 == -0,012
}
END_TEST

START_TEST(test_s21_is_less) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // 1,2345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  ck_assert_int_eq(s21_is_less(dst1, dst2), FALSE);  // 1,2345678 < 1,2
  ck_assert_int_eq(s21_is_less(dst2, dst1), TRUE);   // 1.2 < 1.2345678
  ck_assert_int_eq(s21_is_less(dst2, dst2), FALSE);  // 1.2 < 1.2

  dst1.bits[3] |= mask_minus;
  dst2.bits[3] |= mask_minus;
  ck_assert_int_eq(s21_is_less(dst1, dst2), TRUE);   // -1.2345678 < -1.2
  ck_assert_int_eq(s21_is_less(dst2, dst1), FALSE);  // -1.2 < -1.2345678
  ck_assert_int_eq(s21_is_less(dst2, dst2), FALSE);  // -1.2 < -1.2

  dst1.bits[3] = mask_del;
  ck_assert_int_eq(s21_is_less(dst1, dst2), FALSE);  //  1.2345678 < -1.2
  ck_assert_int_eq(s21_is_less(dst2, dst1), TRUE);   //  -1.2 < 1.2345678

  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;
  dst2.bits[0] = 0;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;
  ck_assert_int_eq(s21_is_less(dst1, dst2), FALSE);  // 0 < 0
}
END_TEST

START_TEST(test_s21_less_or_equal) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // 1,2345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0 | mask_del2;

  ck_assert_int_eq(s21_is_less_or_equal(dst1, dst2),
                   FALSE);                                   // 1,2345678 <= 1,2
  ck_assert_int_eq(s21_is_less_or_equal(dst2, dst1), TRUE);  // 1.2 <= 1.2345
  ck_assert_int_eq(s21_is_less_or_equal(dst2, dst2), TRUE);  // 1.2 <= 1.2

  dst1.bits[3] |= mask_minus;
  dst2.bits[3] |= mask_minus;
  ck_assert_int_eq(s21_is_less_or_equal(dst1, dst2),
                   TRUE);  // -1.2345678 <= -1.2
  ck_assert_int_eq(s21_is_less_or_equal(dst2, dst1),
                   FALSE);  // -1.2 <= -1.2345678
  ck_assert_int_eq(s21_is_less_or_equal(dst2, dst2), TRUE);  // -1.2 <= -1.2

  dst1.bits[3] = mask_del;
  ck_assert_int_eq(s21_is_less_or_equal(dst1, dst2),
                   FALSE);  //  1.2345678 <= -1.2
  ck_assert_int_eq(s21_is_less_or_equal(dst2, dst1),
                   TRUE);  //  -1.2 <= 1.2345678

  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;
  dst2.bits[0] = 0;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;
  ck_assert_int_eq(s21_is_less_or_equal(dst1, dst2), TRUE);  // 0 <= 0
}
END_TEST

START_TEST(test_s21_is_greater) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // 1,2345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_is_greater(dst1, dst2), TRUE);   // 1.2345678 > 1.2
  ck_assert_int_eq(s21_is_greater(dst2, dst1), FALSE);  // 1.2 > 1.2345678
  ck_assert_int_eq(s21_is_greater(dst2, dst2), FALSE);  // 1.2 > 1.2

  dst1.bits[3] |= mask_minus;
  dst2.bits[3] |= mask_minus;
  ck_assert_int_eq(s21_is_greater(dst1, dst2), FALSE);  // -1.2345678 > -1.2
  ck_assert_int_eq(s21_is_greater(dst2, dst1), TRUE);   // -1.2 > 1.2345678
  ck_assert_int_eq(s21_is_greater(dst2, dst2), FALSE);  // -1.2 > -1.2

  dst1.bits[3] = mask_del;
  ck_assert_int_eq(s21_is_greater(dst1, dst2), TRUE);   //  1.2345678 > -1.2
  ck_assert_int_eq(s21_is_greater(dst2, dst1), FALSE);  //  -1.2 > 1.2345678

  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;
  dst2.bits[0] = 0;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;
  ck_assert_int_eq(s21_is_greater(dst1, dst2), FALSE);  // 0 > 0
}
END_TEST

START_TEST(test_s21_is_greater_or_equal) {
  s21_decimal dst1, dst2;

  int mask_minus = 1 << SIGN;
  int mask_del = 7 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // 1,2345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 1,2
  dst2.bits[0] = 12;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_is_greater_or_equal(dst1, dst2),
                   TRUE);  // 1.2345678 >= 1.2
  ck_assert_int_eq(s21_is_greater_or_equal(dst2, dst1),
                   FALSE);  // 1.2 >= 1.2345678
  ck_assert_int_eq(s21_is_greater_or_equal(dst2, dst2), TRUE);  // 1.2 >= 1.2

  dst1.bits[3] |= mask_minus;
  dst2.bits[3] |= mask_minus;
  ck_assert_int_eq(s21_is_greater_or_equal(dst1, dst2),
                   FALSE);  // -1.2345678 >= -1.2
  ck_assert_int_eq(s21_is_greater_or_equal(dst2, dst1),
                   TRUE);  // -1.2 >= 1.2345678
  ck_assert_int_eq(s21_is_greater_or_equal(dst2, dst2), TRUE);  // -1.2 >= -1.2

  dst1.bits[3] = mask_del;
  ck_assert_int_eq(s21_is_greater_or_equal(dst1, dst2),
                   TRUE);  //  1.2345678 >= -1.2
  ck_assert_int_eq(s21_is_greater_or_equal(dst2, dst1),
                   FALSE);  //  -1.2 >= 1.2345678

  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;
  dst2.bits[0] = 0;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;
  ck_assert_int_eq(s21_is_greater_or_equal(dst1, dst2), TRUE);  // 0 >= 0
}
END_TEST

START_TEST(test_s21_from_int_to_decimal) {
  s21_decimal val;

unsigned mask_minus = 1 << SIGN;

  s21_from_int_to_decimal(0, &val);
  ck_assert_int_eq(val.bits[0], 0);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], 0);

  s21_from_int_to_decimal(-128, &val);
  ck_assert_int_eq(val.bits[0], 128);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_uint_eq(val.bits[3], mask_minus);

  s21_from_int_to_decimal(127, &val);
  ck_assert_int_eq(val.bits[0], 127);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], 0);

  s21_from_int_to_decimal(-2147483647, &val);
  ck_assert_int_eq(val.bits[0], 2147483647);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_uint_eq(val.bits[3], mask_minus);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal) {
  s21_decimal val;

  unsigned mask_minus = 1 << SIGN;
  int mask_del_4 = 4 << START_SCALE;
  int mask_del_8 = 8 << START_SCALE;
  int mask_del_9 = 9 << START_SCALE;
  int mask_del_11 = 11 << START_SCALE;
  int mask_del_12 = 12 << START_SCALE;
  unsigned mask_del_4_minus = mask_del_4 | mask_minus;
  unsigned mask_del_9_minus = mask_del_9 | mask_minus;
  unsigned mask_del_12_minus = mask_del_12 | mask_minus;

  ck_assert_int_eq(s21_from_float_to_decimal(0.0f, &val), 0);
  ck_assert_int_eq(val.bits[0], 0);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3] >> 31 & 1, 0);

  ck_assert_int_eq(s21_from_float_to_decimal(-128.25f, &val), 0);
  ck_assert_int_eq(val.bits[0], 1282500);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
 ck_assert_int_eq(val.bits[3], mask_del_4_minus);

  ck_assert_int_eq(s21_from_float_to_decimal(127.08f, &val), 0);
  ck_assert_int_eq(val.bits[0], 1270800);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_4);

  ck_assert_int_eq(s21_from_float_to_decimal(-214.748f, &val), 0);
  ck_assert_int_eq(val.bits[0], 2147480);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_4_minus);

  ck_assert_int_eq(s21_from_float_to_decimal(214.7483f, &val), 0);
  ck_assert_int_eq(val.bits[0], 2147483);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_4);

  ck_assert_int_eq(s21_from_float_to_decimal(0.02f, &val), 0);
  ck_assert_int_eq(val.bits[0], 2000000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_8);

  ck_assert_int_eq(s21_from_float_to_decimal(-0.000001f, &val), 0);
  ck_assert_int_eq(val.bits[0], 1000000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_12_minus);

  ck_assert_int_eq(s21_from_float_to_decimal(-0.009403f, &val), 0);
  ck_assert_int_eq(val.bits[0], 9403000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_9_minus);

  ck_assert_int_eq(s21_from_float_to_decimal(0.00009403f, &val), 0);
  ck_assert_int_eq(val.bits[0], 9403000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], mask_del_11);

  ck_assert_int_eq(s21_from_float_to_decimal(0.000000000000009403f, &val), 0);
  ck_assert_int_eq(val.bits[0], 9403000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], 21 << START_SCALE);

  ck_assert_int_eq(s21_from_float_to_decimal(0.0 / 0.0, &val), 1);
  ck_assert_int_eq(val.bits[0], 9403000);
  ck_assert_int_eq(val.bits[1], 0);
  ck_assert_int_eq(val.bits[2], 0);
  ck_assert_int_eq(val.bits[3], 21 << START_SCALE);
}
END_TEST

float getPrecision(float src) {
  int exponent = floor(log10(fabs(src)));
  return pow(10, exponent - 6);
}

START_TEST(test_s21_from_decimal_to_int) {
  s21_decimal dst;
  int value = -1;
  int mask_minus = 1 << SIGN;
  int mask_del = 2 << START_SCALE;

  // 1
  dst.bits[0] = 1;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = 0;

  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, 1);

  // -1
  value = -2;
  dst.bits[3] = mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, -1);

  // 12345
  value = -1;
  dst.bits[0] = 123456;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = 0;

  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, 123456);

  // -12345
  value = -1;
  dst.bits[3] = mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, -123456);

  // переполнение 2 байта
  value = -1;
  dst.bits[0] = 0;
  dst.bits[1] = 123456;
  dst.bits[2] = 0;
  dst.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 1);
  ck_assert_int_eq(value, -1);

  // переполнение 3 байта
  value = -1;
  dst.bits[0] = 0;
  dst.bits[1] = 0;
  dst.bits[2] = 123456;
  dst.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 1);
  ck_assert_int_eq(value, -1);

  // 1234.56
  value = -1;
  dst.bits[0] = 123456;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = mask_del;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, 1234);

  // -1234.56
  value = -1;
  dst.bits[0] = 123456;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = mask_del | mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, -1234);

  // переполненное float
  value = -1;
  dst.bits[0] = 0;
  dst.bits[1] = 123456;
  dst.bits[2] = 0;
  dst.bits[3] = mask_del;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 1);
  ck_assert_int_eq(value, -1);

  // 0
  value = -1;
  dst.bits[0] = 0;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, 0);

  // макс инт
  value = -1;
  dst.bits[0] = 0b01111111111111111111111111111111;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = 0;

  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, 0b01111111111111111111111111111111);

  // макс минус инт
  value = -1;
  dst.bits[0] = 0b01111111111111111111111111111111;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = mask_minus;

  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 0);
  ck_assert_int_eq(value, -2147483647);

  // макс инт инт
  value = -1;
  dst.bits[0] = 0b11111111111111111111111111111111;
  dst.bits[1] = 0;
  dst.bits[2] = 0;
  dst.bits[3] = 0;

  ck_assert_int_eq(s21_from_decimal_to_int(dst, &value), 1);
  ck_assert_int_eq(value, -1);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float) {
  s21_decimal val;

  int mask_minus = 1 << SIGN;
  int mask_del_2 = 2 << START_SCALE;
  int mask_del_4 = 4 << START_SCALE;
  int mask_del_6 = 6 << START_SCALE;
  int mask_del_8 = 8 << START_SCALE;
  int mask_del_10 = 10 << START_SCALE;
  int mask_del_2_minus = mask_del_2 | mask_minus;
  int mask_del_4_minus = mask_del_4 | mask_minus;
  int mask_del_6_minus = mask_del_6 | mask_minus;
  int mask_del_8_minus = mask_del_8 | mask_minus;
  int mask_del_10_minus = mask_del_10 | mask_minus;

  float result = -1;
  // 0.0
  val.bits[0] = 0;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 0);

  result = -2;
  // 1.0
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 1);
  result = -2;
  // -1.0
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -1);

  result = -2;
  // 1.51
  val.bits[0] = 151;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_2;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 1.51);
  result = -2;
  // -1.51
  val.bits[0] = 151;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_2_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -1.51);

  result = -2;
  // 0.000001
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_6;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 0.000001);
  result = -2;
  // -0.000001
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_6_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -0.000001);

  result = -2;
  // 0.00000001
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_8;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, (float)0.00000001);
  result = -2;
  // -0.00000001
  val.bits[0] = 1;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_8_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, (float)-0.00000001);

  result = -2;
  // 123.4567
  val.bits[0] = 1234567;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_4;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 123.4567);
  result = -2;
  // -123.4567
  val.bits[0] = 1234567;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_4_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -123.4567);

  result = -2;
  // 1234567
  val.bits[0] = 1234567;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 1234567.0);

  result = -2;
  // -1234567
  val.bits[0] = 1234567;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -1234567.0);

  result = -2;
  // занят 1 байт
  val.bits[0] = 0;
  val.bits[1] = 1234;
  val.bits[2] = 0;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, 5.29999 * pow(10, 12), pow(10, 12 - 6));

  result = -2;
  // занят 2 байт
  val.bits[0] = 0;
  val.bits[1] = 0;
  val.bits[2] = 1234;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, 2.276328 * pow(10, 22), pow(10, 22 - 6));

  result = -2;
  // 12345678 (переполнение целой части float)
  val.bits[0] = 12345678;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = 0;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, 12345678.0f);

  result = -2;
  // -12345678 (переполнение целой части float)
  val.bits[0] = 12345678;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq(result, -12345678.0f);

  result = -2;
  // 12345678 (переполнение дробной части float)
  val.bits[0] = 12345678;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_8;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, 0.1234568, 0.0000001);

  result = -2;
  // -12345678 (переполнение дробной части float)
  val.bits[0] = 12345678;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_8_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, -0.1234567, 0.0000001);

  result = -2;
  // 1234567899 (переполнение дробной части float)
  val.bits[0] = 1234567899;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_10;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, 0.1234567, 0.0000001);

  result = -2;
  // -1234567899 (переполнение дробной части float)
  val.bits[0] = 1234567899;
  val.bits[1] = 0;
  val.bits[2] = 0;
  val.bits[3] = mask_del_10_minus;
  ck_assert_int_eq(s21_from_decimal_to_float(val, &result), 0);
  ck_assert_float_eq_tol(result, -0.1234567, 0.0000001);
}
END_TEST

START_TEST(test_s21_add) {
  s21_decimal dst1, dst2, result, result_to_eq;

  unsigned mask_minus = 1 << SIGN;
  int mask_del = 6 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // -----------------Операции с целыми числами-----------------

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // 3
  result_to_eq.bits[0] = 3;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  // 0
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], result_to_eq.bits[0]);  // 1+2 = 3;

  // -1
  dst1.bits[3] = mask_minus;
  // -2
  dst2.bits[3] = mask_minus;
  // -3
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)+(-2) = -3;

  // 2
  dst2.bits[3] = 0;
  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)+2 = 1;

  // 1
  dst1.bits[3] = 0;
  // -2
  dst2.bits[3] |= mask_minus;
  // -1
  result_to_eq.bits[3] |= mask_minus;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1+(-2) = -1;

  //----------------Операции с целым числом и дробным-----------------

  // 16
  dst1.bits[0] = 16;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 21.6
  dst2.bits[0] = 216;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0 | mask_del2;

  // 37.6
  result_to_eq.bits[0] = 376;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 16 + 21,6 = 37,6;

  // -21,6
  dst2.bits[3] = mask_minus | mask_del2;

  // -5.6
  result_to_eq.bits[0] = 56;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 56);
  ck_assert_int_eq(result.bits[3], mask_minus | mask_del2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 16 + (-21,6) = -5,6;

  // -16
  dst1.bits[3] = 0 | mask_minus;
  // 376
  result_to_eq.bits[0] = 376;
  // -37,6
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-16) + (-21,6) = -37,6;

  // ----------------Операции с двумя дробными числами-----------------

  // 12345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  // 12,345678
  dst1.bits[3] = mask_del;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  // 14,045678
  result_to_eq.bits[0] = 14045678;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 + 1,7 = 14,045678;

  // - 1,7
  dst2.bits[3] = mask_minus | mask_del2;
  result_to_eq.bits[0] = 10645678;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 + (-1,7) = 10,645678;

  // -12,345678
  dst1.bits[3] = mask_minus | mask_del;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;
  // -10,645678
  result_to_eq.bits[3] = mask_minus | mask_del;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-12,345678) + 1,7 = -10,645678;

  // ----------------Операции с нулем-----------------

  // 0
  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  // 1,7
  result_to_eq.bits[0] = 17;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0 + 1,7 = 1,7;

  // 0,0
  dst1.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0,0 + 1,7 = 1,7;

  // -0
  dst1.bits[3] = 0 | mask_minus;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-0) + 1,7 = 1,7;

  // -0,0
  dst1.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // -0,0 + 1,7 = 1,7;
  // -1,7
  dst1.bits[0] = 17;
  dst1.bits[3] = mask_minus | mask_del2;
  // 0
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // -1,7 + 1,7 = 0;

  // ----------------Операции с переполнением-----------------

  // 79,228,162,514,264,337,593,543,950,335
  dst1.bits[0] = 0b11111111111111111111111111111111;
  dst1.bits[1] = 0b11111111111111111111111111111111;
  dst1.bits[2] = 0b11111111111111111111111111111111;
  dst1.bits[3] = 0;

  // -0.6
  dst2.bits[0] = 6;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_minus | mask_del2;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // 79,228,162,514,264,337,593,543,950,334
  result_to_eq.bits[0] = 0b11111111111111111111111111111110;
  result_to_eq.bits[1] = 0b11111111111111111111111111111111;
  result_to_eq.bits[2] = 0b11111111111111111111111111111111;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 0);  // функция возвращает 0;
  // ck_assert_int_eq(result.bits[0], 0b11111111111111111111111111111111);
  // ck_assert_int_eq(result.bits[1], 0b11111111111111111111111111111111);
  // ck_assert_int_eq(result.bits[2], 0b11111111111111111111111111111111);
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1); // макс + (-0,6) =
  // банковское округление;

  // 0.6
  dst2.bits[3] = mask_del2;
  // 79,228,162,514,264,337,593,543,950,334
  result_to_eq.bits[0] = 0b11111111111111111111111111111111;
  result_to_eq.bits[1] = 0b11111111111111111111111111111111;
  result_to_eq.bits[2] = 0b11111111111111111111111111111111;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_add(dst1, dst2, &result), 1);  // функция возвращает 0;

  // 6
  dst2.bits[3] = 0;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_add(dst1, dst2, &result),
      1);  // макс + 6 = функция возвращает 1 (число слишком велико);

  //минус макс число
  dst1.bits[3] = mask_minus;
  // -6
  dst2.bits[3] = mask_minus;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата для сравнения
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_add(dst1, dst2, &result),
      2);  // - макс + (-6) = функция возвращает 2 (число слишком мало);
}
END_TEST

START_TEST(test_s21_sub) {
  s21_decimal dst1, dst2, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del = 6 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // -----------------Операции с целыми числами-----------------

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // 3
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus;

  // 0
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], result_to_eq.bits[0]);  // 1-2 = 1;

  // -1
  dst1.bits[3] = mask_minus;
  // -2
  dst2.bits[3] = mask_minus;
  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)-(-2) = 1;

  // 2
  dst2.bits[3] = 0;
  // -3
  result_to_eq.bits[0] = 3;
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)-(+2) = -3;

  // 1
  dst1.bits[3] = 0;
  // -2
  dst2.bits[3] = mask_minus;
  // 3
  result_to_eq.bits[0] = 3;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1-(-2) = -1;

  //----------------Операции с целым числом и дробным-----------------

  // 16
  dst1.bits[0] = 16;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 21.6
  dst2.bits[0] = 216;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  // -5.6
  result_to_eq.bits[0] = 56;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 16 - 21,6 = -5,6;

  // -21,6
  dst2.bits[3] = mask_minus | mask_del2;

  // 37.6
  result_to_eq.bits[0] = 376;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 376);
  ck_assert_int_eq(result.bits[3], mask_del2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 16 - (-21,6) = 37,6;

  // -16
  dst1.bits[3] = mask_minus;
  // 5.6
  result_to_eq.bits[0] = 56;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-16) - (-21,6) = 5,6;

  // 21.6
  dst2.bits[3] = mask_del2;
  // -37.6
  result_to_eq.bits[0] = 376;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-16) - (21,6) = -37,6;

  // ----------------Операции с двумя дробными числами-----------------

  // 12,345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  result_to_eq.bits[0] = 10645678;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  // 14,045678
  result_to_eq.bits[3] = mask_del;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 - 1,7 = 10,645678;

  // - 1,7
  dst2.bits[3] = mask_minus | mask_del2;
  result_to_eq.bits[0] = 14045678;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 - (-1,7) = 14,045678;

  // -12,345678
  dst1.bits[3] = mask_minus | mask_del;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;
  // -14,045678
  result_to_eq.bits[3] = mask_minus | mask_del;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-12,345678) - 1,7 = -14,045678;

  // ----------------Операции с нулем-----------------

  // 0
  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  // -1,7
  result_to_eq.bits[0] = 17;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0 - 1,7 = -1,7;

  // 0,0
  dst1.bits[3] = mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0,0 - 1,7 = -1,7;

  // 0,0
  dst1.bits[3] = mask_del2;
  // 1.7
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_sub(dst2, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1,7 - 0.0 = 1,7;

  // -0
  dst1.bits[3] = 0 | mask_minus;
  //-1.7
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-0) - 1,7 = -1,7;

  // -0,0
  dst1.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // -0,0 - 1,7 = -1,7;

  // ----------------Операции с переполнением-----------------

  // 79,228,162,514,264,337,593,543,950,335
  dst1.bits[0] = 0b11111111111111111111111111111111;
  dst1.bits[1] = 0b11111111111111111111111111111111;
  dst1.bits[2] = 0b11111111111111111111111111111111;
  dst1.bits[3] = 0;

  // -0.6
  dst2.bits[0] = 6;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_minus | mask_del2;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // 79,228,162,514,264,337,593,543,950,335
  result_to_eq.bits[0] = 0b11111111111111111111111111111111;
  result_to_eq.bits[1] = 0b11111111111111111111111111111111;
  result_to_eq.bits[2] = 0b11111111111111111111111111111111;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 1);  // функция возвращает 0;
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1); // макс - (-0,6) =
  // банковское округление;

  // 0.6
  dst2.bits[3] = mask_del2;
  // 79,228,162,514,264,337,593,543,950,334
  result_to_eq.bits[0] = 0b11111111111111111111111111111110;
  result_to_eq.bits[1] = 0b11111111111111111111111111111111;
  result_to_eq.bits[2] = 0b11111111111111111111111111111111;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // макс - 0,6 = банковское округление;

  // 6
  dst2.bits[3] = 0;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  result_to_eq.bits[0] = 0b11111111111111111111111111111001;
  result_to_eq.bits[1] = 0b11111111111111111111111111111111;
  result_to_eq.bits[2] = 0b11111111111111111111111111111111;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_sub(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // макс - 6 = 79,228,162,514,264,337,593,543,950,329;

  //минус макс число
  dst1.bits[3] = mask_minus;
  // 6
  dst2.bits[3] = 0;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата для сравнения
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_sub(dst1, dst2, &result),
      2);  // - макс - 6 = функция возвращает 2 (число слишком мало);
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq),
  //                  1);  // result не изменился;

  //макс число
  dst1.bits[3] = 0;
  // -6
  dst2.bits[3] = mask_minus;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата для сравнения
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_sub(dst1, dst2, &result),
      1);  // макс - (-6) = функция возвращает 1 (число слишком велико);
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq),
  //                  1);  // result не изменился;
}
END_TEST

START_TEST(test_s21_mul) {
  s21_decimal dst1, dst2, result, result_to_eq;

  unsigned mask_minus = 1 << SIGN;
  int mask_del = 6 << START_SCALE;
  int mask_del_7 = 7 << START_SCALE;
  int mask_del_12 = 12 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // -----------------Операции с целыми числами-----------------

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // 2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  // 0
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1*2 = 2;

  // -1
  dst1.bits[3] = mask_minus;
  // -2
  dst2.bits[3] = mask_minus;
  // 1
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)*(-2) = 2;

  // 2
  dst2.bits[3] = 0;
  // -2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-1)*(+2) = -2;

  // 1
  dst1.bits[3] = 0;
  // -2
  dst2.bits[3] = mask_minus;
  // -2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1*(-2) = -1;

  //----------------Операции с целым числом и дробным-----------------

  // 16
  dst1.bits[0] = 16;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 21.6
  dst2.bits[0] = 216;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  // 345.6
  result_to_eq.bits[0] = 3456;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 16 * 21,6 = -5,6;

  // -21,6
  dst2.bits[3] = mask_minus | mask_del2;

  // -345.6
  result_to_eq.bits[0] = 3456;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 3456);
  ck_assert_int_eq(result.bits[3], mask_minus | mask_del2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 16 -*(-21,6) = 37,6;

  // -16
  dst1.bits[3] = mask_minus;
  // 345.6
  result_to_eq.bits[0] = 3456;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-16) * (-21,6) = 345,6;

  // 21.6
  dst2.bits[3] = mask_del2;
  // -345.6
  result_to_eq.bits[0] = 3456;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-16) * (21,6) = -345,6;

  // ----------------Операции с двумя дробными числами-----------------

  // 12,345678
  dst1.bits[0] = 12345678;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  // 20.9876526
  result_to_eq.bits[0] = 209876526;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del_7;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 * 1,7 = 20.9876526;

  // - 1,7
  dst2.bits[3] = mask_minus | mask_del2;
  // -20.9876526
  result_to_eq.bits[3] = mask_minus | mask_del_7;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,345678 * (-1,7) = -20.9876526;

  // -12,345678
  dst1.bits[3] = mask_minus | mask_del;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-12,345678) * 1,7 = -14,045678;

  // 152.415765279684
  result_to_eq.bits[0] = 0b00001111100011000011001111000100;
  result_to_eq.bits[1] = 35487;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del_12;
  ck_assert_int_eq(s21_mul(dst1, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // -12,345678 * (-12,345678) = 152.415765279684;

  // ----------------Операции с нулем-----------------

  // 0
  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 17
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  // 1,7
  dst2.bits[3] = 0 | mask_del2;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0 * 1,7 = 0;

  // 0,0
  dst1.bits[3] = mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0,0 * 1,7 = 0;

  ck_assert_int_eq(s21_mul(dst2, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1,7 * 0.0 = 0;

  // -0
  dst1.bits[3] = mask_minus;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-0) * 1,7 = 0;

  // -0,0
  dst1.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // -0,0 * 1,7 = 0;

  // ----------------Операции с переполнением-----------------

  // 79,228,162,514,264,337,593,543,950,335
  dst1.bits[0] = 0b11111111111111111111111111111111;
  dst1.bits[1] = 0b11111111111111111111111111111111;
  dst1.bits[2] = 0b11111111111111111111111111111111;
  dst1.bits[3] = 0;

  // 2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_mul(dst1, dst2, &result),
      1);  //  макс * 2 = функция возвращает 1 (число слишком велико);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // число не изменилось;

  // -2
  dst2.bits[3] = mask_minus;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_mul(dst1, dst2, &result),
      2);  // макс * (-2) = функция возвращает 2 (число слишком мало);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // число не изменилось;

  // 0.2
  dst2.bits[3] = mask_del2;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 0);  // функция возвращает 0;

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = UINT32_MAX;
  dst2.bits[1] = UINT32_MAX;
  dst2.bits[2] = UINT32_MAX;
  dst2.bits[3] = 0;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 1);  // функция возвращает 0;

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 1 << SIGN;

  dst2.bits[0] = UINT32_MAX;
  dst2.bits[1] = UINT32_MAX;
  dst2.bits[2] = UINT32_MAX;
  dst2.bits[3] = 0;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 2);  // функция возвращает 0;

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = 234;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 1 << START_SCALE;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 1);  // функция возвращает 0

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 28 << START_SCALE;

  dst2.bits[0] = UINT32_MAX;
  dst2.bits[1] = UINT32_MAX;
  dst2.bits[2] = UINT32_MAX;
  dst2.bits[3] = 28 << START_SCALE;

  ck_assert_int_eq(s21_mul(dst1, dst2, &result), 1);  // функция возвращает 0
}
END_TEST

START_TEST(test_s21_div) {
  s21_decimal dst1, dst2, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del = 6 << START_SCALE;
  int mask_del_5 = 5 << START_SCALE;
  int mask_del_12 = 12 << START_SCALE;
  int mask_del2 = 1 << START_SCALE;

  // -----------------Операции с целыми числами-----------------

  // 6
  dst1.bits[0] = 6;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 3
  dst2.bits[0] = 3;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // 2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  // 0
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], result_to_eq.bits[0]);  // 6/3 = 2;

  // 0,5
  result_to_eq.bits[0] = 50;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 2 < START_SCALE;
  ck_assert_int_eq(s21_div(dst2, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], result_to_eq.bits[0]);  // 3/6 = 0.5;

  // -6
  dst1.bits[3] = mask_minus;
  // -3
  dst2.bits[3] = mask_minus;
  // 2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-6)/(-3) = 2;

  // 3
  dst2.bits[3] = 0;
  // -2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // (-6)*(+3) = -2;

  // 6
  dst1.bits[3] = 0;
  // -3
  dst2.bits[3] = mask_minus;
  // -2
  result_to_eq.bits[0] = 2;
  result_to_eq.bits[3] = mask_minus;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1*(-2) = -1;

  //----------------Операции с целым числом и дробным-----------------

  // 1.6
  dst1.bits[0] = 16;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del2;

  // 4
  dst2.bits[0] = 4;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  // 0.4
  result_to_eq.bits[0] = 4;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1.6 / 4 = 0,6;

  // -4
  dst2.bits[3] = mask_minus;

  // -2.5
  result_to_eq.bits[0] = 25;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus | mask_del2;
  ck_assert_int_eq(s21_div(dst2, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // -4 / (1,6) = -2,5;

  // -1.6
  dst1.bits[3] = mask_del2 | mask_minus;
  // 0.4
  result_to_eq.bits[0] = 4;
  result_to_eq.bits[3] = mask_del2;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-1.6) / (-4) = 0.4;

  // -0.000016
  dst1.bits[3] = mask_del | mask_minus;
  result_to_eq.bits[0] = 4;
  result_to_eq.bits[3] = mask_del;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-0.000016) / (-4) = 0.4;

  // ----------------Операции с двумя дробными числами-----------------

  // 12,000002
  dst1.bits[0] = 12000002;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del;

  // 0.2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  // 60.00001
  result_to_eq.bits[0] = 6000001;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del_5;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,000002 / 0.2 = 60.00001;

  // -0,2
  dst2.bits[3] = mask_minus | mask_del2;
  // -60.00001
  result_to_eq.bits[3] = mask_minus | mask_del_5;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 12,000002 / (-0,2) = -60.00001;

  // -12,000002
  dst1.bits[3] = mask_minus | mask_del;
  // 0,2
  dst2.bits[3] = mask_del2;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // (-12,000002) / (0,2) = -60.00001;

  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_div(dst1, dst1, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // -12,000002 / (-12,000002) = 1;

  // 152.415765279684
  dst1.bits[0] = 0b00001111100011000011001111000100;
  dst1.bits[1] = 35487;
  dst1.bits[2] = 0;
  dst1.bits[3] = mask_del_12;

  // 12,345678
  dst2.bits[0] = 12345678;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del;

  // 12,345678
  result_to_eq.bits[0] = 12345678;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_del;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // 152.415765279684 / 12,345678 = 12,345678;

  // ----------------Операции с нулем-----------------

  // 0
  dst1.bits[0] = 0;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // 1.7
  dst2.bits[0] = 17;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0 / 1,7 = 0;

  // 0,0
  dst1.bits[3] = mask_del2;
  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);  // функция возвращает 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 0,0 * 1,7 = 0;

  ck_assert_int_eq(s21_div(dst2, dst1, &result),
                   3);  // функция возвращает 3 (деление на 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);  // 1,7 * 0.0 = 0;

  // // ----------------Операции с переполнением-----------------

  // 79,228,162,514,264,337,593,543,950,335
  dst1.bits[0] = 0b11111111111111111111111111111111;
  dst1.bits[1] = 0b11111111111111111111111111111111;
  dst1.bits[2] = 0b11111111111111111111111111111111;
  dst1.bits[3] = 0;

  // 2
  dst2.bits[0] = 2;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = mask_del2;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_div(dst1, dst2, &result),
      1);  //  макс / 0.2 = функция возвращает 1 (число слишком велико);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // число не изменилось;

  // -2
  dst2.bits[3] = mask_minus | mask_del2;
  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // обнуление результата
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;
  ck_assert_int_eq(
      s21_div(dst1, dst2, &result),
      2);  // макс * (-2) = функция возвращает 2 (число слишком мало)
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // число не изменилось;

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = 1;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 0;

  result_to_eq.bits[0] = UINT32_MAX;
  result_to_eq.bits[1] = UINT32_MAX;
  result_to_eq.bits[2] = UINT32_MAX;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = 1;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 1 << START_SCALE;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result), 1);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX >> 1;
  dst1.bits[3] = 0;

  dst2.bits[0] = 5;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 1 << START_SCALE;

  result_to_eq.bits[0] = UINT32_MAX - 1;
  result_to_eq.bits[1] = UINT32_MAX;
  result_to_eq.bits[2] = UINT32_MAX;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result),
                   0);  // (DecimalMAX / 2) / 0.5 == DecimalMAX
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = UINT32_MAX;
  dst2.bits[1] = UINT32_MAX;
  dst2.bits[2] = UINT32_MAX;
  dst2.bits[3] = 0;

  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  // ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = UINT32_MAX;
  dst2.bits[1] = UINT32_MAX;
  dst2.bits[2] = UINT32_MAX;
  dst2.bits[3] = 28 << START_SCALE;

  // 10 ^ 28
  result_to_eq.bits[0] = 0b00010000000000000000000000000000;
  result_to_eq.bits[1] = 0b00111110001001010000001001100001;
  result_to_eq.bits[2] = 0b100000010011111100111001011110;
  result_to_eq.bits[3] = 0;

  // ck_assert_int_eq(s21_div(dst1, dst2, &result), 0);
  // ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = 1;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 28 << START_SCALE;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result), 1);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);

  dst1.bits[0] = UINT32_MAX;
  dst1.bits[1] = UINT32_MAX;
  dst1.bits[2] = UINT32_MAX;
  dst1.bits[3] = 0;

  dst2.bits[0] = 1;
  dst2.bits[1] = 0;
  dst2.bits[2] = 0;
  dst2.bits[3] = 28 << START_SCALE | 1 << SIGN;

  result_to_eq.bits[0] = 0;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  ck_assert_int_eq(s21_div(dst1, dst2, &result), 2);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq), 1);
}
END_TEST

START_TEST(test_s21_floor) {
  s21_decimal dst1, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del3 = 3 << START_SCALE;

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем 1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -1
  dst1.bits[3] = mask_minus;

  // -1
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем -1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 0
  dst1.bits[0] = 0;
  dst1.bits[3] = 0;

  // 0
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.345
  dst1.bits[0] = 12345;
  dst1.bits[3] = mask_del3;

  // 12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем 12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.345
  dst1.bits[3] = mask_del3 | mask_minus;

  // -13
  result_to_eq.bits[0] = 13;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем -12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3;

  // 12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем 12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3 | mask_minus;

  // -12
  result_to_eq.bits[0] = 13;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем -12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -19.745
  dst1.bits[0] = 19745;
  dst1.bits[3] = mask_del3 | mask_minus;

  // -12
  result_to_eq.bits[0] = 20;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_floor(dst1, &result), 0);  // Округляем -12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;
}
END_TEST

START_TEST(test_s21_round) {
  s21_decimal dst1, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del3 = 3 << START_SCALE;

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -1
  dst1.bits[3] = mask_minus;

  // -1
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем -1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 0
  dst1.bits[0] = 0;
  dst1.bits[3] = 0;

  // 0
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.345
  dst1.bits[0] = 12345;
  dst1.bits[3] = mask_del3;

  // 12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.345
  dst1.bits[3] = mask_del3 | mask_minus;

  // -12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем -12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3;

  // 13
  result_to_eq.bits[0] = 13;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3 | mask_minus;

  // -13
  result_to_eq.bits[0] = 13;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем -12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.545
  dst1.bits[0] = 12545;
  dst1.bits[3] = mask_del3;

  // 13
  result_to_eq.bits[0] = 13;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 12.545;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 19.545
  dst1.bits[0] = 19545;
  dst1.bits[3] = mask_del3;

  // 20
  result_to_eq.bits[0] = 20;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_round(dst1, &result), 0);  // Округляем 19.545;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;
}
END_TEST

START_TEST(test_s21_truncate) {
  s21_decimal dst1, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del3 = 3 << START_SCALE;

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // 1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем 1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -1
  dst1.bits[3] = mask_minus;

  // -1
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем -1;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 0
  dst1.bits[0] = 0;
  dst1.bits[3] = 0;

  // 0
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем 0;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.345
  dst1.bits[0] = 12345;
  dst1.bits[3] = mask_del3;

  // 12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем 12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.345
  dst1.bits[3] = mask_del3 | mask_minus;

  // -12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем -12.345;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3;

  // 12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем 12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.745
  dst1.bits[0] = 12745;
  dst1.bits[3] = mask_del3 | mask_minus;

  // -12
  result_to_eq.bits[0] = 12;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_truncate(dst1, &result), 0);  // Округляем -12.745;
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;
}
END_TEST

START_TEST(test_s21_negate) {
  s21_decimal dst1, result, result_to_eq;

  int mask_minus = 1 << SIGN;
  int mask_del3 = 3 << START_SCALE;

  // 1
  dst1.bits[0] = 1;
  dst1.bits[1] = 0;
  dst1.bits[2] = 0;
  dst1.bits[3] = 0;

  // обнуление результата
  result.bits[0] = 0;
  result.bits[1] = 0;
  result.bits[2] = 0;
  result.bits[3] = 0;

  // -1
  result_to_eq.bits[0] = 1;
  result_to_eq.bits[1] = 0;
  result_to_eq.bits[2] = 0;
  result_to_eq.bits[3] = mask_minus;

  ck_assert_int_eq(s21_negate(dst1, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -1
  dst1.bits[3] = mask_minus;

  // 1
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_negate(dst1, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // 0
  dst1.bits[0] = 0;
  dst1.bits[3] = 0;

  // 0
  result_to_eq.bits[0] = 0;
  result_to_eq.bits[3] = 0;

  ck_assert_int_eq(s21_negate(dst1, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   0);  // функция возвращает 0;

  // 12.345
  dst1.bits[0] = 12345;
  dst1.bits[3] = mask_del3;

  // -12.345
  result_to_eq.bits[0] = 12345;
  result_to_eq.bits[3] = mask_minus | mask_del3;

  ck_assert_int_eq(s21_negate(dst1, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;

  // -12.345
  dst1.bits[3] = mask_del3 | mask_minus;

  // 12.345
  result_to_eq.bits[0] = 12345;
  result_to_eq.bits[3] = mask_del3;

  ck_assert_int_eq(s21_negate(dst1, &result), 0);
  ck_assert_int_eq(s21_is_equal(result, result_to_eq),
                   1);  // функция возвращает 0;
}
END_TEST

Suite *example_suite_create(void) {
  Suite *suite = suite_create("Example");
  TCase *tcase_equals = tcase_create("Equals");

  tcase_set_timeout(tcase_equals, 100);

  tcase_add_test(tcase_equals, tests_additional);
  // тесты на сравнение
  tcase_add_test(tcase_equals, test_is_equal);
  tcase_add_test(tcase_equals, test_is_not_equal);
  tcase_add_test(tcase_equals, test_s21_is_less);
  tcase_add_test(tcase_equals, test_s21_less_or_equal);
  tcase_add_test(tcase_equals, test_s21_is_greater);
  tcase_add_test(tcase_equals, test_s21_is_greater_or_equal);
  // тесты на преобразование
  tcase_add_test(tcase_equals, test_s21_from_int_to_decimal);
  tcase_add_test(tcase_equals, test_s21_from_float_to_decimal);
  tcase_add_test(tcase_equals, test_s21_from_decimal_to_int);
  tcase_add_test(tcase_equals, test_s21_from_decimal_to_float);
  // тесты на арифметику
  tcase_add_test(tcase_equals, test_s21_add);
  tcase_add_test(tcase_equals, test_s21_sub);
  tcase_add_test(tcase_equals, test_s21_mul);
  tcase_add_test(tcase_equals, test_s21_div);
  // тесты на другие функции
  tcase_add_test(tcase_equals, test_s21_floor);
  tcase_add_test(tcase_equals, test_s21_round);
  tcase_add_test(tcase_equals, test_s21_truncate);
  tcase_add_test(tcase_equals, test_s21_negate);

  suite_add_tcase(suite, tcase_equals);

  return suite;
}

int main(void) {
  Suite *suite = example_suite_create();
  SRunner *suite_runner = srunner_create(suite);

  srunner_run_all(suite_runner, CK_NORMAL);
  int failed_count = srunner_ntests_failed(suite_runner);
  srunner_free(suite_runner);

  if (failed_count != 0) {
    return 1;
  }

  return 0;
}
