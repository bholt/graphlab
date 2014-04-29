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
  
end