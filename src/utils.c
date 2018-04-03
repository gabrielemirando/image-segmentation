#include <string.h>

#include "utils.h"

char *file_extension(char *file)
{
    char *ext;

    ext = strrchr(file, '.');
    if (!ext) {
        return "";
    }

    return ext + 1;
}

int strings_equal(char *str_one, char *str_two)
{
    return strcmp(str_one, str_two) == 0;
}
