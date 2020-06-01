### EquationSolver

This is a command-line program whose purpose is to balance chemical equations. The user provides unbalanced equation in the form of command-line arguments, and
the program prints this equation with coefficients adjusted. For now, EquationSolver is able to solve non-redox reactions.

To work correctly, the equation must be given according to the following rules:
1. Compound and signes between them must be provided as a space-separated arguments
2. Allowed signs are '+' and '='
3. To represent a compound you can use alphabetical characters, square brackets, curly braces, numbers and '+' and '-' sings.
4. Elements in compounds must be typed according to the chemical principle: the first letter in upper case, the second in lower case, e.g. NaCl.
5. Digit denoting the amount of an element should be placed directly after element or group of elements, e.g. Na2SO4, Ca{NO3}2.
6. For a group of elements, use curly braces instead of parentheses, e.g. Ca{NO3}2.
7. For complex compound use square brackets, e.g. K2[Cu{NO3}4].
8. No digits at the beginning of the compound allowed.
9. Charge of the compound place at the end as single '+' or '-' signs, eventually sign followed by a number, e.g. Na+, Fe+3.

Example input:
```
./final NaOH + H2SO4 = Na2SO4 + H2O
```
Example output:
```
2NaOH + 1H2SO4 = 1Na2SO4 + 2H2O
```

Here are some equations (properly formatted) for you to test EquationSolver.

CaCO3 = CaO + CO2  
Mg{OH}2 + HNO3 = Mg{NO3}2 + H2O  
CuCl2 + NH3 + H2O = Cu{OH}2 + NH4Cl  
Fe2{SO4}3 + NH4SCN = {NH4}3[Fe{SCN}6] + {NH4}2SO4  
