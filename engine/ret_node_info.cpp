#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/wait.h>

#include "record_structures.h"

using namespace std;
int d = 0;

node_record retrive(int x){
	int PERFILENODES = BLOCK_SZ / sizeof(node_record);
	
	char s[30] = "node_records/";
	sprintf(s + strlen(s), "%d.txt", x / PERFILENODES);
	int fd = open(s, O_RDONLY);
	if(fd == -1){
		perror("Cannot access file");
		exit(2);
	}
	++d;//cerr << "Disk Access " << ++d << '\n';
	node_record NODE[PERFILENODES];
	for(int i = 0; i < PERFILENODES; ++i){
		read(fd, &NODE[i], sizeof(node_record));
	}

	return NODE[x%PERFILENODES];
}


int main(int argc, char ** argv){
	if(argc < 2){
		cerr << "run with <node_id> <service> as arguments\n";
		return 1;
	}
	int x = atoi(argv[1]);
	node_record nr = retrive(x);
	cout << "node id: " << nr.node_id << '\n';
	if(argc == 2 || strcmp(argv[2], "in-degree") == 0){
		cout << "in degree: " << nr.in_deg << '\n';
	}
	if(argc == 2 || strcmp(argv[2], "out-degree") == 0){
		cout << "out degree: " << nr.out_deg << '\n';
	}
	if(argc == 2 || strcmp(argv[2], "scc-id") == 0){
		cout << "scc id: " << nr.scc_id << '\n';
	}
	if(argc == 2 || strcmp(argv[2], "scc-sz") == 0){	
		cout << "scc size: " << nr.scc_sz << '\n';
	}
	if(argc == 2 || strcmp(argv[2], "pagerank") == 0){	
		cout << "pagerank: " << nr.pagerank << '\n';
	}
	if(argc == 2 || strcmp(argv[2], "ranking") == 0){	
		cout << "ranking: " << nr.ranking << '\n';
	}

	cerr << "[" << d << " disk accesses]\n";

	return 0;
}