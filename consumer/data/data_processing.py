
from datetime import datetime
import os
from pathlib import Path
from pprint import pprint
from typing import List, Optional
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

IMAGE_DIR = Path(__file__).parent / "images"
DATA_DIR = Path(__file__).parent / "times_database" / "vostro"


def convert_data_to_csv(
    test_type: str = "rabbitmq_2consumer",
    source_path: Path = DATA_DIR / "rabbitmq_2consumer",
    dest_path: Path = DATA_DIR / "rabbitmq_2consumer",
):
    files = source_path.glob("*.txt")
    data = {"test": [], "packet": [], "time": []}
    for test, file in enumerate(files):
        if test == 100:
            break
        times = file.read_text().split("\n")
        for packet, time in enumerate(times):
            if time:
                data["test"].append(test)
                data["packet"].append(packet)
                data["time"].append(int(time))
    now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    pd.DataFrame(data).to_csv(dest_path / f"results_{now}.csv")


def plot_hists(
    test_configs: List[tuple],
    legend: List[str],
    colors: List[tuple],
    time_limit=None,
):
    for i, test_config in enumerate(test_configs):
        broker, consumers, config = test_config
        test_str = f"{broker}_{consumers}consumer_{config}"
        file = sorted(list((DATA_DIR / test_str).glob("**/*.csv")), key=lambda p: p.stem, reverse=True)[0]
        df = pd.read_csv(file)
        time_limit = df["time"].mean() + df["time"].std() if time_limit is None else time_limit
        times = df['time']
        times = times[times < time_limit]
        # plt.clf()
        # ax = times.hist(bins=200)
        sns.kdeplot(times, fill=True, color=colors[i])

    plt.xlabel("Tempo (us)")
    plt.ylabel("Densidade")
    plt.legend(legend)
    # plt.title(broker.capitalize())
    plt.xlim((0, time_limit))
    plt.savefig(IMAGE_DIR / f"times_hists.png")


def plot_hist(
    consumers: int,
    broker: str,
    config: str,
    time_limit=None,
):
    test_str = f"{broker}_{consumers}consumer_{config}"
    file = sorted(list((DATA_DIR / test_str).glob("**/*.csv")), key=lambda p: p.stem, reverse=True)[0]
    df = pd.read_csv(file)
    time_limit = df["time"].mean() + df["time"].std() if time_limit is None else time_limit
    times = df['time']
    times = times[times < time_limit]
    plt.clf()
    # ax = times.hist(bins=200)
    sns.kdeplot(times, fill=True)
    plt.xlabel("Tempo (us)")
    plt.legend([f"{consumers} consumidores"])
    plt.title(broker.capitalize())
    plt.xlim((0, time_limit))
    plt.savefig(IMAGE_DIR / f"times_hist_{test_str}.png")


def plot_average(
    kafka_averages: np.ndarray,
    rabbitmq_averages: np.ndarray,
    test_type: str,
    title: str,
    rmq_color=(0, 0.5, 0),
    kafka_color=(0.5, 0, 0)
):

    max_tests = min([len(rabbitmq_averages), len(kafka_averages)])
    max_tests = min(max_tests, 100)
    rabbitmq_averages = rabbitmq_averages[:max_tests]
    kafka_averages = kafka_averages[:max_tests]

    packets = list(range(max_tests))
    plt.clf()
    plt.scatter(packets, rabbitmq_averages, color=rmq_color, alpha=0.5)
    plt.plot(packets, rabbitmq_averages.mean() * np.ones((len(packets))), color=rmq_color)
    plt.scatter(packets, kafka_averages, color=kafka_color, alpha=0.5)
    plt.plot(packets, kafka_averages.mean() * np.ones((len(packets))), color=kafka_color)
    plt.legend(["RabbitMQ", "Média RabbitMQ", "Kafka", "Média Kafka"])
    plt.xlabel("Teste")
    plt.ylim(0, 2000)
    plt.ylabel("Tempo (us)")
    plt.title(title)
    plt.savefig(IMAGE_DIR / f"compare_avgs_{test_type}.png")
    # plt.show()


def plot_time(time: np.ndarray, filename: str):

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


def load_data(test_type: str) -> List[np.uint]:

    path = DATA_DIR / test_type
    times_file = sorted(list(path.glob("**/*.csv")), key=lambda p: p.stem, reverse=True)[0]

    time_lists = []
    df = pd.read_csv(times_file)
    for group, data in df.groupby("test"):
        time_lists.append(np.uint(list(data['time'])))

    return time_lists


def compare_kafka_with_rabbitmq(
    test_type: str = '2consumer',
    kafka_config: str = 'lowlatency',
    plot=False
):

    kafka_test = f'kafka_{test_type}_{kafka_config}'
    rabbitmq_test = f'rabbitmq_{test_type}_default'

    kafka_times: list[np.uint] = []
    rabbitmq_times: list[np.uint] = []

    kafka_times = load_data(kafka_test)
    rabbitmq_times = load_data(rabbitmq_test)

    kafka_averages = np.uint([times.mean() for times in kafka_times])
    kafka_worst = kafka_averages.argmax()
    kafka_best = kafka_averages.argmin()
    # plot_time(kafka_times[kafka_best], "kafka")

    rabbitmq_averages = np.uint([times.mean() for times in rabbitmq_times if times.mean() < 100000])
    rabbitmq_worst = rabbitmq_averages.argmax()
    rabbitmq_best = rabbitmq_averages.argmin()

    kafka_averages = remove_outliers(kafka_averages)
    rabbitmq_averages = remove_outliers(rabbitmq_averages)

    print("-"*20)
    print(test_type)
    print(f"Kafka {kafka_config}: {kafka_averages.mean()}")
    print(f"RabbitMQ: {rabbitmq_averages.mean()}")

    if plot:
        # plot_time(rabbitmq_times[rabbitmq_worst], "rabbitqm")
        title = "Low Latency Kafka" if kafka_config == "lowlatency" else "Default Kafka"
        kafka_color = (0.5, 0, 0) if kafka_config == "default" else (0.8, 0.3, 0)
        plot_average(kafka_averages, rabbitmq_averages, f"{test_type}_{kafka_config}", title, kafka_color=kafka_color)

    return rabbitmq_averages.mean(), kafka_averages.mean()


def remove_outliers(values: np.ndarray) -> np.ndarray:
    low_q = np.quantile(values, 0.05)
    high_q = np.quantile(values, 0.95)

    return np.array([v for v in values if v > low_q and v < high_q])


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
    # test_types = [
    #     ("kafka", 1, "default"),
    #     ("kafka", 1, "lowlatency"),
    #     ("kafka", 2, "lowlatency"),
    #     ("rabbitmq", 1, "default"),
    #     ("rabbitmq", 2, "default"),
    # ]
    # for test_type in test_types:
    #     broker, consumers, config = test_type
    #     test_type = f"{broker}_{consumers}consumer_{config}"
    #     convert_data_to_csv(test_type)
    #     plot_hist(consumers, broker, test_type)
    compare_kafka_with_rabbitmq()


if __name__ == "__main__":
    main()
