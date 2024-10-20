#ifndef UTIL_H
#define UTIL_H

#define container_of(pointer, type, member) \
    (type *)((char *) (pointer) - (char *) &((type *) 0)->member)

#endif
