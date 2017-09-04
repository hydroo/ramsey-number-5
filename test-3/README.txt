- Create all colorings with a DFS instead of binary counting
    - This way we can prune the tree

- Go back to using arguments instead of compile time foo

In the first implementation without pruning R(3, 4) > 8 takes 6.5 seconds up from 0.8 in test-2 :(.
Not sure whether it's because of dynamic_bitset of the DFS enumeration.
It should be one of the two or a mix.
