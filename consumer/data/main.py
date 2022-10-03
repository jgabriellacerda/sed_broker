
from pathlib import Path
from pprint import pprint
import matplotlib.pyplot as plt
import numpy as np

DATA_DIR = Path(__file__).parent

def main():
    # print(matplotlib.get_backend())
    with open(DATA_DIR / "kafka_consumer_times.txt", encoding='utf-8') as file:
        lines = file.readlines()
        times = [line.rstrip() for line in lines]
        kafka_times = np.uint(times)
    with open(DATA_DIR / "rabbitmq_consumer_times.txt", encoding='utf-8') as file:
        lines = file.readlines()
        times = [line.rstrip() for line in lines]
        rabbitmq_times = np.uint(times)
        # pprint(times)

    x = list(range(len(kafka_times)))
    # print(x)
    plt.scatter(x,kafka_times, color='steelblue')
    plt.plot(x, kafka_times.mean() * np.ones((len(x))), color='blue')
    plt.scatter(x,rabbitmq_times, color="greenyellow")
    plt.plot(x, rabbitmq_times.mean() * np.ones((len(x))), color='green')
    plt.xlabel("Pacote")
    plt.ylabel("Tempo (us)")
    plt.legend(["Kafka", "Média Kafka","RabbitMQ", "Média RabbitMQ"])
    plt.savefig(DATA_DIR / "figure.png")
    plt.show()

if __name__ == "__main__":
    main()