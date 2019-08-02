# Configuration for the benchmarks
from parser import Description

rehash = Description('Rehash',
        global_timing = True,
        description = 'Rehash benchmark',
        details = 'Measures the time taken to insert an element which will lead to a rehash'
)

insert_erase_random = Description(
    name = 'BM_Insert_Erase_Random',
    legend = 'nanoseconds per element / operation',
    description = 'Random insert and erase benchmark',
    details = 'some details'
)

descriptions = dict()
descriptions[rehash.name] = rehash
descriptions[insert_erase_random.name] = insert_erase_random
