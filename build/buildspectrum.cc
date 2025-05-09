void buildspectrum() {
//
        TString command = "hadd -f neutron.root neutrons?.root";
	gSystem->Exec(command);
	TCanvas *cg = new TCanvas();
	TGraph *g = new TGraph("Co59.dat");
//	TGraph *g = new TGraph("Au197.dat");
	for(int n=0;n<g->GetN();n++) g->GetX()[n]*=1e-6;//eV->MeV
	g->Draw();
	cg->SetLogx();
	cg->SetLogy();

//	TFile *f = new TFile("Ep1912keV_no_backing.root");
	TFile *f = new TFile("neutron.root");
//	TFile *f = new TFile("no_back.root");
	TTree *tree = (TTree*)f->Get("data");
        std::vector<int> *feventid = 0;
        std::vector<double> *fEn = 0;
        std::vector<double> *fX = 0;
        std::vector<double> *fY = 0;
        std::vector<double> *fZ = 0;
        std::vector<double> *flength = 0;
        std::vector<TString> *fname = 0;
        tree->SetBranchAddress("feventno",&feventid);
        tree->SetBranchAddress("fEn",&fEn);
        tree->SetBranchAddress("fname",&fname);
        tree->SetBranchAddress("fX",&fX);
        tree->SetBranchAddress("fY",&fY);
        tree->SetBranchAddress("fZ",&fZ);
        tree->SetBranchAddress("flength",&flength);
	TH1F *h1 = new TH1F("h1","Energy spectrum [MeV] (length weighted)",500,0,1);
	TH1F *h1b = new TH1F("h1b","Energy spectrum epithermal (length weighted)",10000,0,0.001);
	TH1F *h2 = new TH1F("h2","Energy spectrum",500,0,1);
	TH1F *h2b = new TH1F("h2b","Energy spectrum epithermal",10000,0,0.001);
	TH2F *hxy = new TH2F("hxy",";x [mm];y [mm]",100,-250,250,100,-250,250);
	TH1F *hEncut = new TH1F("hEncut","",500,0,1.0);
	TH1F *hEncutside = new TH1F("hEncutside","",500,0,1.0);
	TH1F *hcharge = new TH1F("hcharge","",100,0,10);
	const int nentries=tree->GetEntries();
	double XSsum=0;
	int total=0;
	TH1F *hXS = new TH1F("hXS","Log(10) of energy in MeV, XS weighted",12,-10,2);
	cout<<"ENTRIES: "<<nentries<<endl;
	int scattered = 0;
	int activation = 0;
	for(int n=1;n<nentries;n++) {
		tree->GetEntry(n);
		double Echarge = 0;
		int oldeventid = 0;
		for(int i=0;i<fEn->size();i++) {
			if(feventid->at(i)!=oldeventid) {
				if(Echarge>0) {
					hcharge->Fill(Echarge);
				}
//				cout<<Echarge<<endl;
				Echarge=0;
			}
			oldeventid = feventid->at(i);
//			if(!(fname->at(i)=="neutron" || fname->at(i)=="gamma")) cout<<fname->at(i)<<"\t"<<fEn->at(i)<<endl;

			if(fname->at(i)=="Be10") activation++;
			if(fname->at(i)=="Be9") scattered++;
			if(fname->at(i)!="neutron" && fname->at(i)!="gamma") {//Only charged particles
//				if(fEn->at(i)>0) cout<<fname->at(i)<<"\t"<<fEn->at(i)<<endl;
				Echarge+=fEn->at(i);
			}
                        if(fname->at(i) == "neutron" && fEn->at(i)>0) {
				XSsum+=g->Eval(fEn->at(i));//Averaged XS
//				cout<<fEn->at(i)<<"MeV\t"<<g->Eval(fEn->at(i))<<endl;
				hXS->Fill(TMath::Log10((fEn->at(i))),1/sqrt(fEn->at(i)));
				total+=1;
				hxy->Fill(fX->at(i),fY->at(i));
				if(fabs(fX->at(i))<20 && fabs(fY->at(i))<20) hEncut->Fill(fEn->at(i));
				if(fabs(fX->at(i)-200)<20 && fabs(fY->at(i)-200)<20) hEncutside->Fill(fEn->at(i));
                                h1->Fill(fEn->at(i),flength->at(i));
                                h1b->Fill(fEn->at(i),flength->at(i));
                                h2->Fill(fEn->at(i));
                                h2b->Fill(fEn->at(i));
                        }
                }
	}
	TCanvas *c1 = new TCanvas("c1","",1200,600,1200,600);
	h2->SetMaximum(10);
	h2->Scale(0.01);
	h1->Scale(0.01);
	h2->Draw("HIST");
	h2->SetLineColor(kBlack);
	h1->Draw("SAMEHIST");
	TF1 *fMB = new TF1("fMB","[0]*x*TMath::Exp(-x/[1])",0.001,0.2);
	fMB->SetParameter(0,10000);
	fMB->SetParameter(1,0.03);
//	h2->Fit(fMB,"I");
//	fMB->Draw("SAME");
//	g->Draw("SAME");
	TCanvas *c2 = new TCanvas();
	h2b->Draw("HIST");
	h1b->Draw("SAMEHIST");
	g->Draw("SAME");
	cout<<"MACS-like XS = "<<XSsum/total<<" barns"<<endl;
	TCanvas *c3 = new TCanvas();
	hXS->Draw("HIST");
	ofstream outs;
	outs.open("spectrum.dat");
	for(int x=0;x<h2->GetXaxis()->GetNbins();x++) {
		outs<<h2->GetXaxis()->GetBinCenter(x+1)<<"\t"<<h2->GetBinContent(x+1)<<endl;
	}
	outs.close();
	TCanvas *cxy = new TCanvas();
	hxy->Draw("COLZ");
	TCanvas *cEn = new TCanvas();
	hEncut->Draw();
	hEncutside->SetLineColor(kRed);
	hEncutside->Draw("SAME");
	TCanvas *cC = new TCanvas();
	hcharge->Draw();
	cout<<"Activation: "<<activation<<" from "<<total<<endl;
	cout<<"Scattered: "<<scattered<<" from "<<total<<endl;
}
