FROM gcc:latest

RUN update-alternatives --install /usr/bin/gfortran gfortran /usr/local/bin/gfortran 999
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -yq cmake tshark libssl-dev libboost-all-dev
RUN wget https://github.com/alanxz/rabbitmq-c/archive/v0.9.0.tar.gz && tar -zxvf v0.9.0.tar.gz
RUN cd rabbitmq-c-0.9.0 && mkdir build && cd build && cmake -DBUILD_STATIC_LIBS=ON  ..  && cmake --build . --target install && rm -rf rabbitmq-c-0.9.0
RUN wget https://github.com/alanxz/SimpleAmqpClient/archive/v2.4.0.tar.gz && tar -zxvf v2.4.0.tar.gz
RUN cd SimpleAmqpClient-2.4.0 && mkdir build && cd build && cmake ..  && cmake --build . --target install && rm -rf  SimpleAmqpClient-2.4.0
RUN wget https://github.com/nlohmann/json/releases/download/v3.6.1/json.hpp

WORKDIR /consumer_module
COPY ./entrypoint.sh /consumer_module
RUN chmod +x /consumer_module/entrypoint.sh
CMD  ./entrypoint.sh
