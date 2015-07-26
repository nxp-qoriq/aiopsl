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
#ifndef DTC_H_
#define DTC_H_


#include "fsl_types.h"

#include "dpl/fdt.h"
#include "dpl/util.h"
#if 0
#ifdef DEBUG
#define debug(fmt,args...)	printf(fmt, ##args)
#else
#define debug(fmt,args...)
#endif


#define DEFAULT_FDT_VERSION	17

/*
 * Command line options
 */
extern int quiet;		/* Level of quietness */
extern int reservenum;		/* Number of memory reservation slots */
extern int minsize;		/* Minimum blob size */
extern int padsize;		/* Additional padding to blob */
extern int phandle_format;	/* Use linux,phandle or phandle properties */

#define PHANDLE_LEGACY	0x1
#define PHANDLE_EPAPR	0x2
#define PHANDLE_BOTH	0x3
#endif
typedef uint32_t cell_t;


#define streq(a, b)	(strcmp((a), (b)) == 0)
#define strneq(a, b, n)	(strncmp((a), (b), (n)) == 0)

#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define PTR_MOVE(_ptr, _offset)     (void*)((uint8_t*)(_ptr) + (_offset)) /* TODO - added macro */ 







/* Data blobs */
enum markertype {
	REF_PHANDLE,
	REF_PATH,
	LABEL,
};

struct  marker {
	enum markertype type;
	int offset;
	char *ref;
	struct marker *next;
};

struct data {
	int len;
	char *val;
	struct marker *markers;
};

#define empty_data ((struct data){ /* all .members = 0 or NULL */ })


#define for_each_marker(m) \
	for (; (m); (m) = (m)->next)
#define for_each_marker_of_type(m, t) \
	for_each_marker(m) \
		if ((m)->type == (t))

void data_free(struct data d);

struct data data_grow_for(struct data d, int xlen);
#if 0
struct data data_copy_mem(const char *mem, int len);
struct data data_copy_escape_string(const char *s, int len);
struct data data_copy_file(FILE *f, size_t len);

struct data data_append_data(struct data d, const void *p, int len);
struct data data_insert_at_marker(struct data d, struct marker *m,
				  const void *p, int len);
struct data data_merge(struct data d1, struct data d2);
struct data data_append_cell(struct data d, cell_t word);
struct data data_append_integer(struct data d, uint64_t word, int bits);
struct data data_append_re(struct data d, const struct fdt_reserve_entry *re);
struct data data_append_addr(struct data d, uint64_t addr);
struct data data_append_byte(struct data d, uint8_t byte);
struct data data_append_zeroes(struct data d, int len);
struct data data_append_align(struct data d, int align);

struct data data_add_marker(struct data d, enum markertype type, char *ref);

int data_is_one_string(struct data d);

/* DT constraints */

#define MAX_PROPNAME_LEN	31
#define MAX_NODENAME_LEN	31
#endif
/* Live trees */
struct label {
	int deleted;
	char *label;
	struct label *next;
};

struct property {
	int deleted;
	char *name;
	struct data val;

	struct property *next;

	struct label *labels;
};

struct node {
	int deleted;
	char *name;
	struct property *proplist;
	struct node *children;

	struct node *parent;
	struct node *next_sibling;

	char *fullpath;
	int basenamelen;

	cell_t phandle;
	int addr_cells, size_cells;

	struct label *labels;
};

struct devices {
    char *name;
    struct devices *next;
}; /* TODO - check if necesary -added structure */

#define for_each_label_withdel(l0, l) \
	for ((l) = (l0); (l); (l) = (l)->next)

#define for_each_label(l0, l) \
	for_each_label_withdel(l0, l) \
		if (!(l)->deleted)

#define for_each_property_withdel(n, p) \
	for ((p) = (n)->proplist; (p); (p) = (p)->next)

#define for_each_property(n, p) \
	for_each_property_withdel(n, p) \
		if (!(p)->deleted)

#define for_each_child_withdel(n, c) \
	for ((c) = (n)->children; (c); (c) = (c)->next_sibling)

#define for_each_child(n, c) \
	for_each_child_withdel(n, c) \
		if (!(c)->deleted)

void add_label(struct label **labels, char *label);
void delete_labels(struct label **labels);

struct property *build_property(char *name, struct data val);
struct property *build_property_delete(char *name);
struct property *chain_property(struct property *first, struct property *list);
struct property *reverse_properties(struct property *first);





struct node *build_node(struct property *proplist, struct node *children);
struct node *build_node_delete(void);
#if 0
struct node *name_node(struct node *node, char *name);
struct node *chain_node(struct node *first, struct node *list);
struct node *merge_nodes(struct node *old_node, struct node *new_node);
#endif
void add_property(struct node *node, struct property *prop);
void delete_property_by_name(struct node *node, char *name);
void delete_property(struct property *prop);
void add_child(struct node *parent, struct node *child);
void delete_node_by_name(struct node *parent, char *name);
void delete_node(struct node *node);

const char *get_unitname(struct node *node);
struct property *get_property(struct node *node, const char *propname);
cell_t propval_cell(struct property *prop);
struct property *get_property_by_label(struct node *tree, const char *label,
				       struct node **node);
struct marker *get_marker_label(struct node *tree, const char *label,
				struct node **node, struct property **prop);
struct node *get_subnode(struct node *node, const char *nodename);
struct node *get_node_by_path(struct node *tree, const char *path);
#if 0
struct node *get_node_by_label(struct node *tree, const char *label);
struct node *get_node_by_phandle(struct node *tree, cell_t phandle);
struct node *get_node_by_ref(struct node *tree, const char *ref);
cell_t get_node_phandle(struct node *root, struct node *node);

uint32_t guess_boot_cpuid(struct node *tree);
#endif
/* Boot info (tree plus memreserve information */

struct reserve_info {
	struct fdt_reserve_entry re;

	struct reserve_info *next;

	struct label *labels;
};

struct reserve_info *build_reserve_entry(uint64_t start, uint64_t len);
struct reserve_info *chain_reserve_entry(struct reserve_info *first,
					 struct reserve_info *list);
struct reserve_info *add_reserve_entry(struct reserve_info *list,
				       struct reserve_info *new);


struct boot_info {
	struct reserve_info *reservelist;
	struct node *dt;		/* the device tree */
	uint32_t boot_cpuid_phys;
};

struct boot_info *build_boot_info(struct reserve_info *reservelist,
				  struct node *tree, uint32_t boot_cpuid_phys);
struct boot_info *dtc_from_blob(const unsigned int *dt_blob);
void sort_tree(struct boot_info *bi);
#if 0
/* Checks */

void parse_checks_option(bool warn, bool error, const char *optarg);
void process_checks(int force, struct boot_info *bi);

/* Flattened trees */

void dt_to_blob(FILE *f, struct boot_info *bi, int version);
void dt_to_asm(FILE *f, struct boot_info *bi, int version);

struct boot_info *dt_from_blob(const char *fname);

/* Tree source */

void dt_to_source(FILE *f, struct boot_info *bi);
struct boot_info *dt_from_source(const char *f);

/* FS trees */

struct boot_info *dt_from_fs(const char *dirname);
#endif

/* added functions */

struct property *get_property_by_path(struct node *tree, char *path, const char *propname);

struct data *get_property_val(struct node *tree, char *path, const char *propname);
const struct property *get_properties(struct node *tree, char *path);
const struct devices *get_devices(struct node *tree, char *path);

#endif /* _DTC_H */
#endif
