#pragma once

#include <QStringList>
#include <vector>

class ITProject;
class ITSurface;
class ITControlPoint;

class UndoRedoSystem
{
private:
	int commandPointer;

	std::vector<QStringList> commands;
	std::vector<QStringList> reverseCommands;

	std::vector< std::tuple<unsigned int, unsigned int, std::vector<ITControlPoint*> > > deletedRows;
	std::vector< std::tuple<unsigned int, unsigned int, std::vector<ITControlPoint*> > > deletedColumns;
	std::vector< std::tuple<unsigned int, ITSurface*, ITSurface* > > deletedSurfaces;

public:
	UndoRedoSystem();

	void registerCommand(QStringList command, QStringList reverseCommand);

	void undo();
	void redo();

	void registerRow(unsigned int surfaceID, const unsigned int i, std::vector<ITControlPoint*> row);
	void registerColumn(unsigned int surfaceID, const unsigned int j, std::vector<ITControlPoint*> column);
	void registerSurface(ITSurface* s, ITSurface* bs);

	void redoSurfaceDelete(ITProject* p);
	void redoRowDelete(ITProject* p);
	void redoColumnDelete(ITProject* p);
};