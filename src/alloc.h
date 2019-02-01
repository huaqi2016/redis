/*
 * Copyright (c) 2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ALLOC_H__
#define __ALLOC_H__

#include <stdint.h>
#include "zmalloc.h"
#include "memkind_malloc.h"

/* Typedefs for user data allocator */
typedef void *(*ualloc)(size_t size);
typedef void *(*ucalloc)(size_t size);
typedef void *(*ufree)(void *ptr);
typedef void *(*urealloc)(void *ptr, size_t size);

int je_get_defrag_hint(void* ptr, int *bin_util, int *run_util);

/* If you want to change this structure remember to update allocCompare */
struct __alloc {
    void *(*alloc)(size_t size);
    void *(*calloc)(size_t size);
    void *(*realloc)(void *ptr, size_t size);
    void (*free)(void *ptr);
    size_t (*alloc_size)(void *ptr);
    void *(*alloc_no_tcache)(size_t size);
    void (*free_no_tcache)(void *ptr);
    int (*get_defrag_hint)(void* ptr, int *bin_util, int *run_util);
    void *(*memcpy)(void* dst, const void * src, size_t num);
    void *(*memset)( void* ptr, int value, size_t num);
};
typedef const struct __alloc *alloc;

static const struct __alloc __z_alloc = {
    zmalloc,
    zcalloc,
    zrealloc,
    zfree,
    je_malloc_usable_size, /*zmalloc_size,*/
    zmalloc_no_tcache,
    zfree_no_tcache,
    je_get_defrag_hint,
    zmemcpy,
    zmemset
};
static const struct __alloc *z_alloc = &__z_alloc;

static void * mmalloc_ram_info_wrapper (size_t size)
{
    void* ptr = mmalloc(size + MEMKIND_PREFIX_SIZE);
    uint64_t *is_ram = ptr;
    *is_ram = 0;
    return (void*)((char*)ptr + MEMKIND_PREFIX_SIZE);
}

static const struct __alloc __m_alloc = {
    mmalloc_ram_info_wrapper,
    mcalloc,
    mrealloc,
    mfree,
    mmalloc_usable_size,
    mmalloc,
    mfree,
    mget_defrag_hint,
    mmemcpy,
    mmemset
};
static const struct __alloc *m_alloc = &__m_alloc;

int allocCompare(alloc lhs, alloc rhs);

#endif /* __ALLOC_H__ */
