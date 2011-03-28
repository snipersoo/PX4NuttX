/****************************************************************************
 * include/nuttx/dirent.h
 *
 *   Copyright (C) 2007, 2009, 2011 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_DIRENT_H
#define __INCLUDE_NUTTX_DIRENT_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <dirent.h>

#include <nuttx/fs.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The internal representation of type DIR is just a container for an inode
 * reference, a position, a dirent structure, and file-system-specific
 * information.
 *
 * For the root psuedo-file system, we need retain only the 'next' inode
 * need for the next readdir() operation.  We hold a reference on this
 * inode so we know that it will persist until closedir is called.
 */

struct fs_psuedodir_s
{
  struct inode *fd_next;             /* The inode for the next call to readdir() */
};

#ifndef CONFIG_DISABLE_MOUNTPOINT
#ifdef CONFIG_FS_FAT
/* For fat, we need to return the start cluster, current cluster, current
 * sector and current directory index.
 */

struct fs_fatdir_s
{
  uint32_t     fd_startcluster;        /* Start cluster number of the directory */
  uint32_t     fd_currcluster;         /* Current cluster number being read */
  size_t       fd_currsector;          /* Current sector being read */
  unsigned int fd_index;               /* Current index of the directory entry to read */
};
#endif /* CONFIG_FS_FAT */

#ifdef CONFIG_FS_ROMFS
/* For ROMFS, we need to return the offset to the current and start positions
 * of the directory entry being read
 */

struct fs_romfsdir_s
{
  uint32_t     fr_firstoffset;         /* Offset to the first entry in the directory */
  uint32_t     fr_curroffset;          /* Current offset into the directory contents */
};
#endif /* CONFIG_FS_ROMFS */

#ifdef CONFIG_APPS_BINDIR
/* The apps/ pseudo bin/ directory.  The state value is simply an index */

struct fs_binfsdir_s
{
  unsigned int fb_index;               /* Index to the next named entry point */
};
#endif
#endif /* CONFIG_DISABLE_MOUNTPOINT */

struct fs_dirent_s
{
  /* This is the node that was opened by opendir.  The type of the inode
   * determines the way that the readdir() operations are performed. For the
   * psuedo root psuedo-file system, it is also used to support rewind.
   *
   * We hold a reference on this inode so we know that it will persist until
   * closedir() is called (although inodes linked to this inode may change).
   */

  struct inode *fd_root;

  /* At present, only mountpoints require special handling flags */

#ifndef CONFIG_DISABLE_MOUNTPOINT
  unsigned int fd_flags;
#endif

  /* This keeps track of the current directory position for telldir */

  off_t fd_position;

  /* Retained control information depends on the type of file system that
   * provides is provides the mountpoint.  Ideally this information should
   * be hidden behind an opaque, file-system-dependent void *, but we put
   * the private definitions in line here for now to reduce allocations.
   */

  union
    {
      /* Private data used by the built-in psuedo-file system */

      struct fs_psuedodir_s psuedo;

      /* Private data used by other file systems */

#ifndef CONFIG_DISABLE_MOUNTPOINT
#ifdef CONFIG_FS_FAT
      struct fs_fatdir_s    fat;
#endif
#ifdef CONFIG_FS_ROMFS
      struct fs_romfsdir_s  romfs;
#endif
#ifdef CONFIG_APPS_BINDIR
      struct fs_binfsdir_s  binfs;
#endif
#endif
   } u;

  /* In any event, this the actual struct dirent that is returned by readdir */

  struct dirent fd_dir;              /* Populated when readdir is called */
};

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __INCLUDE_NUTTX_DIRENT_H */

