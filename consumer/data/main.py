
import os
from pathlib import Path
from pprint import pprint
from typing import List, Optional
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
    plt.savefig(DATA_DIR / f"compare_avgs_{test_type}.png")
    plt.show()


def plot_time(time: np.uint, filename: str):

    ylim = time.mean()*2
    packets = list(range(len(time)))
    color = (0, 0.5, 0)
    plt.scatter(packets, time, color=color, alpha=0.3)
    plt.plot(packets, time.mean() *
             np.ones((len(packets))), color=color)
    plt.legend(["Times"])
    plt.xlabel("Packet")
    plt.ylim(0, ylim)
    plt.ylabel("Time (us)")
    plt.savefig(DATA_DIR / f"{filename}.png")
    plt.show()


def load_data(data_dir, startswith: Optional[str] = None) -> List[np.uint]:

    path = DATA_DIR / 'times_database' / data_dir
    times_files = [file for file in os.listdir(path)
                   if os.path.isfile(os.path.join(path, file))]

    if startswith:
        times_files = [
            file for file in times_files if file.startswith(startswith)]

    times_list = []

    for file_name in times_files:

        with open(path / file_name, encoding='utf-8') as file:
            lines = file.readlines()
            times = np.array([int(line.rstrip()) for line in lines])
            times = np.uint(times)
            times_list.append(times)

    return times_list


def compare_kafka_with_rabbitmq():
    test_type = '2consumer'
    kafka_config = 'low'

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


def plot_single_time():

    img_dir = 'vostro/kafka_2consumer_low'

    consumer1_times: list[np.uint] = load_data(
        img_dir, 'kafka_consumer_consumer1')
    consumer2_times: list[np.uint] = load_data(
        img_dir, 'kafka_consumer_consumer2')

    consumer1_averages = np.uint([times.mean() for times in consumer1_times])
    consumer1_worst = consumer1_averages.argmax()
    consumer1_best = consumer1_averages.argmin()

    img_dir = f'vostro/kafka_1consumer_lowlatency'

    kakfa_singleconsumer_times: list[np.uint] = load_data(
        img_dir, 'kafka_consumer')
    kafkasingle_averages: List[np.uint] = [times.mean()
                                           for times in kakfa_singleconsumer_times]
    kafkasingle_avgs_array = np.uint(kafkasingle_averages)
    kafkasingle_worst = kafkasingle_avgs_array.argmax()
    kafkasingle_best = kafkasingle_avgs_array.argmin()

    # plot_time(consumer1_times[consumer1_worst], "kafka_2consumer_worst")
    # plot_time(consumer1_times[consumer1_best], "kafka_2consumer_best")
    plot_time(
        kakfa_singleconsumer_times[kafkasingle_best], "kafka_1consumer_best")


def main():
    # plot_single_time()
    compare_kafka_with_rabbitmq()


if __name__ == "__main__":
    main()
