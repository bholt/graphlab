require 'igor'

Igor do

  @cols = [:dataset, :graph_opts, :nnode, :replication_factor, :time]
  
  @order = :time
  
  def selected
    c = @cols
    o = @order
    results{select(*c).order(o)}
  end

  def sq
    case `hostname`
    when /pal/
      puts `squeue -ppal -o '%.7i %.4P %.17j %.8u %.2t %.10M %.6D %R'`
    else
      puts `squeue -o '%.7i %.9P %.17j %.8u %.2t %.10M %.6D %R'`
    end
  end
  
  $twitter = '/pic/projects/grappa/twitter/bintsv4/twitter-all.bintsv4'
  $twitter_multi = '/pic/projects/grappa/twitter-multi/bintsv4/'
  $friendster = '/pic/projects/grappa/friendster/bintsv4/friendster.bintsv4'
  $friendster_multi = '/pic/projects/grappa/friendster/multi-bintsv4/'
  
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
    out.gsub( /total_bytes_sent: (#{REG_NUM}) GB\ntotal_network_bytes_sent: (#{REG_NUM}) GB\ntotal_msgs_sent: (#{REG_NUM})/){
      h[:total_sent_gb] = $~[1].to_f
      h[:total_network_sent_gb] = $~[2].to_f
      h[:total_msgs_sent] = $~[3].to_f
    }
    h
  }
  
end