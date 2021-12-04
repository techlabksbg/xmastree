

def dist(l1, l2)
  (l1[0..2].zip(l2[0..2]).map{|x| (x[0]-x[1])**2}.sum)**0.5
end

def avg(l1, l2)
  (l1[0..2]).zip(l2[0..2]).map{|x| (x[0]+x[1])/2.0}+[0.99]
end

def cleanup(list)
 (2...(list.size-1)).each{|i|
  if (dist(list[i-1], list[i])>20 && dist(list[i], list[i+1])>20 && list[i][3]>2)
    puts  "[#{i}]  "+ list[i-1].inspect+" -> "+ list[i].inspect + " <- "+ list[i+1].inspect
    list[i] = avg(list[i-1], list[i+1])
    puts "  -->  "+list[i].inspect
  end
 }
end


data = ARGV.map{|file|
  File.readlines(file).map{|e| e.split.map{|x| x.to_f}}
}

data.map{|d| cleanup(d)}

data.each.with_index{|d,i|
  f = File.open("out#{i}.txt", "w")
  d.each{|l|
    f.puts(l.map{|e| "%.1f" % e}.join(" "))
  }
  f.close
}




