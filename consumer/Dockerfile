FROM gcc:latest

RUN apt update && update-alternatives --install /usr/bin/gfortran gfortran /usr/local/bin/gfortran 999
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq cmake tshark libssl-dev libboost-all-dev

# Kafka
RUN wget https://github.com/edenhill/librdkafka/archive/master.zip
RUN unzip master.zip && cd librdkafka-master && ./configure --install-deps && make && make install
RUN wget https://github.com/mfontanini/cppkafka/archive/v0.3.1.tar.gz && tar -zxvf v0.3.1.tar.gz
RUN cd cppkafka-0.3.1 && mkdir build && cd build && cmake -DBUILD_STATIC_LIBS=ON ..  && cmake --build . --target install && rm -rf cppkafka-0.3.1

# RabbitMQ
RUN wget https://github.com/alanxz/rabbitmq-c/archive/refs/tags/v0.13.0.tar.gz && tar -zxvf v0.13.0.tar.gz
RUN cd rabbitmq-c-0.13.0 && mkdir build && cd build && cmake -DBUILD_STATIC_LIBS=ON  ..  && cmake --build . --target install && rm -rf rabbitmq-c-0.13.0
RUN wget https://github.com/alanxz/SimpleAmqpClient/archive/v2.4.0.tar.gz && tar -zxvf v2.4.0.tar.gz
RUN cd SimpleAmqpClient-2.4.0 && mkdir build && cd build && cmake ..  && cmake --build . --target install && rm -rf  SimpleAmqpClient-2.4.0

# JSON
RUN wget https://github.com/nlohmann/json/releases/download/v3.6.1/json.hpp

WORKDIR /consumer_module
COPY ./entrypoint.sh /consumer_module
COPY ./src /consumer_module/src
COPY ./inc /consumer_module/inc
RUN mkdir /consumer_module/bin
RUN mkdir /consumer_module/data
RUN mkdir /consumer_module/data/times_database
RUN chmod +x /consumer_module/entrypoint.sh
CMD  ./entrypoint.sh
