# kitchenconv

Ever needed to convert 3/4 cup of butter into grams? Look no further. This is a simple and intuitive command-line tool to convert units when cooking.

Features:
* Uses plain language as input and output.
* Conversions to/from units of volume, weight or temperature
* Includes european and US units.
* Conversions from volume to weight (or weight to volume) is possible if you tell the program what substance you are trying to convert (e.g., butter or flour).
* Supports all kinds of numeric notations as input, including simple numbers (1, 10), fractions (3/4, 9/8), and scientific notation (1e3, 5e-2).
* Written in pure C++, no header, no dependencies: will compile and run fast everywhere.

Usage examples:
```bash
> ./kitchenconv 1 cup to ml
  1 cup is 236.6 ml

> ./kitchenconv 3/4 cup to ml
  3/4 cup is 177.45 ml

> ./kitchenconv 1 cup butter to g
  1 cup of butter is 226.805 g

> ./kitchenconv 0.4 kg to lb
  0.4 kg is 0.881834 lb
```

To compile, run the script ```make.sh``` (needs GCC), or use your favorite compiler.
