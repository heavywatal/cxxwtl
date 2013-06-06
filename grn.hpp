// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef GRN_HPP_
#define GRN_HPP_

#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/graphml.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

typedef boost::property<boost::vertex_name_t, double> VertexProperty;
typedef boost::property<boost::edge_weight_t, double> EdgeProperty;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                       VertexProperty, EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

Graph make_graph(const std::vector<std::vector<double> >& adj_matrix) {
    const size_t order = adj_matrix.size();
    std::vector<std::pair<size_t, size_t> > adj_list;
    std::vector<double> weights;
    for (size_t i=0; i<order; ++i) {
        const auto& row = adj_matrix[i];
        for (size_t j=0; j<order; ++j) {
            if (row[j] != 0.0) {
                adj_list.push_back({j, i});
                weights.push_back(row[j]);
            }
        }
    }
    Graph graph(adj_list.begin(), adj_list.end(), weights.begin(), order);
    return graph;
}


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GRN_HPP_
