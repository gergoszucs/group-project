#pragma once

#include <QStringList>
#include <vector>

class UndoRedoSystem
{
private:
	int commandPointer;

	std::vector<QStringList> commands;
	std::vector<QStringList> reverseCommands;

public:
	UndoRedoSystem();

	void registerCommand(QStringList command, QStringList reverseCommand);

	void undo();
	void redo();
};