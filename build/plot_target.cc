void plot_target() {
	TFile *f = TFile::Open("output.root");
	TTree *tree = (TTree*)f->Get("data");
	tree->Print();
	std::vector<double> *fEn = 0;
	std::vector<double> *fX = 0;
	std::vector<double> *fY = 0;
	std::vector<double> *fZ = 0;
	std::vector<TString> *fname = 0;
	tree->SetBranchAddress("fEn",&fEn);
	tree->SetBranchAddress("fname",&fname);
	tree->SetBranchAddress("fX",&fX);
	tree->SetBranchAddress("fY",&fY);
	tree->SetBranchAddress("fZ",&fZ);
	const int nevents = tree->GetEntries();
	TH2F *hHit = new TH2F("hHit",";[mm];[mm]",100,-100,100,100,-100,100);
	TH2F *hE = new TH2F("hE",";[mm];[mm]",100,-100,100,100,-100,100);
	for(int n=0;n<nevents;n++) {
		tree->GetEntry(n);
		for(int i=0;i<fEn->size();i++) {
			cout<<fname->at(i)<<"\t"<<fEn->at(i)<<"\t"<<fX->at(i)<<"\t"<<fY->at(i)<<"\t"<<fZ->at(i)<<endl;
			if(fname->at(i) == "neutron" && fEn->at(i)>0.) {
				hHit->Fill(fX->at(i),fY->at(i));
				hE->Fill(fX->at(i),fY->at(i),fEn->at(i));
			}
		}
	}
	for(int x=0;x<hE->GetXaxis()->GetNbins();x++) {
		for(int y=0;y<hE->GetYaxis()->GetNbins();y++) {
			if(hHit->GetBinContent(x+1,y+1)>0) hE->SetBinContent(x+1,y+1,hE->GetBinContent(x+1,y+1)/hHit->GetBinContent(x+1,y+1));
		}
	}
	hE->Draw("COLZ");
	return;
}
