# Módulo de Aquisição de Dados

Módulo de aquisição dos dados via SV implementado em C++.

## Rede
Para utilização do docker é necessário criar uma network que permite o forwarding de pacotes de camada L2 e L3. Para isso, é necessário habilitar o forwarding pelo sistema operacional (linux only):

```bash
   # as root
   sysctl net.ipv4.conf.all.forwarding=1
   iptables -P FORWARD ACCEPT
```

Criar as redes para o docker:

```bash
# as root
docker network create -d macvlan -o parent=enp1s0 process_bus_interface
docker network create -d bridge -o parent=enp1s0 station_bus_interface
```
Obs.: trocar **enp1s0** pela sua interface de rede se for necessário. As interfaces de rede ativas podem ser consultadas pelo comando **ip addr**. Se a interface já estiver em uso, você pode encontrar a *network* responsável com o comando `docker network ls` e removê-la com o `docker network rm [NOME_DA_NETWORK]`.

## Criação do container

Para criar o container e adicionar corretamente as redes para seu funcionamento, deve-se utilizar os seguintes comandos:

```bash
   # as root
   docker build -t mad .
   docker create --network=process_bus_interface --name mad --privileged=true -it -v "$PWD":/acquisition_module mad
   docker network connect station_bus_interface mad

```
## Execução

Por fim, para executar o container, basta executar:

```bash
   sudo docker restart mad
```
