#include <iostream>
#include <arm_neon.h>

#if !defined(__aarch64__)
#error "This code requires ARMv8"
#endif

//float64x2_t ;

template<int i> void processSimd(const double *src, double *dst);
template<> void processSimd<1>(const double *src, double *dst)
{
	// round
	float64x2_t a = vld1q_f64(src);

    static const int64x2_t v_sign = vdupq_n_s64(((uint64_t)1) << 63),
        v_05 = vreinterpretq_s64_f64(vdupq_n_f64(0.5f));
    int64x2_t v_addition = vorrq_s64(v_05, vandq_s64(v_sign, vreinterpretq_s64_f64(a)));
    vst1q_f64(dst, v_int64x2(vcvtq_s64_f64(vaddq_f64(a.val, vreinterpretq_f64_s64(v_addition)))));
}

inline v_int64x2 v_round(const v_float64x2& a)
{
    static const int64x2_t v_sign = vdupq_n_s64(((uint64_t)1) << 63),
        v_05 = vreinterpretq_s64_f64(vdupq_n_f64(0.5f));

    int64x2_t v_addition = vorrq_s64(v_05, vandq_s64(v_sign, vreinterpretq_s64_f64(a.val)));
    return v_int64x2(vcvtq_s64_f64(vaddq_f64(a.val, vreinterpretq_f64_s64(v_addition))));
}
}

void processSimd(const double *src, double *dst)
{
}

template<int i> void processNormal(const double *src, double *dst);

template<> void processNormal<1>(const double *src, double *dst)
{
	for(unsigned int i = 0;i < 2;i++)
	{
		int x = (int)value;
		dst[i] = (x + (x < value));
	}
}

int main()
{
	double src[] = {1
//inline v_float64x2 operator / (const v_float64x2& a, const v_float64x2& b)
//{
//    float64x2_t reciprocal = vrecpeq_f64(b.val);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    return v_float64x2(vmulq_f64(a.val, reciprocal));
//}
//inline v_float64x2& operator /= (v_float64x2& a, const v_float64x2& b)
//{
//    float64x2_t reciprocal = vrecpeq_f64(b.val);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    reciprocal = vmulq_f64(vrecpsq_f64(b.val, reciprocal), reciprocal);
//    a.val = vmulq_f64(a.val, reciprocal);
//    return a;
//}
//   Data<R> dataA, dataB;
//        dataB.reverse();
//        R a = dataA, b = dataB;
//
//        Data<R> resC = a / b;
//        for (int i = 0; i < R::nlanes; ++i)
//        {
//            EXPECT_COMPARE_EQ(dataA[i] / dataB[i], resC[i]);
//        }
//
//        return *this;
//    }
//
//
inline v_int64x2 v_round(const v_float64x2& a)
{
    static const int64x2_t v_sign = vdupq_n_s64(((uint64_t)1) << 63),
        v_05 = vreinterpretq_s64_f64(vdupq_n_f64(0.5f));

    int64x2_t v_addition = vorrq_s64(v_05, vandq_s64(v_sign, vreinterpretq_s64_f64(a.val)));
    return v_int64x2(vcvtq_s64_f64(vaddq_f64(a.val, vreinterpretq_f64_s64(v_addition))));
}

inline v_int64x2 v_floor(const v_float64x2& a)
{
    int64x2_t a1 = vcvtq_s64_f64(a.val);
    uint64x2_t mask = vcgtq_f64(vcvtq_f64_s64(a1), a.val);
    return v_int64x2(vaddq_s64(a1, vreinterpretq_s64_u64(mask)));
}

inline v_int64x2 v_ceil(const v_float64x2& a)
{
    int64x2_t a1 = vcvtq_s64_f64(a.val);
    uint64x2_t mask = vcgtq_f64(a.val, vcvtq_f64_s64(a1));
    return v_int64x2(vsubq_s64(a1, vreinterpretq_s64_u64(mask)));
}

inline v_int64x2 v_trunc(const v_float64x2& a)
{ return v_int64x2(vcvtq_s64_f64(a.val)); }

	return 0;
}
