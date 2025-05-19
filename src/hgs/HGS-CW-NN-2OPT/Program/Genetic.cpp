#include "Genetic.h"

// Genetic.cpp
void Genetic::run()
{
    /* INITIAL POPULATION */
    population.generatePopulation();

    int nbIter;
    int nbIterNonProd = 1;
    if (params.verbose) std::cout << "----- STARTING GENETIC ALGORITHM" << std::endl;
    for (nbIter = 0; (params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit) && nbIter <= params.ap.nbIter; nbIter++)
    {
        /* SELECTION AND CROSSOVER */
        crossoverOX(offspring, population.getBinaryTournament(), population.getBinaryTournament());

        /* LOCAL SEARCH */
        localSearch.run(offspring, params.penaltyCapacity, params.penaltyDuration);
        bool isNewBest = population.addIndividual(offspring, true);
        if (!offspring.eval.isFeasible && params.ran() % 2 == 0)
        {
            localSearch.run(offspring, params.penaltyCapacity * 10., params.penaltyDuration * 10.);
            if (offspring.eval.isFeasible) isNewBest = (population.addIndividual(offspring, false) || isNewBest);
        }

        /* TRACKING THE NUMBER OF ITERATIONS SINCE LAST SOLUTION IMPROVEMENT */
        if (isNewBest) nbIterNonProd = 1;
        else nbIterNonProd++;

        /* DIVERSIFICATION, PENALTY MANAGEMENT AND TRACES */
        if (nbIter % params.ap.nbIterPenaltyManagement == 0) population.managePenalties();
        if (nbIter % params.ap.nbIterTraces == 0) population.printState(nbIter, nbIterNonProd);

        /* RESTART MECHANISM */
        if (nbIterNonProd >= maxIterNonProd && (params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit - 5))
        {
            if (params.verbose) std::cout << "----- TRIGGERING POPULATION RESTART AFTER " << nbIterNonProd << " ITERATIONS WITHOUT IMPROVEMENT" << std::endl;
            population.restart();
            nbIterNonProd = 1;
        }
    }
    if (params.verbose) std::cout << "----- GENETIC ALGORITHM FINISHED AFTER " << nbIter << " ITERATIONS. TIME SPENT: " << (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC << std::endl;

    /* POLISHING PHASE */
    if (params.verbose) std::cout << "----- POLISHING BEST SOLUTION" << std::endl;
    const Individual* best = population.getBestFound();
    if (best != nullptr)
    {
        Individual polished(*best);
        if (params.ap.timeLimit == 0 || (double)(clock() - params.startTime) / (double)CLOCKS_PER_SEC < params.ap.timeLimit - 5)
        {
            localSearch.run(polished, params.penaltyCapacity * 100., params.penaltyDuration * 100.);
            population.addIndividual(polished, true);
            if (params.verbose) std::cout << "----- POLISHED COST: " << polished.eval.penalizedCost << std::endl;
        }
    }
}

void Genetic::crossoverOX(Individual & result, const Individual & parent1, const Individual & parent2)
{
	// Frequency table to track the customers which have been already inserted
	std::vector <bool> freqClient = std::vector <bool> (params.nbClients + 1, false);

	// Picking the beginning and end of the crossover zone
	std::uniform_int_distribution<> distr(0, params.nbClients-1);
	int start = distr(params.ran);
	int end = distr(params.ran);

	// Avoid that start and end coincide by accident
	while (end == start) end = distr(params.ran);

	// Copy from start to end
	int j = start;
	while (j % params.nbClients != (end + 1) % params.nbClients)
	{
		result.chromT[j % params.nbClients] = parent1.chromT[j % params.nbClients];
		freqClient[result.chromT[j % params.nbClients]] = true;
		j++;
	}

	// Fill the remaining elements in the order given by the second parent
	for (int i = 1; i <= params.nbClients; i++)
	{
		int temp = parent2.chromT[(end + i) % params.nbClients];
		if (freqClient[temp] == false)
		{
			result.chromT[j % params.nbClients] = temp;
			j++;
		}
	}

	// Complete the individual with the Split algorithm
	split.generalSplit(result, parent1.eval.nbRoutes);
}

Genetic::Genetic(Params & params) : 
	params(params), 
	split(params),
	localSearch(params),
	population(params,this->split,this->localSearch),
	offspring(params),
	maxIterNonProd(3000) // Initialize maximum iterations without improvement
	{}

