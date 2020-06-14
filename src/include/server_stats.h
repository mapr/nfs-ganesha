/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright (C) Panasas Inc., 2013
 * Author: Jim Lieb jlieb@panasas.com
 *
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * -------------
 */

/**
 * @defgroup Server statistics management
 * @{
 */

/**
 * @file server_stats.h
 * @author Jim Lieb <jlieb@panasas.com>
 * @brief Server statistics
 */

#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include <sys/types.h>

#ifdef USE_DBUS

struct op_name {
	char *name;
};

/* latency stats
 */
struct op_latency {
	uint64_t latency;
	uint64_t min;
	uint64_t max;
};

/* v3 ops
 */
struct nfsv3_ops {
	uint64_t op[NFSPROC3_COMMIT+1];
};

/* quota ops
 */
struct qta_ops {
	uint64_t op[RQUOTAPROC_SETACTIVEQUOTA+1];
};

/* nlm ops
 */
struct nlm_ops {
	uint64_t op[NLMPROC4_FREE_ALL+1];
};

/* mount ops
 */
struct mnt_ops {
	uint64_t op[MOUNTPROC3_EXPORT+1];
};

/* v4 ops
 */
struct nfsv4_ops {
	uint64_t op[NFS4_OP_LAST_ONE];
};

/* basic op counter
 */

struct proto_op {
	uint64_t total;		/* total of any kind */
	uint64_t errors;	/* ! NFS_OK */
	uint64_t dups;		/* detected dup requests */
	struct op_latency latency;	/* either executed ops latency */
	struct op_latency dup_latency;	/* or latency (runtime) to replay */
	struct op_latency queue_latency;	/* queue wait time */
};

/* basic I/O transfer counter
 */
struct xfer_op {
	struct proto_op cmd;
	uint64_t requested;
	uint64_t transferred;
};

/* pNFS Layout counters
 */

struct layout_op {
	uint64_t total;		/* total ops */
	uint64_t errors;	/* ! NFS4_OK && !NFS4ERR_DELAY */
	uint64_t delays;	/* NFS4ERR_DELAY */
};

/* NFSv3 statistics counters
 */

struct nfsv3_stats {
	struct proto_op cmds;	/* non-I/O ops = cmds - (read+write) */
	struct xfer_op read;
	struct xfer_op write;
};

/* Mount statistics counters
 */
struct mnt_stats {
	struct proto_op v1_ops;
	struct proto_op v3_ops;
};

/* lock manager counters
 */

struct nlmv4_stats {
	struct proto_op ops;
};

/* Quota counters
 */

struct rquota_stats {
	struct proto_op ops;
	struct proto_op ext_ops;
};

/* NFSv4 statistics counters
 */

struct nfsv40_stats {
	struct proto_op compounds;
	uint64_t ops_per_compound;	/* avg = total / ops_per */
	struct xfer_op read;
	struct xfer_op write;
};

struct nfsv41_stats {
	struct proto_op compounds;
	uint64_t ops_per_compound;	/* for size averaging */
	struct xfer_op read;
	struct xfer_op write;
	struct layout_op getdevinfo;
	struct layout_op layout_get;
	struct layout_op layout_commit;
	struct layout_op layout_return;
	struct layout_op recall;
};

struct transport_stats {
	uint64_t rx_bytes;
	uint64_t rx_pkt;
	uint64_t rx_err;
	uint64_t tx_bytes;
	uint64_t tx_pkt;
	uint64_t tx_err;
};

#ifdef _USE_9P
struct _9p_stats {
	struct proto_op cmds;	/* non-I/O ops */
	struct xfer_op read;
	struct xfer_op write;
	struct transport_stats trans;
	struct proto_op *opcodes[_9P_RWSTAT+1];
};
#endif

struct global_stats {
	struct nfsv3_stats nfsv3;
	struct mnt_stats mnt;
	struct nlmv4_stats nlm4;
	struct rquota_stats rquota;
	struct nfsv40_stats nfsv40;
	struct nfsv41_stats nfsv41;
	struct nfsv41_stats nfsv42; /* Uses v41 stats */
	struct nfsv3_ops v3;
	struct nfsv4_ops v4;
	struct nlm_ops lm;
	struct mnt_ops mn;
	struct qta_ops qt;
};

struct deleg_stats {
	uint32_t curr_deleg_grants; /* current num of delegations owned by
				       this client */
	uint32_t tot_recalls;       /* total num of times client was asked to
				       recall */
	uint32_t failed_recalls;    /* times client failed to process recall */
	uint32_t num_revokes;	    /* Num revokes for the client */
};

extern int global_stats_shmid;
extern struct global_stats *global_st;

#endif

void server_stats_nfs_done(request_data_t *reqdata, int rc, bool dup);

#ifdef _USE_9P
void server_stats_9p_done(u8 msgtype, struct _9p_request_data *req9p);
#endif

void server_stats_io_done(size_t requested,
			  size_t transferred, bool success, bool is_write);
void server_stats_compound_done(int num_ops, int status);
void server_stats_nfsv4_op_done(int proto_op,
				nsecs_elapsed_t start_time, int status);
void server_stats_transport_done(struct gsh_client *client,
				uint64_t rx_bytes, uint64_t rx_pkt,
				uint64_t rx_err, uint64_t tx_bytes,
				uint64_t tx_pkt, uint64_t tx_err);

/* For delegations */
void inc_grants(struct gsh_client *client);
void dec_grants(struct gsh_client *client);
void inc_revokes(struct gsh_client *client);
void inc_recalls(struct gsh_client *client);
void inc_failed_recalls(struct gsh_client *client);

int global_stats_init(void);
void global_stats_free(void);

#endif				/* !SERVER_STATS_H */
/** @} */
