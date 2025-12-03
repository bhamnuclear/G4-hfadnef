#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
: fDetector(det)
{    G4cout<<"Foil messenger created"<<G4endl;
    fFoilDistanceCmd = new G4UIcmdWithADoubleAndUnit("/target/setDistance", this);
    fFoilDistanceCmd->SetGuidance("Set a custom value for the foil distance from the Ti window");
    fFoilDistanceCmd->SetParameterName("foildistance", false);
    fFoilDistanceCmd->SetUnitCategory("Length");
    fFoilDistanceCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

DetectorMessenger::~DetectorMessenger()
{
    delete fFoilDistanceCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    G4cout<<"Foil distance command created "<<newValue<<G4endl;
    if (command == fFoilDistanceCmd)
    {
        double val = fFoilDistanceCmd->GetNewDoubleValue(newValue);
        fDetector->SetFoilDistance(val);
    }

}
