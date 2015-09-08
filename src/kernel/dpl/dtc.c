/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef ARENA_LEGACY_CODE
#include "fsl_types.h"
#include "common/fsl_string.h"

#include "dpl/dtc.h"
#include "dpl/fdt.h"

#define FTF_FULLPATH    0x1
#define FTF_VARALIGN    0x2
#define FTF_NAMEPROPS   0x4
#define FTF_BOOTCPUID   0x8
#define FTF_STRTABSIZE  0x10
#define FTF_STRUCTSIZE  0x20
#define FTF_NOPS    0x40

#define empty_data ((struct data){ /* all .members = 0 or NULL */ })

struct inbuf {
    char *base, *limit, *ptr;
};

static struct version_info {
    int version;
    int last_comp_version;
    int hdr_size;
    int flags;
} version_table[] = {
    {1, 1, FDT_V1_SIZE,
     FTF_FULLPATH|FTF_VARALIGN|FTF_NAMEPROPS},
    {2, 1, FDT_V2_SIZE,
     FTF_FULLPATH|FTF_VARALIGN|FTF_NAMEPROPS|FTF_BOOTCPUID},
    {3, 1, FDT_V3_SIZE,
     FTF_FULLPATH|FTF_VARALIGN|FTF_NAMEPROPS|FTF_BOOTCPUID|FTF_STRTABSIZE},
    {16, 16, FDT_V3_SIZE,
     FTF_BOOTCPUID|FTF_STRTABSIZE|FTF_NOPS},
    {17, 16, FDT_V17_SIZE,
     FTF_BOOTCPUID|FTF_STRTABSIZE|FTF_STRUCTSIZE|FTF_NOPS},
};
#if 0
static void make_fdt_header(struct fdt_header *fdt, struct version_info *vi, int reservesize, int dtsize, int strsize, int boot_cpuid_phys)
{
    int reserve_off;

    reservesize += sizeof(struct fdt_reserve_entry);

    memset(fdt, 0xff, sizeof(*fdt));

    fdt->magic = FDT_MAGIC;
    fdt->version = vi->version;
    fdt->last_comp_version = vi->last_comp_version;

    /* Reserve map should be doubleword aligned */
    reserve_off = ALIGN(vi->hdr_size, 8);

    fdt->off_mem_rsvmap = reserve_off;
    fdt->off_dt_struct = reserve_off + reservesize;
    fdt->off_dt_strings = reserve_off + reservesize + dtsize;
    fdt->totalsize = reserve_off + reservesize + dtsize + strsize;

    if (vi->flags & FTF_BOOTCPUID)
        fdt->boot_cpuid_phys = boot_cpuid_phys;
    if (vi->flags & FTF_STRTABSIZE)
        fdt->size_dt_strings = strsize;
    if (vi->flags & FTF_STRUCTSIZE)
        fdt->size_dt_struct = dtsize;
}
#endif

#if 0
// TODO removed as it's not used

static void inbuf_init(struct inbuf *inb, void *base, void *limit)
{
    inb->base = base;
    inb->limit = limit;
    inb->ptr = inb->base;
}

static void flat_read_chunk(struct inbuf *inb, void *p, int len)
{
  //  if ((inb->ptr + len) > inb->limit)
  //      die("Premature end of data parsing flat device tree\n");

    memcpy(p, inb->ptr, len);

    inb->ptr = PTR_MOVE(inb->ptr , len);
}

static uint32_t flat_read_word(struct inbuf *inb)
{
    uint32_t val;

//    assert(((inb->ptr - inb->base) % sizeof(val)) == 0);

    flat_read_chunk(inb, &val, sizeof(val));

    return val;
}



static void flat_realign(struct inbuf *inb, int align)
{
    int off = inb->ptr - inb->base;

    inb->ptr = PTR_MOVE(inb->base , ALIGN(off, align));
//    if (inb->ptr > inb->limit)
//        die("Premature end of data parsing flat device tree\n");
}

static char *flat_read_string(struct inbuf *inb)
{
    int len = 0;
    const char *p = inb->ptr;
    char *str;

    do {
  //      if (p >= inb->limit)
  //          die("Premature end of data parsing flat device tree\n");
        len++;
    } while ((*p++) != '\0');

    str = strdup(inb->ptr);

    inb->ptr = PTR_MOVE(inb->ptr, len);

    flat_realign(inb, sizeof(uint32_t));

    return str;
}

static char *nodename_from_path(const char *ppath, const char *cpath)
{
    int plen;

    plen = strlen(ppath);

//    if (!strneq(ppath, cpath, plen))
//        die("Path \"%s\" is not valid as a child of \"%s\"\n",
//            cpath, ppath);

    /* root node is a special case */
//   if (!streq(ppath, "/"))
     if (strcmp(ppath, "/"))
        plen++;

    return strdup(PTR_MOVE(cpath , plen));
}

static struct data flat_read_data(struct inbuf *inb, int len)
{
    struct data d = empty_data;

    if (len == 0)
        return empty_data;

    d = data_grow_for(d, len);
    d.len = len;

    flat_read_chunk(inb, d.val, len);

    flat_realign(inb, sizeof(uint32_t));

    return d;
}

static char *flat_read_stringtable(struct inbuf *inb, int offset)
{
    const char *p;

    p = PTR_MOVE(inb->base ,offset);
    while (1) {
//        if (p >= inb->limit || p < inb->base)
//            die("String offset %d overruns string table\n",
//                offset);

        if (*p == '\0')
            break;

        p++;
    }

    return strdup(PTR_MOVE(inb->base , offset));
}

static struct property *flat_read_property(struct inbuf *dtbuf,
                       struct inbuf *strbuf, int flags)
{
    uint32_t proplen, stroff;
    char *name;
    struct data val;

    proplen = flat_read_word(dtbuf);
    stroff = flat_read_word(dtbuf);

    name = flat_read_stringtable(strbuf, stroff);

    if ((flags & FTF_VARALIGN) && (proplen >= 8))
        flat_realign(dtbuf, 8);

    val = flat_read_data(dtbuf, proplen);

    return build_property(name, val);
}

static struct reserve_info *flat_read_mem_reserve(struct inbuf *inb)
{
    struct reserve_info *reservelist = NULL;
    struct reserve_info *new;
    struct fdt_reserve_entry re;

    /*
     * Each entry is a pair of u64 (addr, size) values for 4 cell_t's.
     * List terminates at an entry with size equal to zero.
     *
     * First pass, count entries.
     */
    while (1) {
        flat_read_chunk(inb, &re, sizeof(re));
//        re.address  = fdt64_to_cpu(re.address);
//        re.size = fdt64_to_cpu(re.size);
        if (re.size == 0)
            break;

        new = build_reserve_entry(re.address, re.size);
        reservelist = add_reserve_entry(reservelist, new);
    }

    return reservelist;
}

static struct node *unflatten_tree(struct inbuf *dtbuf,
                   struct inbuf *strbuf,
                   const char *parent_flatname, int flags)
{
    struct node *node;
    char *flatname;
    uint32_t val;

    node = build_node(NULL, NULL);

    flatname = flat_read_string(dtbuf);

    if (flags & FTF_FULLPATH)
        node->name = nodename_from_path(parent_flatname, flatname);
    else
        node->name = flatname;

    do {
        struct property *prop;
        struct node *child;

        val = flat_read_word(dtbuf);
        switch (val) {
        case FDT_PROP:
//            if (node->children)
//                fprintf(stderr, "Warning: Flat tree input has "
//                    "subnodes preceding a property.\n");
            prop = flat_read_property(dtbuf, strbuf, flags);
            add_property(node, prop);
            break;

        case FDT_BEGIN_NODE:
            child = unflatten_tree(dtbuf,strbuf, flatname, flags);
            add_child(node, child);
            break;

        case FDT_END_NODE:
            break;

        case FDT_END:
//            die("Premature FDT_END in device tree blob\n");
            break;

        case FDT_NOP:
//            if (!(flags & FTF_NOPS))
//                fprintf(stderr, "Warning: NOP tag found in flat tree"
//                    " version <16\n");

            /* Ignore */
            break;

        default:
            break;
//            die("Invalid opcode word %08x in device tree blob\n",
//                val);
        }
    } while (val != FDT_END_NODE);

    return node;
}

struct boot_info *dtc_from_blob(const unsigned int *dt_blob)
{
//    FILE *f;
    uint32_t magic, totalsize, version, size_dt, boot_cpuid_phys;
    uint32_t off_dt, off_str, off_mem_rsvmap;
//    int rc;
    char *blob;
    struct fdt_header *fdt;
//    char *p;
    struct inbuf dtbuf, strbuf;
    struct inbuf memresvbuf;
//    int sizeleft;
    struct reserve_info *reservelist;
    struct node *tree;
    uint32_t val;
    int flags = 0;
    
#if 0
    magic = fdt32_to_cpu(magic);
    if (magic != FDT_MAGIC)
        die("Blob has incorrect magic number\n");

    rc = fread(&totalsize, sizeof(totalsize), 1, f);
    if (ferror(f))
        die("Error reading DT blob size: %s\n", strerror(errno));
    if (rc < 1) {
        if (feof(f))
            die("EOF reading DT blob size\n");
        else
            die("Mysterious short read reading blob size\n");
    }


    if (totalsize < FDT_V1_SIZE)
        die("DT blob size (%d) is too small\n", totalsize);

    blob = fsl_malloc(totalsize);
#endif
    fdt = (struct fdt_header *)dt_blob;


    magic = fdt->magic;
    totalsize = fdt->totalsize;
#if 0
    sizeleft = totalsize - sizeof(magic) - sizeof(totalsize);
    p = blob + sizeof(magic)  + sizeof(totalsize);

    while (sizeleft) {
        if (feof(f))
            die("EOF before reading %d bytes of DT blob\n",
                totalsize);

        rc = fread(p, 1, sizeleft, f);
        if (ferror(f))
            die("Error reading DT blob: %s\n",
                strerror(errno));

        sizeleft -= rc;
        p += rc;
    }
#endif

    off_dt = fdt->off_dt_struct;
    off_str = fdt->off_dt_strings;
    off_mem_rsvmap = fdt->off_mem_rsvmap;
    version = fdt->version;
    boot_cpuid_phys = fdt->boot_cpuid_phys;
   
#if 0
    if (off_mem_rsvmap >= totalsize)
        die("Mem Reserve structure offset exceeds total size\n");

    if (off_dt >= totalsize)
        die("DT structure offset exceeds total size\n");

    if (off_str > totalsize)
        die("String table offset exceeds total size\n");
#endif
    if (version >= 3) {
        uint32_t size_str = fdt->size_dt_strings;
//        if (off_str+size_str > totalsize)
//            die("String table extends past total size\n");

        inbuf_init(&strbuf, PTR_MOVE(fdt, off_str),PTR_MOVE(fdt, off_str + size_str));
    } else {
        inbuf_init(&strbuf, PTR_MOVE(fdt, off_str), PTR_MOVE(fdt ,totalsize));
    }

    if (version >= 17) {
        size_dt = fdt->size_dt_struct;
//        if (off_dt+size_dt > totalsize)
//            die("Structure block extends past total size\n");
    }

    if (version < 16) {
        flags |= FTF_FULLPATH | FTF_NAMEPROPS | FTF_VARALIGN;
    } else {
        flags |= FTF_NOPS;
    }

    inbuf_init(&memresvbuf, PTR_MOVE(fdt , off_mem_rsvmap),PTR_MOVE(fdt, totalsize));
    inbuf_init(&dtbuf, PTR_MOVE(fdt , off_dt),PTR_MOVE(fdt , totalsize));

    reservelist = flat_read_mem_reserve(&memresvbuf);
//    reservelist = NULL;
    val = flat_read_word(&dtbuf);

//    if (val != FDT_BEGIN_NODE)
//        die("Device tree blob doesn't begin with FDT_BEGIN_NODE (begins with 0x%08x)\n", val);

    tree = unflatten_tree(&dtbuf, &strbuf, "", flags);

    val = flat_read_word(&dtbuf);
//    if (val != FDT_END)
//        die("Device tree blob doesn't end with FDT_END\n");

//    fsl_free(blob);
//    fclose(f);
    return build_boot_info(reservelist, tree, boot_cpuid_phys);
}

#endif
#endif
