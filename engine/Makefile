

all:
	-mkdir -p adj_lists
	-mkdir -p node_records
	-mkdir -p indices
	g++ -Wall -Wextra -pg metadata_reader.cpp -o rd
	g++ -Wall -Wextra -pg load_graph.cpp -o ld
	g++ -Wall -Wextra -pg ret_node_info.cpp -o ret
	g++ -Wall -Wextra -pg query_scc.cpp -o q_scc
	g++ -Wall -Wextra -pg shortest_path.cpp -o s_path
	g++ -Wall -Wextra -pg neighbours.cpp -o neigh

	
