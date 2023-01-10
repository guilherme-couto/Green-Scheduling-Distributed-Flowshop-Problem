import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors as colors
import csv


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
    cmap = cm.get_cmap('jet', num_jobs)
    gantt_colors = [ colors.to_hex(cmap(i)) for i in range(num_jobs)]
    #print(gantt_colors)

    for i, t in enumerate(timelines):
        gnt.broken_barh(t, (10*(len(timelines)-i), 9), facecolors =gantt_colors)
        #print(t)

    plt.show()
