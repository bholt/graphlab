/*
 * Copyright (c) 2009 Carnegie Mellon University.
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *
 */

#include <vector>
#include <string>
#include <fstream>

#include <graphlab.hpp>
#include <graphlab/util/timer.hpp>
#include <graphlab/rpc/fiber_buffered_exchange.hpp>

// #include <graphlab/macros_def.hpp>


/**
 * \ingroup warp
 *
 * This Warp function provides a simple parallel for loop over all vertices
 * in the graph, or in a given set of vertices. A large number of light-weight 
 * threads called fibers are used to run the user specified function,
 * allowing the user to make what normally will be blocking calls, on 
 * the neighborhood of each vertex.
 *
 * \code
 * float pagerank_map(graph_type::edge_type edge, graph_type::vertex_type other) {
 *  return other.data() / other.num_out_edges();
 * }
 *
 * void pagerank(graph_type::vertex_type vertex) {
 *    // computes an aggregate over the neighborhood using map_reduce_neighborhood
 *    vertex.data() = 0.15 + 0.85 * warp::map_reduce_neighborhood(vertex,
 *                                                                IN_EDGES,
 *                                                                pagerank_map);
 * }
 *
 * ...
 * // runs the pagerank function on all the vertices in the graph.
 * parfor_all_vertices(graph, pagerank); 
 * \endcode
 *
 * \param graph A reference to the graph object
 * \param fn A function to run on each vertex. Has the prototype void(GraphType::vertex_type). Can be a boost::function
 * \param vset A set of vertices to run on
 * \param nfibers Number of fiber threads to use. Defaults to 10000
 * \param stacksize Size of each fiber stack in bytes. Defaults to 16384 bytes
 *
 * \see graphlab::warp::map_reduce_neighborhood()
 * \see graphlab::warp::warp_graph_transform()
 */
template <typename F>
void parfor(distributed_control& dc, size_t n, F body, size_t nfibers = 10000, size_t stacksize = 16384) {
  atomic<size_t> ctr;
  dc.barrier();
  dc.set_fast_track_requests(false);
  fiber_group group;
  group.set_stacksize(stacksize);
  
  auto f = [&ctr,body]{
    while (1) {
      size_t is = ctr.inc_ret_last();
      if (i >= n) break;
      body(i);
    }
  };
  
  for (size_t i = 0;i < nfibers; ++i) {
    group.launch(boost::bind(&decltype(f)::operator(), &f));
  }
  group.join();
  dc.barrier();
}

int main(int argc, char** argv) {
  // Initialize control plain using mpi
  graphlab::mpi_tools::init(argc, argv);
  graphlab::distributed_control dc;
  global_logger().set_log_level(LOG_INFO);

  // Parse command line options -----------------------------------------------
  graphlab::command_line_options clopts("GUPS Benchmark");
  
  int log2_size_a = 30, log2_size_b = 20;
  clopts.attach_option("log2_size_a", log2_size_a, "Size of target array.");
  clopts.attach_option("log2_size_b", log2_size_b, "Size of index array.");
    
  if(!clopts.parse(argc, argv)) {
    dc.cout() << "Error in parsing command line arguments." << std::endl;
    return EXIT_FAILURE;
  }
  
  size_t sizeA = 1 << log2_size_a;
  size_t sizeB = 1 << log2_size_b;
    
  procid_t nnode = dc.numprocs();
  
  size_t n_local_a = sizeA / nnode;
  size_t n_local_b = sizeB / nnode;
  
  int64_t * local_a = new int64_t[n_local_a];
  int64_t * local_b = new int64_t[n_local_b];
  
  srand(12345);
  
  for (size_t i=0; i<n_local_a; i++) local_a[i] = 0;
  for (size_t i=0; i<n_local_b; i++) local_b[i] = rand() % sizeA;
  
  fiber_buffered_exchange<int64_t> xchg;
  
  size_t before_bytes_sent = dc.bytes_sent();
  dc.all_reduce(before_bytes_sent);
  size_t before_network_bytes_sent = dc.network_bytes_sent();
  dc.all_reduce(before_network_bytes_sent);
  size_t before_msgs_sent = dc.calls_sent();
  dc.all_reduce(before_msgs_sent);

  parfor(dc, []{
    
  });
  
  size_t GB = (1L<<30);
  size_t total_bytes_sent = dc.bytes_sent();
  dc.all_reduce(total_bytes_sent);
  size_t total_network_bytes_sent = dc.network_bytes_sent();
  dc.all_reduce(total_network_bytes_sent);
  size_t total_msgs_sent = dc.calls_sent();
  dc.all_reduce(total_msgs_sent);
  
  dc.cout() << "\n"
    << "total_bytes_sent: "
      << static_cast<double>(total_bytes_sent - before_bytes_sent) / GB << " GB\n"
    << "total_network_bytes_sent: "
      << static_cast<double>(total_network_bytes_sent - before_network_bytes_sent) / GB << " GB\n"
    << "total_msgs_sent: "
      << static_cast<double>(total_msgs_sent - before_msgs_sent) << "\n";
  
  // Tear-down communication layer and quit -----------------------------------
  graphlab::mpi_tools::finalize();
  return EXIT_SUCCESS;
} // End of main


// We render this entire program in the documentation


