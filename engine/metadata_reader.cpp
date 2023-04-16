#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/wait.h>

#include "record_structures.h"

using namespace std;

int N, M, mx_out, mx_in, mx_tot, scc_count, large_scc;
int mo_sz, mi_sz, mt_sz, large_scc_sz;
int P, diam;

int main(){
	ifstream input_file("metadata.txt");
	input_file >> N >> M >> mx_out >> mo_sz >> mx_in >> mi_sz >> mx_tot >> mt_sz >> scc_count >> large_scc >> large_scc_sz;
	input_file >> diam >> P;
	vector<pair<double, int>> V_pg(P);
	for(int i = 0; i < P; ++i){
		int x;
		double y;
		input_file >> y >> x;
		V_pg[i] = {y, x};
	}
	input_file.close();

	cout << "number of nodes          : " << N << '\n';
	cout << "number of edges          : " << M << '\n';
	cout << "node with most out degree: " << mx_out << " [out degree: " << mo_sz << "]\n";
	cout << "node with most in degree : " << mx_in << " [in degree: " << mi_sz << "]\n";
	cout << "node with most degere    : " << mx_tot << " [total degree: " << mt_sz << "]\n";
	cout << "number of SCC's          : " << scc_count << '\n';
	cout << "id of largest SCC        : " << large_scc << " [size: " << large_scc_sz << "]\n";

	if(diam >= 0){
		cout << "diameter                 : " << diam << '\n';
	}
	cout << "Top " << P << " nodes ranked with pagerank are: \n";
	int j = 0;
	for(pair<double, int> pp : V_pg){
		cout << "\t" << ++j << ". node " << pp.second << "\t[pagerank = " << pp.first << "]\n";  
	}
	cerr << "[" << 1 << " disk accesses]\n";

	return 0;
}