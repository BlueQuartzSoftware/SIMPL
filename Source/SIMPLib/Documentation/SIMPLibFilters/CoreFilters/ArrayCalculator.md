Array Calculator {#arraycalculator}
=============

## Group (Subgroup) ##
Core (Misc)

## Description ##
This **Filter** will do calculations on arrays using the equation that the user set into the filter.  The calculations follow common mathematical order-of-operations rules.  The results of these calculations will be stored in a new array of the user's choosing.  Arrays can have many different mathematical operations performed on them, for example:

Confidence Index + 3 / ln(4*Fit)

This equation multiplies every element in the Fit array by 4 and then finds the natural logarithm of those results.  Then, it divides those results from 3, and then adds the results of that to every element in the Confidence Index array.

When an operation is being performed that involves two arrays, Foo and Bar, then the operation is performed element-wise on Foo[0] and Bar[0], Foo[1] and Bar[1], Foo[2] and Bar[2], Foo[3] and Bar[3], ...