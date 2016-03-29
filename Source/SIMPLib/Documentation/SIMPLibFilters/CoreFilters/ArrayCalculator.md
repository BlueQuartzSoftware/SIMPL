Array Calculator {#arraycalculator}
=============

## Group (Subgroup) ##
Core (Misc)

## Description ##
This **Filter** will do calculations on arrays using the equation that the user set into the filter.  The calculations follow common mathematical order-of-operations rules.  The results of these calculations will be stored in a new array of the user's choosing.

## Operators ##

###Binary Operators###

**\+**: Addition

**\-**: Subtraction

**\***: Multiplication

**/**: Division

**x^y**: 'x' to the 'y' power

###Unary Operators (1 Argument)###

**e^y**: e to the 'y' power

**sin**, **cos**, **tan**, **asin**, **acos**, **atan**: Trigonometric operators

**abs**: Absolute value

**ceil**: Ceiling

**Floor**: Floor

**Log10**: Logarithm, base 10

**ln**: Natural logarithm

**sqrt**: Square root

###Unary Operators (2 Arguments)###

**Log(b,x)**: Logarithm with base 'b' and value 'x'

**root(r,ind)**: Root with radicand 'r' and index 'ind'


## Examples ##

**Confidence Index + 3 / ln(4\*Fit)**

This equation multiplies every element in the **Fit** array by **4** and then finds the **natural logarithm** of those results.  Then, it divides those results from **3**, and then **adds** the results of that to every element in the **Confidence Index** array.

**Array1[0] - Array2[1]**

This equation subtracts **component 1** from every tuple in **Array2** from **component 0** from every tuple in **Array1**.  The resultant array will have the same number of tuples as **Array1** and **Array2**, and only 1 component.

**Array1 + Array2**

This equation adds every tuple in **Array2** with every tuple in **Array1**.  The resultant array will have the same number of tuples and components as **Array1** and **Array2**.

**6 / 3**

This equation divides **6** by **3** and stores the result in an array with tuple size 1 and component size 1.

**root(8, 3)**

This equation takes the cube root of **8** and stores the result in an array with tuple size 1 and component size 1.


**3 ^ Array1**

This equation raises **3** to the power of the value stored in every component of every tuple in **Array1**.  The resultant array has the same tuple and component size of **Array1**.

## Explicit Array Names ##

It is possible to force the calculator to treat a term in the equation as an array name.  This is done by putting double quotes around the desired term. For example:

1. If there is an array name called "+", the user can create a valid equation such as **3 + "+"**.  This equation adds three to every element in the "+" array.

2. For an array name called "452", the user can create a valid equation such as **6 \* "452"**.  This equation multiplies every element in the "452" array by 6.

3. For array names called "-" and "4" with 3 components each, the user can create a valid equation such as **"-"[2] + "4"[0]**.  This equation adds component 2 from every tuple in array "-" to component 0 from every tuple in array "4".

In many cases, attempting to write equations like these without putting the double quotes around the desired array name will cause an error due to ambiguity.