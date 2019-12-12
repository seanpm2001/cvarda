#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fopen, fclose, fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_SNV_Table* snv = vrd_SNV_table_init(1000, 1 << 24);
    assert(NULL != snv);

    int ret = vrd_SNV_table_insert(snv, 5, "chr1", 10, 1, 1, 10, 1);
    assert(0 == ret);

    ret = vrd_SNV_table_insert(snv, 5, "chr1", 10, 1, 1, 10, 1);
    assert(0 == ret);

    FILE* stream = fopen("snv_export.varda", "w");
    assert(NULL != stream);

    vrd_SNV_table_export(snv, stream);

    fclose(stream);

    vrd_SNV_table_destroy(&snv);
    assert(NULL == snv);

    return EXIT_SUCCESS;
} // main
