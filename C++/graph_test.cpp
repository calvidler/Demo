/*

  == Explanation and rational of testing ==

  Initially, the default constructor is tested.  GetNodes() is assumed to be working
  correctly at this stage and returns an empty vector of the nodes contained in the graph.

  In the next scenario, the default constructor is used and nodes are inserted.
  IsNode() is called before inserting a new node and returns false, and once the node is
  inserted, returns true. GetNodes() is called and returns a vector containing the value
  of the node inserted.  GetNodes can now be confirmed to be behaving correctly with both
  an empty and non-empty graph.

  Now that inserting nodes has been tested, we attempt to construct a graph using
  that start and end of a const_iterator to a vector<N>.

  The following methods are then tested:
  * InsertEdge
    - either src or dst nodes do not exist
    - new edge inserted
    - attempt to insert duplicate edge
  * IsConnected
    - either src or dst nodes do not exist
    - Valid nodes that are connected and not connected
  * GetConnected
    - from node that does not exist
    - node that does exist
      - with no edges
      - with multiple edges to same node
  * GetWeights
    - src or dst node does not exist
    - between two nodes with no edges
    - between two nodes with multiple edges, checking that the  weights are sorted in
      increasing order
  



  Now that inserting and checking nodes and edges has been tested, we can test
  constructing a graph using a vector of tuples<N, N, E>.

  * == and != comparators
    - Two empty graphs
    - Empty and non-empty graph
    - Two identical non-empty graphs
    - Two graphs with identical nodes but one containing an edge
    - Two identical graphs but one edge weight differs from the other
  * erase edge
    - erase edge between node that does not exist
    - erase edge that does not exist between two valid nodes
    - erase valid edge
  * delete node
    - attempt to delete node that is not in the graph
    - delete node that is the dst node of an edge from another node
  * Copy constructor
    - copy construct empty graph
    - copy construct non-empty graph
      - modify original graph and ensure copied graph does not change
  * Move constructor
    - move construct empty graph
    - move construct non-empty graph
      - new graph contains all nodes and edges of original graph
      - ensure original graph is cleared
      - able to insert new node into moved-from graph
  * << operator
    - Empty graph
    - Non-empty graph containing nodes with and without edges
      - sorted by increasing order of src node, dst node, weight
  * Replace
    - attempt to replace node that does not exist
    - attempt to replace with node that already exists
    - valid replacement
      - ensure nodes with edges to oldNode now contain edges to newNode
  * MergeReplace
    - attempt to replace node that does not exist
    - attempt to replace with node that does not exist
    - Valid replacement
      - Ensure all edges to oldNode now point to newNode
      - Ensure all edges from oldNode now from newNode
      - Ensure any edges that may have been duplicated in the process have been removed
  * Clear
    - All nodes have been removed
    - new nodes can be added to cleared graph
  * Find
    - edge containing node that doesn't exist
    - valid nodes but with no edge
    - valid nodes with edge between them
  * Iterators
    - forward and reverse iterators
    - valid increment and decrement operations

*/

#include "assignments/dg/graph.h"

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#include "catch.h"

SCENARIO("Default constructor used") {
  GIVEN("A Graph<std::string, int> constructed with the default constructor") {
    gdwg::Graph<std::string, int> g;
    WHEN("GetNodes() is called") {
      auto nodes = g.GetNodes();
      THEN("You get an empty vector") { REQUIRE(nodes.size() == 0); }
    }
  }
}

SCENARIO("Inserting nodes into the graph") {
  GIVEN("An empty Graph<std::string, int>") {
    gdwg::Graph<std::string, int> g;
    WHEN("A node does not exist in the graph") { REQUIRE(g.IsNode("A") == false); }
    AND_WHEN("A new node is inserted") {
      {
        std::string s = "A";
        REQUIRE(g.InsertNode(s) == true);
      }
      REQUIRE(g.IsNode("A") == true);
      THEN("GetNodes() will return a vector of size 1, containing 'A'") {
        auto nodes = g.GetNodes();
        REQUIRE(nodes.size() == 1);
        REQUIRE(*nodes.cbegin() == "A");
        REQUIRE(g.IsNode("A") == true);
      }
      THEN("When attempting to insert another node 'A',"
           "InsertNode() returns false and g does not change") {
        REQUIRE(g.InsertNode("A") == false);
        auto nodes = g.GetNodes();
        REQUIRE(nodes.size() == 1);
        REQUIRE(nodes.at(0) == "A");
      }
    }
  }
}

SCENARIO("Construct Graph using vector const_iterators") {
  GIVEN("A Graph<std::string, int> constructed using a vector<std::string>") {
    std::vector<std::string> v{"C", "B", "A"};
    gdwg::Graph<std::string, int> g{v.begin(), v.end()};
    WHEN("GetNodes() is called") {
      auto nodes = g.GetNodes();
      THEN("You get a vector of size 3, containing the elements of v in alphbetical order") {
        REQUIRE(nodes.size() == v.size());
        std::sort(v.begin(), v.end());
        for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
          REQUIRE(nodes.at(i) == v.at(i));
        }
      }
    }
  }
}

SCENARIO("Inserting edges into a Graph") {
  GIVEN("A non-empty Graph<std::string, int>") {
    std::vector<std::string> v{"C", "B", "A"};
    gdwg::Graph<std::string, int> g{v.begin(), v.end()};
    WHEN("There are no edges between 'A' and 'B'") {
      THEN("IsConnected() will return false") {
        REQUIRE(g.IsConnected("A", "B") == false);
        REQUIRE(g.IsConnected("B", "A") == false);
      }
    }
    WHEN("An edge is inserted from 'A' to 'B'") {
      REQUIRE(g.InsertEdge("A", "B", 1) == true);
      THEN("'A' is connected to 'B'") {
        REQUIRE(g.IsConnected("A", "B") == true);
        REQUIRE(g.IsConnected("B", "A") == false);
      }
      AND_WHEN("Inserting another edge with different weight from 'A' to 'B'") {
        REQUIRE(g.InsertEdge("A", "B", 2) == true);
      }
      AND_WHEN("Attempting to insert and edge that already exists") {
        REQUIRE(g.InsertEdge("A", "B", 1) == false);
      }
    }
    WHEN("Inserting an edge between a node that that does not exist") {
      REQUIRE_THROWS_WITH(
          g.InsertEdge("A", "b", 1),
          "Cannot call Graph::InsertEdge when either src or dst node does not exist");
      REQUIRE_THROWS_WITH(
          g.InsertEdge("a", "B", 1),
          "Cannot call Graph::InsertEdge when either src or dst node does not exist");
    }
  }
}

SCENARIO("Checking and getting connected edges between nodes") {
  GIVEN("A Graph<std::string, double>") {
    std::vector<std::string> v{"A", "B", "C"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    g.InsertEdge("A", "B", 1.5);
    g.InsertEdge("A", "B", 2.5);
    g.InsertEdge("A", "C", 0.5);
    g.InsertEdge("B", "A", 1.5);
    WHEN("Calling IsConnected and src does not exist") {
      THEN("Require to catch throw runtime_error") {
        REQUIRE_THROWS_WITH(
            g.IsConnected("D", "B"),
            "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      }
    }
    WHEN("Calling IsConnected and dst does not exist") {
      THEN("Require to catch throw runtime_error") {
        REQUIRE_THROWS_WITH(
            g.IsConnected("A", "D"),
            "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      }
    }
    WHEN("Graph does not contain node and attempt to call GetConnected") {
      THEN("Require to catch throw out_of_range") {
        REQUIRE_THROWS_WITH(g.GetConnected("D"),
                            "Cannot call Graph::GetConnected if src doesn't exist in the graph");
      }
    }
    WHEN("Calling GetConnected on node that has no outgoing edges") {
      auto v = g.GetConnected("C");
      THEN("The vector returned will be empty") { REQUIRE(v.size() == 0); }
    }
    WHEN("Calling GetConnected on node that has multiple outgoing edges, "
         "including to the same dst node") {
      auto v = g.GetConnected("A");
      THEN("The vector returned will contain all of the distinct dst nodes "
           "and in increasing order") {
        REQUIRE(v.size() == 2);
        REQUIRE(v.at(0) == "B");
        REQUIRE(v.at(1) == "C");
      }
    }
  }
}

SCENARIO("Getting edge weights") {
  GIVEN("A graph containing nodes with edges between some") {
    std::vector<std::string> v{"A", "B", "C"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    g.InsertEdge("A", "B", 1.5);
    g.InsertEdge("A", "B", 2.5);
    g.InsertEdge("A", "C", 0.5);
    g.InsertEdge("B", "A", 1.5);
    WHEN("Getting edge weights from nodes that to not exist") {
      WHEN("src node does not exist") {
        THEN("Require to catch throw out_of_range") {
          REQUIRE_THROWS_WITH(
              g.GetWeights("D", "A"),
              "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
        }
      }
      WHEN("dest node that does not exist") {
        THEN("Require to catch throw out_of_range") {
          REQUIRE_THROWS_WITH(
              g.GetWeights("A", "D"),
              "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
        }
      }
    }
    WHEN("Calling GetWeights on node that has no outgoing edges") {
      auto v = g.GetWeights("C", "A");
      THEN("The vector returned will be empty") { REQUIRE(v.size() == 0); }
    }
    WHEN("Calling GetConnected on node that has multiple outgoing edges, "
         "including to the same dst node") {
      auto v = g.GetWeights("A", "B");
      THEN("The vector returned will outgoing weights in increasing order") {
        REQUIRE(v.size() == 2);
        REQUIRE(v.at(0) == 1.5);
        REQUIRE(v.at(1) == 2.5);
      }
    }
  }
}

SCENARIO("Construct Graph using vector<std::tuple<N, N, E>> const_iterators") {
  GIVEN("A vector of tuples<N, N, E>") {
    std::string s1{"A"};
    std::string s2{"B"};
    std::string s3{"C"};
    auto e1 = std::make_tuple(s1, s2, 5.4);
    auto e2 = std::make_tuple(s2, s3, 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    WHEN("A Graph<std::string, double> is constructed using the tuple vector") {
      gdwg::Graph<std::string, double> g{e.begin(), e.end()};
      THEN("GetNodes should return a vector containing the strings 'A', 'B', 'C'") {
        auto nodes = g.GetNodes();
        REQUIRE(nodes.size() == 3);
        REQUIRE(nodes.at(0) == "A");
        REQUIRE(nodes.at(1) == "B");
        REQUIRE(nodes.at(2) == "C");
      }
      THEN("The graph should only contain an edge from 'A' to 'B' (5.4), "
           "and an edge from 'B' to 'C' (7.6)") {
        REQUIRE(g.IsConnected("A", "C") == false);
        REQUIRE(g.IsConnected("B", "A") == false);
        REQUIRE(g.IsConnected("C", "A") == false);
        REQUIRE(g.IsConnected("C", "B") == false);
        auto v = g.GetWeights("A", "B");
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(0) == 5.4);
        v = g.GetWeights("B", "C");
        REQUIRE(v.size() == 1);
        REQUIRE(v.at(0) == 7.6);
      }
    }
  }
}

SCENARIO("== and != comparators") {
  GIVEN("Two empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g1;
    gdwg::Graph<std::string, double> g2;
    WHEN("Comparing using == and !=") {
      REQUIRE((g1 == g2) == true);
      REQUIRE((g1 != g2) == false);
    }
  }
  GIVEN("Two identical Graph<std::string, double> with no edges") {
    std::vector<std::string> v{"A", "B", "C"};
    gdwg::Graph<std::string, double> g1{v.begin(), v.end()};
    gdwg::Graph<std::string, double> g2{v.begin(), v.end()};
    WHEN("Comparing using == and != ") {
      REQUIRE((g1 == g2) == true);
      REQUIRE((g1 != g2) == false);
    }
    WHEN("A node is added to one graph") {
      g1.InsertNode("D");
      THEN("g1 and g2 are no longer equal") {
        REQUIRE((g1 == g2) == false);
        REQUIRE((g1 != g2) == true);
      }
    }
    WHEN("An edge is added to one graph") {
      g1.InsertEdge("A", "B", 1.5);
      THEN("g1 and g2 are no longer equal") {
        REQUIRE((g1 == g2) == false);
        REQUIRE((g1 != g2) == true);
      }
      AND_WHEN("The same edge but with different weight is added to the other graph") {
        g2.InsertEdge("A", "B", 1.0);
        THEN("g1 and g2 are not equal") {
          REQUIRE((g1 == g2) == false);
          REQUIRE((g1 != g2) == true);
        }
      }
      AND_WHEN("The same edge is added to the other graph") {
        g2.InsertEdge("A", "B", 1.5);
        THEN("g1 and g2 are equal") {
          REQUIRE((g1 == g2) == true);
          REQUIRE((g1 != g2) == false);
        }
      }
    }
  }
}

SCENARIO("Can copy one graph into another") {
  GIVEN("two valid nodes on graph with edges connecting them") {
    std::vector<std::string> v{"A", "B", "C"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    g.InsertEdge("A", "B", 1.2);
    g.InsertEdge("A", "B", 3.0);
    g.InsertEdge("B", "A", -1.2);

    WHEN("One  graph copies contents of another with an operator") {
      gdwg::Graph<std::string, double> f;
      f = g;
      THEN("node should contain all the same edges and nodes") {
        std::vector<std::string> nodes{"A", "B", "C"};
        std::vector<double> edges{1.2, 3.0};
        REQUIRE(f.GetNodes() == nodes);
      }
    }
    WHEN("One  graph copies contents of another with an constructor") {
      gdwg::Graph<std::string, double> f{g};
      THEN("node should contain all the same edges and nodes") {
        std::vector<std::string> nodes{"A", "B", "C"};
        std::vector<double> edges{1.2, 3.0};
        REQUIRE(f.GetNodes() == nodes);
      }
    }
    WHEN("One Graph moves content of another with move operator") {
      gdwg::Graph<std::string, double> f;
      f = std::move(g);
      THEN("node should contain all the same edges and nodes and the moved graph should be empty") {
        std::vector<std::string> nodes{"A", "B", "C"};
        std::vector<double> edges{1.2, 3.0};
        REQUIRE(f.GetNodes() == nodes);

        gdwg::Graph<std::string, double> empty;
        REQUIRE(g == empty);
      }
    }
    WHEN("One Graph moves content of another with move operator") {
      gdwg::Graph<std::string, double> f{std::move(g)};
      THEN("node should contain all the same edges and nodes and the moved graph should be empty") {
        std::vector<std::string> nodes{"A", "B", "C"};
        std::vector<double> edges{1.2, 3.0};
        REQUIRE(f.GetNodes() == nodes);

        gdwg::Graph<std::string, double> empty;
        REQUIRE(g == empty);
      }
    }
  }
}

SCENARIO("Erase an edge from the graph") {
  GIVEN("A non-empty Graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    WHEN("You remove an edge from the graph") {
      REQUIRE(g.erase("first", "second", 0) == true);
      THEN("edge should be remove from graph") {
        std::vector<int> edges{1};
        REQUIRE(g.GetWeights("first", "second") == edges);
      }
    }
    WHEN("Attempting to remove edge between a node that does not exist") {
      THEN("erase returns false") {
        REQUIRE(g.erase("first", "third", 1) == false);
        REQUIRE(g.erase("third", "first", 1) == false);
      }
    }
    WHEN("Attempting to remove an non-existent edge between two nodes in the graph") {
      REQUIRE(g.erase("first", "second", -1) == false);
      THEN("Edges from 'first' to 'second' remain the same") {
        std::vector<int> edges{0, 1};
        REQUIRE(g.GetWeights("first", "second") == edges);
      }
    }
  }
}

SCENARIO("Deleting a node from a graph") {
  GIVEN("A Graph<std::string, int> containing two nodes with an edge between them") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("second", "first", 1);
    WHEN("You delete a node that is not in the graph") { REQUIRE(g.DeleteNode("third") == false); }
    WHEN("You delete a node that that is in the graph") {
      REQUIRE(g.DeleteNode("first") == true);
      THEN("The graph now only contains the node 'second'") {
        auto nodes = g.GetNodes();
        REQUIRE(nodes.size() == 1);
        REQUIRE(g.IsNode("first") == false);
        REQUIRE(g.IsNode("second") == true);
      }
      AND_WHEN("'first' is reinserted") {
        REQUIRE(g.InsertNode("first") == true);
        THEN("There is no longer an edge from 'second' to 'first'") {
          REQUIRE(g.IsConnected("second", "first") == false);
        }
      }
    }
  }
}

SCENARIO("Copy constructor") {
  GIVEN("An empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g;
    WHEN("Using copy constructor") {
      gdwg::Graph<std::string, double> gCopy{g};
      THEN("gCopy == g") { REQUIRE(gCopy == g); }
      AND_WHEN("Adding a node to g") {
        g.InsertNode("A");
        THEN("gCopy does not change and is no longer equal to g") {
          REQUIRE(gCopy.IsNode("A") == false);
          REQUIRE(gCopy != g);
        }
      }
    }
  }
  GIVEN("An non-empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertEdge("A", "B", 5.4);
    WHEN("Using copy constructor") {
      gdwg::Graph<std::string, double> gCopy{g};
      THEN("gCopy == g") { REQUIRE(gCopy == g); }
      WHEN("Edge is deleted from original graph") {
        REQUIRE(g.erase("A", "B", 5.4));
        REQUIRE(g.IsConnected("A", "B") == false);
        THEN("The edge is not deleted in the copied graph") {
          REQUIRE(gCopy.IsConnected("A", "B") == true);
          std::vector<double> edges{5.4};
          REQUIRE(gCopy.GetWeights("A", "B") == edges);
        }
      }
      WHEN("Node is deleted from original graph") {
        REQUIRE(g.DeleteNode("A") == true);
        THEN("The copied graph does not change") {
          REQUIRE(gCopy.IsNode("A") == true);
          std::vector<double> edges{5.4};
          REQUIRE(gCopy.GetWeights("A", "B") == edges);
        }
      }
    }
  }
}

SCENARIO("Move constructor") {
  GIVEN("An empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g;
    WHEN("Using move constructor") {
      gdwg::Graph<std::string, double> gMove{std::move(g)};
      THEN("The original graph and the moved-to graph both are empty") {
        auto nodes = g.GetNodes();
        REQUIRE(nodes.size() == 0);
        nodes = gMove.GetNodes();
        REQUIRE(nodes.size() == 0);
      }
      AND_WHEN("A node is added to the original graph") {
        g.InsertNode("A");
        THEN("g now contains a node 'A' and gMove is unchanged") {
          REQUIRE(g.IsNode("A") == true);
          auto nodes = gMove.GetNodes();
          REQUIRE(nodes.size() == 0);
        }
      }
    }
  }
  GIVEN("A non-empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertEdge("A", "B", 5.4);
    WHEN("Using move constructor") {
      gdwg::Graph<std::string, double> gMove{std::move(g)};
      THEN("The original graph is now empty and gMove contains the nodes 'A' and 'B' "
           "and an edge from 'A' to 'B' with weight 5.4") {
        REQUIRE(g.GetNodes().size() == 0);
        auto nodes = gMove.GetNodes();
        REQUIRE(nodes.size() == 2);
        REQUIRE(nodes.at(0) == "A");
        REQUIRE(nodes.at(1) == "B");
        std::vector<double> edges{5.4};
        REQUIRE(gMove.GetWeights("A", "B") == edges);
      }
    }
  }
}

SCENARIO("Output stream") {
  GIVEN("An empty Graph<std::string, double>") {
    gdwg::Graph<std::string, double> g;
    WHEN("The graph is piped into a sstream") {
      std::stringstream ss;
      ss << g;
      THEN("The stream contains an empty string") { REQUIRE(ss.str() == ""); }
    }
    WHEN("A node is inserted into the graph") {
      g.InsertNode("A");
      THEN("The output stream will contain the following string") {
        std::stringstream ss;
        ss << g;
        REQUIRE(ss.str() == "A (\n)\n");
      }
      AND_WHEN("A second node is inserted") {
        g.InsertNode("B");
        THEN("The output stream will contain the following string") {
          std::stringstream ss;
          ss << g;
          REQUIRE(ss.str() == "A (\n)\nB (\n)\n");
        }
      }
    }
    WHEN("Multiple nodes containing edges between them are inserted") {
      g.InsertNode("A");
      g.InsertNode("B");
      g.InsertNode("C");
      g.InsertEdge("B", "A", 5);
      g.InsertEdge("B", "C", 3.5);
      g.InsertEdge("B", "C", -1);
      g.InsertEdge("C", "A", 2.2);
      THEN("The output stream will contain the graph structure in increasing order of "
           "src node, then dst node, and then edge weight") {
        std::stringstream ss;
        ss << g;
        REQUIRE(ss.str() == "A (\n"
                            ")\n"
                            "B (\n"
                            "  A | 5\n"
                            "  C | -1\n"
                            "  C | 3.5\n"
                            ")\n"
                            "C (\n"
                            "  A | 2.2\n"
                            ")\n");
      }
    }
  }
}

SCENARIO("Replacing a graphs node with another node") {
  GIVEN("A non-empty Graph<std::string, int>") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    WHEN("You change one node in a graph for another that does not exist in graph ") {
      bool r = g.Replace("first", "last");
      THEN("The graph successfully makes the change and contains new node") {
        REQUIRE(r);
        REQUIRE(g.IsNode("last"));
        REQUIRE(g.IsConnected("last", "second"));
      }
    }
    WHEN("You change one node in a graph for another that exist in graph ") {
      bool r = g.Replace("first", "second");
      THEN("The replace is unsuccessful no change made and in right order (lexigraphical)") {
        REQUIRE(!r);
      }
    }
    WHEN("Attempting to replace a node that does not exist") {
      THEN("Throw runtime_error exception") {
        REQUIRE_THROWS_WITH(g.Replace("third", "second"),
                            "Cannot call Graph::Replace on a node that doesn't exist");
      }
    }
  }
}

SCENARIO("Merge replacing a graphs node with another node") {
  GIVEN("A non-empty Graph<std::string, int>") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("last");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("last", "second", 0);
    g.InsertEdge("last", "first", 2);

    WHEN("You change one node in a graph for another along with its edges") {
      g.MergeReplace("first", "last");
      THEN("The old node has been removed from the graph and all edges have been reassigned to the "
           "new."
           " Any edges that are duplicated in the process are removed") {
        REQUIRE(g.IsNode("first") == false);
        REQUIRE(g.IsNode("last") == true);
        auto edges = g.GetWeights("last", "last");
        REQUIRE(edges.size() == 1);
        REQUIRE(edges.at(0) == 2);
        edges = g.GetWeights("last", "second");
        REQUIRE(edges.size() == 1);
        REQUIRE(edges.at(0) == 0);
      }
    }
    WHEN("oldData node that does not exist") {
      THEN("Require to catch throw runtime_error") {
        REQUIRE_THROWS_WITH(
            g.MergeReplace("third", "second"),
            "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("newData node that does not exist") {
      THEN("Require to catch throw runtime_error") {
        REQUIRE_THROWS_WITH(
            g.MergeReplace("first", "third"),
            "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
  }
}

SCENARIO("Clear graph") {
  WHEN("Have node that contains edges and nodes be cleared") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.Clear();
    THEN("graph should be empty") { REQUIRE(g.GetNodes().size() == 0); }
    AND_WHEN("A new node is added") {
      g.InsertNode("A");
      THEN("The graph contains the new node") { REQUIRE(g.IsNode("A")); }
    }
  }
}

SCENARIO("You can search for a edge from a graph using find") {
  WHEN("You find an edge in a graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", -1);
    g.InsertEdge("first", "second", 1);

    g.InsertNode("third");
    g.InsertEdge("first", "third", -2);

    auto it = g.find("first", "third", -2);
    THEN("You should return iterator to the edge") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");  //&& dst == "third" && weight == -2);
      REQUIRE(dst == "third");
      REQUIRE(weight == -2);
    }
  }
  WHEN("No edge is found") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", -1);
    g.InsertEdge("first", "second", 1);

    g.InsertNode("third");
    g.InsertEdge("first", "third", -2);

    auto it = g.find("first", "third", 1);
    THEN("You should return iterator to the gdwg::Graph<N, E>::cend()") { REQUIRE(it == g.cend()); }
  }
  WHEN("No Node is not found") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", -1);
    g.InsertEdge("first", "second", 1);

    g.InsertNode("third");
    g.InsertEdge("first", "third", -2);

    auto it = g.find("last", "third", 0);
    THEN("You should return iterator to the gdwg::Graph<N, E>::cend()") { REQUIRE(it == g.cend()); }
  }
}

SCENARIO("You can iterate over the graph forward as a const") {
  WHEN("you iterate to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("aaa");
    g.InsertNode("bbb");
    g.InsertNode("third");
    g.InsertEdge("aaa", "bbb", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.cbegin();
    ++it;
    ++it;
    ++it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "third");
      REQUIRE(weight == -2);
    }
  }
  WHEN("You iterate over an graph with no edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    THEN("cbegin and cend() are the same") { REQUIRE(g.cbegin() == g.cend()); }
  }
}

SCENARIO("You can iterate over the graph reverse") {
  WHEN("you iterate back to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("third");
    g.InsertNode("aaa");
    g.InsertNode("zzz");
    g.InsertEdge("zzz", "aaa", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.crbegin();
    ++it;
    ++it;
    ++it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "second");
      REQUIRE(weight == 1);
    }
  }
  WHEN("You iterate over an graph with no edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    THEN("crbegin and crend() are the same") { REQUIRE(g.crbegin() == g.crend()); }
  }
}

SCENARIO("You can iterate over the graph forward") {
  WHEN("you iterate forward to an edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("third");
    g.InsertNode("aaa");
    g.InsertNode("zzz");
    g.InsertEdge("aaa", "aaa", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.begin();
    ++it;
    ++it;
    ++it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order ") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "third");
      REQUIRE(weight == -2);
    }
  }
  WHEN("You iterate over an graph with no edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    THEN("begin and end() are the same") { REQUIRE(g.begin() == g.end()); }
  }
}

SCENARIO("You can iterate over the graph in reverse") {
  WHEN("you iterate forward to an edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("third");
    g.InsertNode("aaa");
    g.InsertNode("zzz");
    g.InsertEdge("zzz", "aaa", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.rbegin();
    ++it;
    ++it;
    ++it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "second");
      REQUIRE(weight == 1);
    }
  }
  WHEN("You iterate over an graph with no edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    THEN("rbegin and rend() are the same") { REQUIRE(g.rbegin() == g.rend()); }
  }
}

SCENARIO("You can decrement over the graph using forward const iterator") {
  WHEN("you iterate to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("aaa");
    g.InsertNode("bbb");
    g.InsertNode("third");
    g.InsertEdge("aaa", "bbb", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.cend();
    --it;
    --it;
    --it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "second");
      REQUIRE(weight == 1);
    }
  }
}

SCENARIO("You can decrement over the graph using forward iterator") {
  WHEN("you iterate to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("aaa");
    g.InsertNode("bbb");
    g.InsertNode("third");
    g.InsertEdge("aaa", "bbb", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.end();
    --it;
    --it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "third");
      REQUIRE(weight == -2);
    }
  }
}

SCENARIO("You can decrement over the graph using reverse const iterator") {
  WHEN("you iterate to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("aaa");
    g.InsertNode("bbb");
    g.InsertNode("third");
    g.InsertEdge("aaa", "bbb", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.crend();
    --it;
    --it;
    --it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "second");
      REQUIRE(weight == 1);
    }
  }
}

SCENARIO("You can decrement over the graph using reverse iterator") {
  WHEN("you iterate to and edge in the graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("first");
    g.InsertNode("second");
    g.InsertNode("aaa");
    g.InsertNode("bbb");
    g.InsertNode("third");
    g.InsertEdge("aaa", "bbb", 100);
    g.InsertEdge("first", "second", 0);
    g.InsertEdge("second", "first", -1);
    g.InsertEdge("first", "second", 1);
    g.InsertEdge("first", "third", -2);
    auto it = g.rend();
    --it;
    --it;
    THEN("Iterator should point to edges on graph in increasing lexicographical order then edge "
         "order and be const") {
      auto src = (std::get<0>(*it));
      auto dst = (std::get<1>(*it));
      auto weight = (std::get<2>(*it));
      REQUIRE(src == "first");
      REQUIRE(dst == "second");
      REQUIRE(weight == 0);
    }
  }
}
