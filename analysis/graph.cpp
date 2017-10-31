//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/tokenizer.hpp>

using namespace boost;

int main() {
    std::ifstream datafile("./kevin-bacon.dat");
    if (!datafile) {
        std::cerr << "No ./kevin-bacon.dat file" << std::endl;
        return EXIT_FAILURE;
    }

    using Graph = adjacency_list<vecS, vecS, directedS,
                                 property<vertex_name_t, std::string>>;
    using Vertex = graph_traits<Graph>::vertex_descriptor;
    using Edge = graph_traits<Graph>::edge_descriptor;
    using PropertyMap = property_map<Graph, vertex_name_t>::type;
    using NameVertexMap = std::map<std::string, Vertex>;

    Graph g;
    NameVertexMap functions;

    for (std::string line; std::getline(datafile, line);) {
        tokenizer<escaped_list_separator<char>> token(line);
        for (auto iter = token.begin(); iter != token.end(); iter++) {
            NameVertexMap::iterator pos;
            if (functions.find(*iter) == functions.end()) {
                Vertex v = add_vertex(*iter, g);
                functions[*iter] = v;
            }
        }
    }

    datafile.clear();
    datafile.seekg(0);

    for (std::string line; std::getline(datafile, line);) {
        tokenizer<escaped_list_separator<char>> token(line);
        auto iter = token.begin();
        Vertex u = functions[*iter++];
        for (; iter != token.end(); iter++) {
            Vertex v = functions[*iter];
            add_edge(u, v, g);
        }
    }

    std::vector<Vertex> predecessor(
        num_vertices(g),
        graph_traits<Graph>::null_vertex());  // the predecessor array

    breadth_first_search(
        g, functions["__might_sleep"],
        visitor(make_bfs_visitor(record_predecessors(
            iterator_property_map<
                std::vector<Vertex>::iterator,
                property_map<Graph, vertex_index_t>::const_type>(
                predecessor.begin(), get(vertex_index, g)),
            on_tree_edge()))));
    return 0;
}
