#pragma once

#include <string>
#include <vector>

class ITBeamElement;
class ITSurface;
class ITBeamNode;

class ITBeam
{

private:

	std::string _Name;

	float _U; // Intrinsic coordinate.
	float _V; // Intrinsic coordinate.

	std::vector <ITBeamElement*> *_MyBeamElements;
	std::vector <ITBeamNode*> *_MyBeamNodes;

	std::vector <ITBeamNode*> *_MyLeftGhostBeamNodes;
	std::vector <ITBeamNode*> *_MyRightGhostBeamNodes;

	std::vector <ITBeamElement*> *_MyLeftGhostBeamElements;
	std::vector <ITBeamElement*> *_MyRightGhostBeamElements;

	ITSurface *_MyITSurface;

	double _EB_dt; // The Lax Richtmyer delta t.

	std::vector < std::vector <ITBeamNode*> > *_MyNodeHistory; // The history of beam nodes.

public:

	ITBeam(ITSurface* s);
	~ITBeam(void);

	// Utilities.
	void createBeamElements();
	void createGhostBeamElements();

	void createBeamNodes();
	void createGhostBeamNodes();

	void propagateBeamElements();
	void propagateGhostBeamElements();

	void propagateBeamNodes();
	void propagateGhostBeamNodes();
	void computeMyLaxRichtmyerDeltaT();

	// IO


	// Accessors.	

	std::string get_Name();
	void set_Name(std::string s);

	float get_U();
	void set_U(float u);

	float get_V();
	void set_V(float v);

	std::vector <ITBeamElement*> *get_MyBeamElements();
	void set_MyBeamElements(std::vector <ITBeamElement*> *_bes);

	std::vector <ITBeamNode*> *get_MyBeamNodes();
	void set_MyBeamNodes(std::vector <ITBeamNode*> *_bes);

	ITSurface *get_MyITSurface();
	void set_MyITSurface(ITSurface *s);

	double get_EB_dt();
	void set_EB_dt(double d);

	std::vector <ITBeamNode*> *get_MyLeftGhostBeamNodes();
	void set_MyLeftGhostBeamNodes(std::vector <ITBeamNode*> *bes);

	std::vector <ITBeamNode*> *get_MyRightGhostBeamNodes();
	void set_MyRightGhostBeamNodes(std::vector <ITBeamNode*> *bes);

	std::vector <ITBeamElement*> *get_MyLeftGhostBeamElements();
	void set_MyLeftGhostBeamElements(std::vector <ITBeamElement*> *_bes);

	std::vector <ITBeamElement*> *get_MyRightGhostBeamElements();
	void set_MyRightGhostBeamElements(std::vector <ITBeamElement*> *_bes);

	std::vector < std::vector <ITBeamNode*> > *get_MyNodeHistory();
	void set_MyNodeHistory(std::vector < std::vector <ITBeamNode*> > *h);

protected:


};