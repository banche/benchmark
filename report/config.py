# Configuration for the benchmarks
from parser import Description

rehash = Description('Rehash',
        global_timing = True,
        description = 'Rehash benchmark',
        details = 'Measures the time taken to insert an element which will lead to a rehash'
)

descriptions = dict()
descriptions[rehash.name] = rehash
