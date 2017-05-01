#pragma once

#include <QStringList>
#include <vector>

class ITProject;
class ITSurface;
class ITControlPoint;
enum PLANE;

class UndoRedoSystem
{
private:
	int commandPointer;

	std::vector<QStringList> commands;
	std::vector<QStringList> reverseCommands;

	std::vector< std::tuple<unsigned int, unsigned int, std::vector<ITControlPoint*> > > deletedRows;
	std::vector< std::tuple<unsigned int, unsigned int, std::vector<ITControlPoint*> > > deletedColumns;
	std::vector< std::tuple<unsigned int, ITSurface*, ITSurface* > > deletedSurfaces;
	std::vector< std::tuple< std::vector< ITControlPoint* >, float, float, float > > pointGroupsMove;
	std::vector< std::tuple< std::vector< ITControlPoint* >, float, float, float, float, PLANE > > pointGroupsRotate;

public:
	UndoRedoSystem();

	void reset();

	void registerCommand(QStringList command, QStringList reverseCommand);

	void undo();
	void redo();

	void registerRow(unsigned int surfaceID, const unsigned int i, std::vector<ITControlPoint*> row);
	void registerColumn(unsigned int surfaceID, const unsigned int j, std::vector<ITControlPoint*> column);
	void registerSurface(ITSurface* s, ITSurface* bs);

	void redoSurfaceDelete(ITProject* p);
	void redoRowDelete(ITProject* p);
	void redoColumnDelete(ITProject* p);

	void undoGroupPointsMove(ITProject* p, const unsigned int id);
	void undoGroupPointsRotate(ITProject* p, const unsigned int id);
	void redoGroupPointsMove(ITProject* p, const unsigned int id);
	void redoGroupPointsRotate(ITProject* p, const unsigned int id);
	unsigned int registerPointsGroupMove(std::vector<ITControlPoint*> vec, const float dx, const float dy, const float dz);
	unsigned int registerPointsGroupRotate(std::vector<ITControlPoint*> vec, const float dAngle, const float centerX, const float centerY, const float centerZ, const PLANE plane);
};