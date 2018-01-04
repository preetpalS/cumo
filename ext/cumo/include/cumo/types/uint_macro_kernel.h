#include "xint_macro_kernel.h"

#define m_abs(x)     (x)
#define m_sign(x)    (((x)==0) ? 0:1)

__device__ static inline dtype int_reciprocal(dtype x) {
    switch (x) {
    case 1:
        return 1;
    case 0:
        return 0; // as CUDA kernel 1/0 results in 0.
        //rb_raise(rb_eZeroDivError, "divided by 0");
    default:
        return 0;
    }
}
