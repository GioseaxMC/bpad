#ifdef BETZY_HANDLE_INCLUDES
    #include <stdlib.h>
    #include <stdio.h>
#endif

#define read_file(filename, buffer) do { \
    FILE* file = fopen(filename, "rb"); \
    if (!file) { \
        perror("Error opening file"); \
        buffer = NULL; \
        break; \
    } \
    fseek(file, 0, SEEK_END); \
    long fileSize = ftell(file); \
    fseek(file, 0, SEEK_SET); \
    buffer = (char*)malloc(fileSize + 1); \
    if (!buffer) { \
        perror("Memory allocation failed"); \
        fclose(file); \
        break; \
    } \
    if (fread(buffer, 1, fileSize, file) != fileSize) { \
        perror("Error reading file"); \
        free(buffer); \
        fclose(file); \
        buffer = NULL; \
        break; \
    } \
    buffer[fileSize] = '\0';  /* Null-terminate the buffer */ \
    fclose(file); \
} while (0)

static void scrash() {
    *(volatile int*)0 = 0;
}

static void crash(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
    scrash();
}

// Open file for writing (overwrite)
FILE* ofile_write(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        crash("Failed to open file for writing");
    }
    return f;
}

// Open file for appending (preserve contents)
FILE* ofile_append(const char* filename) {
    FILE* f = fopen(filename, "ab");
    if (!f) {
        crash("Failed to open file for appending");
    }
    return f;
}

// Write data to a file
void write_file(FILE* f, const void* data, size_t size) {
    if (fwrite(data, 1, size, f) != size) {
        crash("Failed to write to file");
    }
}

// if __len is a function, it will be called several times
#define foreach(__ptr, __len, __item_tn, __body)          \
        for (int __iter = 0; __iter < __len; __iter++)    \
            {                                             \
                __item_tn = (__ptr)[__iter];              \
                do                                        \
                    __body                                \
                while(0);                                 \
            }

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

#define assert(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", \
                    #cond, __FILE__, __LINE__); \
            fflush(stderr); \
            *(volatile int *)0 = 0; \
        } \
    } while (0)

#define da_append(da, item) \
    do { \
        if ((da).count >= (da).capacity) { \
            (da).capacity = (da).capacity == 0 ? DA_INIT_CAP : (da).capacity * 2; \
            (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
            assert((da).items != NULL && "Buy more RAM brodie"); \
        } \
        (da).items[(da).count++] = (item); \
    } while (0)

#define streq(str1, str2) (strcmp(str1, str2) == 0)

int str_is_ualnum(char* str) { // check if string is alnum or _
    for (char c = *str; *str != 0; str++) {
        if (! (isalnum(*str) || *str == '_')) {
            return 0;
        }
    }
    return 1;
}

#define getsign(__val) (2*( (__val) >= 0) - 1)
#define babs(__val) ((__val) * getsign(__val))

#define elif else if

#define when(__cond, __body) case __cond: __body break;

typedef char bool;

#define not !

#define true 1
#define false 0

#ifdef debugging
bool DEBUGGING = true;
#else
bool DEBUGGING = false;
#endif //debugging

#define TC_RED     "\x1b[31m"
#define TC_GREEN   "\x1b[32m"
#define TC_YELLOW  "\x1b[33m"
#define TC_BLUE    "\x1b[34m"
#define TC_MAGENTA "\x1b[35m"
#define TC_CYAN    "\x1b[36m"
#define TC_RESET   "\x1b[0m"

#define debug if (DEBUGGING)