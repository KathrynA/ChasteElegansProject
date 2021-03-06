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

#ifndef STATECHARTCELLCYCLEMODELSERIALIZABLE_HPP_
#define STATECHARTCELLCYCLEMODELSERIALIZABLE_HPP_

#include "AbstractCellCycleModel.hpp"
#include "ChasteSerialization.hpp"
#include "Identifiable.hpp"
#include <boost/serialization/base_object.hpp>
#include "SmartPointers.hpp"
#include <boost/serialization/vector.hpp>
#include "RandomNumberGenerator.hpp"
//At the moment, the statechart model used by this file is changed by altering THIS HEADER.
//I need to find an easier way to set the statechart model as an input variable...
#include "BasicStatechart.hpp"



/*This class is essentially a wrapper that goes around a statechart model of cell behaviour. 
* It ensures that to the rest of Chaste it appears as a normal cell cycle model.
* In fact, the whole thing is basically a normal cell cycle model, plus an extra member
* variable which is a pointer to a statechart. The statechart updates whenever UpdateCellCyclePhase
* is called, and can set the two flags ReadyToDivide and CellCyclePhase.
* 
* Because boost statecharts don't support archiving, this wrapper also deals with saving
* the current state of the statechart and any variables associated with it when required. 
* The current state is encoded as a single integer for archiving purposes, while statechart associated 
* variables are stored in an array. 
*/

class StatechartCellCycleModelSerializable : public AbstractCellCycleModel
{

/** Standard serialization block, doesn't deal with saving the statechart. */
friend class boost::serialization::access;
    
template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellCycleModel>(*this);
        // Make sure any RandomNumberGenerator singleton gets saved too, to avoid phasing
        SerializableSingleton<RandomNumberGenerator>* p_wrapper = RandomNumberGenerator::Instance()->GetSerializationWrapper();
        archive & p_wrapper;    
    }

public:
    
    /*Holds a pointer to this cell's statechart*/
    boost::shared_ptr<CellStatechart> pStatechart;    
    
    bool mLoadingFromArchive;
    std::vector<double> TempVariableStorage;
    int TempStateStorage;

    /*Constructor. This:
    * 1) Makes a new AbstractCellCycleModel
    * 2) Overwrites certain cell cycle phase durations intended for the crypt with values suitable for C.elegans
    * 3) Leaves the statechart's pointer to its cell NULL
    */
    StatechartCellCycleModelSerializable(bool LoadingFromArchive=false);    

    /*Because a cell cycle model doesn't have a pointer to its cell until AFTER construction, this is the method
    * where we set the cell pointer for this class AND pass it to the statechart. The method SetCell has been changed 
    * in AbstractCellCycleModel to be virtual, allowing it to be overriden safely. 
    */
    void SetCell(CellPtr pCell); 

    /**
    * @return whether the cell is ready to divide (enter M phase). Set by the statechart.
    */
    bool ReadyToDivide();    
    
    /**
    * This method updates the statechart, and the statechart will in turn update the current phase and
    * set the flag ReadyToDivide if appropriate
    */
    void UpdateCellCyclePhase();    
    
    /**
    * Builder method to create new instances of the cell-cycle model for daughter cells.
    */
    AbstractCellCycleModel* CreateCellCycleModel();    
    
    /*
    * Method that ensures all newly created cells are desynchronised at the start of a simulation
    */
    void Initialise();

    //Only a slight change to this method from the normal one - we don't need to reset the phase to
    //M after division because the statechart will handle it.
    void ResetForDivision();    
    
    //2 new setter methods - these expose two protected variables of AbstractCellCycleModel to the statechart:
    //the Phase/ReadyToDivide flags - otherwise it can't set them.
    void SetCellCyclePhase(CellCyclePhase_ Phase);
    void SetReadyToDivide(bool Ready);      

    //Standard stuff. Has to be here though.
    void OutputCellCycleModelParameters(out_stream& rParamsFile);

};




#include "SerializationExportWrapper.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(StatechartCellCycleModelSerializable)

//ARCHIVING METHODS FOR STATECHART

namespace boost
{
namespace serialization
{

    //Fetch state encoding and variable array from chart and put in archive
    template<class Archive>
    inline void save_construct_data(
        Archive & ar, const StatechartCellCycleModelSerializable* t, const BOOST_PFTO unsigned int file_version)
    {
        // Archive other member variables
        int state = t->pStatechart->GetState();
        ar << state;
        std::vector<double> v = t->pStatechart->GetVariables();
        int numberOfVars=v.size();
        ar << numberOfVars;
        for(int i=0; i<v.size(); i++){
            ar<<v.at(i);
        }
    }
    
    template<class Archive>
    inline void load_construct_data(
        Archive & ar, StatechartCellCycleModelSerializable* t, const unsigned int file_version)
    {
    
        int state;
        ar >> state;
        int numberOfVars;
        ar >> numberOfVars;
        std::vector<double> v;
        for(int i=0; i<numberOfVars; i++){
            double value;
            ar >> value;
            v.push_back(value);
        }
        
        // Construct a new cell cycle model and set the statechart's state and variable values.
        ::new(t)StatechartCellCycleModelSerializable(true);
        t->TempStateStorage=state;
        t->TempVariableStorage=v;
    }
}
} // namespace ...


#endif  /*STATECHARTCELLCYCLEMODELSERIALIZABLE_HPP_*/
