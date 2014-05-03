#!/usr/bin/env ruby
require 'igor'
require_relative 'igor_common'

Igor do
  
  sbatch_flags.push *%w[
    --account=pal
    --partition=pal
    --time=1:00:00
  ]
  
  database '~/osdi.sqlite', :pagerank
  
  path = File.expand_path(File.dirname(__FILE__)) + "/release/toolkits/graph_analytics"
  
  command "source ~/graphlab.env.sh; mpirun #{path}/%{exe} --graph %{path} --format bintsv4 --graph_opts='%{graph_opts}' --iterations=%{iterations}"
  
  params {
    multi false
    path expr("multi ? '/pic/projects/grappa/twitter-multi/bintsv4/' : '/pic/projects/grappa/twitter/bintsv4/twitter-all.bintsv4'")
    exe 'pagerank_delta'
    dataset 'twitter'
    version 'graphlab'
    ppn 1
    nnode 16
    graph_opts 'ingress=oblivious,usehash=1'
    iterations 0
  }
    
  interact
end
