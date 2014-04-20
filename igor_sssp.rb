#!/usr/bin/env ruby
require 'igor'

Igor do
  
  sbatch_flags.push *%w[
    --account=pal
    --partition=pal
    --time=1:00:00
  ]
  
  database '~/osdi.sqlite', :pagerank
  
  path = File.expand_path(File.dirname(__FILE__)) + "/release/toolkits/graph_analytics"
  
  command "source ~/graphlab.env.sh; mpirun #{path}/sssp --graph /pic/projects/grappa/twitter/bintsv4/twitter-all.bintsv4 --format bintsv4 --graph_opts='%{graph_opts}' --max_degree_source=1 "
  
  params {
    dataset 'twitter'
    version 'graphlab'
    ppn 1
    nnode 16
    graph_opts 'ingress=oblivious,usehash=1'
  }
  
  parser {|out|
    h = {}
    out.gsub(/Finished Running engine in (#{REG_NUM}) seconds\./){ h[:time] = $~[1] }    
    out.gsub(/nverts: (\d+)\s*\n\s*nedges: (\d+)\s*\n\s*nreplicas: (\d+)\s*\n\s*replication factor: (#{REG_NUM})/){
      h[:nverts] = $~[1]
      h[:nedges] = $~[2]
      h[:nreplicas] = $~[3]
      h[:replication_factor] = $~[4]
    }
    out.gsub(/(\d+) iterations completed\./){ h[:iterations_run] = $~[1].to_i }
    h
  }
  
  interact
end
