#include "Command.h"
#include "Server.h"
#include <string>
#include <SFML/System.hpp>
#include <iostream>
#include <sstream>

Command::Command(Server &server)
{
	this->server = &server;

	std::cout << "Server command list /help" << std::endl;

	sf::Thread listener(&Command::CommandListener, this);
	listener.launch();
}

void Command::CommandListener()
{
	while (true)
	{
		std::getline(std::cin, this->rawCommand);
		
		if (this->rawCommand.length() > 0)
		{

			std::stringstream str(this->rawCommand);
			std::string temp;

			while (std::getline(str, temp, ' ')) {
				command.push_back(temp);
			}

			if (command.at(0) == "/help")
			{
				Command::commandList();
			}
			else if (command.at(0) == "/players")
			{
				Command::playerList();
			}
			else if (command.at(0) == "/ban")
			{
				if (command.size() >= 2)
				{
					Command::banPlayer(command.at(1));
				}
				else
				{
					std::cout << "Syntax error: /ban Username" << std::endl;
				}
			}
			else if (command.at(0) == "/pardon")
			{
				if (command.size() >= 2)
				{
					Command::pardonPlayer(command.at(1));
				}
				else
				{
					std::cout << "Syntax error: /pardon Username" << std::endl;
				}
			}
			else if (command.at(0) == "/kick")
			{
				if (command.size() >= 2)
				{
					Command::pardonPlayer(command.at(1));
				}
				else
				{
					std::cout << "Syntax error: /kick Username" << std::endl;
				}
			}

			command.clear();

		}
	}
}

void Command::commandList()
{
	std::cout << "Server commands:" << std::endl;
	std::cout << "/help -> Display server commands" << std::endl;
	std::cout << "/players -> Display online players" << std::endl;
	std::cout << "/ban Username -> Ban player" << std::endl;
	std::cout << "/pardon Username -> Unban player" << std::endl;
	std::cout << "/kick Username -> Kick player" << std::endl;
}

void Command::playerList()
{
	std::cout << server->getPlayerCount() << " players online" << std::endl;
	server->printAllPlayers();
}

void Command::pardonPlayer(std::string &username)
{
	server->pardonPlayer(username);
}

void Command::kickPlayer(std::string &username)
{
	Player *player = server->findPlayerByUsername(username);

	if (player != nullptr)
	{
		std::cout << player->getUsername() << " [" << player->getId() << "] " << "kicked!" << std::endl;
		server->disconnectPlayer(*player);
	}
	else
	{
		std::cout << "Player " << username << " not found!" << std::endl;
	}
}

void Command::banPlayer(std::string &username)
{

	Player *player = server->findPlayerByUsername(username);

	if (player != nullptr)
	{
		std::cout << player->getUsername() << " [" << player->getId() << "] " << "banned from server!" << std::endl;
		server->banPlayer(*player);
	}
	else
	{
		std::cout << "Player " << username << " not found!" << std::endl;
	}

}
