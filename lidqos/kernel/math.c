#ifndef _MATH_C_
#define _MATH_C_

#include <kernel/math.h>

double abs(double x)
{
	if (x < 0)
	{
		return -x;
	}
	return x;
}

double pown(double x, int n)
{
	double r = 1.0;
	for (int i = 0; i < n; ++i)
	{
		r *= x;
	}
	return r;
}

double pow(double x, double y)
{
	int sign = 0;
	if (y == 0)
	{
		return 1.0;
	}
	if (y < 0)
	{
		y = -y;
		sign = 1;
	}
	int n = (int) y;
	double r = pown(x, n);
	y -= n;
	double r2 = y * (ln(x) / ln(2.0));
	double r3 = 0.0;
	if (r2 >= 1.0)
	{
		int n2 = (int) r2;
		r3 = pown(2.0, n2);
		r2 -= n2;
		r3 *= (_pow2(r2) + 1);
	}
	else
	{
		r3 = _pow2(r2) + 1;
	}
	r *= r3;

	if (sign)
	{
		return 1 / r;
	}

	return r;
}

double tan(double x)
{
	return _sin(x) / _cos(x);
}

double cot(double x)
{
	return _cos(x) / _sin(x);
}

double sin(double x)
{
	return _sin(x);
}

double cos(double x)
{
	return _cos(x);
}

double atan(double x)
{
	return _atan(x, 1.0);
}

double asin(double x)
{
	return atan(x / sqrt(1 - x * x));
}

double acos(double x)
{
	return atan(sqrt(1 - x * x) / x);
}

double ln(double x)
{
	return log2(x) / log2(E);
}

double log(double base, double x)
{
	return log2(x) / log2(base);
}

double log10(double x)
{
	return log2(x) / log2(10.0);
}

double log2(double x)
{
	return _log2x(0.5, x) / 0.5;
}

double sqrt(double x)
{
	if (x < 0)
	{
		return -1.0;
	}
	if (x == 0)
	{
		return 0.0;
	}
	double x0, x1;
	x0 = x;
	x1 = x / 2.0;
	while (abs(x0 - x1) > 0.0000000000000001)
	{
		x0 = x1;
		x1 = (x0 + (x / x0)) / 2;
	}
	return x1;
}

#endif
