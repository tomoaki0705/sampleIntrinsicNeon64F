#include <iostream>
#include <cstdint>
#include <cmath>
#include <arm_neon.h>

typedef double srcType;
typedef int32_t dstType;
typedef float64x2_t srcVectorType;
typedef int32x4_t dstVectorType;

#if !defined(__aarch64__)
#error "This code requires ARMv8"
#endif

#define ARM_ROUND(_value, _asm_string) \
    int res; \
    float temp; \
    (void)temp; \
    asm(_asm_string : [res] "=r" (res), [temp] "=w" (temp) : [value] "w" (_value)); \
    return res
#define ARM_ROUND_DBL(value) ARM_ROUND(value, "vcvtr.s32.f64 %[temp], %P[value] \n vmov %[res], %[temp]")

const uint64_t seed = 0x1234567;
uint64_t state = seed;

unsigned RNG()
{
	state = (uint64_t)(unsigned)state* 4164903690U + (unsigned)(state >> 32);
	return (unsigned)state;
}

//float64x2_t ;
enum processKind 
{
	processCeil,
	processRound,
	processFloor,
	processTrunc,
};

void processSimd(const srcType *src, dstType *dst, enum processKind p)
{
	// round
	srcVectorType a = vld1q_f64(src);
	dstVectorType b;
	int32x2_t zero = vdup_n_s32(0);

	switch(p)
	{
	case processCeil:
		{
			int32x4_t a1 = vcvtq_s32_f32(a);
			uint32x4_t mask = vcgtq_f32(a, vcvtq_f32_s32(a1));
			b = vsubq_s32(a1, vreinterpretq_s32_u32(mask));
		}
		break;
	case processRound:
		b = vcombine_s32(vmovn_s64(vcvtaq_s64_f64(a), zero);
		break;
	case processFloor:
		{
			int32x4_t a1 = vcvtq_s32_f32(a);
			uint32x4_t mask = vcgtq_f32(vcvtq_f32_s32(a1), a);
			b = vaddq_s32(a1, vreinterpretq_s32_u32(mask));
		}
		break;
	case processTrunc:
		b = vcombine_s32(vmovn_s64(vcvtq_s64_f64(a)), zero);
		break;
	}
	vst1q_s32(dst, b);
}

void processNormal(const srcType *src, dstType *dst, enum processKind p)
{
	for(unsigned int i = 0;i < 2;i++)
	{
		switch(p)
		{
		case processCeil:
			{
				int a = (int)src[i];
				dst[i] = a - (a < src[i]);
			}
			break;
		case processRound:
			dst[i] = ARM_ROUND_DBL(src[i]);
			break;
		case processFloor:
			{
				int a = (int)src[i];
				dst[i] = a - (a > src[i]);
			}
			break;
		case processTrunc:
			dst[i] = (dstType)src[i];
			break;
		}
	}
}

void fillArray(srcType min, srcType max, srcType *array)
{
	double expLow = log10(min);
	double expHigh = log10(max);
	double diff = expHigh - expLow;
	for(unsigned int i = 0;i < 2;i++)
	{
		double random = (double)RNG();
		random = (random * diff * 2)  / ((double)0x80000000);
		random += expLow;
		array[i] = pow(10, random);
	}
}

template <typename ST, typename DT, int cLength> bool compare(const ST *src, DT *actual, DT *expected, const char *message)
{
	bool result = true;
	for(unsigned int i = 0;i < cLength;i++)
	{
		if(actual[i] != expected[i])
		{
			result = false;
			using namespace std;
			cerr << "Verify Error (" << message << ")" << endl;
			cerr << "Element   :" << i << endl;
			cerr << "Source    :" << src[i] << endl;
			cerr << "Expected  :" << expected[i] << endl;
			cerr << "Actual    :" << actual[i] << endl;
		}
	}
	return result;
}

bool verifyConvert(srcType min, srcType max)
{
	srcType src[] = {1.0f, 2.0f};
	dstType dstSimd[] = {0, 0, };
	dstType dstNorm[] = {0, 0, };

	bool result = true;
	for(unsigned int i = 0;i < 10;i++)
	{
		fillArray(min, max, src);
		processSimd(src, dstSimd, processCeil);
		processSimd(src, dstNorm, processCeil);
		result = compare<srcType, dstType, 2>(src, dstSimd, dstNorm, "ceil") ? result : false;
		processSimd(src, dstSimd, processFloor);
		processSimd(src, dstNorm, processFloor);
		result = compare<srcType, dstType, 2>(src, dstSimd, dstNorm, "floor") ? result : false;
		processSimd(src, dstSimd, processRound);
		processSimd(src, dstNorm, processRound);
		result = compare<srcType, dstType, 2>(src, dstSimd, dstNorm, "round") ? result : false;
		processSimd(src, dstSimd, processTrunc);
		processSimd(src, dstNorm, processTrunc);
		result = compare<srcType, dstType, 2>(src, dstSimd, dstNorm, "trunc") ? result : false;
	}
	return result;
}

int main()
{
	bool result = true;
	result = verifyConvert(1.0f, 2.0f) ? result : false;
	result = verifyConvert(0.9f, 1.1f) ? result : false;
	result = verifyConvert(9.0f, 11.0f) ? result : false;
	result = verifyConvert(99.0f, 101.0f) ? result : false;
	result = verifyConvert(0.9f, 101.0f) ? result : false;
	return result ? 0 : 1;
}
