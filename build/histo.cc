void histo() {
	ifstream in;
	in.open("histogram.txt");
	TH1F *hE = new TH1F("hE","",1000,0.,1.);
	while(in.good()) {
		TString dum1;
		double dum2;
		in>>dum1>>dum2;
		hE->Fill(dum2);
	}
	hE->Draw();
}
