#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

#include "assignments/dg/graph.h"

// Note: At the moment, there is no client.sampleout. Please do your own testing

int main() {
  gdwg::Graph<std::string, int> g;
  {
  	std::string s("hello");
  	g.InsertNode(s);
  }

  std::cout << g.IsNode("hello") << "\n";

  g.InsertNode("how");
  g.InsertNode("are");
  g.InsertNode("you?");

  g.InsertEdge("hello", "how", 5);
  g.InsertEdge("hello", "are", 8);
  g.InsertEdge("hello", "are", 2);

  g.InsertEdge("how", "you?", 3);
  g.InsertEdge("how", "are", 10);

  g.InsertEdge("how", "you?", 1);
  g.InsertEdge("how", "hello", 4);

  g.InsertEdge("are", "you?", 3);
  g.InsertEdge("are", "are", 10);
  
  // auto it = g.begin();
  // std::cout << "first edge: <" << std::get<0>(*it) << " " << std::get<1>(*it) << " " << std::get<2>(*it) << ">\n";

  std::cout << g << "\n";

  std::cout << "FULL GRAPH\n";
  for (auto it = g.begin(); it != g.end(); ++it) {
    std::cout << "<" << std::get<0>(*it) << " ";
    std::cout << std::get<1>(*it) << " ";
    std::cout << std::get<2>(*it) << ">\n";
  }

  auto gCopy{g};
  std::cout << "Copy constructed\n" << gCopy;

  auto gCopyAssign = g;
  std::cout << "Copy Assignment\n" << gCopyAssign;

  auto gMove{std::move(gCopy)};
  std::cout << "FULL GRAPH gMove\n" << gMove;

  std::cout << "gCopy after move()\n" << gCopy;

  auto gMoveAssign = std::move(gCopyAssign);
  std::cout << "Move Assignment:\n" << gMoveAssign;
  std::cout << "gCopyAssign after move assignment\n" << gCopyAssign;


  std::cout << "IS CONNECTED 'how' -> 'how': " << g.IsConnected("how", "how") << "\n";
  std::cout << "IS CONNECTED 'how' -> 'hello': " << g.IsConnected("how", "hello") << "\n";

  std::cout << "GET CONNECTED FROM 'how'\n";
  auto connected = g.GetConnected("how");
  for (auto e : connected) {
    std::cout << e << "\n";
  }

  try {
    auto connected = g.GetConnected("h");
  } catch (std::out_of_range& e) {
    std::cout << "exception thrown: " << e.what() << "\n";
  }

  std::cout << "GET WEIGHTS FROM 'how' TO 'you?'\n";
  auto weights = g.GetWeights("how", "you?");
  for (auto w : weights) {
    std::cout << w << "\n";
  }

  std::cout << "Delete non-existent edge: " << g.erase("how", "hello", 0) << "\n";
  std::cout << "Delete edge <how, hello, 4>: " << g.erase("how", "hello", 4) << "\n";

  std::cout << "CREVERSE IT\n";
  for (auto it = g.crbegin(); it != g.crend(); it++) {
    std::cout << "<" << std::get<0>(*it) << " ";
    std::cout << std::get<1>(*it) << " ";
    std::cout << std::get<2>(*it) << ">\n";
  }

  std::cout << "MergeReplace(how, are)\n";
  g.MergeReplace("how", "are");
  for (auto it = g.begin(); it != g.end(); ++it) {
    std::cout << "<" << std::get<0>(*it) << " ";
    std::cout << std::get<1>(*it) << " ";
    std::cout << std::get<2>(*it) << ">\n";
  }

  std::cout << "FIND EDGE AND ITERATE\n";
  for (auto it = g.find("hello", "are", 5); it != g.cend(); ++it) {
    std::cout << "<" << std::get<0>(*it) << " ";
    std::cout << std::get<1>(*it) << " ";
    std::cout << std::get<2>(*it) << ">\n"; 
  }


  std::cout << "ERASE EDGE USING erase()\n";
  for (auto it = g.cbegin(); it != g.cend(); ++it) {
    if (std::get<0>(*it) == "hello" && std::get<1>(*it) == "are") {
      it = g.erase(it);
      break;
    }
  }

  for (auto it = g.cbegin(); it != g.cend(); ++it) {
    std::cout << "<" << std::get<0>(*it) << " ";
    std::cout << std::get<1>(*it) << " ";
    std::cout << std::get<2>(*it) << ">\n";
  }

  std::cout << "Replace 'hello' with 'goodbye'\n";
  g.Replace("hello", "goodbye");
  // g.PrintGraph();
  for (auto it = g.begin(); it != g.end(); it++) {
    std::cout << "<" << std::get<0>(*it) << " " << std::get<1>(*it) << " " << std::get<2>(*it) << ">\n";  
  }

  std::cout << "'hello' is node: " << g.IsNode("hello") << "\n";

  std::cout << "Delete 'how'\n";
  g.DeleteNode("how");

  for (auto it = g.begin(); it != g.end(); it++) {
    std::cout << "<" << std::get<0>(*it) << " " << std::get<1>(*it) << " " << std::get<2>(*it) << ">\n";  
  }
  
  std::cout << "==operator\n";
  std::vector<std::string> v{"Hello", "how", "are", "you?"};
  gdwg::Graph<std::string, int> g1{v.cbegin(), v.cend()};
  gdwg::Graph<std::string, int> g2{v.cbegin(), v.cend()};
  
  g1.InsertEdge("Hello", "how", 5);
  g1.InsertEdge("Hello", "are", 8);
  g1.InsertEdge("Hello", "are", 2);
  g1.InsertEdge("how", "you?", 3);
  g1.InsertEdge("how", "are", 10);
  
  g2.InsertEdge("Hello", "how", 5);
  g2.InsertEdge("Hello", "are", 8);
  g2.InsertEdge("Hello", "are", 2);
  g2.InsertEdge("how", "you?", 3);
  g2.InsertEdge("how", "are", 10);

  std::cout << "g1 == g2: " << (g1 == g2) << "\n";
  std::cout << "g != g1: " << (g != g1) << "\n";

  // Vector initialisation
  // std::vector<std::string> v{"Hello", "how", "are", "you"};
  gdwg::Graph<std::string, double> b{v.begin(), v.end()};

  // List initialisation
  gdwg::Graph<std::string, double> gl{"Hello", "how", "are", "you"};

  std::string s1{"Hello"};
  std::string s2{"how"};
  std::string s3{"are"};
  auto e1 = std::make_tuple(s1, s2, 5.4);
  auto e2 = std::make_tuple(s2, s3, 7.6);
  auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
  gdwg::Graph<std::string, double> c{e.begin(), e.end()};

  std::cout << "Graph tuple initialisation\n";

  auto vec = g.GetNodes();
  for (auto& it : vec) {
    std::cout << it << "\n";
  }

  std::cout << "Clear graph\n";
  g.Clear();
  for (auto it = g.begin(); it != g.end(); it++) {
    std::cout << "<" << std::get<0>(*it) << " " << std::get<1>(*it) << " " << std::get<2>(*it) << ">\n";  
  }

  // std::vector<std::string> v{"Hello", "how", "are", "you"};
  

  // std::cout << g << '\n';

  // gdwg::Graph<std::string, int> g2{g};

  // std::cout << g2 << "\n";

  // // This is a structured binding.
  // // https://en.cppreference.com/w/cpp/language/structured_binding
  // // It allows you to unpack your tuple.
  // for (const auto& [from, to, weight] : g) {
  //   std::cout << from << " -> " << to << " (weight " << weight << ")\n";
  // }
}
