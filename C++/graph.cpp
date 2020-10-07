#include <algorithm>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

//////////////////
// CONSTRUCTORS //
//////////////////

// Const Iterator Consrtuctor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator begin,
                         typename std::vector<N>::const_iterator end) noexcept {
  for (auto it = begin; it != end; ++it) {
    // std::cout << "Insert: " << *it << "\n";
    InsertNode(*it);
  }
}

// Tuple initialisation <src, dst, weight>
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end) noexcept {
  for (auto it = begin; it != end; ++it) {
    // Check if src node already in graph
    if (!this->IsNode(std::get<0>(*it))) {
      this->InsertNode(std::get<0>(*it));
    }
    // Check if dst node already in graph
    if (!this->IsNode(std::get<1>(*it))) {
      this->InsertNode(std::get<1>(*it));
    }
    this->InsertEdge(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it));
  }
}

// List Constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::initializer_list<N> lst) noexcept {
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    // std::cout << "Insert: " << *it << "\n";
    InsertNode(*it);
  }
}

// Copy Constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const typename gdwg::Graph<N, E>& g) noexcept {
  for (auto node = g.nodes_.cbegin(); node != g.nodes_.cend(); node++) {
    this->InsertNode(*node->first);
  }
  for (auto it = g.cbegin(); it != g.cend(); it++) {
    this->InsertEdge(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it));
  }
}

// Move Constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename gdwg::Graph<N, E>&& g) noexcept : nodes_{std::move(g.nodes_)} {}

////////////////
// OPERATIONS //
////////////////

// Copy Assignment
template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(const gdwg::Graph<N, E>& g) noexcept {
  *this = gdwg::Graph<N, E>(g);
  return *this;
}

// Move Assignment
template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(gdwg::Graph<N, E>&& g) noexcept {
  this->nodes_ = std::move(g.nodes_);
  return *this;
}

/////////////
// METHODS //
/////////////

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) noexcept {
  if (!this->IsNode(val)) {
    auto node = std::make_shared<N>(val);
    this->nodes_[node] = std::make_shared<Node>(node);
    return true;
  } else {
    return false;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!this->IsNode(src) || !this->IsNode(dst)) {
    throw std::runtime_error{
        "Cannot call Graph::InsertEdge when either src or dst node does not exist"};
  }

  auto src_shared = std::make_shared<N>(src);
  auto dst_shared = std::make_shared<N>(dst);

  std::weak_ptr<N> d = nodes_.find(dst_shared)->first;

  // Check if edges already exists. Return false
  for (auto e : nodes_.find(src_shared)->second->edges_) {
    if (*e.first.lock() == dst && e.second == w) {
      return false;
    }
  }

  // Add outgoing edge from src node
  nodes_.find(src_shared)->second->edges_.push_back(std::make_pair(d, w));
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& val) noexcept {
  if (!this->IsNode(val)) {
    return false;
  }

  try {
    this->nodes_.erase(std::make_shared<N>(val));
  } catch (...) {
    return false;
  }

  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw std::runtime_error{"Cannot call Graph::Replace on a node that doesn't exist"};
  }
  if (IsNode(newData)) {
    return false;
  }
  auto oldDataPtr = std::make_shared<N>(oldData);
  *(this->nodes_.find(oldDataPtr)->second->value_) = newData;

  return true;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error{
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph"};
  }
  // push back edges from oldData->edges to newData->edges
  auto oldDataPtr = std::make_shared<N>(oldData);
  auto newDataPtr = std::make_shared<N>(newData);
  std::shared_ptr<Node> oldNode = nodes_.find(oldDataPtr)->second;
  std::shared_ptr<Node> newNode = nodes_.find(newDataPtr)->second;

  // std::shared_ptr<N> oldNodeVal = oldNode->second->value;
  std::shared_ptr<N> newNodeVal = newNode->value_;

  // copy over edges to newNode
  newNode->edges_.sort(Edge::edgeComparator);
  oldNode->edges_.sort(Edge::edgeComparator);
  newNode->edges_.merge(oldNode->edges_, Edge::edgeComparator);

  for (auto n : this->nodes_) {
    auto edges = n.second->edges_;
    for (auto e = edges.begin(); e != edges.end();) {
      // clean up edges containing deleted nodes
      if ((*e).first.expired()) {
        e = edges.erase(e);
      } else {
        if (*e->first.lock() == oldData) {
          E weight = (*e).second;
          // erase old edge
          e = edges.erase(e);
          InsertEdge(*n.first, newData, weight);
        } else {
          e++;
        }
      }
    }
    // Remove duplicate edges
    n.second->edges_.sort(Edge::edgeComparator);
    n.second->edges_.unique(Edge::edgeEquals);
  }

  DeleteNode(oldData);
}

template <typename N, typename E>
void gdwg::Graph<N, E>::Clear() noexcept {
  nodes_.clear();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& val) noexcept {
  if (this->nodes_.count(std::make_shared<N>(val)) > 0) {
    return true;
  } else {
    return false;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error{
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph"};
  }
  auto edges = nodes_.find(std::make_shared<N>(src))->second->edges_;
  for (auto e = edges.begin(); e != edges.end();) {
    // clean up edges containing deleted nodes
    if ((*e).first.expired()) {
      e = edges.erase(e);
    } else if (*e->first.lock() == dst) {
      return true;
    } else {
      e++;
    }
  }
  return false;
}

// //getter
template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() noexcept {
  std::vector<N> vec;
  for (auto it = nodes_.cbegin(); it != nodes_.cend(); ++it) {
    vec.push_back(*it->first);
    // std::cout<<"getting: " << *it->first << "\n";
  }

  std::sort(vec.begin(), vec.end());
  return vec;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) {
  if (!IsNode(src)) {
    throw std::out_of_range{"Cannot call Graph::GetConnected if src doesn't exist in the graph"};
  }
  std::vector<N> vec;
  auto edges = this->nodes_.find(std::make_shared<N>(src))->second->edges_;

  for (auto e = edges.cbegin(); e != edges.cend(); ++e) {
    if (std::count(vec.begin(), vec.end(), *e->first.lock()) < 1) {
      vec.push_back(*e->first.lock());
    }
  }
  std::sort(vec.begin(), vec.end());
  std::unique(vec.begin(), vec.end());
  return vec;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range{
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph"};
  }
  std::vector<E> vec;
  auto edges = this->nodes_.find(std::make_shared<N>(src))->second->edges_;

  for (auto e = edges.cbegin(); e != edges.cend();) {
    if ((*e).first.expired()) {
      e = edges.erase(e);
    } else if (*e->first.lock() == dst) {
      vec.push_back(e->second);
      e++;
    } else {
      e++;
    }
  }
  if (vec.size() != 0) {
    std::sort(vec.begin(), vec.end());
  }

  return vec;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& src, const N& dst, const E& w) noexcept {
  if (IsNode(src) && IsNode(dst)) {
    auto it = cbegin();
    while (it != cend()) {
      if (*(*it.curr_node_).first == src && (*it.edge_it_).second == w) {
        if (!(*it.edge_it_).first.expired()) {
          if (*(*it.edge_it_).first.lock() == dst) {
            return it;
          }
        } else {
          return cend();
        }
      }
      it++;
    }
  }
  return cend();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) noexcept {
  if (!IsNode(src) || !IsNode(dst)) {
    return false;
  }
  auto src_node = nodes_.find(std::make_shared<N>(src));
  for (auto e = src_node->second->edges_.begin(); e != src_node->second->edges_.end();) {
    if ((*e).first.expired()) {
      e = src_node->second->edges_.erase(e);
    } else if (*e->first.lock() == dst && e->second == w) {
      e = src_node->second->edges_.erase(e);
      return true;
    } else {
      e++;
    }
  }
  return false;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::erase(const_iterator it) noexcept {
  if (it == cend()) {
    return cend();
  }
  // auto curr_edge = it.edge_it_;
  // curr_edge++;
  it.edge_it_ = it.curr_node_->second->edges_.erase(it.edge_it_);
  // if at last edge of curr_node_
  if (it.edge_it_ == it.curr_node_->second->edges_.cend()) {
    // it.edge_it_ = it.curr_node_->second->edges.erase(it.edge_it_);
    it.edge_it_--;
    it++;
  }

  return it;
}

///////////////
// ITERATORS //
///////////////

template <typename N, typename E>
typename gdwg::Graph<N, E>::Iterator& gdwg::Graph<N, E>::Iterator::operator++() {
  ++edge_it_;
  // If at last edge of node
  if (edge_it_ == curr_node_->second->edges_.cend()) {
    while (curr_node_ != it_end_) {
      ++curr_node_;
      if (curr_node_ != it_end_) {
        // Check if curr_node_ has any edges
        if (curr_node_->second->edges_.cbegin() == curr_node_->second->edges_.cend()) {
          // skip node
          continue;
        } else {
          break;
        }
      } else {  // We are at end of map so don't bother checking if any edges
        break;
      }
    }
    if (curr_node_ != it_end_) {
      curr_node_->second->edges_.sort(Edge::edgeComparator);
      edge_it_ = curr_node_->second->edges_.cbegin();
    } else {
      return *this;
    }
  }
  // Clean up edge if it contains node that has been deleted
  if ((*edge_it_).first.expired()) {
    edge_it_ = curr_node_->second->edges_.erase(edge_it_);
    edge_it_--;
    *this = ++(*this);
  }
  return *this;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::Iterator gdwg::Graph<N, E>::Iterator::operator++(int) {
  auto copy{*this};
  ++(*this);
  return copy;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::Iterator& gdwg::Graph<N, E>::Iterator::operator--() {
  if (curr_node_ == it_end_) {
    --curr_node_;
    while (curr_node_->second->edges_.cbegin() == curr_node_->second->edges_.cend()) {
      --curr_node_;
    }
    // std::cout << "OK\n";
    edge_it_ = curr_node_->second->edges_.cend();
  }

  // If at last edge of node
  if (edge_it_ == curr_node_->second->edges_.cbegin()) {
    --curr_node_;
    // Check if curr_node_ has any edges
    while (curr_node_->second->edges_.cbegin() == curr_node_->second->edges_.end()) {
      --curr_node_;
    }
    curr_node_->second->edges_.sort(Edge::edgeComparator);
    edge_it_ = curr_node_->second->edges_.cend();
  }
  --edge_it_;
  // Clean up edge if it contains node that has been deleted
  if ((*edge_it_).first.expired()) {
    edge_it_ = curr_node_->second->edges_.erase(edge_it_);
    // edge_it_++;
    *this = --(*this);
  }
  return *this;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::Iterator gdwg::Graph<N, E>::Iterator::operator--(int) {
  auto copy{*this};
  --(*this);
  return copy;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {
  auto begin = nodes_.cbegin();
  auto last = nodes_.cend();
  while (begin != last && begin->second->edges_.cbegin() == begin->second->edges_.cend()) {
    begin++;
  }
  if (begin == last) {
    return cend();
  }
  begin->second->edges_.sort(Edge::edgeComparator);
  auto edges = begin->second->edges_.cbegin();
  const_iterator it{last, begin, edges};
  // make sure edge has not expired
  while ((*it.edge_it_).first.expired()) {
    // std::cout << "FIRST EDGE IT EXPIRED\n";
    it++;
  }
  if (begin != last) {
    return it;
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {
  return const_iterator{nodes_.cend(), nodes_.cend(), {}};
}
