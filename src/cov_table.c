#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "ascii_trie.h"  // vrd_ASCII_Trie, vrd_ascii_trie_*
#include "itv_tree.h"    // vrd_Itv_Tree, vrd_itv_tree_*


struct vrd_Cov_Table
{
    vrd_ASCII_Trie* restrict trie;

    size_t ref_capacity;
    size_t tree_capacity;
    size_t next;
    vrd_Itv_Tree* restrict tree[];
}; // vrd_Cov_Table


vrd_Cov_Table*
vrd_cov_table_init(size_t const ref_capacity,
                   size_t const ref_size_capacity,
                   size_t const tree_capacity)
{
    if ((size_t) UINT32_MAX <= ref_capacity ||
        (size_t) UINT32_MAX <= ref_size_capacity ||
        (size_t) UINT32_MAX <= tree_capacity)
    {
        return NULL;
    } // if

    vrd_Cov_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->tree[0]) *
                                        ref_capacity);
    if (NULL == table)
    {
        return NULL;
    } // if

    table->trie = vrd_ascii_trie_init(ref_size_capacity);
    if (NULL == table->trie)
    {
        free(table);
        return NULL;
    } // if

    table->ref_capacity = ref_capacity;
    table->tree_capacity = tree_capacity;
    table->next = 0;

    return table;
} // vrd_cov_table_init


void
vrd_cov_table_destroy(vrd_Cov_Table* restrict* const table)
{
    if (NULL != table)
    {
        for (size_t i = 0; i < (*table)->next; ++i)
        {
            vrd_itv_tree_destroy(&(*table)->tree[i]);
        } // for
        vrd_ascii_trie_destroy(&(*table)->trie);
        free(*table);
        *table = NULL;
    } // if
} // vrd_cov_table_destroy


int
vrd_cov_table_insert(vrd_Cov_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id)
{
    assert(NULL != table);

    vrd_Itv_Tree* restrict tree = vrd_ascii_trie_find(table->trie,
                                                      len,
                                                      reference);
    if (NULL == tree)
    {
        if (table->ref_capacity <= table->next)
        {
            return -1;
        } // if

        table->tree[table->next] =
            vrd_itv_tree_init(table->tree_capacity);
        if (NULL == table->tree[table->next])
        {
            return -1;
        } // if

        tree = table->tree[table->next];
        table->next += 1;

        if (NULL == vrd_ascii_trie_insert(table->trie,
                                          len,
                                          reference,
                                          tree))
        {
            return -1;
        } // if
    } // if

    if (NULL == vrd_itv_tree_insert(tree, start, end, sample_id))
    {
        return -1;
    } // if

    return 0;
} // vrd_cov_table_insert


size_t
vrd_cov_table_query(vrd_Cov_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const start,
                    size_t const end,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);

    vrd_Itv_Tree const* const restrict tree =
        vrd_ascii_trie_find(table->trie, len, reference);
    if (NULL == tree)
    {
        return 0;
    } // if

    return vrd_itv_tree_query(tree, start, end, subset);
} // vrd_cov_table_query


size_t
vrd_cov_table_remove(vrd_Cov_Table* const restrict table,
                     vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);
    assert(NULL != subset);

    size_t count = 0;
    for (size_t i = 0; i < table->next; ++i)
    {
        count += vrd_itv_tree_remove(table->tree[i], subset); // OVERFLOW
    } // for

    return count;
} // vrd_cov_table_remove
