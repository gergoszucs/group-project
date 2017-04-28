#include "undoRedoSystem.h"


#include "global.h"

UndoRedoSystem::UndoRedoSystem()
	: commandPointer(0)
{
}

void UndoRedoSystem::registerCommand(QStringList command, QStringList reverseCommand)
{
	//clean commands from now redundant branch
	if (commandPointer == -1)
	{
		commands.erase(commands.begin(), commands.end());
		reverseCommands.erase(reverseCommands.begin(), reverseCommands.end());
	}
	else if ((commandPointer != commands.size() - 1) && (commands.size() != 0))
	{
		commands.erase(commands.begin() + commandPointer + 1, commands.end());
		reverseCommands.erase(reverseCommands.begin() + commandPointer + 1, reverseCommands.end());
	}

	commands.push_back(command);
	reverseCommands.push_back(reverseCommand);
	commandPointer = commands.size() - 1;
}

void UndoRedoSystem::undo()
{
	if ((commandPointer >= 0) && (commandPointer < reverseCommands.size()))
	{
		w->executeCommand("UNDO", reverseCommands[commandPointer], false);

		commandPointer--;
	}
}

void UndoRedoSystem::redo()
{
	w->setMyTextDataField(QString::number(commandPointer) + " " + QString::number(commands.size() - 1) + " " + ((commandPointer >= -1) ? "true" : "false") + " " + ((commandPointer < commands.size() - 1) ? "true" : "false"));
	if ((commandPointer >= -1) && (commandPointer < (int)(commands.size() - 1)))
	{
		commandPointer++;

		w->executeCommand("REDO", commands[commandPointer], false);\
	}
}
