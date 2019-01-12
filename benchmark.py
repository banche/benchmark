#!/usr/bin/env python3
import json
import argparse
from pprint import pprint
import collections

import plotly
import plotly.plotly as py
import plotly.graph_objs as go

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
    def __init__(self, benchmark_name, iterations, real_time, cpu_time, unit):
        self.name, self.t_params, self.size = parse_benchmark_name(benchmark_name)
        self.full_name = parse_benchmark_full_name(benchmark_name)
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



def parse_benchmark_json(input: dict):
    bkey = 'benchmarks'
    benchmarks = collections.defaultdict(list)

    if not bkey in input.keys():
        raise RuntimeError('Missing benchmarks key!')
    ib = input[bkey]

    for bench in ib:
        benchmark = Benchmark.from_json(bench)
        benchmarks[benchmark.full_name].append(benchmark)

    # TODO parse some of the context information to generate the final report
    return benchmarks


def plot_benchmarks(benchmarks : dict()):
    data = collections.OrderedDict()
    plot_titles = list()
    for name, benchmark in benchmarks.items():
        x_values = [b.size for b in benchmark]
        y_values = [b.cpu_time/b.size for b in benchmark]
        params = benchmark[0].t_params
        plot_key = benchmark[0].name
        if len(params) == 3:
            line_name = params[2] + '<' + params[0] + ', ' + params[1] + '>'
            plot_key += '<' + params[0] + ', ' + params[1] + '>'
        else:

            line_name = params[1] + '<' + params[0] + ', Action<' + params[0] + '>>'
            plot_key += '<' + params[0] + ', Action<' + params[0] + '>>'

        trace = go.Scatter(
            x = x_values,
            y = y_values,
            mode = 'lines+markers',
            name = line_name,
            legendgroup = name,
        )
        if not plot_key in data.keys():
            data[plot_key] = list()
            plot_titles.append(plot_key)
        data[plot_key].append(trace)


    fig = plotly.tools.make_subplots(rows=len(data.keys()), subplot_titles=plot_titles)
    i = 1
    for k, traces in data.items():
        for t in traces:
            fig.append_trace(t, i, 1)
        i+= 1

    fig['layout'].update(height=2800, width=1200, title='hashmap performance')
    plotly.offline.plot(fig)

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser(description='Parse google benchmark output')
    arg_parser.add_argument('file', help='input filename')
    args = arg_parser.parse_args()

    with open(args.file, 'r+') as f:
        data = json.load(f)
        benchmarks = parse_benchmark_json(data)
        pprint(benchmarks)
        plot_benchmarks(benchmarks)
