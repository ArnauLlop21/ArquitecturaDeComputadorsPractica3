for i in 15000 25000 35000 45000
do
    for j in 2 4 8 16 32 64 128 256
    do
        srun -p orca -c 128 time ./linear_AC $i $j
        srun -p teen -c 32 time ./linear_AC $i $j
    done
done