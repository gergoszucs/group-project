#include "undoRedoSystem.h"

#include "global.h"
#include "ITSurface.h"
#include "ITProject.h"
#include "ITControlPoint.h"

UndoRedoSystem::UndoRedoSystem()
	: commandPointer(-1)
{
}

void UndoRedoSystem::reset()
{
	for (auto it : deletedRows)
	{
		for (auto itt : std::get<2>(it))
		{
			delete(itt);
		}
	}
	deletedRows.erase(deletedRows.begin(), deletedRows.end());

	for (auto it : deletedColumns)
	{
		for (auto itt : std::get<2>(it))
		{
			delete(itt);
		}
	}
	deletedColumns.erase(deletedColumns.begin(), deletedColumns.end());

	for (auto it : deletedSurfaces)
	{
		delete(std::get<1>(it));
		delete(std::get<2>(it));
	}
	deletedSurfaces.erase(deletedSurfaces.begin(), deletedSurfaces.end());

	pointGroupsMove.erase(pointGroupsMove.begin(), pointGroupsMove.end());
	pointGroupsRotate.erase(pointGroupsRotate.begin(), pointGroupsRotate.end());

	commands.erase(commands.begin(), commands.end());
	reverseCommands.erase(reverseCommands.begin(), reverseCommands.end());
	
	commandPointer = -1;
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
	if ((commandPointer >= -1) && (commandPointer < (int)(commands.size() - 1)))
	{
		commandPointer++;

		w->executeCommand("REDO", commands[commandPointer], false);
	}
}

void UndoRedoSystem::registerRow(unsigned int surfaceID, const unsigned int i, std::vector<ITControlPoint*> row)
{
	deletedRows.push_back(std::make_tuple(surfaceID, i, row));
}

void UndoRedoSystem::registerColumn(unsigned int surfaceID, const unsigned int j, std::vector<ITControlPoint*> column)
{
	deletedColumns.push_back(std::make_tuple(surfaceID, j, column));
}

void UndoRedoSystem::registerSurface(ITSurface * s, ITSurface * bs)
{
	unsigned int surfaceID = s->getControlPoint(0, 0)->get_K();

	deletedSurfaces.push_back(std::make_tuple(surfaceID, s, bs));
}

void UndoRedoSystem::redoSurfaceDelete(ITProject* p)
{
	if (deletedSurfaces.size() == 0) throw std::exception("NOTHING_TO_REDO");

	int originalSurfaceID = std::get<0>(deletedSurfaces.back());

	if ((originalSurfaceID < 0) || (originalSurfaceID > p->get_MySurfaces()->size())) throw std::exception("RECREATION_OF_SURFACE_IMPOSSIBLE");

	auto it = p->get_MySurfaces()->begin() + originalSurfaceID;
	auto itBase = p->get_MyBaseSurfaces()->begin() + originalSurfaceID;

	if (originalSurfaceID == p->get_MySurfaces()->size())
	{
		p->get_MySurfaces()->push_back(std::get<1>(deletedSurfaces.back()));
		p->get_MyBaseSurfaces()->push_back(std::get<2>(deletedSurfaces.back()));
	}
	else
	{
		p->get_MySurfaces()->insert(it, std::get<1>(deletedSurfaces.back()));
		p->get_MyBaseSurfaces()->insert(itBase, std::get<2>(deletedSurfaces.back()));
	}

	for (int k = 0; k < p->get_MySurfaces()->size(); k++)
	{
		p->getSurface(k)->reassignIdentifiers(k);
		p->getSurface(k)->manageComputationOfInterpolatedPoints();
	}

	deletedSurfaces.pop_back();
}

void UndoRedoSystem::redoRowDelete(ITProject* p)
{
	if (deletedRows.size() == 0) throw std::exception("NOTHING_TO_REDO");

	int surfaceID = std::get<0>(deletedRows.back());

	if ((surfaceID < 0) || (surfaceID > p->get_MySurfaces()->size())) throw std::exception("RECREATION_OF_ROW_IMPOSSIBLE");

	int rowID = std::get<1>(deletedRows.back());

	if ((rowID < 0) || (rowID > p->getSurface(surfaceID)->sizeX())) throw std::exception("RECREATION_OF_ROW_IMPOSSIBLE");

	std::vector<ITControlPoint*> tmp;

	for (int i = 0; i < std::get<2>(deletedRows.back()).size(); i++)
	{
		ITControlPoint* p = std::get<2>(deletedRows.back())[i];

		ITControlPoint* pCopy = new ITControlPoint(p->get_X(), p->get_Y(), p->get_Z());

		pCopy->set_U(0.0);
		pCopy->set_V(0.0);

		tmp.push_back(pCopy);
	}

	p->getSurface(surfaceID)->addRow(rowID, std::get<2>(deletedRows.back()));
	p->getBaseSurface(surfaceID)->addRow(rowID, tmp);

	p->getSurface(surfaceID)->reassignIdentifiers(surfaceID);
	p->getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	deletedRows.pop_back();
}

void UndoRedoSystem::redoColumnDelete(ITProject* p)
{
	if (deletedColumns.size() == 0) throw std::exception("NOTHING_TO_REDO");

	int surfaceID = std::get<0>(deletedColumns.back());

	if ((surfaceID < 0) || (surfaceID > p->get_MySurfaces()->size())) throw std::exception("RECREATION_OF_ROW_IMPOSSIBLE");

	int columnID = std::get<1>(deletedColumns.back());

	if ((columnID < 0) || (columnID > p->getSurface(surfaceID)->sizeY())) throw std::exception("RECREATION_OF_ROW_IMPOSSIBLE");

	std::vector<ITControlPoint*> tmp;

	for (int i = 0; i < std::get<2>(deletedColumns.back()).size(); i++)
	{
		ITControlPoint* p = std::get<2>(deletedColumns.back())[i];

		ITControlPoint* pCopy = new ITControlPoint(p->get_X(), p->get_Y(), p->get_Z());

		pCopy->set_U(0.0);
		pCopy->set_V(0.0);

		tmp.push_back(pCopy);
	}

	p->getSurface(surfaceID)->addColumn(columnID, std::get<2>(deletedColumns.back()));
	p->getBaseSurface(surfaceID)->addColumn(columnID, tmp);

	p->getSurface(surfaceID)->reassignIdentifiers(surfaceID);
	p->getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	deletedColumns.pop_back();
}

void UndoRedoSystem::undoGroupPointsMove(ITProject * p, const unsigned int id)
{
	auto data = pointGroupsMove[id];

	for (auto it : std::get<0>(data))
	{
		int surfaceID = it->get_K();
		int i = it->get_I();
		int j = it->get_J();
		float posX = -std::get<1>(data);
		float posY = -std::get<2>(data);
		float posZ = -std::get<3>(data);

		p->movePoint(surfaceID, i, j, posX, posY, posZ, false);
	}
}

void UndoRedoSystem::undoGroupPointsRotate(ITProject * p, const unsigned int id)
{
	auto data = pointGroupsRotate[id];

	for (auto it : std::get<0>(data))
	{
		int surfaceID = it->get_K();
		int i = it->get_I();
		int j = it->get_J();
		float posX = std::get<1>(data);
		float posY = std::get<2>(data);
		float posZ = std::get<3>(data);
		float angle = -std::get<4>(data);
		PLANE plane = std::get<5>(data);

		p->rotatePoint(surfaceID, i, j, posX, posY, posZ, angle, plane, false);
	}
}

void UndoRedoSystem::redoGroupPointsMove(ITProject * p, const unsigned int id)
{
	auto data = pointGroupsMove[id];

	for (auto it : std::get<0>(data))
	{
		int surfaceID = it->get_K();
		int i = it->get_I();
		int j = it->get_J();
		float posX = std::get<1>(data);
		float posY = std::get<2>(data);
		float posZ = std::get<3>(data);

		p->movePoint(surfaceID, i, j, posX, posY, posZ, false);
	}
}

void UndoRedoSystem::redoGroupPointsRotate(ITProject * p, const unsigned int id)
{
	auto data = pointGroupsRotate[id];

	for (auto it : std::get<0>(data))
	{
		int surfaceID = it->get_K();
		int i = it->get_I();
		int j = it->get_J();
		float posX = std::get<1>(data);
		float posY = std::get<2>(data);
		float posZ = std::get<3>(data);
		float angle = std::get<4>(data);
		PLANE plane = std::get<5>(data);

		p->rotatePoint(surfaceID, i, j, posX, posY, posZ, angle, plane, false);
	}
}

unsigned int UndoRedoSystem::registerPointsGroupMove(std::vector<ITControlPoint*> vec, const float dx, const float dy, const float dz)
{
	pointGroupsMove.push_back(std::make_tuple(vec, dx, dy, dz));
	return pointGroupsMove.size() - 1;
}

unsigned int UndoRedoSystem::registerPointsGroupRotate(std::vector<ITControlPoint*> vec, const float dAngle, const float centerX, const float centerY, const float centerZ, const PLANE plane)
{
	pointGroupsRotate.push_back(std::make_tuple(vec, centerX, centerY, centerZ, dAngle, plane));
	return pointGroupsRotate.size() - 1;
}
