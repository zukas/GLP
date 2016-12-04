#ifndef SYSDEF_H
#define SYSDEF_H

#ifdef __GNUG__
#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define FORCE_INLINE __attribute__((always_inline))
#endif

#endif // SYSDEF_H
