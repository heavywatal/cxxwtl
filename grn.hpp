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

enum graph_frequency_t {graph_frequency};

namespace boost {
    BOOST_INSTALL_PROPERTY(graph, frequency);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

typedef boost::property<boost::vertex_index1_t, std::string,
        boost::property<boost::vertex_index2_t, std::string> > VertexProperty;
typedef boost::property<boost::edge_weight_t, double> EdgeProperty;

typedef std::map<std::string, std::string> AttrMap;

typedef boost::property<boost::graph_graph_attribute_t, AttrMap,
        boost::property<boost::graph_vertex_attribute_t, AttrMap,
        boost::property<boost::graph_edge_attribute_t, AttrMap> > > GraphProperty;


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
//                       boost::no_property, EdgeProperty, GraphProperty> Graph;
                       VertexProperty, EdgeProperty, GraphProperty> Graph;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

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

template <class GraphType, class Value> inline
void insert_graphattr(GraphType* graph, const std::string& key, const Value& value) {
    auto attrmap = boost::get_property(*graph, boost::graph_graph_attribute);
    attrmap[key] = std::to_string(value);
    boost::set_property(*graph, boost::graph_graph_attribute, attrmap);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class M1, class M2>
class double_writer {
  public:
    double_writer(const M1& m1, const std::string& key1, const M2& m2, const std::string& key2)
        : m1_(m1), key1_(key1), m2_(m2), key2_(key2) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& ost, const VertexOrEdge& voe) const {
        ost << "["
            << key1_ << "=" << boost::get(m1_, voe) << ","
            << key2_ << "=" << boost::get(m2_, voe) << "]";
    }
  private:
    const M1 m1_;
    const std::string key1_;
    const M2 m2_;
    const std::string key2_;
};
template <class M1, class M2> inline
double_writer<M1, M2>
make_double_writer(const M1& m1, const std::string& key1, 
                   const M2& m2, const std::string& key2) {
    return double_writer<M1, M2>(m1, key1, m2, key2);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

std::ostream& write_graphviz(std::ostream& ost,
                 const std::vector<std::vector<double> >& adj_matrix,
                 const size_t frequency=0,
                 const std::vector<std::string>& normal_input={},
                 const std::vector<std::string>& novel_input={})
{
    Graph graph = make_graph(adj_matrix);
    for (size_t i=0; i<normal_input.size(); ++i) {
        boost::put(boost::vertex_index1, graph, 
                   boost::vertex(i, graph), normal_input[i]);
    }
    for (size_t i=0; i<novel_input.size(); ++i) {
        boost::put(boost::vertex_index2, graph,
                   boost::vertex(i, graph), novel_input[i]);
    }
    insert_graphattr(&graph, "frequency", frequency);
    boost::write_graphviz(ost, graph,
        make_double_writer(boost::get(boost::vertex_index1, graph), "normal",
                           boost::get(boost::vertex_index2, graph), "novel"),
        make_property_writer(boost::get(boost::edge_weight, graph), "weight"),
        boost::make_graph_attributes_writer(graph));
    return ost;
}

template <class GraphType> inline
std::string str_graphml(GraphType& graph) {
    std::ostringstream oss;
    oss << std::fixed;
    oss.precision(5);
    boost::dynamic_properties dp;
 //   dp.property("node_id", boost::get(boost::vertex_index, graph));
    dp.property("input", boost::get(typename GraphType::vertex_property_type::tag_type(), graph));
    dp.property("weight", boost::get(typename GraphType::edge_property_type::tag_type(), graph));
    boost::write_graphml(oss, graph, dp, /*ordered_vertices=*/true);
    return oss.str();
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // GRN_HPP_
