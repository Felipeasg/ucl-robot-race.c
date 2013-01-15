#define KRED  "\x1B[31m"
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"

#define should(message, test) do { if (!(test)){ printf(KRED "    \xe2\x9c\x98  * %s\n" KNRM, message); tests_fail++; } \
                                  else { printf(KGRN "    \xE2\x9C\x94 " KNRM " * %s\n", message);} } while (0)
#define it(message, test) do { printf("\n%s should:\n", message); test(); tests_run++; } while (0)
#define describe(message) do { printf("\n  %s:\n", message); } while (0)
extern int tests_run;
extern int tests_fail;

