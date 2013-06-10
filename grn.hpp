// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef GRN_HPP_
#define GRN_HPP_

#include <string>
#include <sstream>

#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/graphml.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

enum vertex_input_t {vertex_input};
enum graph_frequency_t {graph_frequency};

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, input);
    BOOST_INSTALL_PROPERTY(graph, frequency);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

typedef boost::property<vertex_input_t, double> VertexProperty;
typedef boost::property<boost::edge_weight_t, double> EdgeProperty;

typedef std::map<std::string, std::string> AttrMap;

typedef boost::property<boost::graph_graph_attribute_t, AttrMap,
        boost::property<boost::graph_vertex_attribute_t, AttrMap,
        boost::property<boost::graph_edge_attribute_t, AttrMap> > > GraphProperty;


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
//                       boost::no_property, EdgeProperty, GraphProperty> Graph;
                       VertexProperty, EdgeProperty, GraphProperty> Graph;

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

template <class Values>
class property_writer {
  public:
    property_writer(const Values& v, const std::string& key)
        : values_(v), key_(key) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& ost, const VertexOrEdge& voe) const {
        ost << "[" << key_ << "=" << boost::get(values_, voe) << "]";
    }
  private:
    const Values values_;
    const std::string key_;
};
template <class Values> inline
property_writer<Values>
make_property_writer(const Values& values, const std::string& key="label") {
    return property_writer<Values>(values, key);
}

inline std::string str_graphviz(Graph& graph) {
    std::ostringstream oss;
    oss << std::fixed;
    oss.precision(5);
    boost::write_graphviz(oss, graph,
//        boost::default_writer(),
        make_property_writer(boost::get(VertexProperty::tag_type(), graph), "input"),
        make_property_writer(boost::get(boost::edge_weight, graph), "weight"),
        boost::make_graph_attributes_writer(graph)
        );
    return oss.str();
}

template <class GraphType> inline
std::string str_graphml(GraphType& graph) {
    std::ostringstream oss;
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    dp.property("input", boost::get(GraphType::vertex_property_type::tag_type(), graph));
    dp.property("weight", boost::get(GraphType::edge_property_type::tag_type(), graph));
    boost::write_graphml(oss, graph, dp);
    return oss.str();
}

template <class Graph, class V> inline
void put_vertices_property(Graph* g, const V& values) {
    for (size_t i=0; i<values.size(); ++i) {
        boost::put(vertex_input, *g, boost::vertex(i, *g), values[i]);
    }
}

template <class GraphType, class Value> inline
void insert_graphattr(GraphType* graph, const std::string& key, const Value& value) {
    auto attrmap = boost::get_property(*graph, boost::graph_graph_attribute);
    attrmap[key] = std::to_string(value);
    boost::set_property(*graph, boost::graph_graph_attribute, attrmap);
}

std::string dot_grn(const std::vector<std::vector<double> >& adj_matrix,
                    const std::vector<double>& receptors,
                    const size_t frequency) {
    Graph graph = make_graph(adj_matrix);
    put_vertices_property(&graph, receptors);
    insert_graphattr(&graph, "frequency", frequency);
    return str_graphviz(graph);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GRN_HPP_
