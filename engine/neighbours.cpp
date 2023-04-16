#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/wait.h>

#include "record_structures.h"

using namespace std;
int d = 0;

index_entry retrive_index(int x){
	int INDEX_PER_FILE = BLOCK_SZ / sizeof(index_entry);
	index_entry indices[INDEX_PER_FILE];
	char s[30] = "indices/";
	sprintf(s + strlen(s), "%d.txt", x / INDEX_PER_FILE);
	int fd_2 = open(s, O_RDONLY);
	if(fd_2 == -1){
		perror("Error in creating index");
		exit(32);
	}
	++d;//cerr << "Disk Access " << ++d << '\n';
	for(int i = 0; i < INDEX_PER_FILE; ++i){
		read(fd_2, &indices[i], sizeof(index_entry));
	}
	close(fd_2);

	return indices[x%INDEX_PER_FILE];
}

int bytesread = 0;
inline void read_int(int & fd, int & x){
	if(bytesread % BLOCK_SZ == 0){
		close(fd);
		char s[30] = "adj_lists/";
		sprintf(s + strlen(s), "%d.txt", bytesread / BLOCK_SZ);
		fd = open(s, O_RDONLY);
		if(fd == -1){
			perror("Error in write_to_file");
			exit(16);
		}
		++d;//cerr << "Disk Access " << ++d << '\n';
	}
	read(fd, &x, sizeof(int));
	bytesread += sizeof(int);
}

void get_adj_list(int x, int fn, int fo, vector<int> & g){
	bytesread = fn * BLOCK_SZ + fo;
	char temp[BLOCK_SZ];
	char s[30] = "adj_lists/";
	sprintf(s + strlen(s), "%d.txt", fn);
	int fd = open(s, O_RDONLY);
	if(fd == -1){
		perror("Error in creating index");
		exit(32);
	}
	++d;//cerr << "Disk Access " << ++d << '\n';
	read(fd, temp, fo);
	int n, sz;
	read_int(fd, n);
	assert(x == n);
	read_int(fd, sz);
	g.resize(sz);
	// cerr << "{" << n << ", " << sz << "}\n";
	for(int i = 0; i < sz; ++i){
		read_int(fd, g[i]);
		// cerr << "\t" << g[i] << '\n';
	}

	close(fd);
}


int main(int argc, char ** argv){
	if(argc < 2){
		cerr << "run with <node_id_1> as arguments\n";
		return 1;
	}
	int N;
	ifstream input_file("metadata.txt");
	++d;// cerr << "Disk Access " << ++d << '\n';
	input_file >> N;
	input_file.close();


	int x = atoi(argv[1]);

	if((unsigned)x >= (unsigned)N){
		cerr << "node out of bound!\n";
		return 2;
	}

	vector<int> g;
	index_entry idx_x = retrive_index(x);
	get_adj_list(x, idx_x.file_num, idx_x.file_off, g);
	cout << "Outgoing neighbours of node " << x << " are : \n\t{";
	for(int i = 0; i < (int)g.size(); ++i){
		cout << g[i] << ((i + 1 != (int)g.size()) ? ", " : "");
		if(i % 10 == 9){ cout << "\n\t";}
	}
	cout << "}\n";

	cerr << "[" << d << " disk accesses]\n";

	return 0;
}
