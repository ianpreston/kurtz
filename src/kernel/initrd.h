#ifndef _INITRD_H
#define _INITRD_H
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    char filename[100];
    char mode[8];
    char user_id[8];
    char group_id[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    uint8_t link_ind;
    char link_name[100];
} __attribute__((packed)) __attribute__((aligned(512))) tar_header_t;


// Public interface
void load_initrd(void *tar_archive);

// Internal interface
tar_header_t* load_tar_file(tar_header_t *header);
bool is_header_null(tar_header_t *header);
uint32_t load_header_size(tar_header_t *header);

#endif
