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
	if(argc < 3){
		cerr << "run with <node_id_1> <node_id_2> as arguments\n";
		return 1;
	}
	int N;
	ifstream input_file("metadata.txt");
	++d;//cerr << "Disk Access " << ++d << '\n';
	input_file >> N;
	input_file.close();


	int x = atoi(argv[1]);
	int y = atoi(argv[2]);

	if((unsigned)x >= (unsigned)N || (unsigned)y >= (unsigned)N){
		cerr << "nodes out of bound!\n";
		return 2;
	}

	int dist[N];
	int par[N];
	par[x] = x;
	queue<int> Q;
	vector<bool> vis(N, false);

	dist[x] = 0;
	dist[y] = -1;
	Q.push(x);
	vis[x] = true;
	vector<int> g;
	while(!Q.empty()){
		int a = Q.front();
		Q.pop();

		index_entry idx_a = retrive_index(a);
		get_adj_list(a, idx_a.file_num, idx_a.file_off, g);
		for(int b : g){
			if(!vis[b]){
				dist[b] = dist[a] + 1;
				par[b] = a;
				if(b == y){
					cout << "Distance from node " << x << " to node " << y << " is " << dist[y] << '\n';
					if(argc > 3){
						stack<int> path;
						do{
							path.push(y);
							y = par[y];
						}while(y != x);
						path.push(x);
						cout << "The shortest path is: \n\t{";
						int cntr = 0;
						while(!path.empty()){
						    ++cntr;
							cout << path.top();
							path.pop();
							cout << (!path.empty() ? ", " : "");	
							if(cntr%10 == 9) cout << "\n\t" ;
						}
						cout << "}\n";
					}
					cerr << "[" << d << " disk accesses]\n";
					exit(0);
				}
				vis[b] = true;
				Q.push(b);
			}
		}
	}
	cout << "Node " << y << " cannot be reached from node " << x << '\n';
	cerr << "[" << d << " disk accesses]\n";


	return 0;
}
