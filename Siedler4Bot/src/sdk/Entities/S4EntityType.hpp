#pragma once

enum class S4EntityType : unsigned char
{
    ETSettler = 1,
    ETShip = 2,
    ETLandVehicle = 4,
    ETBuilding = 8,
    ETGood = 16,
    ETPlant = 32,
    ETUnknownEntityType = 64,
    ETAnimal = 128,
};
