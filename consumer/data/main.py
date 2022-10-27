
from os import listdir
from pathlib import Path
from pprint import pprint
import matplotlib.pyplot as plt
import numpy as np

DATA_DIR = Path(__file__).parent


def plot_average(kafka_averages: np.ndarray, rabbitmq_averages: np.ndarray):

    max_tests = min([len(rabbitmq_averages), len(kafka_averages)])
    rabbitmq_averages = rabbitmq_averages[:max_tests]
    kafka_averages = kafka_averages[:max_tests]

    packets = list(range(max_tests))
    rmq_color = (0, 0.5, 0)
    kafka_color = (0.5, 0, 0)
    plt.scatter(packets, rabbitmq_averages, color=rmq_color, alpha=0.5)
    plt.plot(packets, rabbitmq_averages.mean() *
             np.ones((len(packets))), color=rmq_color)
    plt.scatter(packets, kafka_averages, color=kafka_color, alpha=0.5)
    plt.plot(packets, kafka_averages.mean() *
             np.ones((len(packets))), color=kafka_color)
    plt.legend(["RabbitMQ", "Média RabbitMQ", "Kafka", "Média Kafka"])
    plt.xlabel("Teste")
    plt.ylim(0)
    plt.ylabel("Tempo (us)")
    plt.savefig(DATA_DIR / "compare_evarages.png")
    plt.show()


def plot_time(time: list, broker_type: str):

    packets = list(range(len(time)))
    color = (0, 0.5, 0)
    kafka_color = (0.5, 0, 0)
    plt.scatter(packets, time, color=color, alpha=0.5)
    plt.legend(["Times"])
    plt.xlabel("Packet")
    plt.ylim(0)
    plt.ylabel("Time (us)")
    plt.savefig(DATA_DIR / f"best_time_{broker_type}.png")
    plt.show()


def main():
    # print(matplotlib.get_backend())

    kafka_times: list[np.uint] = []
    rabbitmq_times: list[np.uint] = []

    times_files = listdir(DATA_DIR / 'times_database')
    for file_name in times_files:

        with open(DATA_DIR / 'times_database' / file_name, encoding='utf-8') as file:
            lines = file.readlines()
            times = np.array([int(line.rstrip()) for line in lines])
            times = np.uint(times)
            if file_name.startswith('kafka'):
                kafka_times.append(times)
            if file_name.startswith('rabbitmq'):
                rabbitmq_times.append(times)

    kafka_averages = np.uint([times.mean() for times in kafka_times])
    kafka_worst = kafka_averages.argmax()
    kafka_best = kafka_averages.argmin()
    # plot_time(kafka_times[kafka_best], "kafka")

    rabbitmq_averages = np.uint([times.mean() for times in rabbitmq_times])
    rabbitmq_worst = rabbitmq_averages.argmax()
    rabbitmq_best = rabbitmq_averages.argmin()

    # plot_time(rabbitmq_times[rabbitmq_best], "rabbitqm")
    plot_average(kafka_averages, rabbitmq_averages)
    return

    # packets = list(range(len(times)))
    # color = (0, random(), 0)
    # plt.scatter(packets, rabbitmq_times[rabbitmq_best], color=color, alpha=0.5)

    # packets = list(range(len(times)))
    # color = (0, random(), 0)
    # plt.scatter(packets, rabbitmq_times[rabbitmq_worst], color=color, alpha=0.5)

    # plt.legend(["Best", "Worst"])


if __name__ == "__main__":
    main()
