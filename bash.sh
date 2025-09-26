#!/bin/bash

# Quick fix for the doctest compilation error
# Run this from your project root directory

echo "Fixing doctest compilation error..."

# Replace the problematic line in tests.cpp
sed -i 's/CHECK((monster\.getBehaviorState() == Monster::PATROLLING || /bool stateValid = (monster.getBehaviorState() == Monster::PATROLLING || /g' test-source-code/tests.cpp
sed -i 's/monster\.getBehaviorState() == Monster::CHASING ||/monster.getBehaviorState() == Monster::CHASING ||/g' test-source-code/tests.cpp
sed -i 's/monster\.getBehaviorState() == Monster::AGGRESSIVE));/monster.getBehaviorState() == Monster::AGGRESSIVE); CHECK(stateValid);/g' test-source-code/tests.cpp

# Also fix the projectile state check
sed -i 's/CHECK((projectile\.getState() == Projectile::EXTENDING ||/bool projStateValid = (projectile.getState() == Projectile::EXTENDING ||/g' test-source-code/tests.cpp
sed -i 's/projectile\.getState() == Projectile::RETRACTING));/projectile.getState() == Projectile::RETRACTING); CHECK(projStateValid);/g' test-source-code/tests.cpp

echo "Fixed complex boolean expressions that were causing doctest compilation errors."
echo "Try building again: cd build && cmake --build ."