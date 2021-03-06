/*

Copyright (c) 2005-2013, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef MOVINGBOUNDARYMODIFIER_HPP_
#define MOVINGBOUNDARYMODIFIER_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "AbstractCellBasedSimulationModifier.hpp"
#include "AbstractMovingBoundaryCondition.hpp"


/**
 * An modifier class which at each simulation timestep, loops through a list of moving boundaries
 * that it knows of and calls each of their UpdateBoundaryCondition functions. During simulation setup
 * moving boundary conditions should be added both to the simulator and to this modifier using the method
 * SpecifyMovingBoundary. This keeps the handling of moving boundaries somewhat separate from the main Chaste
 * code for the time being.
 */
template<unsigned DIM>
class MovingBoundaryModifier : public AbstractCellBasedSimulationModifier<DIM,DIM>
{
    /** Needed for serialization. */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellBasedSimulationModifier<DIM,DIM> >(*this);
        archive & mBoundaryConditions;
    }

protected:

    /** List of moving boundary conditions. */
    std::vector<boost::shared_ptr<AbstractMovingBoundaryCondition<DIM,DIM> > > mBoundaryConditions;

public:

    /**
     * Default constructor.
     */
    MovingBoundaryModifier();

    /**
     * Destructor.
     */
    virtual ~MovingBoundaryModifier();

    /**
     * Overriden UpdateAtEndOfTimeStep method
     *
     * Specifies what to do in the simulation at the end of each timestep.
     *
     * @param rCellPopulation reference to the cell population
     */
     void UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation);

    /**
     * Overriden SetupSolve method
     *
     * Specifies what to do in the simulation before the start of the time loop.
     *
     * @param rCellPopulation reference to the cell population
     * @param outputDirectory the output directory, relative to where Chaste output is stored
     */
    virtual void SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory);

    /**
     * UpdateCellData method
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation);

    /**
     * SpecifyMovingBoundary method adds a boundary condition to the list for update at the end of each timestep
     *
     * @param pBoundaryCondition points to a child of AbstractMovingBoundaryCondition
     */
    void SpecifyMovingBoundary(boost::shared_ptr<AbstractMovingBoundaryCondition<DIM,DIM> >  pBoundaryCondition);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(MovingBoundaryModifier)

#endif /*MOVINGBOUNDARYMODIFIER_HPP_*/
