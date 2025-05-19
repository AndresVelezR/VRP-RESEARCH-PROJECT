//
// Created by chkwon on 3/23/22.
//

#include "AlgorithmParameters.h"
#include <iostream>

extern "C"
struct AlgorithmParameters default_algorithm_parameters() {
	struct AlgorithmParameters ap{};

	ap.nbGranular = 50;           // Reducido de 100 para más iteraciones
    ap.mu = 25;                   // Sin cambios
    ap.lambda = 80;               // Aumentado de 60 para más exploración
    ap.nbElite = 5;               // Reducido de 10 para más diversidad
    ap.nbClose = 10;              // Sin cambios

    ap.nbIterPenaltyManagement = 50; // Sin cambios
    ap.targetFeasible = 0.4;      // Aumentado de 0.3 para más soluciones factibles
    ap.penaltyDecrease = 0.5;     // Aumentado de 0.3 para ajustes menos agresivos
    ap.penaltyIncrease = 1.5;     // Reducido de 2.5 para ajustes menos agresivos

    ap.seed = 0;                  // Sin cambios
    ap.nbIter = 200000;           // Sin cambios
    ap.nbIterTraces = 500;        // Sin cambios
    ap.timeLimit = 120;           // Sin cambios
    ap.useSwapStar = 1;           // Sin cambios


	return ap;
}

void print_algorithm_parameters(const AlgorithmParameters & ap)
{
	std::cout << "=========== Algorithm Parameters =================" << std::endl;
	std::cout << "---- nbGranular              is set to " << ap.nbGranular << std::endl;
	std::cout << "---- mu                      is set to " << ap.mu << std::endl;
	std::cout << "---- lambda                  is set to " << ap.lambda << std::endl;
	std::cout << "---- nbElite                 is set to " << ap.nbElite << std::endl;
	std::cout << "---- nbClose                 is set to " << ap.nbClose << std::endl;
	std::cout << "---- nbIterPenaltyManagement is set to " << ap.nbIterPenaltyManagement << std::endl;
	std::cout << "---- targetFeasible          is set to " << ap.targetFeasible << std::endl;
	std::cout << "---- penaltyDecrease         is set to " << ap.penaltyDecrease << std::endl;
	std::cout << "---- penaltyIncrease         is set to " << ap.penaltyIncrease << std::endl;
	std::cout << "---- seed                    is set to " << ap.seed << std::endl;
	std::cout << "---- nbIter                  is set to " << ap.nbIter << std::endl;
	std::cout << "---- nbIterTraces            is set to " << ap.nbIterTraces << std::endl;
	std::cout << "---- timeLimit               is set to " << ap.timeLimit << std::endl;
	std::cout << "---- useSwapStar             is set to " << ap.useSwapStar << std::endl;
	std::cout << "==================================================" << std::endl;
}
