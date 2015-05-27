#ifndef _MATH_H_
#define _MATH_H_

#define PI 		(3.141592653589793238462643)
#define E 		(2.7182818284590452353602874)
#define SQRT2 	(1.414213562373095145474621858739)

extern double _pow2(double x);
extern double _sin(double x);
extern double _cos(double x);
extern double _atan(double a, double b);
extern double _log2x(double e, double x);

double abs(double x);
double pown(double x, int n);
double pow(double x, double y);
double tan(double x);
double sin(double x);
double cos(double x);
double atan(double x);
double asin(double x);
double acos(double x);
double ln(double x);
double log(double base, double x);
double log10(double x);
double log2(double x);
double sqrt(double x);

#endif
