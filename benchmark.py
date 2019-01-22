#!/usr/bin/env python3
import json
import argparse
from pprint import pprint
import collections
import jinja2
import webbrowser
import os
import sys

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

class PlotTrace(object):

    def __init__(self, name, values):
        self.name = name
        self.values = values

    def __str__(self):
        return '{ %s : %s}' % (self.name, str(self.values))

class PlotBench(object):

    def __init__(self, name, x):
        self.name = name
        self.x = x
        self.traces = list()

    def add_trace(self, trace_name, values):
        self.traces.append(PlotTrace(trace_name, values))

    def __str__(self):
        out = '{ ' + self.name + ':'
        for t in self.traces:
            out += '\n\t' + t.__str__()
        out += '\n}'
        return out


def group_benchmarks(benchmarks : dict()):
    data = collections.OrderedDict()
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
        if not plot_key in data.keys():
            data[plot_key] = PlotBench(plot_key, x_values)
        data[plot_key].add_trace(line_name, y_values)
    return data

template="""
<!DOCTYPE html>
<html lang="en" dir="ltr">
    <head>
        <meta charset="utf-8">
        <title>Hashmap benchmarks</title>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.bundle.js"></script>
    </head>
    <body>
    {% for _, b in benchmarks.items() %}
    <div style="width:60%;">
        <canvas id="{{ b.name }}"></canvas>
    </div>
    {% endfor %}

    <script type="text/javascript">
    var colors = ["#dc143c", "#1e90ff", "#228b22","#ff8c00", "#ffd700","#778899", "#9370db", "#8b4513"]
    var plots = new Array()
    {% for _, b in benchmarks.items() %}
        var plot{{loop.index}} = {
            type : 'line',
            data : {
                labels : {{ b.x }},
                datasets : [
                {% set count = 0 %}
                {% for t in b.traces %}
                    {
                        label : '{{ t.name }}',
                        data : {{ t.values }},
                        fill : false,
                        backgroundColor: colors[{{ loop.index - 1 }}],
                        borderColor: colors[{{ loop.index - 1 }}],
                    },
                    {% set count = count + 1 %}
                {% endfor %}
                ]
            },
            options: {
                responsive: true,
                title: {
                    display: true,
                    text: '{{b.name}}'
                },
                tooltips: {
                    mode: 'index',
                    intersect: false,
                },
                legend : {
                    position : 'right'
                },
                hover: {
                    mode: 'nearest',
                    intersect: true
                },
                scales: {
                    xAxes: [{
                        display: true,
                        scaleLabel: {
                            display: true,
                            labelString: 'Number of elements'
                        }
                    }],
                    yAxes: [{
                        display: true,
                        scaleLabel: {
                            display: true,
                            labelString: 'Nanos per operation (per element)'
                        }
                    }]
                }
            }
        };
    {% endfor %}

    window.onload = function () {

        {% for _,b in benchmarks.items() %}
            var ctx{{loop.index}} = document.getElementById('{{b.name}}').getContext('2d');
            window.myLine = new Chart(ctx{{loop.index}}, plot{{loop.index}});
        {% endfor %}
    }

    </script>
    </body>
</html>
"""

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser(description='Parse google benchmark output and generates charts!')
    arg_parser.add_argument('-f','--file', help='google benchmark output as a json file', default='')
    args = arg_parser.parse_args()

    if args.file != '':
        with open(args.file, 'r+') as f:
            data = json.load(f)
    else:
        data = json.load(sys.stdin)

    benchmarks = parse_benchmark_json(data)
    plot_data = group_benchmarks(benchmarks)

    t = jinja2.Template(template)
    html = t.render(benchmarks=plot_data)
    output = 'benchmarks-results.html'
    with open(output, 'w+') as f:
        f.write(html)
        webbrowser.open(os.path.abspath(output))
        #print(html)
        #pprint(plot_data)
        #pprint(benchmarks)
        #plot_benchmarks(benchmarks)
