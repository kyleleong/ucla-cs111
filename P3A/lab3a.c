#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ext2_fs.h"

#define SPBLK_OFFSET (1024)

#define ERROR_AND_DIE(syscall) do {					\
		fprintf(stderr, "Error: " syscall " (line %d): %s\n",	\
			__LINE__, strerror(errno));			\
		exit(2);						\
	} while (0)

static int blk_sz, fd, ptrs_per_blk;

/*
 * Convert raw mode from inode into desired character.
 *
 * mode   : The mode from the inode structure.
 * return : Character representing file, directory, or symlink.
 */

char
inode_type(uint16_t mode)
{
	char c;

	switch (mode & S_IFMT)
	{
	case S_IFREG: c = 'f'; break;
	case S_IFDIR: c = 'd'; break;
	case S_IFLNK: c = 's'; break;
	default: c = '?';
	}

	return c;
}

/*
 * Convert Unix timestamp to spec-required format.
 *
 * t      : A Unix timestamp.
 * return : A heap-allocated string with formatted time and  date.
 */

char *
format_time(uint32_t t)
{
	char *s;
	int rc;
	struct tm *ts;
	time_t tt;

	s = malloc(20 * sizeof(char));
	if (s == NULL)
		ERROR_AND_DIE("malloc");

	/* Convert it to calendar time. */
	tt = (time_t) t;
	ts = gmtime(&tt);
	if (ts == NULL)
		ERROR_AND_DIE("gmtime");

	/* Format calendar time to string. */
	rc = strftime(s, 20, "%m/%d/%y %H:%M:%S", ts);
	if (rc <= 0)
		ERROR_AND_DIE("strftime");

	return s;
}

/*
 * Print out the required CSV fields for indirect nodes.
 *
 * inode  : The inode of the 'owning file'.
 * blk    : The block number of the indirect node.
 * level  : The number of indirections to be performed.
 * return : Nothing.
 */

static void
indir_recurse(int inode, uint32_t blk, int level, int initial_offset)
{
	int rc, i, offset;
	int *deref_blk;

	if (blk == 0)
		return;

	deref_blk = malloc(blk_sz);
	if (deref_blk == NULL)
		ERROR_AND_DIE("malloc");

	/* Read the entire block (which contains block pointers) at once. */
	rc = pread(fd, deref_blk, blk_sz, blk * blk_sz);
	if (rc == -1)
		ERROR_AND_DIE("pread");

	/* Iterate over each block pointer. */
	for (i = 0; i < (blk_sz / (int) sizeof(int)); i++)
		if (deref_blk[i] != 0 && level >= 1) {
			offset = initial_offset;
			switch (level)
			{
			case 3:
				/* If we're at block x, a triple indirect block,
				   each block y in the triple indirect block is
				   ptrs_per_blk double indirect block, which is
				   actually ptrs_per_blk direct blocks. So, the
				   2nd add is for the double indirection, and
				   the 1st add is cuz we have to account for the
				   1st indirect block (the 13th inode) */
				offset += ptrs_per_blk;
				offset += ptrs_per_blk * ptrs_per_blk * (i + 1);
				break;
			case 2:
				/* If we're at block x, a double indirect block,
				   each block y in the double indirect block
				   is actually ptrs_per_blk direct blocks, so
				   adjust the calculation accordingly. */
				offset += ptrs_per_blk * (i + 1);
				break;
			case 1:
				offset += i;
			}

			printf("INDIRECT,%d,%d,%d,%d,%d\n",
			       inode,
			       level,
			       offset,
			       blk,
			       deref_blk[i]);

			/* When we added the extra ptrs_per_blk it was for the
			   purpose of redirecting the 3rd level pointer to the
			   correct direct block. We don't want this calculation
			   to affect the next recursive calls to this, so we
			   "undo" the addition. */
			if (level == 3)
				offset -= ptrs_per_blk;

			indir_recurse(inode, deref_blk[i], level - 1, offset);
		}
}

int
main(int argc, char *argv[])
{
	int rc, i, j, k;
	struct ext2_super_block sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s ext2fs.img\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		ERROR_AND_DIE("open");

	/*
	 * Superblock summary.
	 */

	rc = pread(fd, &sb, sizeof(sb), SPBLK_OFFSET);
	if (rc == -1)
		ERROR_AND_DIE("pread");

	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
	       sb.s_blocks_count,
	       sb.s_inodes_count,
	       blk_sz = EXT2_MIN_BLOCK_SIZE << sb.s_log_block_size,
	       sb.s_inode_size,
	       sb.s_blocks_per_group,
	       sb.s_inodes_per_group,
	       sb.s_first_ino);

	ptrs_per_blk = blk_sz / 4;

	/*
	 * Group summary.
	 */

	int num_groups, gdt_size, sb_loc, num_blocks, num_inodes;
	struct ext2_group_desc *gdt;
	unsigned int extra_groups, extra_inodes;

	/* Documentation states the number of blocks may not
	   divide evenly into the number of groups. */
	num_groups = sb.s_blocks_count / sb.s_blocks_per_group;
	extra_groups = sb.s_blocks_count % sb.s_blocks_per_group;
	if (extra_groups)
		++num_groups;
	extra_inodes = sb.s_inodes_count % sb.s_inodes_per_group;

	gdt_size = num_groups * sizeof(struct ext2_group_desc);
	gdt = malloc(gdt_size);
	if (gdt == NULL)
		ERROR_AND_DIE("malloc");

	/* Is the superblock (always @ 1024) in the 1st or 0th block? */
	sb_loc = (SPBLK_OFFSET <= blk_sz) ? 1 : 0;
	rc = pread(fd, gdt, gdt_size, (sb_loc + 1) * blk_sz);
	if (rc == -1)
		ERROR_AND_DIE("pread");

	for (i = 0; i < num_groups; i++) {
		/* Account for uneven division amoung groups. */
		num_blocks = (i == num_groups - 1 && extra_groups) ?
			extra_groups : sb.s_blocks_per_group;
		num_inodes = (i == num_groups - 1 && extra_inodes) ?
			extra_inodes : sb.s_inodes_per_group;

		printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
		       i,
		       num_blocks,
		       num_inodes,
		       gdt[i].bg_free_blocks_count,
		       gdt[i].bg_free_inodes_count,
		       gdt[i].bg_block_bitmap,
		       gdt[i].bg_inode_bitmap,
		       gdt[i].bg_inode_table);
	}

	/*
	 * Free block summary.
	 */

	char *bm;
	int blk_num;

	bm = malloc(blk_sz);
	if (bm == NULL)
		ERROR_AND_DIE("malloc");

	for (i = 0; i < num_groups; i++) {
		rc = pread(fd, bm, blk_sz, gdt[i].bg_block_bitmap * blk_sz);
		if (rc == -1)
			ERROR_AND_DIE("pread");

		/* Account for uneven geometry. */
		num_blocks = (i == num_groups - 1 && extra_groups) ?
			extra_groups : sb.s_blocks_per_group;

		for (j = 0; j < blk_sz; j++)
			for (k = 0; k < 8; k++)
				/* Documentation says 0 is free, and 1 is used. */
				if (((bm[j] >> k) & 1) == 0) {
					blk_num = (j * 8) + k + 1;
					if (blk_num <= num_blocks)
						printf("BFREE,%d\n", blk_num);
				}
	}

	/*
	 * Free inode entries.
	 */

	char *im;
	int inode_num;

	im = malloc(blk_sz);
	if (im == NULL)
		ERROR_AND_DIE("malloc");

	for (i = 0; i < num_groups; i++) {
		rc = pread(fd, im, blk_sz, gdt[i].bg_inode_bitmap * blk_sz);
		if (rc == -1)
			ERROR_AND_DIE("pread");

		/* Account for uneven geometry. */
		num_inodes = (i == num_groups - 1 && extra_inodes) ?
			extra_inodes : sb.s_inodes_per_group;

		for (j = 0; j < blk_sz; j++)
			for (k = 0; k < 8; k++)
				/* 0 is free, 1 is used. */
				if (((im[j] >> k) & 1) == 0) {
					inode_num = (j * 8) + k + 1;
					if (inode_num <= num_inodes)
						printf("IFREE,%d\n", inode_num);
				}
	}

	/*
	 * Inode summary and directory entry.
	 */

	char c, name[EXT2_NAME_LEN];
	int it_sz, offset;
	struct ext2_dir_entry dir_ent;
	struct ext2_inode *it;

	for (i = 0; i < num_groups; i++) {
		num_inodes = (i == num_groups - 1 && extra_inodes) ?
			extra_inodes : sb.s_inodes_per_group;
		it_sz = num_inodes * sizeof(struct ext2_inode);
		it = malloc(it_sz);

		rc = pread(fd, it, it_sz, gdt[i].bg_inode_table * blk_sz);
		if (rc == -1)
			ERROR_AND_DIE("pread");

		for (j = 0; j < num_inodes; j++) {
			/* Spec asked for used, non-zero mode, non-zero link. */
			if (((im[j / 8] >> (j % 8)) & 1) &&
			    it[j].i_mode != 0 &&
			    it[j].i_links_count > 0) {
				c = inode_type(it[j].i_mode);

				printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d",
				       j + 1,
				       c,
				       it[j].i_mode & ~S_IFMT,
				       it[j].i_uid,
				       it[j].i_gid,
				       it[j].i_links_count,
				       format_time(it[j].i_ctime),
				       format_time(it[j].i_mtime),
				       format_time(it[j].i_atime),
				       it[j].i_size,
				       it[j].i_blocks);
				/* If there is any relevant block data. */
				if ((c == 'f') ||
				    (c == 'd') ||
				    (c == 's' && it[j].i_size >= 60)) {
					printf(",");
					for (k = 0; k < EXT2_N_BLOCKS; k++) {
						printf("%d", it[j].i_block[k]);
						if (k != EXT2_N_BLOCKS - 1)
							printf(",");
					}
				}

				printf("\n");

				/* Check the indirect blocks for potential block data. */
				indir_recurse(j + 1, it[j].i_block[EXT2_IND_BLOCK], 1, EXT2_NDIR_BLOCKS);
				indir_recurse(j + 1, it[j].i_block[EXT2_DIND_BLOCK], 2, EXT2_NDIR_BLOCKS);
				indir_recurse(j + 1, it[j].i_block[EXT2_TIND_BLOCK],3, EXT2_NDIR_BLOCKS);
			}

			if (S_ISDIR(it[j].i_mode))
				for (k = 0; k < EXT2_NDIR_BLOCKS; k++) {
					offset = 0;
					/* If offset == blk_sz then we reached end of list. */
					while (offset < blk_sz && it[j].i_block[k] != 0) {
						rc = pread(fd, &dir_ent,
							   sizeof(struct ext2_dir_entry),
							   it[j].i_block[k] * blk_sz + offset);
						if (rc == -1)
							ERROR_AND_DIE("pread");

						strncpy(name, dir_ent.name, EXT2_NAME_LEN);
						name[dir_ent.name_len] = '\0';

						if (dir_ent.inode != 0)
							printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",
							       j + 1,
							       offset,
							       dir_ent.inode,
							       dir_ent.rec_len,
							       dir_ent.name_len,
							       name);

						offset += dir_ent.rec_len;
					}
				}
		}
	}

	exit(0);
}
