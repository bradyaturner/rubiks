#ifndef SOLVERCONTROLLER_H
#define SOLVERCONTROLLER_H

#include "rubiks.h"

void solver_init();
int solver_checkSolved(Rubiks *rubiks);
void solver_solve(Rubiks *rubiks);

#endif
