#pragma once
#include <SFML/Network.hpp>

class Player 
{
	private:
		int id = -1;
		std::string username;
		double x, y;
		int r, g, b;
		sf::IpAddress ip;
		unsigned short port;

	public:

		void setData(int &id, sf::IpAddress &ip, unsigned short &port, std::string &username, int &r, int &g, int &b);
		void remove();

		int getId();
		sf::IpAddress getIp();
		unsigned short getPort();

		std::string getUsername();
		int getR();
		int getG();
		int getB();


};