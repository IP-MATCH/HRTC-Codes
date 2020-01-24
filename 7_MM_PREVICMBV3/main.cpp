#include "main.h"

/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

int main(int argc, char **argv){
	
	// local variables
	Allocation allo ;
	string filein = argv[2];
	string path = argv[1];
	string pathAndFileout = argv[3];

	// functions
	allo.load(path,filein);
	allo.printProb();

	manlove(allo);

	allo.printSol();
	allo.printInfo(pathAndFileout);
}


int manlove(Allocation& allo){
	double initTimeModelCPU = getCPUTime();
	GRBEnv env = GRBEnv();

	allo.reduction();
	allo.printProb();
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPU;

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		vector<vector<GRBVar> > isDoctorIAllocatedToHospitalJ (allo.nbDoctors + 2 * allo.nbCouples);
		vector<vector<bool> > isDoctorIAllocatedToHospitalJBool (allo.nbDoctors + 2 * allo.nbCouples);
		
		vector<vector<vector<GRBVar> > > isCoupleIAllocatedToChoiceGroupJNumberK (allo.nbCouples);
		vector<vector<vector<GRBVar> > > alpha1 (allo.nbCouples);
		vector<vector<vector<GRBVar> > > alpha2 (allo.nbCouples);
		
		vector<GRBLinExpr> allocationOfDoctorI(allo.nbDoctors + 2 * allo.nbCouples);
		vector<GRBLinExpr> allocationOfHospitalJ(allo.nbHospitals);

		vector<vector<GRBLinExpr> > fillingOfHospitalJUpToRankKExpr(allo.nbHospitals);
		vector<vector<GRBVar> > fillingOfHospitalJUpToRankK(allo.nbHospitals);
		
		// Initialization
		for (int i = 0; i < allo.nbDoctors; i++){
			int idxDoc = allo.doctors[i].id - 1;
			allocationOfDoctorI[idxDoc] = 0;
			isDoctorIAllocatedToHospitalJ[idxDoc].resize(allo.nbHospitals);
			isDoctorIAllocatedToHospitalJBool[idxDoc].resize(allo.nbHospitals,false);
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				int idxHos = allo.doctors[i].preferences[j] - 1;
				isDoctorIAllocatedToHospitalJ[idxDoc][idxHos] = model.addVar(0, 1, 0, GRB_BINARY);
			}
		}

		for (int i = 0; i < allo.nbCouples; i++){
			int idxDoc1 = allo.couples[i].id1 - 1;
			int idxDoc2 = allo.couples[i].id2 - 1;
			allocationOfDoctorI[idxDoc1] = 0; 
			allocationOfDoctorI[idxDoc2] = 0; 
			isDoctorIAllocatedToHospitalJ[idxDoc1].resize(allo.nbHospitals); 
			isDoctorIAllocatedToHospitalJBool[idxDoc1].resize(allo.nbHospitals,false);
			isDoctorIAllocatedToHospitalJ[idxDoc2].resize(allo.nbHospitals);
			isDoctorIAllocatedToHospitalJBool[idxDoc2].resize(allo.nbHospitals,false);

			isCoupleIAllocatedToChoiceGroupJNumberK[i].resize(allo.couples[i].nbPref);
			alpha1[i].resize(allo.couples[i].nbPref);
			alpha2[i].resize(allo.couples[i].nbPref);

			for (int j = 0; j<allo.couples[i].nbPref; j++){
				isCoupleIAllocatedToChoiceGroupJNumberK[i][j].resize(allo.couples[i].preferences1[j].size());
				alpha1[i][j].resize(allo.couples[i].preferences1[j].size());
				alpha2[i][j].resize(allo.couples[i].preferences2[j].size());
				for(int k=0; k<allo.couples[i].preferences1[j].size(); k++){
					int idxHos1 = allo.couples[i].preferences1[j][k] - 1;
					int idxHos2 = allo.couples[i].preferences2[j][k] - 1;
					isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);
					alpha1[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);
					alpha2[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);				
					isDoctorIAllocatedToHospitalJ[idxDoc1][idxHos1] = model.addVar(0, 1, 0, GRB_BINARY);
					isDoctorIAllocatedToHospitalJ[idxDoc2][idxHos2] = model.addVar(0, 1, 0, GRB_BINARY);
				}
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			allocationOfHospitalJ[j] = 0;
			fillingOfHospitalJUpToRankKExpr[j].resize(allo.hospitals[j].preferences.size());
			fillingOfHospitalJUpToRankK[j].resize(allo.hospitals[j].preferences.size());
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				fillingOfHospitalJUpToRankKExpr[j][k] = 0;
				fillingOfHospitalJUpToRankK[j][k] = model.addVar(0, allo.hospitals[j].cap, 0, GRB_INTEGER);
			}
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.nbDoctors; i++){
			int idxDoc = allo.doctors[i].id - 1;
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				int idxHos = allo.doctors[i].preferences[j] - 1;
				allocationOfDoctorI[idxDoc] += isDoctorIAllocatedToHospitalJ[idxDoc][idxHos];
				isDoctorIAllocatedToHospitalJBool[idxDoc][idxHos] = true;
				allocationOfHospitalJ[idxHos] += isDoctorIAllocatedToHospitalJ[idxDoc][idxHos];
				for (int k= allo.doctors[i].ranks[j]-1; k < allo.hospitals[idxHos].preferences.size(); k++)
					fillingOfHospitalJUpToRankKExpr[idxHos][k] += isDoctorIAllocatedToHospitalJ[idxDoc][idxHos];	
			}
		}

		for (int i = 0; i < allo.nbCouples; i++){
			int idxDoc1 = allo.couples[i].id1 - 1;
			int idxDoc2 = allo.couples[i].id2 - 1;
			for (int j = 0; j<allo.couples[i].nbPref; j++){
				for (int k = 0; k<allo.couples[i].preferences1[j].size(); k++){
					int idxHos1 = allo.couples[i].preferences1[j][k] - 1;
					int idxHos2 = allo.couples[i].preferences2[j][k] - 1;
					allocationOfDoctorI[idxDoc1] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
					allocationOfDoctorI[idxDoc2] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
					allocationOfHospitalJ[idxHos1] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
					allocationOfHospitalJ[idxHos2] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];				
					for (int l= allo.couples[i].ranks1[j][k]-1; l < allo.hospitals[idxHos1].preferences.size(); l++)
						fillingOfHospitalJUpToRankKExpr[idxHos1][l] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
					for (int l= allo.couples[i].ranks2[j][k]-1; l < allo.hospitals[idxHos2].preferences.size(); l++)
						fillingOfHospitalJUpToRankKExpr[idxHos2][l] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
				}
			}
		}

		// Objective function
		for (int i = 0; i < allo.nbDoctors + 2 * allo.nbCouples; i++){
			 objFun += allocationOfDoctorI[i];
		}

		model.setObjective(objFun, GRB_MAXIMIZE);

		// Link between fillingOfHospitalJUpToRankKExpr and fillingOfHospitalJUpToRankK
		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				if(k == 0) model.addConstr(fillingOfHospitalJUpToRankK[j][k] == fillingOfHospitalJUpToRankKExpr[j][k]);
				else  model.addConstr(fillingOfHospitalJUpToRankK[j][k] == fillingOfHospitalJUpToRankK[j][k-1] + (fillingOfHospitalJUpToRankKExpr[j][k] - fillingOfHospitalJUpToRankKExpr[j][k-1]));
			}
		}

		// Link between isDoctorIAllocatedToHospitalJ and isCoupleIAllocatedToChoiceGroupJNumberK
		for (int i = 0; i < allo.nbCouples; i++){
			int idxDoc1 = allo.couples[i].id1 - 1;
			int idxDoc2 = allo.couples[i].id2 - 1;
			vector<bool> tempB1(allo.nbHospitals,false);
			vector<bool> tempB2(allo.nbHospitals,false);
			vector<GRBLinExpr> temp1(allo.nbHospitals);
			vector<GRBLinExpr> temp2(allo.nbHospitals);
			for (int j = 0; j<allo.nbHospitals; j++){
				temp1[j] = 0;
				temp2[j] = 0;
			}
			for (int j = 0; j<allo.couples[i].nbPref; j++){
				for (int k = 0; k<allo.couples[i].preferences1[j].size(); k++){
					int idxHos1 = allo.couples[i].preferences1[j][k] - 1;
					int idxHos2 = allo.couples[i].preferences2[j][k] - 1;
					temp1[idxHos1] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];		
					tempB1[idxHos1] = true;
					temp2[idxHos2] += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];	
					tempB2[idxHos2] = true; 
				}
			}
			for (int j = 0; j<allo.nbHospitals; j++){
				if(tempB1[j]){
					model.addConstr(isDoctorIAllocatedToHospitalJ[idxDoc1][j] == temp1[j]);
					isDoctorIAllocatedToHospitalJBool[idxDoc1][j] = true;
				}
				if(tempB2[j]){
					model.addConstr(isDoctorIAllocatedToHospitalJ[idxDoc2][j] == temp2[j]);		
					isDoctorIAllocatedToHospitalJBool[idxDoc2][j] = true;
				}
			}
		}

		// Unique assignment for doctors costraints
		for (int i = 0; i < allo.nbDoctors + 2 * allo.nbCouples; i++)
			model.addConstr(allocationOfDoctorI[i] <= 1);

		// Hospital capacity contsraints
		for (int j = 0; j < allo.nbHospitals; j++){
			model.addConstr(allocationOfHospitalJ[j] <= allo.hospitals[j].cap);
		}

		// Single stability constraints
		for (int i = 0; i < allo.nbDoctors; i++){
			int idxDoc = allo.doctors[i].id - 1;
			GRBLinExpr leftside = 0;
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				int idxHos = allo.doctors[i].preferences[j] - 1;
				leftside += isDoctorIAllocatedToHospitalJ[idxDoc][idxHos];
				model.addConstr(allo.hospitals[idxHos].cap * (1 - leftside) - fillingOfHospitalJUpToRankK[idxHos][allo.doctors[i].ranks[j]-1] <= 0);
			}	
		} 

		// Double stability constraints
		for (int i = 0; i < allo.nbCouples; i++){
			int idxDoc1 = allo.couples[i].id1 - 1;
			int idxDoc2 = allo.couples[i].id2 - 1;	
			GRBLinExpr leftside = 0;
			for (int j = 0; j<allo.couples[i].nbPref; j++){
				for(int k=0; k<allo.couples[i].preferences1[j].size(); k++)
					leftside += isCoupleIAllocatedToChoiceGroupJNumberK[i][j][k];
				for(int k=0; k<allo.couples[i].preferences1[j].size(); k++){
					//  If the couple chose the same hospital 
					if(allo.couples[i].preferences1[j][k] == allo.couples[i].preferences2[j][k]){ 
						int idxHos = allo.couples[i].preferences1[j][k] - 1;
						int idxDocFav = idxDoc1;
						int idxDocDis = idxDoc2;
						int idxRanFav = allo.couples[i].ranks1[j][k] - 1;
						int idxRanDis = allo.couples[i].ranks2[j][k] - 1;
					
						GRBLinExpr lhs = 0;
						int nblhs = 0;

						if(idxRanFav > idxRanDis){
							int a = idxDocFav; idxDocFav = idxDocDis; idxDocDis = a;
							a = idxRanFav; idxRanFav = idxRanDis; idxRanDis = a;
						}

						lhs += isDoctorIAllocatedToHospitalJ[idxDocFav][idxHos];
						nblhs++;

						for(int l = idxRanFav+1; l<= idxRanDis; l++){
							for(int m = 0; m < allo.hospitals[idxHos].preferences[l].size();m++){
								int idxDocLoc = allo.hospitals[idxHos].preferences[l][m] - 1;
								if(idxDocLoc != idxDocDis && idxDocLoc != idxDocFav){
									lhs += isDoctorIAllocatedToHospitalJ[idxDocLoc][idxHos];
									nblhs++;	
								}
							}
						}	
					
						model.addConstr(nblhs * allo.hospitals[idxHos].cap * ( 1 - leftside) - (nblhs - lhs) <= nblhs * (fillingOfHospitalJUpToRankK[idxHos][idxRanDis] - isDoctorIAllocatedToHospitalJ[idxDocDis][idxHos]));	
					}
					else{ // If the couple chose different hospital
						int idxHos1 = allo.couples[i].preferences1[j][k] - 1;
						int idxHos2 = allo.couples[i].preferences2[j][k] - 1;
						model.addConstr(allo.hospitals[idxHos1].cap * ( 1 - (leftside+alpha1[i][j][k])) <= fillingOfHospitalJUpToRankK[idxHos1][allo.couples[i].ranks1[j][k]-1] - isDoctorIAllocatedToHospitalJ[idxDoc1][idxHos1]);
						model.addConstr(allo.hospitals[idxHos2].cap * ( 1 - (leftside+alpha2[i][j][k])) <= fillingOfHospitalJUpToRankK[idxHos2][allo.couples[i].ranks2[j][k]-1] - isDoctorIAllocatedToHospitalJ[idxDoc2][idxHos2]);
						model.addConstr(alpha1[i][j][k] <= 1-isDoctorIAllocatedToHospitalJ[idxDoc2][idxHos2]);
						model.addConstr(alpha2[i][j][k] <= 1-isDoctorIAllocatedToHospitalJ[idxDoc1][idxHos1]);
						model.addConstr(alpha1[i][j][k] + alpha2[i][j][k] <= 1);
					}
				}
			}
		}

		// Fill the tie density
		int n1 = 0;
		int n2 = 0;
		for (int j = 0; j < allo.oldHospitals.size(); j++){
			n1 += allo.oldHospitals[j].preferences.size();
			for(int k=0; k<allo.oldHospitals[j].preferences.size(); k++){
				n2 += allo.oldHospitals[j].preferences[k].size();
			}
		}
		allo.infos.td = 1.0 - double(n1 - allo.oldHospitals.size()) / double(n2 - allo.oldHospitals.size());
				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

		model.optimize();

		// Filling Info
		allo.infos.timeCPU =  getCPUTime() - initTimeModelCPU;
		allo.infos.UB = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
		allo.infos.opt = false;

		allo.infos.nbVar =  model.get(GRB_IntAttr_NumVars);
		allo.infos.nbCons = model.get(GRB_IntAttr_NumConstrs);
		allo.infos.nbNZ = model.get(GRB_IntAttr_NumNZs);
		GRBModel modelRelaxed = model.relax();
		modelRelaxed.optimize();
		allo.infos.contUB = modelRelaxed.get(GRB_DoubleAttr_ObjVal);

		// Get Info pre preprocessing
		if(model.get(GRB_IntAttr_Status) != 3){
			GRBModel presolvedModel = model.presolve();
			allo.infos.nbVar2 =  presolvedModel.get(GRB_IntAttr_NumVars);
			allo.infos.nbCons2 = presolvedModel.get(GRB_IntAttr_NumConstrs);
			allo.infos.nbNZ2 = presolvedModel.get(GRB_IntAttr_NumNZs);
			GRBModel presolvedModelRelaxed = presolvedModel.relax();
			presolvedModelRelaxed.optimize();
			allo.infos.contUB2 = presolvedModelRelaxed.get(GRB_DoubleAttr_ObjVal);
		}
		else{
			allo.infos.LB = -1;
			allo.infos.UB = -1;
			allo.infos.nbVar2 = -1;
			allo.infos.nbCons2 = -1;
			allo.infos.nbNZ2 = -1;
			allo.infos.contUB2 = -1;
			allo.infos.avgS = -1;
			allo.infos.stdS = -1;
			allo.infos.perS = -1;
			allo.infos.maxS = -1;
			allo.infos.avgC = -1;
			allo.infos.stdC = -1;
			allo.infos.perC = -1;
			allo.infos.maxC = -1;
			allo.infos.avgH = -1;
			allo.infos.stdH = -1;
			allo.infos.perH = -1;
			allo.infos.maxH = -1;
			allo.infos.opt = true;
		}

		// If no solution found
		if (model.get(GRB_IntAttr_SolCount) < 1){
			cout << "Failed to optimize ILP. " << endl;
			if(allo.infos.LB != -1) allo.infos.LB  = 0;
			allo.assignmentByDoctor.resize(allo.nbDoctors, -1);
			allo.assignmentByCouple1.resize(allo.nbCouples, -1);
			allo.assignmentByCouple2.resize(allo.nbCouples, -1);
			allo.assignmentByHospital.resize(allo.nbHospitals);
			return -1;
		}

		// If solution found
		allo.infos.LB = ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON);	
		if(allo.infos.LB == allo.infos.UB) allo.infos.opt = true;

		// Filling Solution
		allo.assignmentByDoctor.resize(allo.nbDoctors + 2 * allo.nbCouples , -1);
		allo.assignmentByHospital.resize(allo.nbHospitals);

		for (int i = 0; i < allo.nbDoctors + 2 * allo.nbCouples; i++){
			for (int j = 0; j<allo.nbHospitals; j++){
				if (isDoctorIAllocatedToHospitalJBool[i][j] == true && ceil(isDoctorIAllocatedToHospitalJ[i][j].get(GRB_DoubleAttr_X) - EPSILON) == 1){
					allo.assignmentByDoctor[i] = j + 1;
					allo.assignmentByHospital[j].push_back(i + 1);
				}
			}
		}

		// Filling the average and the standard deviation

		// Hospitals
		vector<int> maxR(allo.nbHospitals,-1);

		// Single
		int nbChoiceS = 0;
		int nbChoiceSs = 0;
		allo.infos.maxS = 0;
		double noAS = 0.0;
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.oldDoctors[i].nbPref; j++){
				if(allo.assignmentByDoctor[i] == allo.oldDoctors[i].preferences[j]){
					cout << i << " had its choice nb " << j + 1 << endl;
					nbChoiceS += j + 1;
					allo.infos.maxS = max(allo.infos.maxS, j + 1);
					nbChoiceSs += (j + 1) * (j + 1);
					maxR[allo.assignmentByDoctor[i]-1] = max(maxR[allo.assignmentByDoctor[i]-1],allo.oldDoctors[i].ranks[j]);
				}
			}
			if(allo.assignmentByDoctor[i] == -1){
				cout << i << " was not assigned " << endl;
				noAS+=1.0; 
			}
		}
		allo.infos.avgS = float(nbChoiceS) / float(allo.nbDoctors - noAS);
		allo.infos.stdS = sqrt((float(nbChoiceSs) - allo.infos.avgS * allo.infos.avgS)/(allo.nbDoctors - noAS));
		allo.infos.perS = noAS / float(allo.nbDoctors);

		// Couple
	//	double vtaRD = 0; double vtaRU = 0;
		double nbChoiceC = 0;
		double nbChoiceCs = 0;
		allo.infos.maxC = 0.0;
		double noAC = 0.0;

		for (int i = 0; i < allo.nbCouples; i++){
			int count = 0;
			int idxDoc1 = allo.couples[i].id1 - 1;
			int idxDoc2 = allo.couples[i].id2 - 1;
			for (int j = 0; j<allo.oldCouples[i].nbPref; j++){
				for (int k = 0; k<allo.oldCouples[i].preferences1[j].size(); k++){
					count++;
					if(allo.assignmentByDoctor[idxDoc1] == allo.oldCouples[i].preferences1[j][k] && allo.assignmentByDoctor[idxDoc2] == allo.oldCouples[i].preferences2[j][k]){				
						cout << i << " had its choice nb " <<  count << endl;
						nbChoiceC += sqrt(count);
						allo.infos.maxC = max(allo.infos.maxC, sqrt(count));
						nbChoiceCs += sqrt(count) * sqrt(count);
						maxR[allo.assignmentByDoctor[idxDoc1]-1] = max(maxR[allo.assignmentByDoctor[idxDoc1]-1],allo.oldCouples[i].ranks1[j][k]);
						maxR[allo.assignmentByDoctor[idxDoc2]-1] = max(maxR[allo.assignmentByDoctor[idxDoc2]-1],allo.oldCouples[i].ranks2[j][k]);
					}
				}
			}
			if(allo.assignmentByDoctor[idxDoc1] == -1){
				cout << i << " was not assigned " << endl;
				noAC+=1.0;
			}
		}
		allo.infos.avgC = float(nbChoiceC) / float(allo.nbCouples - noAC);
		allo.infos.stdC = sqrt((float(nbChoiceCs) - allo.infos.avgC * allo.infos.avgC)/(allo.nbCouples - noAC));
		allo.infos.perC = noAC / float(allo.nbCouples);

		// Hospital
		int nbChoiceH = 0;
		int nbChoiceHs = 0;
		allo.infos.maxH = 0;
		double noAH = 0.0;

		for(int i=0;i<allo.nbHospitals;i++){
			if(maxR[i] == -1){
				cout << "Hospital " << i << " has no doctors " << endl;
				noAH+=1.0; 
			}
			else{
				cout << "Hospital " << i << " accepts doctors up to rank " << maxR[i] << endl;
				nbChoiceH += maxR[i];
				allo.infos.maxH = max(allo.infos.maxH,maxR[i]);
				nbChoiceHs +=  maxR[i] *  maxR[i];
			}
		}

		allo.infos.avgH = float(nbChoiceH) / float(allo.nbHospitals - noAH);
		allo.infos.stdH = sqrt((float(nbChoiceHs) - allo.infos.avgH * allo.infos.avgH)/(allo.nbHospitals - noAH));
		allo.infos.perH = noAH / float(allo.nbHospitals);
	}

	// Exceptions
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}


	// End
	return 0;
}
