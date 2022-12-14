
import os
from pathlib import Path
from pprint import pprint
import matplotlib.pyplot as plt
import numpy as np

DATA_DIR = Path(__file__).parent


def plot_average(kafka_averages: np.ndarray, rabbitmq_averages: np.ndarray,
                 test_type: str, title: str):

    max_tests = min([len(rabbitmq_averages), len(kafka_averages)])
    max_tests = max_tests if max_tests < 100 else 100
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
    plt.ylim(0, 4000)
    plt.ylabel("Tempo (us)")
    plt.title(title)
    plt.savefig(DATA_DIR / f"compare_evarages_{test_type}.png")
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


def load_data(dir):

    path = DATA_DIR / 'times_database' / dir
    times_files = (file for file in os.listdir(path)
                   if os.path.isfile(os.path.join(path, file)))

    times_list = []

    for file_name in times_files:

        with open(path / file_name, encoding='utf-8') as file:
            lines = file.readlines()
            times = np.array([int(line.rstrip()) for line in lines])
            times = np.uint(times)
            times_list.append(times)

    return times_list


def main():
    # print(matplotlib.get_backend())

    test_type = '1consumer'
    kafka_config = 'lowlatency'

    kafka_dir = f'vostro/kafka_{test_type}_{kafka_config}'
    rabbitmq_dir = f'vostro/rabbitmq_{test_type}'

    kafka_times: list[np.uint] = []
    rabbitmq_times: list[np.uint] = []

    kafka_times = load_data(kafka_dir)
    rabbitmq_times = load_data(rabbitmq_dir)

    kafka_averages = np.uint([times.mean() for times in kafka_times])
    kafka_worst = kafka_averages.argmax()
    kafka_best = kafka_averages.argmin()
    # plot_time(kafka_times[kafka_best], "kafka")

    rabbitmq_averages = np.uint(
        [times.mean() for times in rabbitmq_times if times.mean() < 100000])
    rabbitmq_worst = rabbitmq_averages.argmax()
    rabbitmq_best = rabbitmq_averages.argmin()

    # plot_time(rabbitmq_times[rabbitmq_worst], "rabbitqm")
    plot_average(kafka_averages, rabbitmq_averages,
                 test_type, "Low Latency Kafka")
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
