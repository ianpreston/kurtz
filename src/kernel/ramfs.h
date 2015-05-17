#ifndef _RAMFS_H
#define _RAMFS_H
#include <stdint.h>
#include <stddef.h>

typedef struct
{
    char *filename;
    uint32_t size;
    void *content;
} ramfs_file_t;

typedef struct ramfs_node
{
    ramfs_file_t *file;
    struct ramfs_node *next;
} ramfs_node_t;


// Public interface (temporary)
// TODO - Need to support file descriptors w/ proper open(), read(), write(), etc
// TODO - Support directories
// TODO - Add a directory list interface
void init_ramfs();

ramfs_file_t* ramfs_retr(char *filename);
ramfs_file_t* ramfs_touch(char *filename);
void ramfs_memcpy(ramfs_file_t *dest, void *src, uint32_t len);

void ramfs_debug();

#endif
