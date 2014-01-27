//Magnet Factory.cpp
//Contains definitions for the Magnet Factory

#include "MagnetFactory.h"

MagnetFactory::MagnetFactory() : numMagnetsInProduction(0) 
{
}

Magnet *MagnetFactory::requestMagnet()
{
	numMagnetsInProduction++;
	return createMagnet();
}

int MagnetFactory::getNumOfMagnetsInProduction() const
{
	return numMagnetsInProduction;
}

Magnet *TopMagnetFactory::createMagnet()
{
	return new TopMagnet();
}

Magnet *BottomMagnetFactory::createMagnet()
{
	return new BottomMagnet();
}