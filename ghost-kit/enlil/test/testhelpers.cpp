#include "testhelpers.h"
#include <cmath>

//Code for this function  taken from
// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
//  and modified for my purposes
bool doubleCompareEqual(double A, double B, double maxRelDiff)
{
    // Calculate the difference.
    double diff = fabs(A - B);
    A = fabs(A);
    B = fabs(B);
    // Find the largest
    double largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}
