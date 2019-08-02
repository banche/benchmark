#!/usr/bin/env python3
import json
import argparse
import jinja2
import webbrowser
import os
import sys

import collections

from parser import Benchmark, load_files
import html as ht

class PlotTrace(object):

    def __init__(self, name, values):
        self.name = name
        self.values = values

    def __str__(self):
        return '{ %s : %s}' % (self.name, str(self.values))

class PlotBench(object):

    def __init__(self, short_name, name, x):
        self.short_name = short_name
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
    for _, benchmark in benchmarks.items():
        x_values = [b.size for b in benchmark]
        y_values = [b.value() for b in benchmark]
        params = benchmark[0].t_params
        plot_key = benchmark[0].name
        short_name = benchmark[0].name
        if len(params) == 3:
            line_name = params[2] + '<' + params[0] + ', ' + params[1] + '>'
            plot_key += '<' + params[0] + ', ' + params[1] + '>'
        else:
            line_name = params[1] + '<' + params[0] + ', Action<' + params[0] + '>>'
            plot_key += '<' + params[0] + ', Action<' + params[0] + '>>'
        if not plot_key in data.keys():
            data[plot_key] = PlotBench(short_name, plot_key, x_values)
        data[plot_key].add_trace(line_name, y_values)
    return data


if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser(description='Parse google benchmark output and generates charts!')
    arg_parser.add_argument('-c', '--config', help='optional configuration', default='')
    arg_parser.add_argument('-f','--files', help='google benchmark output as a json file', default=[], action='append')
    args = arg_parser.parse_args()

    config = dict()
    if args.config:
        import importlib
        cfg = importlib.import_module(args.config.replace('.py', ''))
        config = cfg.descriptions

    if args.files:
        benchmarks = load_files(args.files)
    else:
        data = json.load(sys.stdin)
        benchmarks = parse_benchmark_json(data)

    plot_data = group_benchmarks(benchmarks)

    t = jinja2.Template(ht.template)
    html = t.render(benchmarks=plot_data, config=config)
    output = 'benchmarks-results.html'
    with open(output, 'w+') as f:
        f.write(html)
        webbrowser.open(os.path.abspath(output))
