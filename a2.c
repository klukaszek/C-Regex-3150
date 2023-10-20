// CIS 3150: Assignment 2
// Author:   Kyle Lukaszek
// ID:       1113798
// Due:      October 20, 2023

// I realize I could have just used fgets twice to read the first line regex and then the second line text
// but I wanted to make sure in the event that the text was on multiple lines that it would still work.

#include "a2.h"

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments were passed
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    if (argv[1] == NULL)
    {
        perror("Error: filename is NULL\n");
        return 1;
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Parse the regex from the file
    char* regexp = parseRegex(file);
    if (regexp == NULL) {
        perror("Error: Failed to parse regex from file");
        fclose(file);
        return 1;
    }

    // Parse the text from the file
    char* text = parseText(file, strlen(regexp));
    if (text == NULL) {
        free(regexp);
        fclose(file);
        perror("Error: Failed to parse text from file");
        return 1;
    }

    // Close the file
    fclose(file);

    int *matches = malloc(strlen(text) * sizeof(int));
    if (matches == NULL) {
        free(regexp);
        free(text);
        perror("Error: Failed to allocate memory for matches");
        return 1;
    }

    // Call match to find all matches of the regex in the text, and store the indices in the matches array
    int num_matches = 0;
    match(regexp, text, matches, &num_matches);

    // Print the results
    if (num_matches == 0)
    {
        printf("no match.\n");
    }
    else
    {
        printf("match");

        for (int i=0; i<num_matches; i++)
        {
            printf(" %d", matches[i]);
        }
        printf("\n");
    }

    // Free all allocated memory
    free(regexp);
    free(text);
    free(matches);

    return 0;
}

/* match: search for regexp anywhere in text */
void match(char *regexp, char *text, int *matches, int *num_matches) {

    // If the regex is empty or NULL, return
    if (strcmp(regexp, "") == 0 || regexp == NULL)
    {
        return;
    }

    // If the matches or num_matches pointers are NULL, return
    if (matches == NULL || num_matches == NULL)
    {
        return;
    }

    // If the text is empty or NULL, return
    if (strcmp(text, "") == 0 || text == NULL)
        return;

    int skip_indices;
    int text_index = 0;

    // If the regex starts with '^', match the regex at the beginning of the text
    if (regexp[0] == '^')
    {
        skip_indices = 0;
        int result = matchhere(regexp + 1, text, &skip_indices);

        // If there was a valid substring match (matchhere returns 1), add the index to the matches array and skip any indices that were part of the matched substring
        if (skip_indices > 0 && result == 1)
        {
            matches[(*num_matches)++] = text_index;
            text += skip_indices;
            text_index += skip_indices;
        }
    }

    // Loop through the text until the end of the string is reached
    while (*text != '\0') 
    {
        skip_indices = 0;

        // Get success/failure of matching the regex at the current index in the text
        int result = matchhere(regexp, text, &skip_indices);

        // If there was a valid substring match (matchhere returns 1), add the index to the matches array and skip any indices that were part of the matched substring
        if (skip_indices > 0 && result == 1)
        {
            matches[(*num_matches)++] = text_index;
            text += skip_indices;
            text_index += skip_indices;
        } 
        else
        {
            text++;
            text_index++;
        }
    }

    // End of indices marker, not really used but I thought it would be nice to have
    matches[*num_matches] = -1; 
}

/* matchhere: search for regexp at beginning of text */
int matchhere(char *regexp, char *text, int *skip_indices) {

    // Check if we are at the end of the regular expression
    if (regexp[0] == '\0') 
        return 1;
    // Handle the case for the '\' character
    if (regexp[0] == '\\')
    {
        if (regexp[1] == *text) 
        {
            // If the next character in the regex matches the current character in the text, continue matching the remaining text and regex
            (*skip_indices)++;
            return matchhere(regexp + 2, text + 1, skip_indices);
        }
        
        return 0;
    }
    // Handle the case for a character set within square brackets '[]'
    if (regexp[0] == '[') 
    {
        // Handle the case for the character set within square brackets '[]'
        int found = matchset(regexp + 1, text, skip_indices);
        if (found) {
            while(*regexp != ']') {
                regexp++;
            }
            regexp++;
            return matchhere(regexp, text + 1, skip_indices); // Continue matching the remaining text and regex
        } else {
            return 0; // If the character set does not match, return 0
        }
    }
    // Check if the next character in the regular expression is '?'
    if (regexp[1] == '?')
    {
        // Check if the remaining regex starting from index 2 matches the text
        if (matchhere(regexp + 2, text, skip_indices))
        {
            // If the remaining regex matches, increment skip_indices and return true
            (*skip_indices)++;
            return 1;
        }

        // Check if the current character in the text matches the regex
        // Also, check if there is more text to process
        if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        {
            // Increment skip_indices and move to the next character in the text and regex
            (*skip_indices)++;
            return matchhere(regexp + 2, text + 1, skip_indices);
        }
    }
    // Check if the next character in the regular expression is '+'
    if (regexp[1] == '+')
    {
        return matchplus(regexp, text, skip_indices);
    }
    // Check if the next character in the regular expression is '*'
    if (regexp[1] == '*')
    {
        return matchstar(regexp[0], regexp + 2, text, skip_indices);
    }
    // Check if we are at the end of the regular expression and the text
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    // Check if the current character in the text matches the regex (wildcard included)
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
    {
        (*skip_indices)++;
        return matchhere(regexp + 1, text + 1, skip_indices);
    }

    // If none of the above cases are true, return 0, this means no match was found even if skip_indices > 0
    return 0;
}

/* matchstar: search for c*regexp at beginning of text */
int matchstar(char c, char *regexp, char *text, int *skip_indices) {
	char *t;

    // Increment skip_indices to account for 0 occurences of the character
    (*skip_indices)++;

    // Advance through the text as long as the character matches c or is a wildcard '.' and increment skip_indices
	for (t = text; *t != '\0' && (*t == c || c == '.'); t++)
    {
        (*skip_indices)++;
    }

	do {	/* * matches zero or more */
		if (matchhere(regexp, t, skip_indices))
        {
            // We decrement skip_indices to account for the 0 occurences of the character
            if (*skip_indices > 1)
            {
                (*skip_indices)--;
            }
            return 1;
        }
	} while (t-- > text);

    return 0;
}

/* matchplus: handle multiple of a character for '+'*/
int matchplus(char *regexp, char *text, int *skip_indices)
{

    int found = 0;
    // Check if the current character in the text matches the regex
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
    {
        (*skip_indices)++;
        found = 1;
        // Keep matching the same character in the text until it changes
        while (*(text + 1) == *text)
        {
            text++;
            (*skip_indices)++;
        }
        
        // Store the number of characters that were matched in the event that the rest of the match fails but the remaining regex is unimportant
        int temp_skip_indices = *skip_indices;

        // Continue matching the remaining text and regex, if the match fails, check if the remaining regex is unimportant
        if(matchhere(regexp + 2, text + 1, skip_indices) == 0)
        {
            int remaining_regex_unimportant = 1;
            char *cur_regex = regexp;

            // Go to the next character in the regex (go past the + character)
            regexp += 2;

            // Check if the remaining regex is important
            /* without this do-while loop, the regex "a+b+a" with the string "abcbefaa" returns "match 0 6" but it should return "no match" */
            do
            {
                // Regex is important if the remaining characters require an occurence (anything that is preceded by a +)
                int flag1 = 0;
                int flag2 = 0;

                // If the character in the text does not match the current character in the regex, we raise the first flag
                if (*regexp != *cur_regex && *regexp != '\0')
                {
                    flag1 = 1;
                    cur_regex = regexp;
                }

                // Go to next character in the regex
                regexp+=1;

                // Check if the next character in the regex is a character that requires an occurence (or is the end of the regex)
                if (isalpha(*regexp) || *regexp == '.' || *regexp == '[' || *regexp == '\0')
                {
                    flag2 = 1;
                }

                //If both flags are raised, the remaining regex is IMPORTANT AND MUST BE MATCHED
                if (flag1 && flag2)
                {
                    remaining_regex_unimportant = 0;
                }

            } while (*regexp++ != '\0' && remaining_regex_unimportant);
            
            // If the remaining regex is important, we set skip_indices to 0 and return 0
            if (remaining_regex_unimportant == 0)
            {
                found = 0;
                *skip_indices = 0;
            }
            // If the remaining regex is unimportant, we set skip_indices to the number of characters that were matched and return 1 (ex. a+a+)
            else
            {
                found = 1;
                *skip_indices = temp_skip_indices;
            }
        }
    }

    if (found)
        return 1;

    return 0;
}

/* matchset: handle character sets within square brackets '[]' */
int matchset(char *set, char *text, int *skip_indices) {
    int negated = 0;
    if (*set == '^') {
        negated = 1;
        set++; // Move past the '^' character
    }
    if (negated) {
        if (*set == *text) {
            return 0; // If the character is found and the set is negated, return 0
        }
    } else {
        while (*set != ']' && *set != '\0') {
            // Check if the next character in the set is a '-', and if it is not the last character in the set (']' or '\0')
            if (*set == '-' && *(set + 1) != ']' && *(set + 1) != '\0') 
            {
                // Check if the character falls within the range specified
                if (in_range(*text, *(set - 1), *(set + 1))) {
                    (*skip_indices)++;
                    return 1;
                }
                // Move past the '-' character
                set++; 

            } 
            else if (*set == *text) 
            {
                (*skip_indices)++;
                return 1; // If the character matches the set, return 1
            }
            set++; // Move to the next character in the set
        }
    }
    return 0; // If no match is found, return 0
}

/* Function to check if a character matches a given range */
int in_range(char c, char start, char end) {
    return c >= start && c <= end;
}

/* Function to parse the regex from the first line of the file */
char *parseRegex(FILE *file)
{
    if (file == NULL)
    {
        perror("Error: file is NULL\n");
        return NULL;
    }

    // Read the regex from the first line of the file
    char regex[MAX_REGEX_LEN];
    if (fgets(regex, sizeof(regex), file) == NULL) {
        perror("Error: Failed to read regex from file");
        return NULL;
    }

    // Removing the trailing newline character from the regex and replacing it with a null terminator
    regex[strcspn(regex, "\n")] = '\0';

    // Allocate memory for the regex and copy it over
    char *regex_ptr = malloc(strlen(regex) + 1);
    strcpy(regex_ptr, regex);

    return regex_ptr;
}

/* Function to parse the file with the regex on the first line and text on subsequent lines */
char* parseText(FILE *file, int regex_len) 
{
    if (file == NULL)
    {
        perror("Error: file is NULL\n");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    // Reset the file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Seek past the regex (regex_len + 1 to account for the newline character)
    fseek(file, regex_len + 1, SEEK_SET);

    // Allocate memory for the text
    char *text = (char *)malloc((file_size + 1 - regex_len) * sizeof(char));
    if (text == NULL) {
        printf("Failed to allocate memory for the text.\n");
        return NULL;
    }

    // Read the text from the file
    size_t bytes_read = fread(text, sizeof(char), file_size - regex_len, file);
    text[bytes_read] = '\0'; // Null-terminate the string

    return text;
}