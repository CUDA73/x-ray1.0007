//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================

#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;

// Predicate Function for use in remove if 
template <class NamePropertyMap>
struct name_equals_predicate
{
  name_equals_predicate(const std::string& x, NamePropertyMap name)
    : m_str(x), m_name(name) { }

  template <class Edge>
  bool operator()(const Edge& e) const {
    return m_str == m_name[e];
  }
  std::string m_str;
  NamePropertyMap m_name;
};
// helper creation function
template <class NamePropertyMap>
inline name_equals_predicate<NamePropertyMap>
name_equals(const std::string& str, NamePropertyMap name) {
  return name_equals_predicate<NamePropertyMap>(str, name);
}

template <class MutableGraph>
void modify_demo(MutableGraph& g)
{
  typedef graph_traits<MutableGraph> GraphTraits;
  typedef typename GraphTraits::vertices_size_type size_type;
  typedef typename GraphTraits::edge_descriptor edge_descriptor;
  size_type n = 0;
  typename GraphTraits::edges_size_type m = 0;
  typename GraphTraits::vertex_descriptor u, v, w;
  edge_descriptor e, e1, e2;
  typename property_map<MutableGraph, edge_name_t>::type
    name_map = get(edge_name, g);
  bool added;
  typename GraphTraits::vertex_iterator vi, vi_end;

  {
    v = add_vertex(g);

    assert(num_vertices(g) == n + 1);
    assert(size_type(vertices(g).second - vertices(g).first) == n + 1);
    assert(v == *std::find(vertices(g).first, vertices(g).second, v));
  }
  {
    remove_vertex(v, g);
    
    assert(num_vertices(g) == n);
    assert(size_type(vertices(g).second - vertices(g).first) == n);
    // v is no longer a valid vertex descriptor
  }
  {
    u = add_vertex(g);
    v = add_vertex(g);

    std::pair<edge_descriptor, bool> p = add_edge(u, v, g);
  
    assert(num_edges(g) == m + 1);
    assert(p.second == true); // edge should have been added
    assert(source(p.first, g) == u);
    assert(target(p.first, g) == v);
    assert(p.first == *std::find(out_edges(u, g).first, 
                                 out_edges(u, g).second, p.first));
    assert(p.first == *std::find(in_edges(v, g).first, 
                                 in_edges(v, g).second, p.first));
  }
  {
    // use tie() for convenience, avoid using the std::pair
    
    u = add_vertex(g);
    v = add_vertex(g);
    
    tie(e, added) = add_edge(u, v, g);

    assert(num_edges(g) == m + 2);
    assert(added == true); // edge should have been added
    assert(source(e, g) == u);
    assert(target(e, g) == v);
    assert(e == *std::find(out_edges(u, g).first, out_edges(u, g).second, e));
    assert(e == *std::find(in_edges(v, g).first, in_edges(v, g).second, e));
  }
  {
    add_edge(u, v, g); // add a parallel edge

    remove_edge(u, v, g);

    assert(num_edges(g) == m + 1);
    bool exists;
    tie(e, exists) = edge(u, v, g);
    assert(exists == false);
    assert(out_degree(u, g) == 0);
    assert(in_degree(v, g) == 0);
  }
  {
    e = *edges(g).first;
    tie(u, v) = incident(e, g);

    remove_edge(e, g);

    assert(num_edges(g) == m);
    assert(out_degree(u, g) == 0);
    assert(in_degree(v, g) == 0);
  }
  {
    add_edge(u, v, g);

    typename GraphTraits::out_edge_iterator iter, iter_end;
    tie(iter, iter_end) = out_edges(u, g);

    remove_edge(iter, g);
    
    assert(num_edges(g) == m);
    assert(out_degree(u, g) == 0);
    assert(in_degree(v, g) == 0);
  }
  {
    w = add_vertex(g);
    tie(e1, added) = add_edge(u, v, g);
    tie(e2, added) = add_edge(v, w, g);
    name_map[e1] = "I-5";
    name_map[e2] = "Route 66";
    
    typename GraphTraits::out_edge_iterator iter, iter_end;
    tie(iter, iter_end) = out_edges(u, g);

    remove_edge_if(name_equals("Route 66", name_map), g);
    
    assert(num_edges(g) == m + 1);

    remove_edge_if(name_equals("I-5", name_map), g);
    
    assert(num_edges(g) == m);
    assert(out_degree(u, g) == 0);
    assert(out_degree(v, g) == 0);
    assert(in_degree(v, g) == 0);
    assert(in_degree(w, g) == 0);
  }
  {
    tie(e1, added) = add_edge(u, v, g);
    tie(e2, added) = add_edge(u, w, g);
    name_map[e1] = "foo";
    name_map[e2] = "foo";
    
    remove_out_edge_if(u, name_equals("foo", name_map), g);
    
    assert(num_edges(g) == m);
    assert(out_degree(u, g) == 0);
  }
  {
    tie(e1, added) = add_edge(u, v, g);
    tie(e2, added) = add_edge(w, v, g);
    name_map[e1] = "bar";
    name_map[e2] = "bar";
    
    remove_in_edge_if(v, name_equals("bar", name_map), g);
    
    assert(num_edges(g) == m);
    assert(in_degree(v, g) == 0);
  }
  {
    add_edge(u, v, g);
    add_edge(u, w, g);
    add_edge(u, v, g);
    add_edge(v, u, g);

    clear_vertex(u, g);
    
    assert(out_degree(u, g) == 0);
    
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
      typename GraphTraits::adjacency_iterator ai, ai_end;
      for (tie(ai, ai_end) = adjacent_vertices(*vi, g);
           ai != ai_end; ++ai)
        assert(*ai != u);
    }
  }
}

int
main()
{
  adjacency_list<listS, vecS, bidirectionalS,
    no_property, property<edge_name_t, std::string> > g;

  modify_demo(g);
  return 0;
}
