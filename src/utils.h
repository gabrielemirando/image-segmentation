#ifndef UTILS_H
#define UTILS_H

/*
 * Function:  file_extension
 * --------------------
 * Retrieves the extension of a file.
 *
 *  file: string containing the path of the file
 *
 *  returns: a string containing the extension of the file
 */
char *file_extension(char *file);

/*
 * Function:  strings_equal
 * --------------------
 * Finds out if two strings are equal.
 *
 *  str_one: first string to compare
 *  str_two: second string to compare
 *
 *  returns: 1 if the strings are equal, 0 otherwise.
 */
int strings_equal(char *str_one, char *str_two);

#endif
