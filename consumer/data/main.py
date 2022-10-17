
from os import listdir
from pathlib import Path
from pprint import pprint
from random import randint, random
from typing import List
import matplotlib.pyplot as plt
import numpy as np

DATA_DIR = Path(__file__).parent


def main():
    # print(matplotlib.get_backend())

    kafka_times: List[List[int]] = []
    rabbitmq_times: List[List[int]] = []

    times_files = listdir(DATA_DIR / 'times_database')
    for file_name in times_files:

        with open(DATA_DIR / 'times_database' / file_name, encoding='utf-8') as file:
            lines = file.readlines()
            times = [line.rstrip() for line in lines]
            times = np.uint(times)
            if file_name.startswith('kafka'):
                kafka_times.append(times)
            if file_name.startswith('rabbitmq'):
                rabbitmq_times.append(times)

    kafka_averages = np.uint([times.mean() for times in kafka_times])
    rabbitmq_averages = np.uint([times.mean() for times in rabbitmq_times])
    rabbitmq_worst = rabbitmq_averages.argmax()
    rabbitmq_best = rabbitmq_averages.argmin()

    # packets = list(range(len(times)))
    # color = (0, random(), 0)
    # plt.scatter(packets, rabbitmq_times[rabbitmq_best], color=color, alpha=0.5)

    # packets = list(range(len(times)))
    # color = (0, random(), 0)
    # plt.scatter(packets, rabbitmq_times[rabbitmq_worst], color=color, alpha=0.5)

    # plt.legend(["Best", "Worst"])

    max_tests = min([len(rabbitmq_averages), len(kafka_averages)])
    rabbitmq_averages = rabbitmq_averages[:max_tests]
    kafka_averages = kafka_averages[:max_tests]

    packets = list(range(max_tests))
    rmq_color = (0, 0.5, 0)
    kafka_color = (0.5, 0, 0)
    plt.scatter(packets, rabbitmq_averages, color=rmq_color, alpha=0.5)
    plt.plot(packets, rabbitmq_averages.mean() * np.ones((len(packets))), color=rmq_color)
    plt.scatter(packets, kafka_averages, color=kafka_color, alpha=0.5)
    plt.plot(packets, kafka_averages.mean() * np.ones((len(packets))), color=kafka_color)
    plt.legend(["RabbitMQ", "Média RabbitMQ", "Kafka", "Média Kafka"])
    plt.xlabel("Teste")
    plt.ylim(0)

    # for times in kafka_times:
    #     packets = list(range(len(times)))
    #     plt.scatter(packets, times, color='steelblue', alpha=0.5)
    # for times in rabbitmq_times:
    #     packets = list(range(len(times)))
    #     color = (0, random(), 0)
    #     plt.scatter(packets, times, color=color, alpha=0.5)

    # plt.plot(x, kafka_times.mean() * np.ones((len(x))), color='blue')
    # plt.scatter(x, rabbitmq_times, color="greenyellow")
    # plt.plot(x, rabbitmq_times.mean() * np.ones((len(x))), color='green')
    # plt.xlabel("Pacote")
    plt.ylabel("Tempo (us)")
    # plt.legend(["Kafka", "Média Kafka", "RabbitMQ", "Média RabbitMQ"])
    # plt.legend(["Kafka", "Média Kafka", "RabbitMQ", "Média RabbitMQ"])
    plt.savefig(DATA_DIR / "figure.png")
    plt.show()


if __name__ == "__main__":
    main()
