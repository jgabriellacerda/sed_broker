
#include <broker.h>

using namespace std;

KafkaProducer::KafkaProducer(Configuration config) : producer(config)
{
    this->broker_name = "kafka_producer";
}

void KafkaProducer::publish(string message)
{
    this->producer.produce(
        MessageBuilder("my_topic").partition(0).payload(message));
    this->producer.flush();
}

KafkaConsumer::KafkaConsumer(Configuration config, string topic) : consumer(config)
{
    this->broker_name = "kafka_consumer";
    consumer.set_assignment_callback([&](TopicPartitionList &topic_partitions)
                                     { cout << "Got assigned " << topic_partitions.size() << " partitions!" << endl; });
    consumer.set_revocation_callback([&](const TopicPartitionList &topic_partitions)
                                     { cout << topic_partitions.size() << " partitions revoked!" << endl; });
    consumer.subscribe({topic});
}

string KafkaConsumer::listen()
{
    json j_message;
    while (true)
    {
        Message msg = consumer.poll();
        if (msg)
        {
            if (!msg.get_error())
            {
                return msg.get_payload();
            }
            else if (!msg.is_eof())
            {
            }
        }
    }
}

RabbitMQProducer::RabbitMQProducer(string host, int port, string username, string password, string svdata_exchange_name, string key)
{
    this->broker_name = "rabbitmq_producer";
    this->amqp_host = host;
    this->amqp_port = port;
    this->amqp_username = username;
    this->amqp_password = password;
    this->svdata_exchange_name = svdata_exchange_name;
    this->routing_key = key;
    this->amqpConnect();
}

void RabbitMQProducer::amqpConnect()
{
    int num_attempts = 20;
    while (num_attempts--)
    {
        try
        {
            this->connection = AmqpClient::Channel::Create(this->amqp_host, this->amqp_port, this->amqp_username, this->amqp_password);
            this->connection->DeclareExchange(this->svdata_exchange_name, Channel::EXCHANGE_TYPE_DIRECT);
            cout << "Conexão com RabbitMQ realizada com sucesso." << endl;
            break;
        }
        catch (exception &e)
        {
            cout << "Erro na conexão com o RabbitMQ:" << e.what() << " Tentativas restantes: " << num_attempts << endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void RabbitMQProducer::publish(string message)
{
    BasicMessage::ptr_t message_str = BasicMessage::Create(message);
    connection->BasicPublish(this->svdata_exchange_name, this->routing_key, message_str);
}

RabbitMQConsumer::RabbitMQConsumer(
    string host,
    int port,
    string username,
    string password,
    string queue_name,
    string svdata_exchange_name,
    string routing_key)
{
    this->broker_name = "rabbitmq_consumer";
    this->amqp_host = host;
    this->amqp_port = port;
    this->amqp_username = username;
    this->amqp_password = password;
    this->queue_name = queue_name;
    this->svdata_exchange_name = svdata_exchange_name;
    this->routing_key = routing_key;
    this->amqpConnect();
}

void RabbitMQConsumer::amqpConnect()
{
    int num_attempts = 20;
    while (num_attempts--)
    {
        try
        {
            this->connection = AmqpClient::Channel::Create(
                this->amqp_host, this->amqp_port, this->amqp_username, this->amqp_password);
            this->connection->DeclareQueue(this->queue_name, false, false, false, true);
            this->connection->BindQueue(this->queue_name, this->svdata_exchange_name, this->routing_key);
            this->consumer_tag = this->connection->BasicConsume(this->queue_name, "", true, true, false);
            cout << "Conexão com RabbitMQ realizada com sucesso." << endl;
            break;
        }
        catch (exception &e)
        {
            cout << "Erro na conexão com o RabbitMQ:" << e.what() << " Tentativas restantes: " << num_attempts << endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

string RabbitMQConsumer::listen()
{
    Envelope::ptr_t envelope = this->connection->BasicConsumeMessage(this->consumer_tag);
    BasicMessage::ptr_t bodyBasicMessage = envelope->Message();
    return bodyBasicMessage->Body();
}