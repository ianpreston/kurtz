#include "libkernel.h"
#include "initrd.h"
#include "terminal.h"


void load_initrd(void *tar_archive)
{
    tar_header_t *header = (tar_header_t*)tar_archive;
    while (true)
    {
        header = load_tar_file(header);
        if (header == 0x0)
        {
            return;
        }
    }
}

tar_header_t* load_tar_file(tar_header_t *header)
{
    if (is_header_null(header))
    {
        return 0;
    }

    uint32_t size = load_header_size(header);

    // TODO - Build a ramfs node
    printf("File name: %s\n", header->filename);
    printf("File size: %x\n", size);

    // Find the location of the next file header. The next header is after: 1) this header, 2) the
    // file described by this header, and 3) up to 512 bytes of alignment.
    uint32_t padding = (size == 512) ? 0 : size % 512;
    tar_header_t *next = (tar_header_t*)(((uint32_t)header) + sizeof(tar_header_t) + size + (512 - padding));
    return next;
}

bool is_header_null(tar_header_t *header)
{
    for (uint8_t i=0; i<100; i++)
    {
        if (header->filename[i] != '\0')
        {
            return false;
        }
    }
    return true;
}

uint32_t load_header_size(tar_header_t *header)
{
    // TODO - Make this less ugly. Also, generalize this method to work
    // for any of the header's ASCII octal fields.
    uint32_t size = 0;
    for (uint32_t i=0; i<10; i++)
    {
        char oct_chr = header->size[10-i];
        if (oct_chr == '0' || oct_chr == '\0')
        {
            continue;
        }
        uint8_t oct_val = ((uint8_t)oct_chr) - 48;
        size += oct_val * pow(8, i);
    }

    return size;
}
