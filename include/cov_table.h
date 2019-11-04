#ifndef VRD_COV_TABLE_H
#define VRD_COV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree
#include "template.h"   // VRD_TEMPLATE


#define VRD_TYPENAME Cov


#include "../src/template_table.h"  // vrd_Cov_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const sample_id);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const restrict self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              vrd_AVL_Tree const* const restrict subset);


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
