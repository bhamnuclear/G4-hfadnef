#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

class DetectorConstruction; // forward declaration

class DetectorMessenger : public G4UImessenger
{
public:
    DetectorMessenger(DetectorConstruction* det);
    virtual ~DetectorMessenger();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    DetectorConstruction* fDetector;
    G4UIcmdWithADoubleAndUnit* fFoilDistanceCmd;
};

#endif
