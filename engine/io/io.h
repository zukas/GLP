#ifndef IO_H
#define IO_H

bool io_init();
void io_deinit();

void log(const char *__restrict format, ...);
void error(const char *__restrict format, ...);
#endif // IO_H
