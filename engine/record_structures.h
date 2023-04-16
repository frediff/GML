#ifndef __RECORD_STRUCTURES_INC
#define __RECORD_STRUCTURES_INC

#define BLOCK_SZ 4096
#define MAX_PG_STORE 30

struct node_record{
	int node_id;
	int in_deg;
	int out_deg;
	int scc_id;
	int scc_sz;
	int ranking;
	double pagerank;
};

struct index_entry{
	int file_num;
	int file_off;
};

#endif