class ITPanel;

class ITControlSurface
{
protected:

	float _DeflectionAngle;
	std::vector <ITPanel*> *_MyITPanels;
	std::vector <float> *_MyDeflectionHistory;

public:

	// Constructors.
	ITControlSurface(void);
	~ITControlSurface(void);

	// Accessors.
	float get_DeflectionAngle();
	void set_DeflectionAngle(float x);

	std::vector <ITPanel*> *get_MyITPanels();
	void set_MyITPanels(std::vector <ITPanel*> *myITPanels);

	std::vector <float> *get_MyDeflectionHistory();
	void set_MyDeflectionHistory(std::vector <float> *myDeflectionHistory);
};