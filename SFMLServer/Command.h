#pragma once
#include <string>
#include "Player.h"
#include "Server.h"
#include <vector>

class Command
{
	private:
		std::string rawCommand;
		std::vector<std::string> command;
		Server *server;

	public:

		Command(Server &server);

		void CommandListener();

		void commandList();
		void playerList();
		void banPlayer(std::string &username);
		void pardonPlayer(std::string &username);
		void kickPlayer(std::string &username);
};