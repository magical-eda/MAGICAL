#!/bin/bash

mergeGR() 
{

    python addPinsToGR.py $2/$1.wcon $2/$1.gr temp.gr VDD
    python addPinsToGR.py $2/$1.sub temp.gr ./output/ROUTING_INPUT.gr GND
    rm temp.gr
}

runRouting()
{
    ./ARouter  --designType ispd08pin --designFile ./output/ROUTING_INPUT.gr --techLef /home/local/eda09/keren/benchmarks/tech/40vhv.lef --techfile /home/local/eda09/keren/benchmarks/tech/techfile --out ./output/out.gds --placementLayout $2/$1.gds --iopin $2/$1.iopin --symnet $2/$1.symnet
}

path=$1
designName=$(basename -- "$path")
echo designName

#Create the output folder if not exist
if [ ! -d ./output ]; then
    mkdir output
fi


mergeGR $designName $path

runRouting $designName $path
