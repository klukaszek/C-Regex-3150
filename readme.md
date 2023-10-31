# CIS 3150 Assignment 2

Name: Kyle Lukaszek

ID: 1113798

Due: October 20th, 2023

## Final Grade: 100%

## Compilation

To compile the program, run the following command:

```bash
make
```

## Usage

To run the program, run the following command:

```bash
./a2 <input_file>
```

The program reads regex from the first line of the input file, and then reads the input strings from the rest of the file (second line). The program then outputs all matches in an input string.

For example, if the input file is:

```
a*b
ab.befaaab?
```    

The program output would be:

```
match 0 3 6
```

This is because the quoted sections "ab"."b"ef"aaab"? match the regex "a*b".

## Description

- The regex parser is implemented in a recursive manner that keeps track of how many characters are shifted so that we can accurately report the positions of each match.

- The parser can handle the following metacharacters:
    - . (dot) - matches any character
    - ? - matches 0 or 1 of the previous character
    - \* - matches 0 or more of the previous character
    - \+ - matches 1 or more of the previous character
    - [a\-zA-Z0-9] - matches any character or range of characters in the brackets (values in brackets are an example)
    - \ - escapes the next character
    - ^ - start of line
    - $ - end of line

## A few test examples that were used to test the program 
these are just examples, the program was tested with many more inputs

- . (dot) and $ metacharacter testing
```
.fa$
abcbefa

output:
match 4
```

- ^ and * metacharacter testing
```
^aa*bcb
abcb

output:
match 0
```

- \* metacharacter testing
```
b*
bgbbcccgcc

output:
match 0 1 2 4 5 6 7 8 9
``` 

- . (dot), *, [], and $ metacharacter testing
```
a.*[zZ]$
abcZ

output:
match 0
```

- ? metacharacter testing
```
ab?aa
abaabbbaaa

output:
match 0 7
```

- Range metacharacter testing
```
aa*b+[a-z]
abcbefaa

output:
match 0

-

aa*b+[a-z]
abCbefaa

output:
no match

-

aa*b+[a-zA-Z1-9]
ab1befaa

output:
match 0

```

- Multiple of the same character with metacharacter and no metacharacter
```
a+b+a
abcbefaa

output:
no match
```

- The program was tested with whitespace.

```
a b
na bm

output:
match 1
```
