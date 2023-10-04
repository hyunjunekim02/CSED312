/* 17.14 Fixed-Point Number implementation

format:
1 sign bit / 17 integer bits / 14 point bits

*/

#include <stdint.h>

//floating point mecro
#define FP (1 << 14)

//fp operation functions
int fp_convert_n_to_fp(int n);
int fp_convert_x_to_integer_rounding_toward_zero(int x);
int fp_convert_x_to_integer_rounding_to_nearest(int x);
int fp_add(int x, int y);
int fp_subtract_y_from_x(int x, int y);
int fp_add_x_and_n(int x, int n);
int fp_subtract_n_from_x(int n, int x);
int fp_multiply_x_by_y(int x, int y);  //x should be int64_t
int fp_multiply_x_by_n(int x, int n);
int fp_divide_x_by_y(int x, int y);    //x should be int64_t
int fp_divide_x_by_n(int x, int n);

int fp_convert_n_to_fp(int n){
    return n * FP;
}

int fp_convert_x_to_integer_rounding_toward_zero(int x){
    return x / FP;
}

int fp_convert_x_to_integer_rounding_to_nearest(int x){
    if(x >= 0){
        return (x+FP/2)/FP;
    }
    else{
        return (x-FP/2)/FP;
    }
}

int fp_add(int x, int y){
    return x + y;
}

int fp_subtract_y_from_x(int x, int y){
    return x - y;
}

int fp_add_x_and_n(int x, int n){
    return x + n * FP;
}

int fp_subtract_n_from_x(int n, int x){
    return x - n * FP;
}

int fp_multiply_x_by_y(int x, int y){
    return ((int64_t)x) * y / FP;
}   //x should be int64_t

int fp_multiply_x_by_n(int x, int n){
    return x * n;
}

int fp_divide_x_by_y(int x, int y){
    return ((int64_t)x) * FP / y;
}   //x should be int64_t

int fp_divide_x_by_n(int x, int n){
    return x / n;
}