#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace gdwg {

template <typename N, typename E>
class Graph {
 private:
  struct Edge {
    static bool edgeComparator(const std::pair<std::weak_ptr<N>, E>& a,
                               const std::pair<std::weak_ptr<N>, E>& b) {
      if (a.first.expired() || b.first.expired()) {
        return true;
      }
      if (*a.first.lock() < *b.first.lock()) {
        return true;
      } else if (*a.first.lock() > *b.first.lock()) {
        return false;
      } else if (a.second < b.second) {
        return true;
      } else {
        return false;
      }
    }
    static bool edgeEquals(const std::pair<std::weak_ptr<N>, E>& a,
                           const std::pair<std::weak_ptr<N>, E>& b) {
      if (a.first.expired() || b.first.expired()) {
        return true;
      }
      if ((*a.first.lock() == *b.first.lock()) && (a.second == b.second)) {
        return true;
      } else {
        return false;
      }
    }
  };

  struct Node {
    explicit Node(std::shared_ptr<N> value) : value_(value) {}
    std::shared_ptr<N> value_;
    mutable std::list<std::pair<std::weak_ptr<N>, E>> edges_;
  };

  struct NodeCompare {
    bool operator()(const std::shared_ptr<N>& a, const std::shared_ptr<N>& b) const {
      return *a < *b;
    }
  };

 public:
  class Iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = std::tuple<N* const, N* const, E* const>;
    using difference_type = int;

    reference operator*() const {
      return {*curr_node_->first, *edge_it_->first.lock(), edge_it_->second};
    }

    Iterator& operator++();
    Iterator operator++(int);

    Iterator& operator--();
    Iterator operator--(int);

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      if (lhs.curr_node_ == rhs.curr_node_ &&
          (lhs.curr_node_ == lhs.it_end_ || rhs.curr_node_ == rhs.it_end_)) {
        return true;
      } else if (lhs.curr_node_ == rhs.curr_node_ && lhs.edge_it_ == rhs.edge_it_) {
        return true;
      }
      return false;
    }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return !(lhs == rhs); }

   private:
    typename std::map<std::shared_ptr<N>, std::shared_ptr<Node>, NodeCompare>::const_iterator
        it_end_;
    typename std::map<std::shared_ptr<N>, std::shared_ptr<Node>, NodeCompare>::const_iterator
        curr_node_;
    typename std::list<std::pair<std::weak_ptr<N>, E>>::const_iterator edge_it_;

    friend class Graph;
    explicit Iterator(const decltype(it_end_)& it_e,
                      const decltype(curr_node_)& curr_node,
                      const decltype(edge_it_)& edge_it)
      : it_end_{it_e}, curr_node_{curr_node}, edge_it_{edge_it} {}
  };

  using const_reverse_iterator = std::reverse_iterator<Iterator>;
  using const_iterator = Iterator;

  // CONSTRUCTORS
  Graph() = default;
  Graph(typename std::vector<N>::const_iterator begin,
        typename std::vector<N>::const_iterator end) noexcept;
  Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
        typename std::vector<std::tuple<N, N, E>>::const_iterator end) noexcept;
  Graph(typename std::initializer_list<N>) noexcept;
  explicit Graph(const typename gdwg::Graph<N, E>& g) noexcept;
  explicit Graph(typename gdwg::Graph<N, E>&& g) noexcept;
  ~Graph() = default;

  // OPERATIONS
  gdwg::Graph<N, E>& operator=(const gdwg::Graph<N, E>& g) noexcept;
  gdwg::Graph<N, E>& operator=(gdwg::Graph<N, E>&& g) noexcept;

  // METHODS
  bool InsertNode(const N& val) noexcept;
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& val) noexcept;
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear() noexcept;
  bool IsNode(const N& val) noexcept;
  bool IsConnected(const N& src, const N& dst);
  std::vector<N> GetNodes() noexcept;
  std::vector<N> GetConnected(const N& src);
  std::vector<E> GetWeights(const N& src, const N& dst);
  const_iterator find(const N& src, const N& dst, const E& w) noexcept;
  bool erase(const N& src, const N& dst, const E& w) noexcept;
  const_iterator erase(const_iterator it) noexcept;

  // ITERATORS
  const_iterator cbegin() const;
  const_iterator cend() const;

  const_iterator begin() { return cbegin(); }
  const_iterator end() { return cend(); }

  const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }

  const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }

  const_reverse_iterator rbegin() { return crbegin(); }
  const_reverse_iterator rend() { return crend(); }

  // FRIENDS
  friend bool operator==(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    std::stringstream os1;
    std::stringstream os2;
    os1 << g1;
    os2 << g2;
    return (os1.str() == os2.str());
  }

  friend bool operator!=(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    return !(g1 == g2);
  }

  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& g) {
    for (auto node : g.nodes_) {
      os << *node.first << " (\n";
      node.second->edges_.sort(Edge::edgeComparator);
      for (auto edge = node.second->edges_.cbegin(); edge != node.second->edges_.cend();) {
        // Clean up edge if it contains dst node that no longer exists
        if (edge->first.expired()) {
          edge = node.second->edges_.erase(edge);
        } else {
          os << "  " << *edge->first.lock() << " | " << edge->second << "\n";
          edge++;
        }
      }
      os << ")\n";
    }
    return os;
  }

  // Debugging
  void PrintGraph();

 private:
  std::map<std::shared_ptr<N>, std::shared_ptr<Node>, NodeCompare> nodes_;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
