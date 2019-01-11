#!/usr/bin/env python3
import json

def parse_benchmark_name(name: str):
    """
    Parses a template benchmark name with a size
    >>> parse_benchmark_name('BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000')
    ('BM_Insert_Random', ['int64_t', 'int64_t', 'std::unordered_map'], 1000)
    """
    base_name = name[0 : name.find('<')]
    t_params = [ x.strip() for x in name[name.find('<') + 1 : name.find('>')].split(',')]
    size = int(name[name.find('/') + 1:])

    return base_name, t_params, size

class Benchmark(object):
    """
    Benchmark class to hold all the information from google benchmark output
    """
    def __init__(self, benchmark_name, iterations, real_time, cpu_time, unit):
        self.name, self.t_params, self.size = parse_benchmark_name(benchmark_name)
        self.iterations = iterations
        self.real_time = real_time
        self.cpu_time = cpu_time
        self.unit = unit

    def __str__(self):
        return '{name: %s, params: %s, size_per_iter: %d, iter: %d, real_time: %f, cpu_time: %f, unit: %s}' % \
                (self.name, str(self.t_params), self.size, self.iterations, self.real_time, \
                 self.cpu_time, self.unit)

    @staticmethod
    def from_json(dct: dict):
        """
        Creates a benchmark from a json dict
        >>> import json
        >>> s = '{ "name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000",'
        >>> s += '"run_name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000",'
        >>> s += '"run_type": "iteration", "iterations": 17959,"real_time": 3.9503851248171341e+04,'
        >>> s += '"cpu_time": 3.9102234367170080e+04, "time_unit": "ns"}'
        >>> Benchmark.from_json(json.loads(s)).__str__()
        "{name: BM_Insert_Random, params: ['int64_t', 'int64_t', 'std::unordered_map'], size_per_iter: 1000, iter: 17959, real_time: 39503.851248, cpu_time: 39102.234367, unit: ns}"
        """
        return Benchmark(
                dct['name'],
                dct['iterations'],
                dct['real_time'],
                dct['cpu_time'],
                dct['time_unit'])


