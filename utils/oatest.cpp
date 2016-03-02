/** \file oatest.cpp

 C++ program: oatest

 oatest: tool for testing new algorithms

 Author: Pieter Eendebak <pieter.eendebak@gmail.com>, (C) 2014

 Copyright: See LICENSE.txt file that comes with this distribution
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "arraytools.h"
#include "arrayproperties.h"
#include "anyoption.h"
#include "tools.h"
#include "extend.h"

#include "oadevelop.h"
#include "lmc.h"

#include "conference.h"

//void mydebug(array_link al, arraydata_t &adata, OAextend &oaextend, int);
//void mydebug2(array_link al, arraydata_t &adata, OAextend &oaextend);

void mydebug10 ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=0 )
{
	double t0,t,dt;
	printf ( "mydebug2 (J5)! al %d %d\n", al.n_rows, al.n_columns );
	oaextend.info();

	al.showarray();
	al.showproperties();
	al.selectFirstColumns ( 6 ).showproperties();

	lmc_t r;
	LMCreduction_t reduction ( &adata );
	t0 =get_time_ms();
	LMCreduction_t reductionx=reduction;
	r = LMCcheck ( al, adata, oaextend, reductionx ) ;
	dt =get_time_ms()-t0;
	reductionx.symms.makeColpermsUnique();
	reductionx.symms.showColperms();
	reductionx.symms.showSymmetries();
	printf ( "original lmc_t: %d, time: %.3f [ms]\n", ( int ) r, 1e3*dt );


	printf ( "--------------------------------------------\n" );

	// pre-compute
	LMCreduction_t reductionsub = calculateSymmetryGroups ( al.deleteColumn ( -1 ), adata,  oaextend );
	reductionsub.symms.colperms[3].clear();
	reductionsub.symms.colperms[4].clear();
	reductionsub.symms.showColperms ( 1 );
	reductionsub.symms.showColcombs ( 1 );

	printf ( "--------------------------------------------\n" );
	if ( 0 ) {
		arraydata_t adfix = adata;
		// split column groups
		std::vector<int> splits; // = symmetrygroup2splits(sg, ad.ncols, verbose);
		// for(int k=0; k<=i+1; k++) splits.push_back(k);
		splits.push_back ( 0 ); // splits.push_back(5);
		adfix.set_colgroups ( splits );

		std::vector<int> w;
		w.push_back ( 0 );
		w.push_back ( 1 );
		w.push_back ( 2 );
		w.push_back ( 4 );
		w.push_back ( 11 );
		std::vector<colindex_t> ww = comb2perm<colindex_t> ( w, adata.ncols );
		print_perm ( ww );
		array_link alx = al.selectColumns ( ww );
		reduction.setArray ( al );

		// FIXME: allow non-properinitialization of array
		//reduction.mode=LMC_REDUCE_INIT;

		r = LMCcheckj5 ( alx, adfix, reduction, oaextend, 1 );
		printf ( "xxx r %d\n", r );
		return;
	}

	lmc_t rn;
	t0 =get_time_ms();
	int niter=1;
	for ( int ix=0; ix<niter; ix++ ) {
		copy_array ( al.array, reduction.array, adata.N, adata.ncols ); // hack?
		rn = LMCcheckSymmetryMethod ( al, adata, oaextend, reduction, reductionsub, dverbose ) ;
	}
	dt =get_time_ms()-t0;
	printf ( "new lmt_t %d: time: %.3f [ms]\n", rn, 1e3*dt );

	if ( r!=rn ) {
		printf ( "ERROR: orig lmc_t %d, new lmc_t %d\n", r, rn );
	}

}

void mydebug5 ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=1 )
{
	double t0,dt;
	oaextend.setAlgorithm ( MODE_J5ORDERXFAST, &adata );

	lmc_t r=LMC_MORE;
	printf ( "mydebug5! al %d %d\n", al.n_rows, al.n_columns );
	LMCreduction_t reduction ( &adata );

	t0=get_time_ms();

	// pre-compute
	//oaextend.setAlgorithm(MODE_LMC_SYMMETRY);
	LMCreduction_t reductionsub = calculateSymmetryGroups ( al.deleteColumn ( -1 ), adata,  oaextend );

	reductionsub.symms.showSymmetries();
	reductionsub.symms.showColcombs();

	dt =get_time_ms()-t0;
	printf ( "### pre-compute: time: %.3f [ms]\n", 1e3*dt );

	int niter=250;
	lmc_t rn = r;
	t0 =get_time_ms();
	for ( int ix=0; ix<niter; ix++ ) {
		copy_array ( al.array, reduction.array, adata.N, adata.ncols ); // hack?
		reduction.reset();
		reduction.updateSDpointer ( al );
		rn = LMCcheckSymmetryMethod ( al, adata, oaextend, reduction, reductionsub, dverbose ) ;
	}
	dt =get_time_ms()-t0;
	printf ( "### new lmt_t %d: time: %.3f [ms]\n", rn, 1e3*dt/niter );


	t0 =get_time_ms();
	LMCreduction_t reductionx=reduction;
	niter=4;
	for ( int ix=0; ix<niter; ix++ ) {
		reductionx.init_state=COPY;
		r = LMCcheck ( al, adata, oaextend, reductionx ) ;
	}
	dt =get_time_ms()-t0;
	if ( niter>0 ) {
		printf ( "### original lmc_t: %d, time: %.3f [ms]\n\n", ( int ) r, 1e3*dt/niter );
	}
}


void mydebug ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=1 )
{
	double t0, dt, t;
	oaextend.setAlgorithm ( MODE_J5ORDERXFAST, &adata );

	lmc_t r=LMC_MORE;
	int niter=1;
	if ( dverbose<0 ) {
		niter=10;
		dverbose=0 ;
	}

	printf ( "mydebug! al %d %d\n", al.n_rows, al.n_columns );
	LMCreduction_t reduction ( &adata );
	LMCreduction_t reductionx = reduction;

	if ( 0 ) {
		t0 =get_time_ms();
		reductionx=reduction;
		//oaextend.setAlgorithm(MODE_J4);
		for ( int ix=0; ix<niter; ix++ ) {
			reductionx.init_state=COPY;
			r = LMCcheck ( al, adata, oaextend, reductionx ) ;
		}
		dt =get_time_ms()-t0;
		printf ( "### original lmc_t: %d, time: %.3f [ms]\n\n", ( int ) r, 1e3*dt );
	}

	if ( 0 ) {
		t0 =get_time_ms();
		reductionx=reduction;
		oaextend.setAlgorithm ( MODE_LMC_SYMMETRY );
		for ( int ix=0; ix<niter; ix++ ) {
			reductionx.init_state=COPY;
			r = LMCcheck ( al, adata, oaextend, reductionx ) ;
		}
		dt =get_time_ms()-t0;
		printf ( "### symmetry lmc_t: %d, time: %.3f [ms]\n\n", ( int ) r, 1e3*dt );

	}

	// FIXME: create safe combination class (using std::vector)

	t0=get_time_ms();

	// pre-compute
	//oaextend.setAlgorithm(MODE_LMC_SYMMETRY);

	LMCreduction_t reductionsub = calculateSymmetryGroups ( al.deleteColumn ( -1 ), adata,  oaextend );

	reductionsub.symms.showSymmetries();

	dt =get_time_ms()-t0;
	printf ( "### pre-compute: time: %.3f [ms]\n", 1e3*dt );

	if ( 0 ) {
		int nc=6;
		printf ( "## symmetrices with %d cols:\n", nc );
		symmetryset xx= reductionsub.symms.symmetries[nc];
		for ( symmetryset::const_iterator it = xx.begin(); it != xx.end(); it++ ) {
			it->show();
		}
	}


	//reductionsub.showColperms(1);
	if ( dverbose ) {
		reductionsub.symms.showColcombs ( 1 );
		reductionsub.symms.showSymmetries ( 1 );
	}


	printf ( "running LMCcheckSymmetryMethod: " );
	if ( dverbose ) {
		adata.show();
		reduction.symms.show();
		reductionsub.symms.show();
	}

	lmc_t rn = r;
	t0 =get_time_ms();
	for ( int ix=0; ix<niter; ix++ ) {
		copy_array ( al.array, reduction.array, adata.N, adata.ncols ); // hack?
		reduction.updateSDpointer ( al );

		rn = LMCcheckSymmetryMethod ( al, adata, oaextend, reduction, reductionsub, dverbose ) ;
	}
	dt =get_time_ms()-t0;
	printf ( "### new lmt_t %d: time: %.3f [ms]\n", rn, 1e3*dt );


	if ( r!=rn ) {
		printf ( "###\nERROR: orig lmc_t %d, new lmc_t %d\n", r, rn );
		reductionx.mode=OA_REDUCE;
		r = LMCcheck ( al, adata, oaextend, reductionx ) ;

		reductionx.transformation->show();

	} else {
		printf ( "check: good!\n\n" );

	}
}

void mydebug1 ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=1 )
{
	oaextend.setAlgorithm ( MODE_J5ORDERX, &adata );

	lmc_t r=LMC_MORE;
	int niter=1;

	if ( dverbose<0 ) {
		niter=5;
		dverbose=0 ;
	}

	printf ( "mydebug1! al %d %d\n", al.n_rows, al.n_columns );
	LMCreduction_t reduction ( &adata );
	reduction.init_state=COPY;

	double t0 =get_time_ms();
	LMCreduction_t reductionx=reduction;
	//oaextend.setAlgorithm(MODE_J4);
	for ( int ix=0; ix<niter; ix++ ) {
		r = LMCcheck ( al, adata, oaextend, reductionx ) ;
	}
	double dt =get_time_ms()-t0;
	printf ( "original lmc_t: %d, time: %.3f [ms]\n", ( int ) r, 1e3*dt );

}

void mydebug2 ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=1 )
{
	lmc_t r=LMC_MORE;
	int niter=1;
	if ( dverbose<0 ) {
		niter=10;
		dverbose=0 ;
	}
	printf ( "mydebug2! al %d %d\n", al.n_rows, al.n_columns );
	oaextend.setAlgorithm ( MODE_J5ORDERX, &adata );

	double t0=get_time_ms(), dt;

	LMCreduction_t reduction ( &adata );
	//  oaextend.setAlgorithm(MODE_J5ORDERX, &adata);

	// pre-compute
	LMCreduction_t reductionsub = calculateSymmetryGroups ( al.deleteColumn ( -1 ), adata,  oaextend );

	dt =get_time_ms()-t0;
	printf ( "pre-compute: time: %.3f [ms]\n", 1e3*dt );

	//reductionsub.showColperms(1);
	if ( dverbose ) {
		reductionsub.symms.showColcombs ( 1 );
		reductionsub.symms.showSymmetries ( 1 );
	}

	printf ( "running LMCcheckXX: " );
	adata.show();
	lmc_t rn = r;
	t0 =get_time_ms();
	for ( int ix=0; ix<niter; ix++ ) {
		copy_array ( al.array, reduction.array, adata.N, adata.ncols ); // hack?
		rn = LMCcheckSymmetryMethod ( al, adata, oaextend, reduction, reductionsub, dverbose ) ;
	}
	dt =get_time_ms()-t0;
	printf ( "new lmt_t %d: time: %.3f [ms]\n", rn, 1e3*dt );

}

void mydebug2d ( array_link al, arraydata_t &adata, OAextend &oaextend, int dverbose=1 )
{
	lmc_t r=LMC_MORE;
	int niter=1;
	if ( dverbose<0 ) {
		niter=10;
		dverbose=0 ;
	}
	printf ( "mydebug2! al %d %d\n", al.n_rows, al.n_columns );
	oaextend.setAlgorithm ( MODE_LMC_SYMMETRY, &adata );

	double t0=get_time_ms(), dt;

	LMCreduction_t reduction ( &adata );

	adata.show();
	lmc_t rn = r;
	t0 =get_time_ms();
	for ( int ix=0; ix<niter; ix++ ) {
		copy_array ( al.array, reduction.array, adata.N, adata.ncols ); // hack?
		reduction.init_state=COPY;
		rn = LMCcheck ( al, adata, oaextend, reduction ) ;
	}
	dt =get_time_ms()-t0;
	printf ( "lmc_symmetry direct lmt_t %d: time: %.3f [ms]\n", rn, 1e3*dt );

}

#include <Eigen/SVD>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/LU>

#include "Deff.h"
#include "arraytools.h"
#include "strength.h"

using namespace Eigen;


//std::vector<double> Defficiencies (const array_link &al, const arraydata_t & arrayclass, int verbose ) ;

array_link finalcheck ( const array_link &al,  const arraydata_t &arrayclass,  std::vector<double> alpha, int verbose, int optimmethod, int niterx, int nabortx )
{
	int nabort=al.n_rows*al.n_columns+2;
	int niter= nabort+5;
	array_link  al2 = optimDeff2level ( al, arrayclass, alpha, verbose>=3,  DOPTIM_UPDATE,  niter, nabort );


	std::vector<double> dd0 = al.Defficiencies();
	double d0 = scoreD ( dd0, alpha );

	std::vector<double> dd = al2.Defficiencies();
	double d = scoreD ( dd, alpha );

	if ( d>d0 ) {
		printf ( "finalcheck: %f -> %f\n", d0, d );
	}

	return al2;
}


//TODO: use optimDeff2level by default
//TODO: make enum for selection strategy?
//TODO: IMPLEMENT flip method as well
//TODO: Run valgrind...


void speedtest ( const Eigen::MatrixXf mymatrix, int fac=1000 )
{
	double t0=get_time_ms();
	int r=-1;
	for ( int i=0; i<1000*fac; i++ ) {
		Eigen::FullPivLU<Eigen::MatrixXf> lu ( mymatrix );
		double thr=lu.threshold();
		r = lu.rank();
	}

	double	dt=get_time_ms()-t0;

	printf ( "speedtest %.3f (rank %d)\n", dt, r );
}

void speedtest ( const Eigen::MatrixXd mymatrix, int fac=1000 )
{
	double t0=get_time_ms();
	int r=-1;
	for ( int i=0; i<1000*fac; i++ ) {
		Eigen::FullPivLU<Eigen::MatrixXd> lu ( mymatrix );
		double thr=lu.threshold();
		r = lu.rank();
	}

	double	dt=get_time_ms()-t0;

	printf ( "speedtest %.3f (rank %d)\n", dt, r );
}
#include "graphtools.h"


template < class Type>
/// return the condition number of a matrix
double conditionNumber ( const array_link M )
{
	MatrixXd A = arraylink2eigen ( M );
	JacobiSVD<Matrix<Type,-1,-1> > svd ( A );
	double cond = svd.singularValues() ( 0 ) / svd.singularValues() ( svd.singularValues().size()-1 );
	return cond;
}






/// convert 2-level design to second order interaction matrix
inline void array2eigenxf ( const array_link &al, Eigen::MatrixXd &mymatrix )
{
	int k = al.n_columns;
	int n = al.n_rows;
	int m = 1 + k + k* ( k-1 ) /2;

	mymatrix = Eigen::MatrixXd::Zero ( n,m );

	// init first column
	int ww=0;
	for ( int r=0; r<n; ++r ) {
		mymatrix ( r, ww ) = 1;
	}

	// init array
	ww=1;
	for ( int c=0; c<k; ++c ) {
		int ci = c*n;
		for ( int r=0; r<n; ++r ) {
			mymatrix ( r, ww+c ) = al.array[r+ci];
		}
	}

	// init interactions
	ww=k+1;
	for ( int c=0; c<k; ++c ) {
		int ci = c*n;
		for ( int c2=0; c2<c; ++c2 ) {
			int ci2 = c2*n;

			const array_t * p1 = al.array+ci;
			const array_t * p2 = al.array+ci2;
			for ( int r=0; r<n; ++r ) {
				mymatrix ( r, ww ) = ( *p1+*p2 ) %2;
				p1++;
				p2++;
			}
			ww++;
		}
	}

	mymatrix.array() *= 2;
	mymatrix.array() -= 1;
}


int main ( int argc, char* argv[] )
{
	AnyOption opt;
	/* parse command line options */
	opt.setFlag ( "help", 'h' );   /* a flag (takes no argument), supporting long and short form */
	opt.setOption ( "output", 'o' );
	opt.setOption ( "input", 'I' );
	opt.setOption ( "rand", 'r' );
	opt.setOption ( "verbose", 'v' );
	opt.setOption ( "ii", 'i' );
	opt.setOption ( "xx", 'x' );
	opt.setOption ( "dverbose", 'd' );
	opt.setOption ( "rows" );
	opt.setOption ( "cols" );
	opt.setOption ( "nrestarts" );
	opt.setOption ( "niter" );
	opt.setOption ( "mdebug", 'm' );
	opt.setOption ( "oaconfig", 'c' ); /* file that specifies the design */

	opt.addUsage ( "Orthonal Array: oatest: testing platform" );
	opt.addUsage ( "Usage: oatest [OPTIONS] [FILE]" );
	opt.addUsage ( "" );
	opt.addUsage ( " -h --help  			Prints this help " );
	opt.processCommandArgs ( argc, argv );


	int randvalseed = opt.getIntValue ( 'r', 1 );
	int ix = opt.getIntValue ( 'i', 11 );
	int r = opt.getIntValue ( 'r', 0 );
	int xx = opt.getIntValue ( 'x', 3 );
	int niter  = opt.getIntValue ( "niter", 100 );
	int verbose  = opt.getIntValue ( "verbose", 1 );

	char *input = opt.getValue ( 'I' );
	if ( input==0 )
		input="pp0.oa";

	srand ( randvalseed );
	if ( randvalseed==-1 ) {
		randvalseed=time ( NULL );
		printf ( "random seed %d\n", randvalseed );
		srand ( randvalseed );
	}


	print_copyright();
	//cout << system_uname();
	setloglevel ( NORMAL );

	/* parse options */
	if ( opt.getFlag ( "help" ) || opt.getFlag ( 'h' ) || opt.getArgc() <0 ) {
		opt.printUsage();
		exit ( 0 );
	}

	setloglevel ( SYSTEM );

	if ( 0 ) {

		arraylist_t ll = readarrayfile ( "/home/eendebakpt/tmp/sp0-split-10/sp0-split-10-pareto-64.2-2-2-2-2-2-2-2-2-2.oa" );
		array_link al=ll[0];

		int r0= ( al ).rank();
		int r=array2xf ( al ).rank();
		printf ( "rank: %d %d\n",  r0,r );

		arraydata_t arrayclass=arraylink2arraydata ( al, 1 );

		OAextend oaextend=OAextend();
		oaextend.checkarrays=0;
		oaextend.setAlgorithm ( MODE_J5ORDERXFAST, &arrayclass );
		setloglevel ( NORMAL );

		arrayclass.show();
		oaextend.info();

		printf ( "extend!\n" );
		al.show();

		int current_col=al.n_columns;
		arraylist_t extensions;
		int nr_extensions = extend_array ( al.array, &arrayclass, current_col,extensions, oaextend );

//arraylist_t ww=extend_array(al, arrayclass, oaextend);

		return 0;
	}

	{
			// reduce conference
		
		
	array_link al = exampleArray(19,1);
//	conference_transformation_t T(al);
//	T.randomize();
//	conference_transformation_t Ti = T.inverse();
//	array_link alx = Ti.apply(T.apply(al));		
//	myassert(alx==al, "transformation of conference matrix")	
	
	printf("input array:\n");
	al.showarray();
		array_link alx = reduceConference(al, verbose);

		alx.showarray();
		exit(0);
	}

	{
		array_link al = exampleArray ( r,1 );
		array_link alsub = al.selectFirstColumns ( al.n_columns-3 );


		{
			double t0;
			t0 = get_time_ms();
			arraylist_t ll = readarrayfile ( input );
			for ( int i=0; i<niter; i++ ) {
				al=ll[i % ll.size()];
				std::vector<int> j5 = al.Jcharacteristics ( 5 );
			}
			std::vector<int> j5 = al.Jcharacteristics ( 5 );
			display_vector ( j5 );
			printf ( "\n" );

			return 0;
		}

		rankStructure rs ( xx );
		printfd ( "here\n" );

		rs.info();
		rs.verbose=1;

		int rd=-1, r=-1;

		if ( 1 ) {
			printfd ( "here\n" );
			int r = rs.rankxf ( al );
			int rd= rs.rankxfdirect ( al );
			printf ( "rank %d %d\n", r, rd );
			assert ( r==rd );
		}

		if ( rs.verbose ) {
			Eigen::ColPivHouseholderQR<Eigen::MatrixXd>::PermutationType subperm = rs.matrixP();
			Eigen::MatrixXi  xx= subperm.indices();

			printf ( "rs.decomp.colsPerm:\n" );
			std::cout << xx.transpose() << std::endl;
		}

		printf ( "\ntimings:\n" );
		rs.verbose=0;
		double t0;

		t0 = get_time_ms();
		arraylist_t ll = readarrayfile ( input );
		for ( int i=0; i<niter; i++ ) {
			al=ll[i % ll.size()];
			r = rs.rankxf ( al );
		}
		printf ( "rankxf: %.2f [s] ...\n", get_time_ms() - t0 );
		t0= get_time_ms();
		for ( int i=0; i<niter; i++ ) {
			al=ll[i % ll.size()];
			rd = rs.rankxfdirect ( al );
			//break;
		}
		printf ( "rankxfdirect: %.2f [s]\n", get_time_ms() - t0 );


		return 0;
	}

	arraylist_t ll = readarrayfile ( input );
	printf ( "oatest: %d arrays\n", ( int ) ll.size() );
	for ( int i=0; i< ( int ) ll.size(); i++ ) {
		array_link al = ll[i];

		array_link A = array2xf ( al );

		//Eigen::MatrixXd mymatrix = arraylink2eigen ( A );
		//printf("condition number: %e\n", conditionNumber(mymatrix) );

		if ( ( int ) i==-1 ) {
			A.show();
			//A.transposed().showarray();
			A.showarray();
		}
		if ( int ( i ) ==-1 ) {
			al.showarray();
		}
		int rank =-1;

		if ( 0 ) {
			// use float: about 20% speed profit
			array_link A = array2xf ( al );
			Eigen::MatrixXd mymatrix = arraylink2eigen ( A );
			int fac=4;
			speedtest ( mymatrix, fac );

			Eigen::MatrixXf mymatrixf = mymatrix.cast<float>();
			speedtest ( mymatrixf, fac );


			exit ( 0 );
		}

		if ( r==2 ) {
			al=exampleArray ( 1,1 );
			array_link A = array2xf ( al );
			Eigen::MatrixXd mymatrix = arraylink2eigen ( A );
			//mymatrix=arraylink2eigen ( al );

			Eigen::FullPivLU<Eigen::MatrixXd> lu ( mymatrix );

			double thr=lu.threshold();
			rank = lu.rank();

			//MatrixXd l; //= MatrixXd::Identity();
//l.block<5,3>(0,0).triangularView<StrictlyLower>() = lu.matrixLU();
			cout << "Here is the L part:" << endl;
			MatrixXd lx = lu.matrixLU();
			MatrixXd l = MatrixXd::Identity ( mymatrix.rows() , mymatrix.rows() ); // = lu.matrixLU();
			eigenInfo ( l );
			eigenInfo ( lx );
			int q = lx.rows();

			l.block ( 0,0, mymatrix.rows(), q ).triangularView<StrictlyLower>() = lx.block ( 0,0,mymatrix.rows(), q );
			cout << l << endl;
			cout << "Here is the U part:" << endl;
			MatrixXd u = lu.matrixLU().triangularView<Upper>();
			cout << u << endl;
			eigenInfo ( l );
			eigenInfo ( u );

//cout << l * u ;

			MatrixXd r = lu.permutationP().inverse() * l * u * lu.permutationQ().inverse();

			cout << "Difference:" << endl;
			std::cout << r-mymatrix;
			cout << "Let us now reconstruct the original matrix m:" << endl;
			cout << lu.permutationP().inverse() * l * u * lu.permutationQ().inverse() << endl;

			cout << "good format:" << endl;

			MatrixXd g = l.inverse() * lu.permutationP() * mymatrix * lu.permutationQ();
			cout << g << endl;

			int fac=10;
			speedtest ( mymatrix, fac );
			speedtest ( g, fac );

			exit ( 0 );
		}

		switch ( r ) {
		case 0: {
			rank= A.rank();
			if ( i<0 ) {
				printf ( "rank %d: %d\n", ( int ) i, rank );

			}
		}
		break;
		case 1: {
			Eigen::MatrixXd mymatrix = arraylink2eigen ( A );
			Eigen::ColPivHouseholderQR<Eigen::MatrixXd> lu_decomp ( mymatrix );
			double thr=lu_decomp.threshold();
			rank = lu_decomp.rank();
			if ( ( int ) i<-1 ) {
				printf ( "rank %d: %d\n", ( int ) i, rank );

			}
		}
		break;
		case 2: {
			Eigen::MatrixXd mymatrix = arraylink2eigen ( A );

			Eigen::FullPivHouseholderQR<Eigen::MatrixXd> lu_decomp ( mymatrix );
			//Eigen::FullPivLU<Eigen::MatrixXd> lu_decomp ( mymatrix );
			double thr=lu_decomp.threshold();
			rank = lu_decomp.rank();

			double mp = lu_decomp.maxPivot();

			if ( i<14 ) {
				printf ( "\n---- array %d\n  threshold used: %e / %e\n" , ( int ) i, lu_decomp.threshold(), mp*lu_decomp.threshold() );
				//std::cout << x << std::endl;;
				eigenInfo ( mymatrix );

				Eigen::MatrixXd x = lu_decomp.matrixQ();
				//Eigen::MatrixXd x = lu_decomp.matrixLU();
				//x = lu_decomp2.matrixQ();
				VectorXd q = x.diagonal();

				double minbest=fabs ( q ( 0 ) );
				for ( int x=0; x<q.rows(); x++ ) {
					double val =  q ( x );


					if ( fabs ( val ) <1e-1 ) {
						printf ( "diag %d: %e\n", x,val );
					}
					if ( ( fabs ( val ) <minbest ) && ( fabs ( val ) >thr ) ) {
						minbest=val;
					}
				}
				//printf ( "minbest %e\n", minbest );
				//printf ( "rank %d: %d\n", ( int ) i, rank );
				// std::cout << "rank " << rank << ", diag: " << q << std::endl;
			}
		}
		break;
		case 3: {
			//printf ( " use SVD\n" );
			Eigen::MatrixXd mymatrix = arraylink2eigen ( A );

			JacobiSVD<Eigen::MatrixXd> svd ( mymatrix );

			rank=svd.rank();
		}
		break;

		default
				:
			printf ( "no such r value\n" );
			break;
		}

		if ( i<5 ) {
			printf ( "i %d: rank %d\n", ( int ) i, rank );
		}
	}





	if ( 0 ) {

		array_link al = exampleArray ( 11, 0 );
		al = exampleArray ( ix, 1 );
		al.showproperties();
		{
			arraydata_t arrayclassx = arraylink2arraydata ( al );
			array_transformation_t trx ( arrayclassx );
			trx.show();
			return 0;
		}

		//al=al.reduceLMC();
		array_link al2 = al.randomperm();

		if ( 1 ) {
			int verbose=1;
			array_link alr = al.randomcolperm();
			alr=al.randomperm();

			alr.showarray();


			std::pair<array_link, std::vector<int> > Gc = array2graph ( alr,  verbose );
			arraydata_t arrayclass = arraylink2arraydata ( alr );
			printfd ( "colors: " );
			display_vector ( Gc.second );
			printf ( "\n" );

			std::vector<int> tr = nauty::reduceNauty ( Gc.first, Gc.second );
			printf ( "canon: " );
			display_vector ( tr );
			printf ( "\n" );
			std::vector<int> tri = invert_permutation ( tr );
			array_transformation_t ttm = oagraph2transformation ( tri, arrayclass, verbose );

			array_link Gm = transformGraph ( Gc.first, tri, 1 );
			//printf("G minimal\n"); Gm.showarray(); return 0;


			ttm.show();

			ttm.apply ( alr ).showarray();

			return 0;
		}

	}

	return 0;

}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
