#ifndef ALLOCATION_H
	#define ALLOCATION_H
	
	using namespace std;
	#include <iostream> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <set>

	class Hospital;
	class Doctor;
	class Couple;
	class Assignment;
	class Allocation;

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

	class Doctor{
	public:
		int id;
		int nbPref;
		vector<int> preferences;
		vector<int> ranks;
	
		void print();
	};

/*	*************************************************************************************
	***********************************  COUPLE *****************************************
	************************************************************************************* */

	class Couple{
	public:
		int id1;
		int id2;
		int nbPref;
		int nbTotPref;
		vector<vector<int> > preferences1;
		vector<vector<int> > ranks1;
		vector<vector<int> > preferences2;
		vector<vector<int> > ranks2;
		void print();
	};

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

	class Hospital{
	public:
		int id;
		int cap;
		int nbPref;
		int nbTotPref;
		vector<vector<int> > preferences;
		
		void print();
		bool prefers(const Couple &couple, Allocation *allo);
		bool prefers(int doctor, Allocation *allo);
		int getRankOfDoctor(int doctor);
	};

/*	*************************************************************************************
	************************************* INFO ******************************************
	************************************************************************************* */

	class Info{
	public:
		bool opt;
		double timeCPU;
		double timeCPUPP;
		int LB;
		int UB;
		float contUB;
		int nbCons;
		int nbVar;
		int nbNZ;
		float contUB2;
		int nbCons2;
		int nbVar2;
		int nbNZ2;
		double avgS;
		double stdS;
		double perS;
		int maxS;
		double avgC;
		double stdC;
		double perC;
		double maxC;
		double avgH;
		double stdH;
		double perH;
		int maxH;
		double td;
	};

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */
	class Allocation{
	public:
		// Data read from the file
		string name;
		int nbDoctors;
		int nbCouples;
		int nbHospitals;

		vector<Doctor> doctors; vector<Doctor> oldDoctors;
		vector<Couple> couples; vector<Couple> oldCouples;
		vector<Hospital> hospitals; vector<Hospital> oldHospitals;

		vector<int> idDtoD;
		vector<int> idDtoC;

		// Given by the ILP model
		vector<int> assignmentByDoctor;
		vector<int> assignmentByCouple1;
		vector<int> assignmentByCouple2;
		vector<vector<int> > assignmentByHospital;
		Info infos;
		
		void load(const string& path, const string& filein);
		void printProb();
		int  reductionHosOff();
		int  reductionResApp();
		void polish();
		void reduction();
		void printSol();
		void printInfo(const string& pathAndFileout);

	};
	

#endif 