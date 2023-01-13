import math

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as colors
import csv
import os

def get_points_from_file(path):
    x_array = []
    y_array = []
    with open(path, newline='') as csv_file:
        reader = csv.reader(csv_file, delimiter=',', quotechar='\'')
        next(reader, None)
        for row in reader:
            x_array.append(float(row[1]))
            y_array.append(float(row[2]))

    return x_array, y_array


def get_timelines_from_file(path):
    timelines = []
    with open(path, newline='') as csv_file:
        reader = csv.reader(csv_file, delimiter=',', quotechar = '\'')
        next(reader, None)
        for row in reader:
            timeline = []
            for i in range(0, len(row), 2):
                timeline.append((float(row[i]), float(row[i+1])))
            timelines.append(timeline)

    timelines.reverse()

    return timelines


def gantt_chart(path):
    timelines = get_timelines_from_file(path)
    ytick_labels = range(len(timelines))
    yticks = range(15, len(timelines)*10 + 15, 10)
    fig, gnt = plt.subplots()
    gnt.set_xlabel('Unidades de tempo')
    gnt.set_ylabel('Máquina')
    gnt.set_yticks(yticks)
    gnt.set_yticklabels(ytick_labels)
    gnt.grid(True)

    num_jobs = len(timelines[0])
    cmap = cm.get_cmap('gist_rainbow', num_jobs)
    gantt_colors = [ colors.to_hex(cmap(i)) for i in range(num_jobs)]
    #print(gantt_colors)

    for i, t in enumerate(timelines):
        gnt.broken_barh(t, (10*(len(timelines)-i), 9), facecolors =gantt_colors)
        #print(t)

    plt.show()


def create_ticks(inf, sup, div):
    amplitude = int(sup - inf)
    digits = len(str(amplitude))
    rounded_aplitude = round(amplitude / 10 ** (digits - 1)) * 10 ** (digits - 1)
    spacing = int(rounded_aplitude / div)
    spacing_digits = len(str(spacing))
    rouded_inf = math.floor(inf / 10 ** (spacing_digits - 1)) * 10 ** (spacing_digits - 1)
    rouded_sup = math.ceil(sup / 10 ** (spacing_digits - 1)) * 10 ** (spacing_digits - 1)

    ticks = [rouded_inf - (rouded_inf % spacing)]
    while ticks[len(ticks)-1] <= rouded_sup:
        last_tick = ticks[len(ticks)-1]
        ticks.append(last_tick+spacing)
    return ticks



def plot_before_after(before_path, after_path, str_before, str_after, out=None, x=None, y=None):
    x_before, y_before = get_points_from_file(before_path)
    x_after, y_after = get_points_from_file(after_path)

    fig, ax = plt.subplots(1, 1)
    ax.plot(x_before, y_before, marker='o', linestyle='None', color='#CF4DCE', markersize=7, label=str_before)
    ax.plot(x_after, y_after, marker='o', linestyle='None', color='#00C698', markersize=5, label=str_after)
    ax.legend()
    ax.grid(True)

    if x is not None:
        plt.xticks(create_ticks(x[0], x[1], 10))

    if y is not None:
        plt.yticks(create_ticks(y[0], y[1], 10))

    ax.set_title('Soluções')
    ax.set_xlabel('TFT')
    ax.set_ylabel('TEC')

    if out is not None:
        plt.savefig(out)
        plt.close()
    else:
        plt.show()
        plt.close()






def create_dir_if_not_exists(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass
