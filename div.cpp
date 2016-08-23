#include <iostream>
#include <arm_neon.h>

#if !defined(__aarch64__)
#error "This code requires ARMv8"
#endif

int main()
{
	return 0;
}
