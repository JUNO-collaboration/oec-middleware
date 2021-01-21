#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <vector>

int main(void)
{
    TFile* f = new TFile("planetree.root");
    TTree* event = (TTree*)f->Get("event");
    event->SetMakeClass(1);

    Int_t           EventNo;
    Int_t           Second;
    Int_t           NanoSec;
    std::vector<unsigned short> *PmtIDs = nullptr;
    std::vector<std::vector<short> > *Waveforms = nullptr;

    event->SetBranchAddress("EventNo", &EventNo);
    event->SetBranchAddress("Second", &Second);
    event->SetBranchAddress("NanoSec", &NanoSec);
    event->SetBranchAddress("PmtIDs", &PmtIDs);
    event->SetBranchAddress("Waveforms", &Waveforms);

    Long64_t n = event->GetEntries();

    for ( Long64_t i = 0; i < n; ++i ) {
        event->GetEntry(i);
        std::cout << Waveforms->size() << std::endl;
    }

    return 0;
}
