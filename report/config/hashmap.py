# Configuration for the benchmarks
from parser import Description

insert_sequential = Description(
    'Insert_Sequential',
    description = "Sequential insert",
    details = """
Sequential insertion in a hashmap. We generate a sequence of n consecutive keys from [0,n[ and we then
insert them in order. Note the benchmark reserve the memory to avoid having noise due to the rehashing
policy of the hashmap.
"""
)

rehash = Description(
    'Rehash',
    global_timing = True,
    description = 'Rehash benchmark',
    details = """
Measures the time taken to insert an element which will lead to a rehash.\n
To do so the test inserts until the reserved size and then continue until there is a "costly" insert is hapening. Most of the
hashmap implementation do not support unconditional rehash so that an approximation. In practise for the hashmap tested the
rehash is slow compare to the insert so this seems to work pretty well.
"""
)

insert_erase_random = Description(
    'Insert_Erase_Random',
    legend = 'nanoseconds per element / operation',
    description = 'Random insert and erase of elements',
    details = """
The test will generate 2*n events, n of those are "insert" event and n of those the equivalent delete.\n
Once all the events are generated it it will shuflle everything such as the delete action is always done after the
insert (full-hit when deleting). However between the insert and delete for the same key there is a random number
of actions. It means between the insert and erase of a value it is doing x insert and y erase of other elements.
This simulate data processing such as financial market data processing.
"""
)

descriptions = dict()
descriptions[rehash.name] = rehash
descriptions[insert_erase_random.name] = insert_erase_random
descriptions[insert_sequential.name] = insert_sequential
