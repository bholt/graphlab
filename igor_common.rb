require 'igor'

Igor do

  @cols = [:dataset, :graph_opts, :nnode, :replication_factor, :time]
  
  @order = :time
  
  def selected
    c = @cols
    o = @order
    results{select(*c).order(o)}
  end

end