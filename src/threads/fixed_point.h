/* 17.14 Fixed-Point Number implementation

format:
1 sign bit / 17 integer bits / 14 point bits

*/

#include <stdint.h>
#include "threads/thread.h"

// floating point mecro
#define FP (1 << 14)

// fp operation functions
fp_t fp_n_to_fp(int n);
int fp_x_to_int_round_zero(fp_t x); //not used
int fp_x_to_int_round_nearest(fp_t x);
fp_t fp_add(fp_t x, fp_t y);
fp_t fp_subtract_y_from_x(fp_t y, fp_t x);  //not used
fp_t fp_add_x_and_n(fp_t x, int n);
fp_t fp_subtract_n_from_x(int n, fp_t x);
fp_t fp_multiply_x_by_y(fp_t x, fp_t y);  //x should be int64_t
fp_t fp_multiply_x_by_n(fp_t x, int n);
fp_t fp_divide_x_by_y(fp_t x, fp_t y);    //x should be int64_t
fp_t fp_divide_x_by_n(fp_t x, int n);

fp_t fp_n_to_fp(int n){
    return n * FP;
}

int fp_x_to_int_round_zero(fp_t x){
    return x / FP;
}

int fp_x_to_int_round_nearest(fp_t x){
    if (x >= 0) {
        return (x + FP / 2) / FP;
    }
    else {
        return (x - FP / 2) / FP;
    }
}

fp_t fp_add(fp_t x, fp_t y){
    return x + y;
}

fp_t fp_subtract_y_from_x(fp_t y, fp_t x){
    return x - y;
}

fp_t fp_add_x_and_n(fp_t x, int n){
    return x + n * FP;
}

fp_t fp_subtract_n_from_x(int n, fp_t x){
    return x - n * FP;
}

fp_t fp_multiply_x_by_y(fp_t x, fp_t y){
    return ((int64_t) x) * y / FP;
}   //x should be int64_t

fp_t fp_multiply_x_by_n(fp_t x, int n){
    return x * n;
}

fp_t fp_divide_x_by_y(fp_t x, fp_t y){
    return ((int64_t) x) * FP / y;
}   //x should be int64_t

fp_t fp_divide_x_by_n(fp_t x, int n){
    return x / n;
}