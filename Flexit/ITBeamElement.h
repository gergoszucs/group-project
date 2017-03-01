#pragma once

#include <string>

// Forward declarations.
class ITPoint;

class ITBeamElement
{

private:

	std::string _Name;

	ITPoint* _startPoint;
	ITPoint* _endPoint;


public:

	ITBeamElement(void);
	~ITBeamElement(void);

	// Utilities.
	float length();

	// IO


	// Accessors.	

	std::string get_Name();
	void set_Name(std::string s);

	ITPoint* get_StartPoint();
	void set_StartPoint(ITPoint* p);

	ITPoint* get_EndPoint();
	void set_EndPoint(ITPoint* p);

protected:


};