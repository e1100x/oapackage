/** \file oagraph.cpp

 C++ program: oagraph

 oagraph: tool for testing new algorithms

 Author: Pieter Eendebak <pieter.eendebak@gmail.com>, (C) 2015

 Copyright: See LICENSE.txt file that comes with this distribution
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "arraytools.h"
#include "arrayproperties.h"
#include "anyoption.h"
#include "tools.h"
#include "extend.h"

#include "graphtools.h"


int main ( int argc, char* argv[] )
{
	AnyOption opt;
	/* parse command line options */
	opt.setFlag ( "help", 'h' );   /* a flag (takes no argument), supporting long and short form */
	opt.setOption ( "output", 'o' );
	opt.setOption ( "rand", 'r' );
	opt.setOption ( "verbose", 'v' );
	opt.setOption ( "ii", 'i' );
	opt.setOption ( "dverbose", 'd' );
	opt.setOption ( "oaconfig", 'c' ); /* file that specifies the design */

	opt.addUsage ( "Orthogonal Array: oagraph: testing platform" );
	opt.addUsage ( "Usage: oagraph [OPTIONS] " );
	opt.addUsage ( "" );
	opt.addUsage ( " -h --help  			Prints this help " );
	opt.processCommandArgs ( argc, argv );


	int randvalseed = opt.getIntValue ( 'r', 1 );
	srand ( randvalseed );
	if (randvalseed==-1) {
		srand(time(NULL));	
	}

	int verbose = opt.getIntValue ( 'v', 1 );
	int ix = opt.getIntValue ( 'i', 2 );

	print_copyright();
	setloglevel ( NORMAL );

	/* parse options */
	if ( opt.getFlag ( "help" ) || opt.getFlag ( 'h' ) || opt.getArgc() <0 ) {
		opt.printUsage();
		exit ( 0 );
	}

	setloglevel ( SYSTEM );

	// select a design
	array_link al = exampleArray ( ix, 2 );
	//al.showarray();
	arraydata_t arrayclass = arraylink2arraydata ( al );
	array_transformation_t trans ( arrayclass );
	trans.reset();	trans.randomize();
//	trans.reset(); trans.randomizerowperm();
//trans.reset(); trans.randomizecolperm();
//int j = rand() % al.n_rows; trans.reset(); trans.rperm[j]=0;trans.rperm[0]=j;
//int j = rand() % al.n_columns; //j=randvalseed; trans.reset(); trans.cperm[j]=0;trans.cperm[0]=j;
	array_link alr = trans.apply ( al ); // randomized array

	
	if (1) {
		alr.showarray();
		std::pair<array_link, std::vector<int> > Gc = array2graph ( alr,  verbose );
		
		//for(int k=0; k<Gc.second.size(); k++) Gc.second[k]=0;
		std::vector<int> tr = nauty::reduceNauty ( Gc.first, Gc.second );
		printf ( "canon: " ); display_vector ( tr ); printf ( "\n" );
		//return 0;
		
		Gc.first.showarray();
		std::vector<int> tri = invert_permutation(tr);
		array_link Gm = transformGraph ( Gc.first, tri, 1);
		printf("G minimal\n"); Gm.showarray();
		//printf("j %d\n", j);
		return 0;

		//		array_transformation_t ttm = oagraph2transformation ( tr, arrayclass, verbose );

	}

	// reduce to minimal form
	array_transformation_t ttm = reduceOAnauty ( al, 1 );
	array_link alm = ttm.apply ( al );
	ttm.show();
	
	//alr=alm;
	

	// reduce to minimal form
	array_transformation_t ttrm = reduceOAnauty ( alr, 1 );
	array_link alrm = ttrm.apply ( alr );

	printf("--- al\n"); al.showarraycompact();
	printf("--- alr\n"); alr.showarraycompact();
	printf("--- alm\n"); alm.showarraycompact();
	printf("--- alrm\n"); alrm.showarraycompact();


	if ( alrm!=alm ) {
		printf ( "error with nauty reduction...\n" );
	}

//return 0;

if (0) {
	array_link alx = al.randomperm();
	array_transformation_t ttx = reduceOAnauty ( alx, 0 );
	array_link alxm = ttx.apply ( alx );

	printf ( "--- alxm \n" );
	alxm.showarraycompact();
}

	if ( verbose )
		printf ( "done\n" );


	return 0;

}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
