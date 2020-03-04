#include <iostream>
#include <SFML/Network.hpp>
#include "Player.h"

void Player::setData(int &id, sf::IpAddress &ip, unsigned short &port, std::string &username, int &r, int &g, int &b)
{
	this->id = id;
	this->ip = ip;
	this->port = port;
	this->username = username;
	this->r = r;
	this->g = g;
	this->b = b;
}

void Player::remove()
{
	this->id = -1;
}

int Player::getId()
{
	return this->id;
}

sf::IpAddress Player::getIp()
{
	return this->ip;
}

unsigned short Player::getPort()
{
	return this->port;
}

std::string Player::getUsername()
{
	return this->username;
}

int Player::getR()
{
	return this->r;
}

int Player::getG()
{
	return this->g;
}

int Player::getB()
{
	return this->b;
}