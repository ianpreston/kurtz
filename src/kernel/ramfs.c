#include "ramfs.h"
#include "libkernel.h"
#include "kheap.h"
#include "terminal.h"

static ramfs_node_t *root_node;
static ramfs_node_t *top_node;

void init_ramfs()
{
    ramfs_file_t *root_file = (ramfs_file_t*)kmalloc(sizeof(ramfs_file_t));
    root_file->filename = "<root>";
    root_file->size = 0;
    root_file->content = NULL;

    root_node = (ramfs_node_t*)kmalloc(sizeof(ramfs_node_t));
    root_node->file = root_file;
    root_node->next = NULL;
    top_node = root_node;
}

void ramfs_debug()
{
    ramfs_node_t *cur = root_node;

    while (cur != NULL)
    {
        printf("%s (size %x)\n", cur->file->filename, cur->file->size);
        cur = cur->next;
    }
}

ramfs_file_t* ramfs_retr(char *filename)
{
    ramfs_node_t *cur = root_node;
    while (cur != NULL)
    {
        if (strcmp(cur->file->filename, filename))
        {
            return cur->file;
        }
        cur = cur->next;
    }
    return NULL;
}

ramfs_file_t* ramfs_touch(char *filename)
{
    // Don't create this file if a file with the same name already exists
    if (ramfs_retr(filename) != NULL)
    {
        return NULL;
    }

    ramfs_file_t *new_file = (ramfs_file_t*)kmalloc(sizeof(ramfs_file_t));
    new_file->filename = filename;
    new_file->size = 0;
    new_file->content = NULL;

    ramfs_node_t *new_node = (ramfs_node_t*)kmalloc(sizeof(ramfs_node_t));
    new_node->file = new_file;
    new_node->next = NULL;

    top_node->next = new_node;
    top_node = new_node;

    return new_file;
}

void ramfs_memcpy(ramfs_file_t *dest, void *src, uint32_t len)
{
    if (dest->content != NULL)
    {
        kfree(dest->content);
    }

    dest->content = kmalloc(len);
    dest->size = len;
    memcpy(dest->content, src, len);
}
