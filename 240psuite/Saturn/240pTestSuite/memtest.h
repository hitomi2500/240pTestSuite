#ifndef MEMTEST_H
#define MEMTEST_H

int memtest_test_lines(void * address, int size);
int memtest_test_area(void * address, int size);

#endif /* !MEMTEST_H */
