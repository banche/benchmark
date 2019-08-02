try:
    from parser.benchmark import Benchmark
except:
    from benchmark import Benchmark

import json
import collections


def parse_benchmark_json(input: dict):
    bkey = 'benchmarks'
    benchmarks = collections.defaultdict(list)

    if not bkey in input.keys():
        raise RuntimeError('Missing benchmarks key!')
    ib = input[bkey]

    for bench in ib:
        benchmark = Benchmark.from_json(bench)
        if benchmark is not None:
            benchmarks[benchmark.full_name].append(benchmark)

    # TODO parse some of the context information to generate the final report
    return benchmarks


def merge_dict(to_update: collections.defaultdict(list),
               other: collections.defaultdict(list)):
    """
    Merge benchmarks dictionnaries together
    >>> from collections import defaultdict
    >>> a = defaultdict(list)
    >>> b = defaultdict(list)
    >>> a[1] = [1,2]
    >>> a[2] = [3,4]
    >>> b[1] = [3,4]
    >>> b[2] = [1,2]
    >>> b[3] = [5,6]
    >>> merge_dict(a,b)
    defaultdict(<class 'list'>, {1: [1, 2, 3, 4], 2: [3, 4, 1, 2], 3: [5, 6]})
    """
    for k,v in other.items():
        if k in to_update.keys():
            to_update[k] += v
        else:
            to_update[k] = v
    return to_update

def load_files(files: list):
    benchmarks = collections.defaultdict(list)
    for file in files:
        with open(file, 'r+') as f:
            fdata = json.load(f)
            fbenchmarks = parse_benchmark_json(fdata)
            merge_dict(benchmarks, fbenchmarks)
    return benchmarks

