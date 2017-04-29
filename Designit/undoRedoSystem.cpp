#include "undoRedoSystem.h"

#include "global.h"
#include "ITSurface.h"
#include "ITProject.h"
#include "ITControlPoint.h"

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