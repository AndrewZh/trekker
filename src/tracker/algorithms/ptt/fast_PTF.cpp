#include "fast_PTF.h"

FPTF::FPTF(RandomDoer *_rndmr) {
	init_Frame();
	rndmr = _rndmr;
}

void FPTF::init_Frame() {
	p 			= new float[3];
	T 			= new float[3];
	N1			= new float[3];
	N2			= new float[3];
    
    F           = new float*[3];
    F[0]        = new float[3];
    F[1]        = new float[3];
    F[2]        = new float[3];

	likelihood 	= 0.0;
	prior 		= 1.0;
	posterior 	=-1.0; // This is used to check if the curve is swapped with a candidate curve
}

FPTF::~FPTF() {
	delete[] p;
	delete[] T;
	delete[] N1;
	delete[] N2;
    
    delete[] F[0];
    delete[] F[1];
    delete[] F[2];
    delete[] F;
}

void FPTF::updateF() {
    for (int i=0; i<3; i++) {
        F[i][0] =  T[i];
        F[i][1] = N1[i];
        F[i][2] = N2[i];
    }
}

void FPTF::getARandomFrame() {
	rndmr->getAUnitRandomVector(T);
	rndmr->getAUnitRandomPerpVector(N2,T);
	cross(N1,N2,T);
    updateF();
}

void FPTF::getARandomFrame(Coordinate _seed_init_direction) {
	T[0] = _seed_init_direction.x;
	T[1] = _seed_init_direction.y;
	T[2] = _seed_init_direction.z;
	rndmr->getAUnitRandomPerpVector(N2,T);
	cross(N1,N2,T);
    updateF();
}

void FPTF::swap(FPTF *fptp) {

	index 		= fptp->index;
    k1_ind      = fptp->k1_ind;
    k2_ind      = fptp->k2_ind;

	for (int i=0; i<3; i++) {
		 p[i] 	=  fptp->p[i];
		 T[i] 	=  fptp->T[i];
		N1[i] 	= fptp->N1[i];
		N2[i] 	= fptp->N2[i];
        
        for (int j=0; j<3; j++) {
            F[i][j] = fptp->F[i][j];
        }
        
	}
	
	likelihood 	= fptp->likelihood;
	prior 		= fptp->prior;
	posterior 	= fptp->posterior;

}

// To flip PTF parameterized curve
// flip signs of T, N1 and k1
// keep N2 and k2 as they are
void FPTF::flip() {
	this->walk();

	for (int i=0; i<3; i++) {
		T[i]  	*= -1;
		N1[i] 	*= -1;
	}
	updateF();

	// This flips sign of k1 by changing the corresponding precomputed index
	size_t i 	=     index % PTF_CONSTS::k_resolution;
	size_t j 	= (index - i)/PTF_CONSTS::k_resolution;
	index 		= i+(PTF_CONSTS::k_resolution-j-1)*PTF_CONSTS::k_resolution;

	likelihood 	=  0.0;
	prior 		=  1.0;
	posterior 	= -1.0; // This is used to check if the curve is swapped with a candidate curve
}

void FPTF::walk() {

	for (int i=0; i<3; i++) {
		p[i]  += PTF_CONSTS::curve_consts[index][0]*F[i][0] +  PTF_CONSTS::curve_consts[index][1]*F[i][1]  +  PTF_CONSTS::curve_consts[index][2]*F[i][2];
		T[i]   = PTF_CONSTS::curve_consts[index][3]*F[i][0] +  PTF_CONSTS::curve_consts[index][4]*F[i][1]  +  PTF_CONSTS::curve_consts[index][5]*F[i][2];
		N2[i]  = PTF_CONSTS::curve_consts[index][6]*F[i][0] +  PTF_CONSTS::curve_consts[index][7]*F[i][1]  +  PTF_CONSTS::curve_consts[index][8]*F[i][2];
	}

	normalize(T);
	cross(N1,N2,T);
	normalize(N1);
	cross(N2,T,N1);
    
    updateF();

	likelihood 	=   0.0;
	prior 		=   1.0;
	posterior 	=  -1.0; // This is used to check if the curve is swapped with a candidate curve

}



void FPTF::print() {
	std::cout << "p:  " << p[0] << " " << p[1] << " " << p[2] << std::endl;
	std::cout << "k:  " << getk() << std::endl;
	std::cout << "k1: " << getk1() << std::endl;
	std::cout << "k2: " << getk2() << std::endl;
	std::cout << "T: "  <<  T[0] << " " <<  T[1] << " " <<  T[2] << std::endl;
	std::cout << "N1: " << N1[0] << " " << N1[1] << " " << N1[2] << std::endl;
	std::cout << "N2: " << N2[0] << " " << N2[1] << " " << N2[2] << std::endl;
	std::cout << "likelihood: " << likelihood << std::endl;
	std::cout << "prior: " << prior << std::endl;
	std::cout << "posterior: " << posterior << std::endl << std::endl;
}
