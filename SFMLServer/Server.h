#pragma once
#include <string>
#include <fstream>
#include "Player.h"
#include <SFML/Network.hpp>

class Server 
{
	private:
		const unsigned short *MAX_PLAYERS;
		Player *players;
		std::string *name;
		const unsigned short *port;
		sf::UdpSocket *socket;
		sf::Thread *receive;
		const double version = 0.11;

	public:

		Server();
		
		void onReceiveMessage();

		void createTimer();

		void onPlayerConnect(sf::Packet &packet, sf::IpAddress &receivedAddress, unsigned short &receivedPort);

		void onPlayerUpdate(sf::Packet &packet);

		void onPlayerDisconnect(sf::Packet &packet, sf::IpAddress &receivedAddress, unsigned short &receivedPort);

		void disconnectPlayer(Player &player);

		Player* findPlayerByUsername(std::string &username);

		bool isIpBanned(sf::IpAddress &receivedAddress);

		void banPlayer(Player &player);

		void pardonPlayer(std::string &username);

		void printAllPlayers();

		bool isFull();

		int findEmptyId();

		void sendResponse(Player &player);

		void sendPlayersToPlayer(Player &player);

		void sendServerToList();

		void updateServerList();

		int getPlayerCount();
};