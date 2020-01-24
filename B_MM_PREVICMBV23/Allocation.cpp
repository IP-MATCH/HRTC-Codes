#include "Allocation.h" 

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Doctor::print(){
	cout << "Doctor " << id << "\t Preferences (" << nbPref << ") hospital [ranks] \t";
	for(int i=0; i<nbPref; i++) cout << preferences[i] << " [" << ranks[i] << "]\t" ;
	cout << endl;
}

/*	*************************************************************************************
	************************************* COUPLE ****************************************
	************************************************************************************* */

void Couple::print(){
	cout << "Couple (" << id1 << " " << id2 << ") \t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(int j=0; j<preferences1[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences1[i][j] << "-" << preferences2[i][j] << " [" << ranks1[i][j] <<"-" << ranks2[i][j]<< "]" ;
			}
			cout << ") "; 
	}	
	cout << endl;
}


/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

void Hospital::print(){
	cout << "Hospital " << id << "\t Capacity " << cap << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(int j=0; j<preferences[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences[i][j];
			}
			cout << ") "; 
	}
	cout << endl;
}

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */

void Allocation::load(const string& path, const string& filein){
	// Local variables 
	istringstream iss;
	string parser;
	string garbage;
	string nameFile = path + filein;

	// File opening
	ifstream file(nameFile.c_str(), ios::in);

	// File lecture
	if (file){
		// Name of the instance is filein
		name = filein;

		// Read the number of doctors
		getline(file, parser); iss.str(parser); iss >> nbDoctors; iss.clear(); 

		// Read the number of couples
		getline(file, parser); iss.str(parser); iss >> nbCouples; iss.clear(); 

		// Give the correct size to the indicator arrays
		idDtoD.resize(nbDoctors+ nbCouples*2 + 1);  idDtoC.resize(nbDoctors+ nbCouples*2 + 1);

		// Read the number of hospitals
		getline(file, parser); iss.str(parser); iss >> nbHospitals; iss.clear();

		// Read the preferences of each doctor
		for (int i = 0; i < nbDoctors; i++){
			Doctor d;
			int temp;

			d.nbPref = 0;
			
			getline(file, parser);
			iss.str(parser);
			iss >> d.id;
			while(iss >> temp){
				d.preferences.push_back(temp);
				d.ranks.push_back(-1);
				d.nbPref++;
			}

			idDtoD[d.id] = i;
			idDtoC[d.id] = -1;

			doctors.push_back(d);	
			iss.clear();
		}

		// Read the preferences of each couple
		for (int i = 0; i < nbCouples; i++){
			Couple c;
			int temp;
			istringstream tempIss;
			string tempString;

			c.nbTotPref = 0;
			
			getline(file, parser);
			iss.str(parser);
			iss >> c.id1; iss >> c.id2;

			for(;;){
				char tempChar = iss.get();
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref1;
						vector<int> tempPref2;
						vector<int> minus1;

						while(tempIss >> temp){
							tempPref1.push_back(temp);
							tempIss >> temp;
							tempPref2.push_back(temp);
							minus1.push_back(-1);
						}

						c.preferences1.push_back(tempPref1);
						c.preferences2.push_back(tempPref2);
						c.ranks1.push_back(minus1); 
						c.ranks2.push_back(minus1);
						c.nbTotPref+= tempPref1.size();
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref1;
							vector<int> tempPref2;
							vector<int> minus1;
							
							iss >> temp;
							tempPref1.push_back(temp);
							iss >> temp;
							tempPref2.push_back(temp);
							minus1.push_back(-1);

							c.preferences1.push_back(tempPref1);
							c.preferences2.push_back(tempPref2);
							c.ranks1.push_back(minus1); c.ranks2.push_back(minus1);
							c.nbTotPref+= tempPref1.size();
							tempIss.clear();
						}
					}
				}
			}

			c.nbPref = c.preferences1.size();
			idDtoD[c.id1] = -1; idDtoD[c.id2] = -1;
			idDtoC[c.id1] = i; idDtoC[c.id2] = i;

			couples.push_back(c);
			iss.clear();
		}

		// Read the preferences of each hospital
		for (int i = 0; i < nbHospitals; i++){
			Hospital h;
			int temp;
			istringstream tempIss;
			string tempString;
			
			h.nbTotPref = 0;

			getline(file, parser);
			iss.str(parser);
			iss >> h.id; 
			iss >> h.cap;

			int currRank = 1;

			for(;;){
				char tempChar = iss.get();
				//cout << "Just read " << tempChar << endl;
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref;
	
						while(tempIss >> temp){
							tempPref.push_back(temp);
							if(idDtoD[temp] != -1){
								for(int j=0; j<doctors[idDtoD[temp]].nbPref; j++){
									if(doctors[idDtoD[temp]].preferences[j] == i+1)
										doctors[idDtoD[temp]].ranks[j] = currRank;
								}
							}
							else{
								if(couples[idDtoC[temp]].id1 == temp){
									for(int j=0; j<couples[idDtoC[temp]].nbPref; j++){
										for(int k=0; k<couples[idDtoC[temp]].preferences1[j].size();k++){
											if(couples[idDtoC[temp]].preferences1[j][k] == i+1)
												couples[idDtoC[temp]].ranks1[j][k] = currRank;
										}
									}
								}
								if(couples[idDtoC[temp]].id2 == temp){
									for(int j=0; j<couples[idDtoC[temp]].nbPref; j++){
										for(int k=0; k<couples[idDtoC[temp]].preferences2[j].size();k++){
											if(couples[idDtoC[temp]].preferences2[j][k] == i+1)
												couples[idDtoC[temp]].ranks2[j][k] = currRank;
										}
									}
								}
							}
						}
						h.preferences.push_back(tempPref);
						currRank++;
						h.nbTotPref+= tempPref.size();
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref;
							iss >> temp;
							tempPref.push_back(temp);
							if(idDtoD[temp] != -1){
								for(int j=0; j<doctors[idDtoD[temp]].nbPref; j++){
									if(doctors[idDtoD[temp]].preferences[j] == i+1)
										doctors[idDtoD[temp]].ranks[j] = currRank;
								}
							}
							else{
								if(couples[idDtoC[temp]].id1 == temp){
									for(int j=0; j<couples[idDtoC[temp]].nbPref; j++){
										for(int k=0; k<couples[idDtoC[temp]].preferences1[j].size();k++){
											if(couples[idDtoC[temp]].preferences1[j][k] == i+1)
												couples[idDtoC[temp]].ranks1[j][k] = currRank;
										}
									}
								}
								if(couples[idDtoC[temp]].id2 == temp){
									for(int j=0; j<couples[idDtoC[temp]].nbPref; j++){
										for(int k=0; k<couples[idDtoC[temp]].preferences2[j].size();k++){
											if(couples[idDtoC[temp]].preferences2[j][k] == i+1)
												couples[idDtoC[temp]].ranks2[j][k] = currRank;
										}
									}
								}
							}
							
							h.preferences.push_back(tempPref);
							currRank++;
							h.nbTotPref+= tempPref.size();
							tempIss.clear();
						}
					}
				}
			}

			h.nbPref = h.preferences.size();
			hospitals.push_back(h);
			iss.clear();
		}

		file.close();
		oldDoctors = doctors;
		oldCouples = couples;
		oldHospitals = hospitals;
	}
	else cout << "Could not open the file " << nameFile << endl;
}

void Allocation::printProb(){
	cout << "Instance " << name << endl;
	for(int i=0; i<nbDoctors; i++){
		doctors[i].print();
	}
	for(int i=0; i<nbCouples; i++){
		couples[i].print();
	}
	for(int i=0; i<nbHospitals; i++){
		hospitals[i].print();
	}
}

int Allocation::reductionHosOff(){
	
	int nbTotRem = 0;
	vector<int> remainingCap (nbHospitals); 
	vector<int> currentAssignmentByDoctor (nbDoctors,-1);
	vector<int> maxRank (nbHospitals,-1);
	bool hbm = true;

	for(int j=0; j<nbHospitals;j++){
		remainingCap[j] = hospitals[j].cap;
	}
	
	// Loop - doctor assigned to hospital index in the table, not hospital id

	while(hbm){
		hbm = false;
		for(int j=0; j<nbHospitals;j++){
			for(int k=maxRank[j]+1; k<hospitals[j].nbPref; k++){
				if(remainingCap[j] < hospitals[j].preferences[k].size()) break;
				hbm = true;
				maxRank[j]++;

				for(int i=0; i< hospitals[j].preferences[k].size();i++){
					int idxDoc = hospitals[j].preferences[k][i];
					if(idDtoD[idxDoc] != -1){
						if(currentAssignmentByDoctor[idDtoD[idxDoc]] != -1) 
							remainingCap[currentAssignmentByDoctor[idDtoD[idxDoc]]]++ ;
						currentAssignmentByDoctor[idDtoD[idxDoc]] = j;

						// Remove hospital after j from doctor preferences
						for(int l=0; l<doctors[idDtoD[idxDoc]].nbPref; l++){
							if(doctors[idDtoD[idxDoc]].preferences[l]-1 == j){
								for(int m=l+1 ; m<doctors[idDtoD[idxDoc]].nbPref; m++){
									int idxHos = doctors[idDtoD[idxDoc]].preferences[m]-1;
									for(int n=0; n<hospitals[idxHos].preferences[doctors[idDtoD[idxDoc]].ranks[m]-1].size();n++){								
										if(hospitals[idxHos].preferences[doctors[idDtoD[idxDoc]].ranks[m]-1][n] == idxDoc){
											hospitals[idxHos].preferences[doctors[idDtoD[idxDoc]].ranks[m]-1].erase(hospitals[idxHos].preferences[doctors[idDtoD[idxDoc]].ranks[m]-1].begin() + n); 
											hospitals[idxHos].nbTotPref--;
											nbTotRem++;
										}
									}
								}
								doctors[idDtoD[idxDoc]].nbPref = l+1;
								doctors[idDtoD[idxDoc]].preferences.resize(l+1);
								doctors[idDtoD[idxDoc]].ranks.resize(l+1);		
							}
						}
					}
				}
				remainingCap[j] -= hospitals[j].preferences[k].size();
			}
		}
	}
	
	return nbTotRem;
}

int Allocation::reductionResApp(){

	int nbTotRem = 0;
	vector<int> capacityUsed (nbHospitals, 0); 
	vector<vector<vector<int> > > currentAssignmentByHospital (nbHospitals);
	vector<int> currentAssignmentByDoctor (nbDoctors,-1);
	vector<int> maxRank (nbHospitals,-1);
	bool hbm = true;

	for(int j=0; j<nbHospitals;j++)
		currentAssignmentByHospital[j].resize(hospitals[j].nbPref);
	
	// Loop - doctor assigned to hospital index in the table, not hospital id

	while(hbm){
		hbm = false;
		for(int i=0; i<nbDoctors;i++){
			if(currentAssignmentByDoctor[i] == -1 && doctors[i].preferences.size() > 0){
				hbm = true;
				int idxHos = doctors[i].preferences[0]-1;
				currentAssignmentByDoctor[i] = idxHos;
				currentAssignmentByHospital[idxHos][doctors[i].ranks[0]-1].push_back(i);
				capacityUsed[idxHos]++;
				if(capacityUsed[idxHos] >= hospitals[idxHos].cap){
					int count = 0;
					for(int k=0;k<hospitals[idxHos].nbPref;k++){
						count += currentAssignmentByHospital[idxHos][k].size();
						if(count >= hospitals[idxHos].cap){
							for(int l = k+1; l<hospitals[idxHos].nbPref;l++){
								for(int m=0; m<currentAssignmentByHospital[idxHos][l].size();m++){
									currentAssignmentByDoctor[currentAssignmentByHospital[idxHos][l][m]] = -1;
									capacityUsed[idxHos]--;
								}
								for(int m=0; m<hospitals[idxHos].preferences[l].size();m++){
									int idxDoc = hospitals[idxHos].preferences[l][m];
									if(idDtoD[idxDoc] != -1){
										for(int n=0; n < doctors[idDtoD[idxDoc]].preferences.size();n++){								
											if(doctors[idDtoD[idxDoc]].preferences[n]-1 == idxHos){
												doctors[idDtoD[idxDoc]].preferences.erase(doctors[idDtoD[idxDoc]].preferences.begin() + n); 
												doctors[idDtoD[idxDoc]].ranks.erase(doctors[idDtoD[idxDoc]].ranks.begin() + n); 
												doctors[idDtoD[idxDoc]].nbPref--;
												nbTotRem++;
											}
										}
									}
									if(idDtoC[idxDoc] != -1){
										for(int n=0; n < couples[idDtoC[idxDoc]].preferences1.size();n++){		
											for(int o=0; o < couples[idDtoC[idxDoc]].preferences1[n].size();o++){	
												if(couples[idDtoC[idxDoc]].id1 == idxDoc && couples[idDtoC[idxDoc]].preferences1[n][o]-1 == idxHos){
													// Remove the other member from its corresponding choice
													int idxDoc2 = couples[idDtoC[idxDoc]].id2 - 1;
													int idxHos2 = couples[idDtoC[idxDoc]].preferences2[n][o] - 1;
													int idxRank2 = couples[idDtoC[idxDoc]].ranks2[n][o] - 1;
													bool hbf = false;
													for(int p = 0; p < couples[idDtoC[idxDoc]].preferences2.size();p++){
														for( int q = 0; q < couples[idDtoC[idxDoc]].preferences2[p].size();q++){
															if(couples[idDtoC[idxDoc]].preferences2[p][q] == couples[idDtoC[idxDoc]].preferences2[n][o] && ( p != n || q != o)){
																hbf = true;
															}
														}
													}
													if(!hbf){
														for(int p=0; p<hospitals[idxHos2].preferences[idxRank2].size();p++){								
															if(hospitals[idxHos2].preferences[idxRank2][p]-1 == idxDoc2){
																hospitals[idxHos2].preferences[idxRank2].erase(hospitals[idxHos2].preferences[idxRank2].begin() + p); 
																hospitals[idxHos2].nbTotPref--;
															}
														}
													}
													couples[idDtoC[idxDoc]].preferences1[n].erase(couples[idDtoC[idxDoc]].preferences1[n].begin() + o); 
													couples[idDtoC[idxDoc]].preferences2[n].erase(couples[idDtoC[idxDoc]].preferences2[n].begin() + o); 
													couples[idDtoC[idxDoc]].ranks1[n].erase(couples[idDtoC[idxDoc]].ranks1[n].begin() + o); 
													couples[idDtoC[idxDoc]].ranks2[n].erase(couples[idDtoC[idxDoc]].ranks2[n].begin() + o); 
													couples[idDtoC[idxDoc]].nbTotPref--;
													nbTotRem++;
													o--;
												}
												else{
													if(couples[idDtoC[idxDoc]].id2 == idxDoc && couples[idDtoC[idxDoc]].preferences2[n][o]-1 == idxHos){
														int idxDoc2 = couples[idDtoC[idxDoc]].id1 - 1;
														int idxHos2 = couples[idDtoC[idxDoc]].preferences1[n][o] - 1;
														int idxRank2 = couples[idDtoC[idxDoc]].ranks1[n][o] - 1;
														bool hbf = false;
														for(int p = 0; p < couples[idDtoC[idxDoc]].preferences1.size();p++){
															for( int q = 0; q < couples[idDtoC[idxDoc]].preferences1[p].size();q++){
																if(couples[idDtoC[idxDoc]].preferences1[p][q] == couples[idDtoC[idxDoc]].preferences1[n][o] && ( p != n || q != o)){
																	hbf = true;
																}
															}
														}
														if(!hbf){
															for(int p=0; p<hospitals[idxHos2].preferences[idxRank2].size();p++){								
																if(hospitals[idxHos2].preferences[idxRank2][p]-1 == idxDoc2){
																	hospitals[idxHos2].preferences[idxRank2].erase(hospitals[idxHos2].preferences[idxRank2].begin() + p); 
																	hospitals[idxHos2].nbTotPref--;
																}
															}
														}
														couples[idDtoC[idxDoc]].preferences1[n].erase(couples[idDtoC[idxDoc]].preferences1[n].begin() + o); 
														couples[idDtoC[idxDoc]].preferences2[n].erase(couples[idDtoC[idxDoc]].preferences2[n].begin() + o); 
														couples[idDtoC[idxDoc]].ranks1[n].erase(couples[idDtoC[idxDoc]].ranks1[n].begin() + o); 
														couples[idDtoC[idxDoc]].ranks2[n].erase(couples[idDtoC[idxDoc]].ranks2[n].begin() + o); 
														couples[idDtoC[idxDoc]].nbTotPref--;
														nbTotRem++;
														o--;
													}
												}
											}
										}
									}
								}
								hospitals[idxHos].nbTotPref -= 	hospitals[idxHos].preferences[l].size()	;
							}
							hospitals[idxHos].nbPref = k+1;
							hospitals[idxHos].preferences.resize(k+1);
							currentAssignmentByHospital[idxHos].resize(k+1);
						}
					}
				}
			}
		}
	}

	return nbTotRem;
}

void Allocation::polish(){
	// Remove unused groups for hospitals
	for(int j=0; j<nbHospitals;j++){
		vector<vector<int> > preferences;
		int idxRem = 0;
		for(int k=0;k<hospitals[j].nbPref;k++){
			if(hospitals[j].preferences[k].size() == 0){
				idxRem++;
			}
			else{
				preferences.push_back(hospitals[j].preferences[k]);
				for(int i=0;i<hospitals[j].preferences[k].size();i++){
					int idxDoc = hospitals[j].preferences[k][i];
					if(idDtoD[idxDoc] != -1){
						for(int l=0;l<doctors[idDtoD[idxDoc]].nbPref;l++){
							if(doctors[idDtoD[idxDoc]].preferences[l]-1 == j){
								doctors[idDtoD[idxDoc]].ranks[l] -= idxRem;
							}
						}
					}
					if(idDtoC[idxDoc] != -1){
						if(couples[idDtoC[idxDoc]].id1 == idxDoc){
							for(int l=0;l<couples[idDtoC[idxDoc]].nbPref;l++){
								for(int m=0;m<couples[idDtoC[idxDoc]].preferences1[l].size();m++){
									if(couples[idDtoC[idxDoc]].preferences1[l][m]-1 == j){
										couples[idDtoC[idxDoc]].ranks1[l][m] -= idxRem;
									}
								}
							}
						}
						if(couples[idDtoC[idxDoc]].id2 == idxDoc){
							for(int l=0;l<couples[idDtoC[idxDoc]].nbPref;l++){
								for(int m=0;m<couples[idDtoC[idxDoc]].preferences2[l].size();m++){
									if(couples[idDtoC[idxDoc]].preferences2[l][m]-1 == j){
										couples[idDtoC[idxDoc]].ranks2[l][m] -= idxRem;
									}
								}
							}
						}
					}
				}
			}
		}
		hospitals[j].preferences = preferences;
		hospitals[j].nbPref = preferences.size();
	}
	// Remove unused groups for couples
	for(int i=0; i<nbCouples;i++){
		vector<vector<int> > preferences1;
		vector<vector<int> > preferences2;
		vector<vector<int> > ranks1;
		vector<vector<int> > ranks2;
	
		for(int k=0;k<couples[i].nbPref;k++){
			if(couples[i].preferences1[k].size() != 0){
				preferences1.push_back(couples[i].preferences1[k]);
				preferences2.push_back(couples[i].preferences2[k]);
				ranks1.push_back(couples[i].ranks1[k]);
				ranks2.push_back(couples[i].ranks2[k]);
			}
		}
		couples[i].preferences1 = preferences1;
		couples[i].preferences2 = preferences2;
		couples[i].ranks1 = ranks1;
		couples[i].ranks2 = ranks2;
		couples[i].nbPref = preferences1.size();
	}
}


void Allocation::reduction(){
	int nbRed1 = 0;
	int nbRed2 = 0;
	int i = 0;
	do{
		nbRed1 = reductionHosOff();
		nbRed2 = reductionResApp();
		cout << "Reduction iteration " << i << " reductionHosOff " << nbRed1 << " reductionResApp " << nbRed2 << endl;
		i++;
	}while(nbRed1 + nbRed2 != 0);
	printProb();
	polish();
}


void Allocation::printSol(){
	cout << "Allocation by hospitals" << endl;
	for(int i=0; i<nbHospitals; i++){
		cout << i+1 << ":";
		for(int j=0; j<assignmentByHospital[i].size();j++){
			cout << "\t" << assignmentByHospital[i][j];
		}
		cout << endl;
	}
}

void Allocation::printInfo(const string& pathAndFileout){
	string nameFile = pathAndFileout;
	std::ofstream file(nameFile.c_str(), std::ios::out | std::ios::app);
	file << name << "\t" << infos.opt << "\t" << infos.timeCPU << "\t" << infos.timeCPUPP << "\t"<< infos.LB << "\t" << infos.UB << "\t" << infos.contUB << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ 
		<< "\t" << infos.contUB2 << "\t" << infos.nbVar2 << "\t" << infos.nbCons2 << "\t" << infos.nbNZ2  
		<< "\t" << infos.avgS  << "\t" << infos.stdS << "\t" << infos.perS << "\t" << infos.maxS 
		<< "\t" << infos.avgC  << "\t" << infos.stdC  << "\t" << infos.perC << "\t" << infos.maxS 
		<< "\t" << infos.avgH  << "\t" << infos.stdH  << "\t" << infos.perH << "\t" << infos.maxH 
		<< "\t" << infos.td <<endl;
	file.close();
}
