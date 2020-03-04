#include <iostream>
#include "Server.h"
#include "Command.h"
#include "Timer.h"
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <iomanip>

using nlohmann::json;

enum
{
	CONNECT = 0,
	DISCONNECT = 1,
	UPDATE = 2,
	CONNECTED = 3,
	DISCONNECTED = 4
};

Server::Server()
{
	std::ifstream file("server.cfg");

	if (file.is_open()) 
	{

		std::string line;

		while (getline(file, line)) 
		{
			if (line.find("=")) 
			{

				std::string key, value;
				key = line.substr(0, line.find("="));
				value = line.substr(line.find("=") + 1);

				if (key == "server-name")
				{
					this->name = new std::string(value);
				}

				if (key == "server-port")
				{
					this->port = new const unsigned short(std::stoi(value));
				}

				if (key == "max-players") 
				{
					this->MAX_PLAYERS = new const unsigned short(std::stoi(value));
					this->players = new Player[*MAX_PLAYERS];
				}

			}
		}

		file.close();
		std::cout << "Server configuration loaded!" << std::endl;

		socket = new sf::UdpSocket;
		if (socket->bind(*this->port, sf::IpAddress::Any) != sf::Socket::Done)
		{
			if (sf::Socket::Error)
			{
				std::cout << "Error! Port already in use!" << std::endl;
				system("pause");
				exit(0);
			}
		}

		receive = new sf::Thread(&Server::onReceiveMessage, this);
		receive->launch();

		sf::Thread command(&Server::createTimer, this);
		command.launch();

		Server::sendServerToList();

		Command(*this);
	}
	else 
	{
		std::cout << "Server configuration file not found!" << std::endl;
	}
}

void Server::createTimer()
{
	//Create timer
	unsigned long dt = 6; //seconds
	Timer t;
	t.start();

	//Update server info
	while (true)
	{
		if (!(t.elapsedTime() < dt))
		{
			Server::updateServerList();
			t.reset(); //reset the timer
		}
	}
}

void Server::onReceiveMessage()
{

	sf::IpAddress receivedAddress;
	sf::Packet packet;
	unsigned short receivedPort;
	int action;

	std::cout << "Server started on port: " << socket->getLocalPort() << std::endl;

	while (true) 
	{
		packet.clear();
		socket->receive(packet, receivedAddress, receivedPort);

		packet >> action;

		switch (action)
		{
			case CONNECT:
				onPlayerConnect(packet, receivedAddress, receivedPort);
			break;

			case DISCONNECT:
				onPlayerDisconnect(packet, receivedAddress, receivedPort);
			break;

			case UPDATE:
				onPlayerUpdate(packet);
			break;
		}
	}

}

bool Server::isFull()
{

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() == -1)
			return false;
	}

	return true;
}

int Server::getPlayerCount()
{
	int count = 0;

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != -1)
			count++;
	}

	return count;
}

int Server::findEmptyId()
{
	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() == -1)
			return i;
	}

	return -1;
}

void Server::sendResponse(Player &player)
{
	sf::Packet response;
	response << CONNECT << player.getId();

	socket->send(response, player.getIp(), player.getPort());
}

void Server::sendPlayersToPlayer(Player &player)
{
	sf::Packet response;
	response << CONNECTED << player.getId() << player.getUsername() << player.getR() << player.getG() << player.getB();
	
	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if(players[i].getId() != player.getId() && players[i].getId() != -1)
			socket->send(response, players[i].getIp(), players[i].getPort());
	}

	response.clear();

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != player.getId() && players[i].getId() != -1)
		{
			response << CONNECTED << players[i].getId() << players[i].getUsername() << players[i].getR() << players[i].getG() << players[i].getB();
			socket->send(response, player.getIp(), player.getPort());
		}

	}
}

void Server::sendServerToList()
{
	sf::Http http("http://game-demo.brog.pro/");
	sf::Http::Request request("/add_server.php", sf::Http::Request::Post);

	std::ostringstream stream;
	stream << "name=" << *this->name << "&ip=" << sf::IpAddress::getPublicAddress().toString() << "&port=" << *this->port << "&max=" << *this->MAX_PLAYERS << "&version=" << this->version;
	request.setBody(stream.str());

	sf::Http::Response response = http.sendRequest(request);

	if (response.getStatus() == sf::Http::Response::Ok)
	{
		std::cout << "Server added to list(http://game-demo.brog.pro)" << std::endl;
	}
	else
	{
		std::cout << "request failed" << std::endl;
	}
}

void Server::updateServerList()
{
	sf::Http::Request request("/update_server.php", sf::Http::Request::Post);

	std::ostringstream stream;
	stream << "&ip=" << sf::IpAddress::getPublicAddress().toString() << "&port=" << *this->port << "&players=" << this->getPlayerCount();
	request.setBody(stream.str());

	sf::Http http("http://game-demo.brog.pro/");
	sf::Http::Response response = http.sendRequest(request);

}

void Server::onPlayerConnect(sf::Packet &packet, sf::IpAddress &receivedAddress, unsigned short &receivedPort)
{	
	double version;
	packet >> version;
	if (!isFull() && this->version == version)
	{
		if (!isIpBanned(receivedAddress))
		{
			std::string username;
			int r, g, b;

			packet >> username >> r >> g >> b;
			int id = findEmptyId();

			players[id].setData(id, receivedAddress, receivedPort, username, r, g, b);
			sendResponse(players[id]);
			sendPlayersToPlayer(players[id]);

			std::cout << "Player " << username << "[" << id << "]" << " connected to server!" << players[id].getR() << ", " << players[id].getG() << ", " << players[id].getB() << std::endl;
			Server::updateServerList();
		}
		else
		{
			std::cout << "Ip address is banned! " << receivedAddress.toString() << std::endl;
		}
	}
}

void Server::onPlayerUpdate(sf::Packet &packet)
{
	int id;
	
	packet >> id;

	std::cout << "update: " << id << std::endl;

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != id && players[i].getId() != -1)
		{
			socket->send(packet, players[i].getIp(), players[i].getPort());
		}
	}
}

void Server::onPlayerDisconnect(sf::Packet &packet, sf::IpAddress &receivedAddress, unsigned short &receivedPort)
{
	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getIp() == receivedAddress && players[i].getPort() == receivedPort)
		{
			std::cout << "Player " << players[i].getUsername() << "[" << players[i].getId() << "]" << " disconnected!" << std::endl;
			players[i].remove();
		}
	}

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != -1)
		{
			std::cout << players[i].getIp();
			socket->send(packet, players[i].getIp(), players[i].getPort());
		}
	}

	Server::updateServerList();

}

void Server::disconnectPlayer(Player &player)
{
	sf::Packet disconnect;
	disconnect << DISCONNECTED;

	socket->send(disconnect, player.getIp(), player.getPort());
}

Player* Server::findPlayerByUsername(std::string &username)
{

	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != -1 && players[i].getUsername() == username)
		{
			return &players[i];
		}
	}

	return nullptr;
}



bool Server::isIpBanned(sf::IpAddress &receivedAddress)
{
	std::ifstream i("bans.json");
	json j;
	i >> j;

	i.close();

	for (int i = 0; i < j.size(); i++)
	{
		if (j[i]["ip"] == receivedAddress.toString())
			return true;
	}

	return false;
}

void Server::banPlayer(Player &player)
{
	std::ifstream i("bans.json");
	json j;
	i >> j;
	i.close();

	j.push_back(json::object({ {"username", player.getUsername()}, {"ip", player.getIp().toString()} }));

	Server::disconnectPlayer(player);

	std::ofstream o("bans.json");
	o << std::setw(4) << j << std::endl;
	o.close();
}

void Server::pardonPlayer(std::string &username)
{
	std::ifstream i("bans.json");
	json j;
	i >> j;
	i.close();

	for (int i = 0; i < j.size(); i++)
	{
		if (j[i]["username"] == username)
			j.erase(i);
	}

	std::cout << "Player " << username << " unbanned!" << std::endl;

	std::ofstream o("bans.json");
	o << std::setw(4) << j << std::endl;
	o.close();
}

void Server::printAllPlayers()
{
	for (int i = 0; i < *MAX_PLAYERS; i++)
	{
		if (players[i].getId() != -1)
		{
			std::cout << players[i].getUsername() << " [" << players[i].getId() << "]" << std::endl;
		}
	}
}
