#include "unibilium.h"

#include <stdio.h>
#include <ctype.h>

int main(void) {
	unibi_term *const ut = unibi_from_env();
	if (!ut) {
		perror("unibi_from_env");
		return 1;
	}

	printf("Terminal name: %s\n", unibi_get_name(ut));
	{
		const char **a = unibi_get_aliases(ut);
		if (*a) {
			printf("Aliases: ");
			do {
				printf("%s%s", *a, a[1] ? " | " : "");
				++a;
			} while (*a);
			printf("\n");
		}
	}
	printf("\n");

	printf("Boolean capabilities:\n");
	for (enum unibi_boolean i = unibi_boolean_begin_ + 1; i < unibi_boolean_end_; ++i) {
		if (unibi_get_bool(ut, i)) {
			printf("  %-25s / %s\n", unibi_name_bool(i), unibi_short_name_bool(i));
		}
	}
	printf("\n");

	printf("Numeric capabilities:\n");
	for (enum unibi_numeric i = unibi_numeric_begin_ + 1; i < unibi_numeric_end_; ++i) {
		short n = unibi_get_num(ut, i);
		if (n != -1) {
			printf("  %-25s / %-10s = %hd\n", unibi_name_num(i), unibi_short_name_num(i), n);
		}
	}
	printf("\n");

	printf("String capabilities:\n");
	for (enum unibi_string i = unibi_string_begin_ + 1; i < unibi_string_end_; ++i) {
		const char *s = unibi_get_str(ut, i);
		if (s) {
			/* Most of these strings will contain escape sequences */
			printf("  %-25s / %-10s = ", unibi_name_str(i), unibi_short_name_str(i));
			for (unsigned char c; (c = *s); ++s) {
				switch (c) {
					case '\a': printf("\\a"); break;
					case '\b': printf("\\b"); break;
					case '\f': printf("\\f"); break;
					case '\n': printf("\\n"); break;
					case '\r': printf("\\r"); break;
					case '\t': printf("\\t"); break;
					case '\v': printf("\\v"); break;
					case '\033': printf("\\e"); break;
					default:
						if (isprint(c)) {
							printf("%c", c);
						} else {
							printf("\\%03o", c);
						}
						break;
				}
			}
			printf("\n");
		}
	}

	unibi_destroy(ut);

	return 0;
}
