# Linked Digraph NFA
A composable Directed Graph Structure suitable for use in Thompsons Construction,
Used to implement a simple (mostly) POSIX compliant extended grep.

File Descriptions

    compiler.hpp - contains routines for construction an NFA from a regular expression
    egrep.cpp - contains the i/o routines
    nfa.hpp - contains data structures for states & transitions
    parser.hpp - simple recursive descent parser for regular expressions
    subset_match.hpp - contains the procedure for pattern matching using thompsons 
                        linear time non back-tracking subset construction algorithm.

# egrep

## Supported Operators
Operators

    R|S  - Alternation
    (R) (S) -> (RS) - Concatenation
    R*  - Kleene closure matches zero or more times
    R+  - Kleene + matches one or more times
    R?  - matches zero or once
    .   - wildcard, matches any character
    (R) - regular expressions can use parentheses to override precedence of operators 

Character Classes, match literals of a class

    [aeiou] - match vowels
    [^aeiou] - char class negation, matches anything that ISN'T a vowel
    [0-9A-Fa-f] - match valid hexadecimal characters