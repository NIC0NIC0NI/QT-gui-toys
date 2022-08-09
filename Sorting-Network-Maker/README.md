# Sorting-Network-Maker

Generates figures of sorting networks.

## Correctness and Stability

After the network is generated and shown, it is automatically tested with random integers. The results show that all the sorting networks presented are correct.

A sorting algorithm is defined to be stable if it preserves the relative order of equal elements. We can test the stability through *Settings->Test Example->Test Sorting Stability*. The results show that only bubble sort and odd-even transposition sort are stable, while the rest are unstable.

## Create New Networks

Networks are generated in `sorting_network_generator.cpp` by calling `addComparator(int i, int j)` and `addSynchronizer(int i, int j)`. Note that `addSynchronizer` only affects the layout with *Settings->Split Parallel Levels* enabled. It may increase the 'latency' seen by the layout algorihm, but has no influence on the actual latency of the network.