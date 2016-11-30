#ifndef MEMORY_HELP_H
#define MEMORY_HELP_H

void escape(void *p) { asm volatile("" ::"g"(p) : "memory"); }
void clobber() { asm volatile("" : : : "memory"); }

#endif // MEMORY_HELP_H
