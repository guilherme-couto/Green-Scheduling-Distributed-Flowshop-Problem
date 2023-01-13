import datetime
import random
import os
import time

nums_factories = [2, 3, 4, 5]
nums_jobs = [20, 40, 60, 80, 100]
nums_machines = [4, 8, 16]
v = [1, 1.3, 1.55, 1.75, 2.10]

# Estrutura do aquivo gerado:

# Fábricas
# Máquinas
# Jobs
# Velocidades
# Matriz MaquinasxJobs:
#       Cada linha i é um job
#       Cada coluna j é uma máquina


class Instance:
    num_factories = 0
    num_machines = 0
    num_jobs = 0
    time_matrix = []

    def __init__(self, factories, machines, jobs):
        self.num_factories = factories
        self.num_machines = machines
        self.num_jobs = jobs
        self.time_matrix = [[None]*machines for i in range(jobs)]

    def to_text(self):
        instance_string = f"{self.num_factories}\n{self.num_machines}\n{self.num_jobs}\n"
        speed_modes_str = " ".join(map(str, v))
        instance_string += f"{speed_modes_str}\n"
        for i in range(self.num_jobs):
            for j in range(self.num_machines):
                instance_string += f"{self.time_matrix[i][j]} "
        return instance_string


def generate(seed=0):
    instance_list = []
    random.seed(seed)

    for f in nums_factories:
        for m in nums_machines:
            for n in nums_jobs:
                for _ in range(10):
                    instance = Instance(f, m, n)

                    for i in range(n):
                        for j in range(m):
                            instance.time_matrix[i][j] = int(random.uniform(5, 50))

                    instance_list.append(instance)

    return instance_list


def main():
    seed = int(time.time()*1000 % 1000)
    instance_list = generate(seed=seed)
    create_dir_if_not_exists("instances")
    create_dir_if_not_exists(f"instances/{seed}")
    generate_instance_files(path=f"instances/{seed}", instance_list=instance_list)


def generate_instance_files(path="", instance_list=[]):
    offset = 60
    for i, instance in enumerate(instance_list):
        file = open(f"{path}/{instance.num_factories}-{instance.num_machines}-{instance.num_jobs}__{i%10}.txt", mode='x')
        file.write(instance.to_text())
        file.close()
        #offset += offset
        #if offset%60 == 0:
        #    offset = 60


def create_dir_if_not_exists(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass


if __name__ == "__main__":
    main()
