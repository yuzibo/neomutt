/**
 * Copyright (C) 1996-2002,2013 Michael R. Elkins <me@mutt.org>
 * Copyright (C) 1999-2002 Thomas Roessler <roessler@does-not-exist.org>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This header file contains prototypes for internal functions used by the
 * generic mailbox api.  None of these functions should be called directly.
 */

#ifndef _MUTT_MX_H
#define _MUTT_MX_H 1

#include <stdbool.h>
#include <stdio.h>
#include "where.h"
#ifdef USE_HCACHE
#include "hcache/hcache.h"
#endif

struct Header;
struct Context;
struct Message;
struct stat;

/*
 * struct MxOps - a structure to store operations on a mailbox
 * The following operations are mandatory:
 *  - open
 *  - close
 *  - check
 *
 * Optional operations
 *  - open_new_msg
 */
struct MxOps
{
  int (*open)(struct Context *ctx);
  int (*open_append)(struct Context *ctx, int flags);
  int (*close)(struct Context *ctx);
  int (*check)(struct Context *ctx, int *index_hint);
  int (*sync)(struct Context *ctx, int *index_hint);
  int (*open_msg)(struct Context *ctx, struct Message *msg, int msgno);
  int (*close_msg)(struct Context *ctx, struct Message *msg);
  int (*commit_msg)(struct Context *ctx, struct Message *msg);
  int (*open_new_msg)(struct Message *msg, struct Context *ctx, struct Header *hdr);
};

/* supported mailbox formats */
enum
{
  MUTT_MBOX = 1,
  MUTT_MMDF,
  MUTT_MH,
  MUTT_MAILDIR,
  MUTT_NNTP,
  MUTT_IMAP,
  MUTT_NOTMUCH,
  MUTT_POP,
  MUTT_COMPRESSED,
};

WHERE short DefaultMagic INITVAL(MUTT_MBOX);

#define MMDF_SEP "\001\001\001\001\n"
#define MAXLOCKATTEMPT 5

int mbox_check_empty(const char *path);
void mbox_reset_atime(struct Context *ctx, struct stat *st);

int mh_check_empty(const char *path);

int maildir_check_empty(const char *path);

struct Header *maildir_parse_message(int magic, const char *fname, int is_old, struct Header *h);
struct Header *maildir_parse_stream(int magic, FILE *f, const char *fname, int is_old, struct Header *_h);
void maildir_parse_flags(struct Header *h, const char *path);
bool maildir_update_flags(struct Context *ctx, struct Header *o, struct Header *n);
void maildir_flags(char *dest, size_t destlen, struct Header *hdr);

#ifdef USE_HCACHE
int mh_sync_mailbox_message(struct Context *ctx, int msgno, header_cache_t *hc);
#else
int mh_sync_mailbox_message(struct Context *ctx, int msgno);
#endif

#ifdef USE_NOTMUCH
bool mx_is_notmuch(const char *p);
#endif

FILE *maildir_open_find_message(const char *folder, const char *msg, char **newname);

int mbox_strict_cmp_headers(const struct Header *h1, const struct Header *h2);

void mx_alloc_memory(struct Context *ctx);
void mx_update_context(struct Context *ctx, int new_messages);
void mx_update_tables(struct Context *ctx, int committing);


int mx_lock_file(const char *path, int fd, int excl, int timeout);
int mx_unlock_file(const char *path, int fd);

struct MxOps *mx_get_ops(int magic);
extern struct MxOps mx_maildir_ops;
extern struct MxOps mx_mbox_ops;
extern struct MxOps mx_mh_ops;
extern struct MxOps mx_mmdf_ops;

#endif /* _MUTT_MX_H */
