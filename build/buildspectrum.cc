void buildspectrum() {
//
	TCanvas *cg = new TCanvas();
	TGraph *g = new TGraph("Co59.dat");
//	TGraph *g = new TGraph("Au197.dat");
	for(int n=0;n<g->GetN();n++) g->GetX()[n]*=1e-6;//eV->MeV
	g->Draw();
	cg->SetLogx();
	cg->SetLogy();

//	TFile *f = new TFile("Ep1912keV_no_backing.root");
	TFile *f = new TFile("neutrons.root");
	TTree *tree = (TTree*)f->Get("data");
        std::vector<double> *fEn = 0;
        std::vector<double> *fX = 0;
        std::vector<double> *fY = 0;
        std::vector<double> *fZ = 0;
        std::vector<double> *flength = 0;
        std::vector<TString> *fname = 0;
        tree->SetBranchAddress("fEn",&fEn);
        tree->SetBranchAddress("fname",&fname);
        tree->SetBranchAddress("fX",&fX);
        tree->SetBranchAddress("fY",&fY);
        tree->SetBranchAddress("fZ",&fZ);
        tree->SetBranchAddress("flength",&flength);
	TH1F *h1 = new TH1F("h1","Energy spectrum [MeV] (length weighted)",200,0,0.2);
	TH1F *h1b = new TH1F("h1b","Energy spectrum epithermal (length weighted)",10000,0,0.001);
	TH1F *h2 = new TH1F("h2","Energy spectrum",200,0,.2);
	TH1F *h2b = new TH1F("h2b","Energy spectrum epithermal",10000,0,0.001);
	const int nentries=tree->GetEntries();
	double XSsum=0;
	int total=0;
	TH1F *hXS = new TH1F("hXS","",10,-2,5);
	cout<<"ENTRIES: "<<nentries<<endl;
	for(int n=1;n<nentries;n++) {
		tree->GetEntry(n);
		for(int i=0;i<fEn->size();i++) {
                        if(fname->at(i) == "neutron" && fEn->at(i)>0) {
				XSsum+=g->Eval(fEn->at(i));//Averaged XS
//				cout<<fEn->at(i)<<"MeV\t"<<g->Eval(fEn->at(i))<<endl;
				hXS->Fill(TMath::Log10(g->Eval(fEn->at(i))));
				total+=1;
                                h1->Fill(fEn->at(i),flength->at(i));
                                h1b->Fill(fEn->at(i),flength->at(i));
                                h2->Fill(fEn->at(i));
                                h2b->Fill(fEn->at(i));
                        }
                }

	}
	TCanvas *c1 = new TCanvas();
	h2->Scale(0.01);
	h1->Scale(0.01);
	h2->Draw("HIST");
	h2->SetLineColor(kBlack);
	h1->Draw("SAMEHIST");
	TF1 *fMB = new TF1("fMB","[0]*x*TMath::Exp(-x/[1])",0.01,0.5);
	fMB->SetParameter(0,10000);
	fMB->SetParameter(1,0.03);
//	h2->Fit(fMB);
	//fMB->Draw("SAME");
//	g->Draw("SAME");
	TCanvas *c2 = new TCanvas();
	h2b->Draw("HIST");
	h1b->Draw("SAMEHIST");
	g->Draw("SAME");
	cout<<"MACS-like XS = "<<XSsum/total<<" barns"<<endl;
	TCanvas *c3 = new TCanvas();
	hXS->Draw();
}
