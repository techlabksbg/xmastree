coords = File.read(ARGV[0]).split("\n").map{|line| line.split(" ").map{|x| x.to_f}}
xmin = coords.map{|e| e[0]}.min
ymin = coords.map{|e| e[1]}.min

coords.map!{|c| [c[0]-xmin, c[1]-ymin]}
puts coords.map{|c| c.map{|x| "%.1f" % x}.join(" ")}.join("\n")

