void readone(TString);
TCanvas *c1=0;
TLegend *leg=new TLegend(0.1,0.7,0.3,0.9);
Color_t colours[8]={kRed,kBlack,kBlue,kGreen,kMagenta,kGray,kOrange,kPink-2};
double scalings[8]={0.05,0.25,0.45,0.7,1.02541,1.28512,1.44576,1.6523};
TH1F *h[8]={0};
int counter=0;
void many() {
	readone("Ep1900.root");
	readone("Ep2000.root");
	readone("Ep2100.root");
	readone("Ep2200.root");
	readone("Ep2300.root");
	readone("Ep2400.root");
	readone("Ep2500.root");
	readone("Ep2600.root");
	leg->Draw("SAME");
	TCanvas *c2 = new TCanvas("c2","Difference",800,0,800,600);
	TH1F *hdiff = (TH1F*)h[1]->Clone();
	hdiff->Add(h[0],-1);
	hdiff->Draw();
}

void readone(TString fname) {
	TFile *f = TFile::Open(fname);
	TCanvas *c = (TCanvas*)f->Get("c1");
//	c->ls();
	h[counter] = (TH1F*)c->GetPrimitive("h1");
	int norm_bin = 250;
	int counts = h[counter]->GetBinContent(norm_bin);
//	cout<<"Scale: "<<fname<<"\t"<<h[0]->GetBinContent(norm_bin)/counts<<endl;
//	h[counter]->Scale(h[0]->GetBinContent(norm_bin)/counts);
	h[counter]->Scale(scalings[counter]);
	h[counter]->SetLineColor(colours[counter]);
	h[counter]->SetMaximum(50);
	h[counter]->SetLineWidth(3);

	if(c1==0) {
		c1 = new TCanvas("c1","2.5 x 2.5 x 0.1 cm Be",0,0,800,600);
		h[counter]->Draw("HIST");
	}
	else {
		c1->cd();
		h[counter]->Draw("SAMEHIST");
	}
	leg->AddEntry(h[counter],fname);
	counter++;
}
