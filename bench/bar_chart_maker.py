import matplotlib.pyplot as plt
import numpy as np
import os
import csv
from collections import namedtuple

os.system("")
plt.rcdefaults()
filename = 'benchmarks-iterators-{}.png'


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
    bench_unit = bench_file[name_idx][4]

    BenchmarkRecord = namedtuple('BenchmarkRecord', 'name real_time')
    for row in bench_file[name_idx:]:
        records.append(BenchmarkRecord(row[0], float(row[2])))
    return records, bench_unit


def make_table(benchmark_records, unit, cxx_version, caches, n_iterations, title_iterations):
    title = f'{cxx_version}\n{caches}'
    fig_border = 'steelblue'

    benchmark_records = sorted(benchmark_records, key=lambda e: e.real_time)
    fastest = benchmark_records[0].real_time
    data = [[record.name, record.real_time, round(record.real_time / fastest, 2)] for record in benchmark_records]
    column_headers = ["Time", "Relative speed"]
    row_headers = [x.pop(0) for x in data]

    cell_text = []
    for row in data:
        cell_text.append([f'{round(row[0] / n_iterations, 2)} ns', f'{row[1]} x'])

    row_colors = plt.cm.BuPu(np.full(len(row_headers), 0.1))
    col_colors = plt.cm.BuPu(np.full(len(column_headers), 0.1))

    plt.figure(linewidth=2,
               edgecolor=fig_border)

    the_table = plt.table(cellText=cell_text,
                          rowLabels=row_headers,
                          rowColours=row_colors,
                          colColours=col_colors,
                          cellLoc='center',
                          colLabels=column_headers,
                          loc='center')
    the_table.scale(1.5, 1.5)

    ax = plt.gca()
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)

    plt.box(on=None)
    plt.figtext(.90, 0.05, None, horizontalalignment='center', size=8, weight='light')
    fig = plt.gcf()
    plt.savefig(f'benchmarks-iterators-{cxx_version}.png',
                bbox_inches='tight',
                edgecolor=fig.get_edgecolor(),
                facecolor=fig.get_facecolor(),
                dpi=200)
    plt.clf()


def get_cmake_cxx_version():
    cxx_version = "C++"
    cmake_cxx_string = 'CMAKE_CXX_STANDARD'
    cmake_file = open('CMakeLists.txt', 'r')

    for idx, line in enumerate(cmake_file):
        cmake_cxx_string_index = line.find(cmake_cxx_string)
        if cmake_cxx_string_index == -1:
            continue

        reading_offset = len(cmake_cxx_string) + cmake_cxx_string_index + 1
        cxx_number = line[reading_offset: line.index(')', reading_offset)]
        cxx_version += cxx_number
        break

    return cxx_version


def main():
    benchmark_file_path = os.path.join(os.getcwd(), '..', 'cmake-build-release', 'bench', 'benchmark-iterators.csv')
    cxx_version = get_cmake_cxx_version()

    with open(benchmark_file_path) as benchmarks_file:
        benchmark_csv_list = list(csv.reader(benchmarks_file, delimiter=','))
        caches, name_index = get_cpu_caches(benchmark_csv_list)
        bench_records, unit = get_benchmark_names_and_time(benchmark_csv_list, name_index + 1)

        iterations = 32
        title_iterations = 1
        make_table(bench_records, unit, cxx_version, caches, iterations, title_iterations)

    green = '\033[32m'
    print(f'{green}Successfully created {filename.format(cxx_version)}')


if __name__ == '__main__':
    main()
