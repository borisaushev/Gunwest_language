#include "fileReader.h"

#include <cstdio>

#include "common.h"

static long getFileSize(const char* filename) {
    struct stat st = {};
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }

    return -1;
}

int readFile(const char *file_path, wchar_t** text, int* bytes_read) {
    FILE* file = fopen(file_path, "rb");  // Бинарный режим
    if (!file) {
        PRINTERR("Could not open file %s\n", file_path);
        RETURN_ERR(DSL_FILE_NOT_FOUND, "Could not open file");
    }

    long file_size = getFileSize(file_path);
    DPRINTF("file size: %ld bytes\n", file_size);

    char *buffer = (char *)malloc((size_t) file_size + 1);
    if (!buffer) {
        fclose(file);
        RETURN_ERR(DSL_NULL_PTR, "Out of memory");
    }

    size_t read_bytes = fread(buffer, 1, (size_t) file_size, file);
    buffer[read_bytes] = '\0';
    *bytes_read = (int)read_bytes;
    fclose(file);

    DPRINTF("Read %d bytes\n", *bytes_read);


    *text = (wchar_t *)calloc(read_bytes + 1, sizeof(wchar_t));
    if (!*text) {
        free(buffer);
        RETURN_ERR(DSL_NULL_PTR, "Out of memory");
    }

    size_t converted = mbstowcs(*text, buffer, read_bytes);
    (*text)[converted] = L'\0';

    free(buffer);

    return 0;
}