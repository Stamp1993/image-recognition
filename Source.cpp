#include"NeuralNetwork.h"
#include<map>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstddef>
#include<dirent.h>
#include<stdio.h>

	



VectorXd readfile (string filename) {
	ifstream  data(filename);
	vector<VectorXd> preresult;
	string line;

	while (getline(data, line))
	{
		stringstream  lineStream(line);
		string        cell;

		VectorXd lineVec(3);
		int i = 0;
		while (getline(lineStream, cell, ' '))
		{
			lineVec[i] = stod(cell);
			i++;
		}
		preresult.push_back(lineVec);
	}
	VectorXd result(3 * preresult.size());
	for (int i = 0; i < preresult.size(); i++) {
		VectorXd add = preresult[i];
		add = add / 255;
		result[i * 3] = add[0];
		result[i * 3 + 1] = add[1];
		result[i * 3 + 2] = add[2];
	}
	return result;
}

MatrixXd listFile(const char* dirname) {
	vector<VectorXd> preres;
	DIR *pDIR;
	struct dirent *entry;
	if (pDIR = opendir(dirname)) {
		while (entry = readdir(pDIR)) {
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				string dn = dirname;
				dn = dn + "/" + entry->d_name;
				size_t s = dn.size();
				if (dn[s - 1] == 't') {
					preres.push_back(readfile(dn));
				}
			}
		}
		closedir(pDIR);
	}
	MatrixXd res = MatrixXd(preres.size(), preres[0].size());
	for (int i = 0; i < preres.size(); i++) {
		res.row(i) = preres[i];
	}

	return res;
}

int main() {
	
	MatrixXd clocks = listFile("./clocks");
	MatrixXd crocodiles = listFile("./crocodiles");

	//create test set and training set 50/50
	int sep = 2;
	MatrixXd trainSet((clocks.rows() + crocodiles.rows()) / sep, clocks.cols());
	MatrixXd targets((clocks.rows() + crocodiles.rows()) / sep, 2);

	MatrixXd testSet((clocks.rows() + crocodiles.rows()) / sep, clocks.cols());
	MatrixXd answers((clocks.rows() + crocodiles.rows()) / sep, 2);

	int trs = trainSet.rows() / sep -1;
	int ts = testSet.rows() / sep -1;
	VectorXd cl(2);
	cl << 1, 0;
	cl = cl;
	VectorXd cr(2);
	cr << 0, 1;
	cr = cr;

	for (int i = 0; i < (clocks.rows()); i++) {
		if (randDouble() >= (1.0/sep) && trs>=0) {
			trainSet.row(trs) = clocks.row(i);
			targets.row(trs) = cl;
			trs--;
		}
		else if (ts>=0){
			testSet.row(ts) = clocks.row(i);
			answers.row(ts) = cl;
			ts--;
		}
		else if (trs >= 0) {
			trainSet.row(trs) = clocks.row(i);
			targets.row(trs) = cl;
			trs--;
		} 
		else break;
		
	}
	trs = trainSet.rows()-1;
	ts = testSet.rows()-1;
	for (int i = 0; i < (crocodiles.rows()); i++) {
		if (randDouble() >= (1.0/sep) && trs>trainSet.rows() / sep) {
			trainSet.row(trs) = crocodiles.row(i);
			targets.row(trs) = cr;
			trs--;
		}
		else if (ts> testSet.rows() /sep ) {
			testSet.row(ts) = crocodiles.row(i);
			answers.row(ts) = cr;
			ts--;
		}
		else if (trs > trainSet.rows() / sep) {
			trainSet.row(trs) = crocodiles.row(i);
			targets.row(trs) = cr;
			trs--;
		}
		else break;
		
	}
	

	neuralNetwork nn = neuralNetwork(trainSet.cols(), 0.01, logist);
	nn.addLayer(100);
	nn.addLayer(2);

	//nn.batchBackpropagation(trainSet.transpose(), targets.transpose(), 0.0000000001);
	nn.batchBackpropagation(trainSet.row(0).transpose(), targets.row(0).transpose(), 0.0001);
	
	MatrixXd PRMatr = classicPR(3, (*nn.inputs));
	vector<vector<VectorXd>> PRactivations = nn.activationsRun(PRMatr);
	MatrixXd PRTarget = nn.batchRun(PRMatr);
	
	//variant 1
	/*for (int i = 0; i < trainSet.rows(); i++) {
		VectorXd run = trainSet.row(i).transpose();
		VectorXd target = targets.row(i).transpose();
		PRMatr.col(1) = run;
		PRTarget.col(1) = target;
		nn.prBackpropagation(PRMatr, PRactivations, PRTarget, 0.000001);
	}*/
	//varian 2
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < trainSet.rows(); i++) {
			VectorXd run = trainSet.row(i).transpose();
			VectorXd target = targets.row(i).transpose();
			PRMatr.col(1) = run;
			PRTarget.col(1) = target;
			nn.batchBackpropagation(PRMatr, PRTarget, 0.000001);
		}
	}

	MatrixXd tests = testSet.transpose();
	MatrixXd pred = (nn.batchRun(tests)).transpose();

	double TP = 0;
	double FP = 0;
	double TN = 0;
	double FN = 0;

	for (int i = 0; i < pred.cols(); i++) {
		bool pr = pred.row(i)[0] > pred.row(i)[1];
		bool act = answers.row(i)[0] > answers.row(i)[1];
		if (act) {//positive
			if (pr) {
				TP = TP + 1;
			}
			else {
				FN = FN + 1;
			}
		}
		else {//negative
			if (pr) {
				FP = FP + 1;
			}
			else {
				TN = TN + 1;
			}
		}
	}

	cout << "TP = " << TP << "; FP = " << FP << "; TN = " << TN << "; FN = " << FN << endl;

	cout << "precision = " << (TP / (TP + FP)) << endl;

	cout << "recall = " << (TP / (TP + FN)) << endl;

	cout << "accuracy = " << (TP + TN) / (TP + TN + FP + FN) << endl;

	system("pause");
}