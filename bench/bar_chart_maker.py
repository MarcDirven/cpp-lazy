import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import os
import sys
import csv
from collections import namedtuple


def get_cpu_caches(bench_file):
    cpu_caches = ['L0', 'L1', 'L2', 'L3', 'L4']
    benchmark_names = '\n'.join(row[0].strip() for row in bench_file if len(row) == 1 and
                       any(cache in row[0] for cache in cpu_caches))

    index_row_name = None
    for i in range(len(bench_file)):
        if bench_file[i][0] == 'name':
            index_row_name = i

    return benchmark_names, index_row_name


def get_benchmark_names_and_time(bench_file, name_idx):
    records = []
    bench_unit =  bench_file[name_idx][4]

    BenchmarkRecord = namedtuple('BenchmarkRecord', 'name real_time')
    for row in bench_file[name_idx:]:
        records.append(BenchmarkRecord(row[0], row[2]))
    return records, bench_unit


def make_bar_plot(benchmark_records, unit, cxx_version, caches):
    x_limit_offset = 15
    y_pos = np.arange(len(benchmark_records))
    performance = list(map(lambda record: float(record.real_time), benchmark_records))
    names = list(map(lambda record: record.name, benchmark_records))

    figure, ax = plt.subplots()
    ax.barh(y_pos, performance, align='center', alpha=0.5)
    ax.set_yticks(y_pos)
    ax.set_yticklabels(names, minor=False)

    for i, v in enumerate(performance):
        ax.text(v + 3, i - .05, str(round(v, 2)))

    plt.xlim(right=max(performance) + x_limit_offset)
    plt.xlabel(f'Time in {unit}')
    plt.title(f'Benchmarks for {cxx_version}\n{caches}')

    figure_size = plt.gcf().get_size_inches()
    size_factor = 1.8
    plt.gcf().set_size_inches(size_factor * figure_size)

    plt.savefig(f'benchmarks-iterators-{cxx_version}.png', dpi=400)

def main():
    benchmark_file_path = os.path.join(os.getcwd(), '..', 'cmake-build-release', 'bench', 'benchmark-iterators.csv')
    cxx_version = sys.argv[1]

    with open(benchmark_file_path) as benchmarks_file:
        benchmark_csv_list = list(csv.reader(benchmarks_file, delimiter=','))
        caches, name_index = get_cpu_caches(benchmark_csv_list)
        bench_records, unit = get_benchmark_names_and_time(benchmark_csv_list, name_index + 1)

        make_bar_plot(bench_records, unit, cxx_version, caches)


if __name__ == '__main__':
    main()