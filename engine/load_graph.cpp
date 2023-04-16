#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/wait.h>

#include "record_structures.h"

using namespace std;

#define PG_ITER 30

vector<double> pagerank[2];
vector<vector<int>> g;
vector<vector<int>> g_rev;
int N = 0, M = 0;
int d = 0;

vector<vector<int>> SCC;
vector<int> scc_id, xt;
int CLK, scc_count;
vector<bool> vis;

node_record * NODES;

void dfs_init(int a){
	vis[a] = true;
	for(int b : g[a]) if(!vis[b]) dfs_init(b);
	xt[a] = CLK++;
}
void dfs_scc(int a){
	vis[a] = true;
	scc_id[a] = scc_count;
	SCC[scc_count].push_back(a);
	for(int b : g_rev[a]) if(!vis[b]) dfs_scc(b);
}
void Kosaraju(){
	SCC.resize(N);
	scc_id.resize(N);
	xt.resize(N);
	vis.resize(N);
	vector<int> V(N);
	for(int i = 1; i <= N; ++i){V[i-1] = i-1;	vis[i-1] = false;	SCC[i-1].clear();}
	CLK = scc_count = 0;
	for(int i = 1; i <= N; ++i)	if(!vis[i-1])	dfs_init(i-1);
	sort(V.begin(), V.end(), [&](int a, int b){return xt[a] > xt[b];});
	for(int i = 1; i <= N; ++i)	vis[i-1] = false;
	for(int a : V)	if(!vis[a])	{dfs_scc(a); ++scc_count;}
}
void dbg(){
	for(int i = 0; i < scc_count; ++i){
		cerr<<"SCC["<<i<<"] : ";
		for(int b : SCC[i]) cerr<<b<<' ';
		cerr<<'\n';
	}
}

void PageRank(){
	for(int pg_iter = 1; pg_iter <= PG_ITER; ++pg_iter){
		for(int i = 0; i < N; ++i){
			pagerank[pg_iter & 1][i] = 0;
			for(int x : g_rev[i]){
				pagerank[pg_iter & 1][i] += pagerank[(pg_iter & 1) ^ 1][x] / (double)g[x].size();
			}
		}
	}
}

int bytes_written = 0;
inline void write_to_file(int x, int & fd){
	if(bytes_written % BLOCK_SZ == 0){
		if(bytes_written){
			close(fd);
		}
		char s[30] = "adj_lists/";
		sprintf(s + strlen(s), "%d.txt", bytes_written / BLOCK_SZ);
		fd = open(s, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if(fd == -1){
			perror("Error in write_to_file");
			exit(16);
		}
		++d;//cerr << "Disk Access " << ++d << '\n';
	}
	write(fd, &x, sizeof(int));
	bytes_written += sizeof(int);
}


void load_adj(){
	bytes_written = 0;
	int fd;
	int INDEX_PER_FILE = BLOCK_SZ / sizeof(index_entry);
	index_entry indices[N];
	for(int i = 0; i < N; ++i){
		indices[i].file_num = bytes_written / BLOCK_SZ;
		indices[i].file_off = bytes_written % BLOCK_SZ;
		write_to_file(i, fd);
		write_to_file(g[i].size(), fd);
		for(int x : g[i]){
			write_to_file(x, fd);
		}
	}
	close(fd);

	int i = 0;
	while(i < N){
		char s[30] = "indices/";
		sprintf(s + strlen(s), "%d.txt", i / INDEX_PER_FILE);
		int fd_2 = open(s, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if(fd_2 == -1){
			perror("Error in creating index");
			exit(32);
		}
		++d;//cerr << "Disk Access " << ++d << '\n';
		do{
			write(fd_2, &indices[i], sizeof(index_entry));
			++i;
		}while((i % INDEX_PER_FILE) && i < N);
		close(fd_2);
	}

}

int main(int argc, char ** argv){
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	cerr.tie(NULL);
	// filename not provided
	if(argc < 2){
		cout << "Please provide file name as command line argument!\n";
		return 1;
	}
	if(fork() == 0){
		if(fork() == 0){
			execlp("chmod", "chmod", "+x", "clear.sh", NULL);
		}
		wait(NULL);
		execlp("./clear.sh", "./clear.sh", NULL);
	}
	wait(NULL);
	
	ifstream input_file(argv[1]);
	input_file.tie(NULL);
	string temp_str;
	while(getline(input_file, temp_str)){
		if(temp_str[0] != '#'){
			int x, y;
			sscanf(temp_str.c_str(), "%d%d", &x, &y);
			if(N < max(x, y) + 1){
				N = max(x, y) + 1;
				g.resize(N);
				g_rev.resize(N);
			}
			g[x].push_back(y);
			g_rev[y].push_back(x);
			++M;
			// if(M % 100000 == 0){
			// 	cout << M << ": " << x << " -> " << y << '\n';
			// }
		}
	}
	input_file.close();

	cout << "N = " << N << "\nM = " << M << endl;

	load_adj();

	ofstream outfile;
	outfile.open("metadata.txt");
	++d;//cerr << "Disk Access " << ++d << '\n';
	outfile << N << '\n' << M << '\n';

	// find max out/in/tot degree node
	int mx_out = 0, mx_in = 0, mx_tot = 0;
	for(int i = 1; i < N; ++i){
		int to = g[mx_out].size();
		int ti = g_rev[mx_in].size();
		int tt = g[mx_tot].size() + g_rev[mx_tot].size();
		int co = g[i].size();
		int ci = g_rev[i].size();
		if(to < co){
			mx_out = i;
		}
		if(ti < ci){
			mx_in = i;
		}
		if(tt < co + ci){
			mx_tot = i;
		}
	}
	outfile << mx_out << ' ' << g[mx_out].size() << '\n';
	outfile << mx_in << ' ' << g_rev[mx_in].size() << '\n';
	outfile << mx_tot << ' ' << g[mx_tot].size() + g_rev[mx_tot].size() << '\n';

	// call SCC commands
	Kosaraju();

	pagerank[0].resize(N, 10.0 * N);
	pagerank[1].resize(N, 10.0 * N);
	PageRank();

	// fill in the node attributes
	vector<pair<double, int>> V_pg(N);
	NODES = (node_record *) malloc(sizeof(node_record) * N);
	for(int i = 0; i < N; ++i){
		NODES[i].node_id = i;
		NODES[i].in_deg = g_rev[i].size();
		NODES[i].out_deg = g[i].size();
		NODES[i].scc_id = scc_id[i];
		NODES[i].scc_sz = SCC[scc_id[i]].size();
		NODES[i].pagerank = pagerank[PG_ITER & 1][i];
		V_pg[i] = make_pair(pagerank[PG_ITER & 1][i], i);
	}
	sort(V_pg.begin(), V_pg.end(), greater<pair<double, int>>());
	for(int i = 0; i < N; ++i){
		NODES[V_pg[i].second].ranking = i + 1;
	}
	int PERFILENODES = BLOCK_SZ / sizeof(node_record);
	cerr << "Number of records per file = " << PERFILENODES << '\n';
	int i = 0;
	while(i < N){
		char s[30] = "node_records/";
		sprintf(s + strlen(s), "%d.txt", i / PERFILENODES);
		int fd = open(s, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if(fd == -1){
			perror("Error in writing to node records");
			exit(8);
		}
		++d;//cerr << "Disk Access " << ++d << '\n';
		do{
			write(fd, &NODES[i], sizeof(node_record));
			++i;
		}while((i % PERFILENODES) && i < N);
		close(fd);
	}

	free(NODES);

	outfile << scc_count << '\n';

	// find largest SCC
	int larg_scc = 0;
	for(int i = 1; i < scc_count; ++i){
		if(SCC[larg_scc].size() < SCC[i].size()){
			larg_scc = i;
		}
	}
	outfile << larg_scc << ' ' <<  SCC[larg_scc].size() << '\n';

	int diam = -1;
	if(argc > 2 && N * M <= 5000000){ // calculate diameter;s
		for(int i = 0; i < N; ++i){
			int dist[N];
			queue<int> Q;
			vector<bool> vis(N);
			vis[i] = true;
			dist[i] = 0;
			Q.push(i);
			while(!Q.empty()){
				int a = Q.front();
				Q.pop();
				for(int b : g[a]){
					if(!vis[b]){
						vis[b] = true;
						dist[b] = dist[a] + 1;
						if(diam < dist[b]){
							diam = dist[b];
						}
						Q.push(b);
					}
				}
			}
		}
	}
	outfile << diam << '\n';

	int P = min(N, MAX_PG_STORE);
	outfile << P << '\n';
	for(int i = 0; i < P; ++i){
		outfile << V_pg[i].first << ' ' << V_pg[i].second << '\n';
	}



	outfile.close();
	cerr << "[" << d << " disk accesses]\n";
	return 0;

}