 /* file: minunit.h */
 #define should(message, test) do { if (!(test)){ printf("    \xe2\x9c\x98 * %s\n", message); tests_fail++; } \
                                    else { printf("    \xE2\x9C\x94 * %s\n", message);} } while (0)

 #define it(message, test) do { printf("\n%s should:\n", message); test(); fflush(stdout); tests_run++; } while (0)
 #define describe(message) do { printf("\n  %s:\n", message); } while (0)
 extern int tests_run;
 extern int tests_fail;

