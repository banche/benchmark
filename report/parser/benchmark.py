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

def parse_benchmark_full_name(name: str):
    """
    Returns the benchmark full name including the template types.
    >>> parse_benchmark_full_name('BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000')
    'BM_Insert_Random<int64_t, int64_t, std::unordered_map>'
    """
    return name[0: name.find('/')]

class Benchmark(object):
    """
    Benchmark class to hold all the information from google benchmark output
    """
    __all_benchmarks = dict()

    def __init__(self, benchmark_name, iterations, real_time, cpu_time, unit):
        self.name, self.t_params, self.size = parse_benchmark_name(benchmark_name)
        self.full_name = parse_benchmark_full_name(benchmark_name)
        self.iterations = iterations
        self.real_time = real_time
        self.cpu_time = cpu_time
        self.unit = unit
        self.run_name = benchmark_name
        self.mean = None
        self.median = None
        self.stddev = None

    def cpu_time(self):
        if self.median is None:
            return self.cpu_time
        else:
            return self.median

    def stddev(self):
        return self.stddev

    def __str__(self):
        return '{name: %s, params: %s, size_per_iter: %d, iter: %d, real_time: %f, cpu_time: %f, unit: %s}' % \
                (self.name, str(self.t_params), self.size, self.iterations, self.real_time, \
                 self.cpu_time, self.unit)

    def value(self):
        if self.name.find('Rehash') != -1:
            if self.median is None:
                return self.cpu_time
            else:
                return self.median
        else:
            if self.median is None:
               return self.cpu_time / self.size
            else:
                return self.median / self.size

    def legend(self):
        if self.name.contains('Rehash'):
            return 'Nanoseconds/rehash'
        else:
            return 'Nanoseconds/element'

    @staticmethod
    def from_json(dct: dict):
        """
        Creates a benchmark from a json dict
        >>> import json
        >>> s = '{ "name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000",'
        >>> s += '"run_name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000",'
        >>> s += '"run_type": "iteration", "iterations": 17959,"real_time": 3.9503851248171341e+04,'
        >>> s += '"cpu_time": 3.9102234367170080e+04, "time_unit": "ns"}'
        >>> b = Benchmark.from_json(json.loads(s))
        >>> print(b)
        {name: BM_Insert_Random, params: ['int64_t', 'int64_t', 'std::unordered_map'], size_per_iter: 1000, iter: 17959, real_time: 39503.851248, cpu_time: 39102.234367, unit: ns}
        >>> s = '{ "name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000_mean",'
        >>> s += '"run_name": "BM_Insert_Random<int64_t, int64_t, std::unordered_map>/1000",'
        >>> s += '"run_type": "aggregate", "iterations": 10,"real_time": 3.9503851248171341e+04,'
        >>> s += '"aggregate_name": "mean", "cpu_time": 42, "time_unit": "ns"}'
        >>> print(Benchmark.from_json(json.loads(s)))
        None
        >>> print(b.mean, b.iteration)
        42 10
        """
        if dct['run_name'] == dct['name'] and dct['run_type'] != 'iteration':
            b = Benchmark(
                dct['run_name'],
                dct['iterations'],
                dct['real_time'],
                dct['cpu_time'],
                dct['time_unit'])
            Benchmark.__all_benchmarks[b.run_name] = b
            return b
        else:
            # we have iteration types
            #   - insert if it does not exist
            #   - update existing benchmark if needed
            if dct['run_type'] == 'iteration':
                if dct['run_name'] not in Benchmark.__all_benchmarks.keys():
                    b = Benchmark(
                        dct['run_name'],
                        dct['iterations'],
                        dct['real_time'],
                        dct['cpu_time'],
                        dct['time_unit'])
                    Benchmark.__all_benchmarks[b.run_name] = b
                    return b
            elif dct['run_type'] == 'aggregate':
                assert(dct['run_name'] in Benchmark.__all_benchmarks.keys())
                b = Benchmark.__all_benchmarks[dct['run_name']]
                aggregate_type = dct['aggregate_name']
                if aggregate_type == 'mean':
                    b.mean = dct['cpu_time']
                    b.iteration = dct['iterations']
                elif aggregate_type == 'median':
                    b.median = dct['cpu_time']
                elif aggregate_type == 'stddev':
                    b.stddev = dct['cpu_time']
        return None

