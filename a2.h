// CIS 3150: Assignment 2
// Author:   Kyle Lukaszek
// ID:       1113798
// Due:      October 20, 2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILENAME_LEN 256
#define MAX_REGEX_LEN 1000

// Original code provided by: https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

/* match: search for regexp anywhere in text */
void match(char *regexp, char *text, int *matches, int *num_matches);

/* matchhere: search for regexp in text */
int matchhere(char *regexp, char *text, int *skip_indices);

/* matchstar: search for c*regexp at beginning of text */
int matchstar(char c, char *regexp, char *text, int *skip_indices);

/* matchplus: handle multiple of a character for '+'*/
int matchplus(char *regexp, char *text, int *skip_indices);

/* matchset: handle character sets within square brackets '[]' */
int matchset(char *set, char *text, int *skip_indices);

/* in_range: check if a character is within a given range */
int in_range(char c, char start, char end);

/* parseRegex: parse the regex from the first line in the file */
char* parseRegex(FILE* file);

/* parseText: parse the text on subsequent lines in the file */
char* parseText(FILE* file, int regex_len);