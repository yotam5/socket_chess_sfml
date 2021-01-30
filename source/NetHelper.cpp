#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <msgpack.hpp>
#include <chrono>
#include <thread>
#include <pthread.h>

#define PORT 2002

class NetHelper
{
public:
    NetHelper()
    {  
        //NOTE: EXEXTURE BEFORE INITALIZE GAME
        std::cout << "Enter s for server, c for client:" << std::endl;
        std::cin >> connectionType;
        this->init();
        threadPointer = new std::thread(&NetHelper::getMsg<std::string>, this);
    }
    ~NetHelper() { delete this->threadPointer; }

    template <typename T>
    void sendMsg(T value)
    {
        std::stringstream buffer1;
        msgpack::pack(buffer1, value);
        std::string strValue = buffer1.str();
        sf::Packet packet;
        packet << strValue;
        socket.send(packet);
    }

    template <typename T>
    void getMsg()
    {
        while (true)
        {
            if (socket.receive(this->packet) == sf::TcpSocket::Done)
            {
                this->fullyRecive = true;
                msgpack::object_handle oh1;
                std::string str;
                this->packet >> str;
                this->lastData = str;
            }
        }
    }
    //return std::make_pair(false, msgpack::object(NULL));
    std::string getMsgCopy() const
    {
        return this->lastData;
    }

    bool getFullyRecive() const
    {
        return this->fullyRecive;
    }

    void setFullyRecive(bool value)
    {
        this->fullyRecive = value;
    }

    template <typename T>
    T convertTo(std::string str)
    {
        auto oh1 = msgpack::unpack(str.data(), str.size());
        msgpack::object deserialized = oh1.get();
        T tmp = deserialized.as<T>();
        return tmp;
    }

private:
    std::thread *threadPointer = nullptr;
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    sf::TcpSocket socket;
    char connectionType, mode;
    sf::Packet packet;
    bool fullyRecive = false;
    std::string text = "connected";
    std::string lastData;
    void init()
    {
        if (connectionType == 's')
        {
            sf::TcpListener listner;
            listner.listen(PORT);
            listner.accept(socket);
        }
        else
        {
            socket.connect(ip, PORT);
        }
    }
};
