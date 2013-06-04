executable=/home/aravind/Work/Task-Partitioning/Random-Graph-Generator/RandomGraph
destinationdir=random-graphs-r2

#node=( 256 1024 16384 65536)
#indegree=(1)
#outdegree=(2 4 8)
#CCR=(0.0001 0.001 0.009)
#alpha=(0.1 1.0 10.0)
#beta=(5 25 50 75 95)
#gamma=(25 50 75)

node=( 128 256 512 1024 4096 8192 16384 )
indegree=(1)
outdegree=(2 4 8)
CCR=(0.0001 0.001 0.01 0.1 1)
alpha=(0.1 1.0 10.0)
beta=(5 25 50 75 95)
gamma=(5 25 50 75 95)

mkdir -p $destinationdir;

f()
{
#   for n in $(seq 2 2 20)
#	do
#	done
	for n in "${node[@]}"
	do
		for i in "${indegree[@]}"
		do
			for o in "${outdegree[@]}"
			do
				for c in "${CCR[@]}"
				do
					for a in "${alpha[@]}"
					do
						for b in "${beta[@]}"
						do
							for g in "${gamma[@]}"
							do
								$executable $n $i $o $c $a $b $g >> createdgraphs.log 
#								echo app-"$n"-"$i"-"$o"-"$c"-"$a"-"$b"-"$g".grf
							done
						done
					done
				done
			done
		done
		echo n - $n done
	done	
}



f

mv app*.grf $destinationdir/
