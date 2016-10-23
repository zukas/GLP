#ifndef MEMORY_H
#define MEMORY_H

void escape(void *p) { asm volatile("" ::"g"(p) : "memory"); }
void clobber() { asm volatile("" : : : "memory"); }

#endif // MEMORY_H
