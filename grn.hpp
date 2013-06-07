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

//typedef boost::property<boost::vertex_priority_t, double> VertexProperty;
typedef boost::property<boost::vertex_potential_t, double> VertexProperty;
typedef boost::property<boost::edge_weight_t, double> EdgeProperty;
typedef boost::property<boost::graph_name_t, double> GraphProperty;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                       VertexProperty, EdgeProperty, GraphProperty> Graph;
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

template <class Graph>
class weight_label_writer {
  public:
    weight_label_writer(const Graph& g) : graph_(g) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& ost, const VertexOrEdge& edge) const {
        write(ost, boost::get(boost::edge_weight, graph_, edge));
    }

  private:
    template <class Edge, class Weight>
    void write(std::ostream& ost, const Weight& weight) const {
        ost << "[weight=" << weight << "]";
    }

    const Graph& graph_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GRN_HPP_
