#include "test.h"

void assert_failed(const char *func, const char *cond, const char *file,
                   const int line) {
    char *test_name;
    size_t test_name_size = strlen(func) + 1;

    test_name = (char *)(malloc(sizeof(char) * test_name_size));
    memcpy(test_name, func, test_name_size * sizeof(char));
    strreplace(test_name, '_', ' ');

    fprintf(stderr, "\n\n------------------------------\n\n");
    fprintf(stderr, "Test failed: %s\n"
                    "  Condition: %s\n"
                    "       File: %s\n"
                    "       Line: %d\n",
                    test_name, cond, file, line);
    fprintf(stderr, "\n------------------------------\n\n");
    printf("* The tests failed! Aborting...\n");

    free(test_name);
    abort();
}

int main(void) {
    lexer_run_tests();
    parser_run_tests();
    semantic_run_tests();

    printf("* The tests finished successfully!\n");
    return 0;
}
