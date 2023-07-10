
from pathlib import Path
from consumer.data.data_processing import compare_kafka_with_rabbitmq, convert_data_to_csv, plot_hist


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

    test_types = [
        ("kafka", 1, "default"),
        ("kafka", 1, "lowlatency"),
        ("kafka", 2, "lowlatency"),
        ("kafka", 2, "default"),
        ("rabbitmq", 1, "default"),
        ("rabbitmq", 2, "default"),
    ]
    for test_type in test_types:
        broker, consumers, config = test_type
        test_str = f"{broker}_{consumers}consumer_{config}"
        print(test_str)
        compare_kafka_with_rabbitmq(f"{consumers}consumer", config)
        plot_hist(consumers, broker, config, 2000)


if __name__=="__main__":
    main()