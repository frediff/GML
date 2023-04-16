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
	cerr << "Disk Access " << ++d << '\n';
	node_record NODE[PERFILENODES];
	for(int i = 0; i < PERFILENODES; ++i){
		read(fd, &NODE[i], sizeof(node_record));
	}

	return NODE[x%PERFILENODES];
}


int main(int argc, char ** argv){
	if(argc < 3){
		cerr << "run with <node_id_1> <node_id_2> as arguments\n";
		return 1;
	}
	int x = atoi(argv[1]);
	int y = atoi(argv[2]);
	node_record nrx = retrive(x);
	node_record nry = retrive(y);
	cout << "nodes with id " << nrx.node_id << " and " << nry.node_id << " are in ";

	if(nrx.scc_id != nry.scc_id){
		cout << "different SCC's.\n";
	}else{
		cout << "the same SCC.\n";
	}

	return 0;
}