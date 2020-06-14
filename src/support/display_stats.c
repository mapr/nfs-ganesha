/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright (C) Panasas Inc., 2013
 * Author: Sriram Patil (spsrirampatil@gmail.com)
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * -------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include "gsh_types.h"
#include "gsh_list.h"
#include "gsh_dbus.h"
#include "nfs_core.h"
#include "server_stats.h"

typedef struct global_stats global_stats;

const struct op_name optabv4[] = {
	[0] = {.name = NULL,}, /* Illegal */
	[1] = {.name = NULL,}, /* Illegal */
	[2] = {.name = NULL,}, /* Illegal */
	[NFS4_OP_ACCESS] = {.name = "ACCESS",},
	[NFS4_OP_CLOSE] = {.name = NULL,},
	[NFS4_OP_COMMIT] = {.name = "COMMIT",},
	[NFS4_OP_CREATE] = {.name = "CREATE",},
	[NFS4_OP_DELEGPURGE] = {.name = NULL,},
	[NFS4_OP_DELEGRETURN] = {.name = NULL,},
	[NFS4_OP_GETATTR] = {.name = "GETATTR",},
	[NFS4_OP_GETFH] = {.name = NULL,},
	[NFS4_OP_LINK] = {.name = "LINK",},
	[NFS4_OP_LOCK] = {.name = "LOCK",},
	[NFS4_OP_LOCKT] = {.name = NULL,},
	[NFS4_OP_LOCKU] = {.name = NULL,},
	[NFS4_OP_LOOKUP] = {.name = "LOOKUP",},
	[NFS4_OP_LOOKUPP] = {.name = "LOOKUPP",},
	[NFS4_OP_NVERIFY] = {.name = NULL,},
	[NFS4_OP_OPEN] = {.name = "OPEN",},
	[NFS4_OP_OPENATTR] = {.name = NULL,},
	[NFS4_OP_OPEN_CONFIRM] = {.name = NULL,},
	[NFS4_OP_OPEN_DOWNGRADE] = {.name = NULL,},
	[NFS4_OP_PUTFH] = {.name = NULL,},
	[NFS4_OP_PUTPUBFH] = {.name = NULL,},
	[NFS4_OP_PUTROOTFH] = {.name = NULL,},
	[NFS4_OP_READ] = {.name = "READ",},
	[NFS4_OP_READDIR] = {.name = "READDIR",},
	[NFS4_OP_READLINK] = {.name = "READLINK",},
	[NFS4_OP_REMOVE] = {.name = "REMOVE",},
	[NFS4_OP_RENAME] = {.name = "RENAME",},
	[NFS4_OP_RENEW] = {.name = NULL,},
	[NFS4_OP_RESTOREFH] = {.name = NULL,},
	[NFS4_OP_SAVEFH] = {.name = NULL,},
	[NFS4_OP_SECINFO] = {.name = NULL,},
	[NFS4_OP_SETATTR] = {.name = "SETATTR",},
	[NFS4_OP_SETCLIENTID] = {.name = NULL,},
	[NFS4_OP_SETCLIENTID_CONFIRM] = {.name = NULL,},
	[NFS4_OP_VERIFY] = {.name = NULL,},
	[NFS4_OP_WRITE] = {.name = "WRITE",},
};
/*
   [NFS4_OP_RELEASE_LOCKOWNER] = {.name = NULL,},
   [NFS4_OP_BACKCHANNEL_CTL] = {.name = NULL,},
   [NFS4_OP_BIND_CONN_TO_SESSION] = {.name = NULL,},
   [NFS4_OP_EXCHANGE_ID] = {.name = NULL,},
   [NFS4_OP_CREATE_SESSION] = {.name = NULL,},
   [NFS4_OP_DESTROY_SESSION] = {.name = "DESTROY_SESSION",},
   [NFS4_OP_FREE_STATEID] = {.name = "FREE_STATEID",},
   [NFS4_OP_GET_DIR_DELEGATION] = {.name = "GET_DIR_DELEGATION",},
   [NFS4_OP_GETDEVICEINFO] = {.name = "GETDEVICEINFO",},
   [NFS4_OP_GETDEVICELIST] = {.name = "GETDEVICELIST",},
   [NFS4_OP_LAYOUTCOMMIT] = {.name = "LAYOUTCOMMIT",},
   [NFS4_OP_LAYOUTGET] = {.name = "LAYOUTGET",},
   [NFS4_OP_LAYOUTRETURN] = {.name = "LAYOUTRETURN",},
   [NFS4_OP_SECINFO_NO_NAME] = {.name = "SECINFO_NO_NAME",},
   [NFS4_OP_SEQUENCE] = {.name = "SEQUENCE",},
   [NFS4_OP_SET_SSV] = {.name = "SET_SSV",},
   [NFS4_OP_TEST_STATEID] = {.name = "TEST_STATEID",},
   [NFS4_OP_WANT_DELEGATION] = {.name = "WANT_DELEGATION",},
   [NFS4_OP_DESTROY_CLIENTID] = {.name = "DESTROY_CLIENTID",},
   [NFS4_OP_RECLAIM_COMPLETE] = {.name = "RECLAIM_COMPLETE",},
//NFSv4.2
[NFS4_OP_ALLOCATE] = {.name = "ALLOCATE",},
[NFS4_OP_COPY] = {.name = "COPY",},
[NFS4_OP_COPY_NOTIFY] = {.name = "COPY_NOTIFY",},
[NFS4_OP_DEALLOCATE] = {.name = "DEALLOCATE",},
[NFS4_OP_IO_ADVISE] = {.name = "IO_ADVISE",},
[NFS4_OP_LAYOUTERROR] = {.name = "LAYOUTERROR",},
[NFS4_OP_OFFLOAD_CANCEL] = {.name = "OFFLOAD_CANCEL",},
[NFS4_OP_OFFLOAD_STATUS] = {.name = "OFFLOAD_STATUS",},
[NFS4_OP_READ_PLUS] = {.name = "READ_PLUS",},
[NFS4_OP_SEEK] = {.name = "SEEK",},
[NFS4_OP_WRITE_SAME] = {.name = "WRITE_SAME",},
};
 */

static void print_header(int count)
{
	int i;

	for (i = 0; i < count; ++i) {
		if (optabv4[i].name != NULL
				&& (i != NFS4_OP_READ && i != NFS4_OP_WRITE)) {
			fprintf(stdout, "%-10s", optabv4[i].name);
		}
	}

	fprintf(stdout, "%-10s%-10s\n", optabv4[NFS4_OP_READ].name,
			optabv4[NFS4_OP_WRITE].name);
}

static inline void copy_stats(global_stats *src, global_stats *dest)
{
	memcpy(dest, src, sizeof(global_stats));
}

void do_diffs(global_stats *curr, global_stats *prev, global_stats *diff,
		int opcount)
{
	int i;

	for (i = 0; i < opcount; i++) {
		if (optabv4[i].name != NULL)
			diff->v4.op[i] = curr->v4.op[i] - prev->v4.op[i];
	}

	diff->nfsv40.read.requested = curr->nfsv40.read.requested
		- prev->nfsv40.read.requested;
	diff->nfsv40.read.transferred = curr->nfsv40.read.transferred
		- prev->nfsv40.read.transferred;
	diff->nfsv40.write.requested = curr->nfsv40.write.requested
		- prev->nfsv40.write.requested;
	diff->nfsv40.write.transferred = curr->nfsv40.write.transferred
		- prev->nfsv40.write.transferred;
}

static void print_stats(global_stats *st, int opcount)
{
	int i;

	for (i = 0; i < opcount; i++) {
		if (optabv4[i].name != NULL
				&& (i != NFS4_OP_READ && i != NFS4_OP_WRITE)) {
			fprintf(stdout, "%-10lu", st->v4.op[i]);
		}
	}

	fprintf(stdout, "%-4lu %-5lu", st->v4.op[NFS4_OP_READ],
			(st->nfsv40.read.transferred >> 20));
	fprintf(stdout, "%-4lu %-5lu\n", st->v4.op[NFS4_OP_WRITE],
			(st->nfsv40.write.transferred >> 20));
}

static void dump_stats(global_stats *shm_stats, int opcount)
{
	global_stats prev, curr, diff;
	int j = 0;

	memset(&prev, 0, sizeof(global_stats));
	memset(&curr, 0, sizeof(global_stats));
	memset(&diff, 0, sizeof(global_stats));

	copy_stats(shm_stats, &curr);

	while (1) {
		if (j % 10 == 0) {
			fprintf(stdout, "\n");
			print_header(opcount);
		}

		sleep(1);

		copy_stats(&curr, &prev);

		/* The stats will be updated by ganesha in the shm */
		copy_stats(shm_stats, &curr);
		do_diffs(&curr, &prev, &diff, opcount);
		print_stats(&diff, opcount);

		j++;
	}
}

int main(int argc, char *argv[])
{
	int key = 2049, err, count;
	void *addr = NULL;
	size_t size = sizeof(global_stats);
	global_stats *st = NULL;
	int shmid = shmget(key, size, 0);

	if (shmid < 0) {
		err = errno;
		fprintf(stderr, "***shmget failed, key %d, errno %d\n", key,
				err);
		return err;
	}

	/* attach it now. */
	addr = shmat(shmid, 0, 0);
	if (addr == (void *) -1) {
		err = errno;
		fprintf(stderr, "shmat: could not attach, errno %d\n", errno);
		return err;
	}

	count = sizeof(optabv4) / sizeof(struct op_name);
	st = (global_stats *)addr;

	dump_stats(st, count);
	shmdt(addr);
	return 0;
}
