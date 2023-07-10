
from pathlib import Path
from consumer.data.data_processing import compare_kafka_with_rabbitmq, convert_data_to_csv, plot_hist, plot_hists
import pandas as pd


def save_data(test_type: tuple):

    broker, consumers, config = test_type
    test_str = f"{broker}_{consumers}consumer_{config}"

    source_path = Path(f"{broker}_data/consumer1/data")
    source_path.mkdir(parents=True, exist_ok=True)
    dest_path = Path(f"consumer/data/times_database/vostro/{test_str}/consumer1")
    dest_path.mkdir(parents=True, exist_ok=True)
    convert_data_to_csv(f"{broker}_{consumers}consumer_{config}", source_path, dest_path)

    if consumers == 2:
        source_path = Path(f"{broker}_data/consumer2/data")
        source_path.mkdir(parents=True, exist_ok=True)
        dest_path = Path(f"consumer/data/times_database/vostro/{test_str}/consumer2")
        dest_path.mkdir(parents=True, exist_ok=True)
        convert_data_to_csv(f"{broker}_{consumers}consumer_{config}", source_path, dest_path)


def main():
    # test_type = ("kafka", 1, "default")
    # test_type = ("kafka", 1, "lowlatency")
    test_type = ("kafka", 2, "default")
    # test_type = ("kafka", 2, "lowlatency")
    # test_type = ("rabbitmq", 1, "default")
    # test_type = ("rabbitmq", 2, "default")
    # save_data(test_type)
    # return

    # plot_hists(
    #     [("rabbitmq", 1, "default"), ("kafka", 1, "default"), ("kafka", 1, "lowlatency")],
    #     # [("rabbitmq", 2, "default"),  ("kafka", 2, "default"), ("kafka", 2, "lowlatency")],
    #     ["RabbitMQ", "Kafka padrão", "Kafka baixa latência"],
    #     [(0, 0.5, 0), (0.5, 0, 0), (0.8, 0.3, 0)]
    # )
    # return

    test_types = [
        ("kafka", 1, "default"),
        ("kafka", 1, "lowlatency"),
        ("kafka", 2, "lowlatency"),
        ("kafka", 2, "default"),
        ("rabbitmq", 1, "default"),
        ("rabbitmq", 2, "default"),
    ]
    times_dict = {"consumers": [], "kafka_config": [], "kafka_time": [], "rabbitmq_time": []}
    for test_type in test_types:
        broker, consumers, config = test_type
        test_str = f"{broker}_{consumers}consumer_{config}"
        print(test_str)
        rmq_time, kafka_time = compare_kafka_with_rabbitmq(f"{consumers}consumer", config)
        times_dict["consumers"].append(consumers)
        times_dict["kafka_config"].append(config)
        times_dict["rabbitmq_time"].append(rmq_time)
        times_dict["kafka_time"].append(kafka_time)

    pd.DataFrame(times_dict).to_csv("consumer/data/times.csv")
    # plot_hist(consumers, broker, config, 2000)


if __name__ == "__main__":
    main()
