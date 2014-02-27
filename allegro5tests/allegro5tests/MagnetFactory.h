//Magnet Factory.h
//Contains Declarations for the Magnet Factory
#pragma once
#include "Magnets.h"

class MagnetFactory
{
public:
	MagnetFactory();
	Magnet *requestMagnet();
	int getNumOfMagnetsInProduction() const;

protected:
	virtual Magnet *createMagnet() = 0;

private:
	int numMagnetsInProduction;
};

class TopMagnetFactory : public MagnetFactory
{
protected:
	virtual Magnet *createMagnet();
};

class BottomMagnetFactory : public MagnetFactory
{
protected:
	virtual Magnet *createMagnet();
};

