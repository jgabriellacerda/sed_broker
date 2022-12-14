# SED

sudo service docker start

## RabbitMQ

sudo docker-compose -f docker-compose_rabbitmq.yaml up --build --force-recreate

## Kafka

sudo docker-compose -f docker-compose.yaml up --build --force-recreate

sudo docker exec -it thirdlayer1 /bin/bash