#pragma once

class State
{
private:
public:
	//State Functions
	virtual void StartState(){};
	virtual void UpdateState(){};
	virtual void EndState(){};
};