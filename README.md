# SED

sudo service docker start

## RabbitMQ

- In ```consumer/src/main.cpp``` and ```producer/src/main.cpp``` change the object of the BrokerConsumer and BrokerProducer interfaces to the corresponding RabbitMQ implementations.
- Build the consumer image.
```
cd consumer
sudo docker build -t rabbitmq-gcc-consumer .
```
- Run the containers.
```
sudo docker compose -f docker-compose_rabbitmq.yaml up --build --force-recreate
```
- To delete files inside the volume folders:
```
sudo docker exec -it rabbitmq-consumer1 /bin/bash
rm data/*
```
```
sudo docker exec -it rabbitmq-consumer2 /bin/bash
rm data/*
```


## Kafka

- In ```consumer/src/main.cpp``` and ```producer/src/main.cpp``` change the object of the BrokerConsumer and BrokerProducer interfaces to the corresponding Kafka implementations.
- Build the consumer image.
```
cd consumer
sudo docker build -t kafka-gcc-consumer .
```
- Run the containers.
```
sudo docker compose -f docker-compose.yaml up --build --force-recreate
```
- To delete files inside the volume folders:
```
sudo docker exec -it kafka-consumer1 /bin/bash
rm data/*
```
```
sudo docker exec -it kafka-consumer2 /bin/bash
rm data/*
```