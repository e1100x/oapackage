/** \file conference.cpp

 Author: Pieter Eendebak <pieter.eendebak@gmail.com>, (C) 2015

 Copyright: See LICENSE.txt file that comes with this distribution
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <algorithm>

#include <vector>

#include "arraytools.h"
#include "arrayproperties.h"
#include "extend.h"

#include "graphtools.h"

#include "lmc.h"
#include "conference.h"

/*

######## Extension numbers

When generating candidate extension columns many properties of the column are determined by N and the position of the zero in the column. (If the first 2 columns are in normal form).
Below we write down the conditions and solve the resulting equations.

#### Zero in middle block

Values in column k for k > 1 and k <= N/2 + 1 = q + 2:

First value 1, value at position k is 0.

From the remaining N-2 values we have q=(N-2)/2 positive. Inner product with column 0 is then satisfied.

Value v at position 1 is irrevant to innerproduct with column 1, this can be either +1 or -1

Top: 2 elements, Upper half: N/2-1 elements, bottom: N/2-1 elements

put q1 in upper half and q2 in lower half. we need:

[Case v=-1]

(inner prod with col 0 == 0):

q1+q2=q

(inner prod with col 1 == 0)

q1 - (N/2 - 1 - 1 -q1) - [ q2-(N/2-1-q2) ]  + 1= 0

--> 2q1 - 2q2  +2 = 0
--> q1 - q2  = -1

---> q1+(q1+1)=q --> 2q1 = q -1  --> q1 = q/2 - 1/2 = (q-1)/2

[Case v=1] (<-- q even/odd?)

(inner prod 0 == 0):

q1+q2+1=q

(inner prod 1 == 0)

q1 - (N/2 -1 -1 -q1) - [ q2-(N/2-1-q2) ]  + 1 = 0

--> 2q1 - 2q2 + 2 = 0
--> q1 - q2 + 1 =0

---> q1+(q1+1)+1=q --> 2q1 = q - 2 --> q1 = q/2 - 1

Example:

N=8
q=3
q1=1
q2=2
v=1

N=6
q=2
q1=0
q2=1
n1=1
v=1

#### Zero in last block

Values in column k for k > 1 and k > N/2 + 1 = 2 + q:
First value 1, value at position k is 0.

From the remaining N-2 values we have q=(N-2)/2 positive. Inner product with column 0 is then satisfied.

Value v at position 1 is irrevant to innerproduct with column 1, this can be either +1 or -1

Top: 2 elements, Upper half: N/2-1 elements, bottom: N/2-1 elements

The zero is in the bottom part. Let q1 and q2 be the number of +1 signs in the upper and lower half, respectively.
We need:

[Case v=-1]

(inner prod with col 0 == 0):

q1+q2=q

(inner prod with col 1 == 0)

1 + q1 + (q-1-q2) = q

[Case v=1] (<-- q even)

(inner prod 0 == 0):

q1+q2+1=q

(inner prod 1 == 0)

1 + q1 + (q-1-q2) = q

==> v=+1: q1=(q-1)/2, q2=q1
==> v=-1: q1=q/2, q2=q1

Examples:

N=8
q=3
q1=?
q2=?
v=?

N=6
q=2
q1=?0
q2=?
n1=?
v=1

 */

/// return true of the argument is even
inline bool iseven ( int q )
{
	return ( q%2 ) ==0;
}

/// return parameters of a conference design
void getConferenceNumbers ( int N,int k, int &q, int &q1, int &q2, int &v )
{
	q = ( N-2 ) /2;

	if ( k< 2+q ) {
		if ( iseven ( q ) ) {
			q1=q/2-1;
			v=1;
			q2=q1+1;
		} else {
			q1= ( q-1 ) /2;
			v=-1;
			q2=q1+1;
		}
	} else {

		if ( iseven ( q ) ) {
			q1=q/2;
			v=-1;
			q2=q1;
		} else {
			q1= ( q-1 ) /2;
			v=1;
			q2=q1;
		}
	}
	//printfd ( "getConferenceNumbers: k %d, q %d: q1 q2 %d, %d\n", k, q, q1, q2 );
}

conference_t::conference_t ( int N, int k )
{

	this->N = N;
	this->ncols = k;
	this->ctype = CONFERENCE_NORMAL;
	this->itype = CONFERENCE_ISOMORPHISM;
	this->j3zero = 0;
}

array_link conference_t::create_root_three ( ) const
{
	array_link al ( this->N, 3, 0 ); // c.ncols

	al.at ( 0,0 ) =0;
	for ( int i=1; i<this->N; i++ ) {
		al.at ( i, 0 ) = 1;
	}
	for ( int i=0; i<this->N; i++ ) {
		if ( i==1 ) {
			al.at ( 1,1 ) =0;
			continue;
		}
		if ( i<=this->N/2 )
			al.at ( i, 1 ) = 1;
		else
			al.at ( i, 1 ) = -1;
	}

	int q, q1, q2, v;
	const int k = 2;
	const int N = this->N;
	getConferenceNumbers ( this->N, k, q, q1, q2, v );

	for ( int i=3; i<N; i++ )
		al.at ( i,2 ) =-1;
	al.at ( 0,2 ) =1;
	al.at ( 1,2 ) =v;
	al.at ( 2,2 ) =0;
	for ( int i=0; i<q1; i++ )
		al.at ( 2+1+i,2 ) = 1;
	for ( int i=0; i<q2; i++ )
		al.at ( 2+q+i,2 ) = 1;

	return al;
}
array_link conference_t::create_root ( ) const
{
	array_link al ( this->N, 2, 0 ); // c.ncols

	al.at ( 0,0 ) =0;
	for ( int i=1; i<this->N; i++ ) {
		al.at ( i, 0 ) = 1;
	}
	for ( int i=0; i<this->N; i++ ) {
		if ( i==1 ) {
			al.at ( 1,1 ) =0;
			continue;
		}
		if ( i<=this->N/2 )
			al.at ( i, 1 ) = 1;
		else
			al.at ( i, 1 ) = -1;
	}

	return al;
}

bool isConferenceFoldover ( const array_link &al, int verbose )
{

	array_link alt = al.transposed();
	array_link alt2 = alt*-1;

	for ( int i=0; i<al.n_rows; i++ ) {
		array_link alx = alt.selectColumns ( i );
		int foundcol=0;
		for ( int j=0; j<al.n_rows; j++ ) {
			array_link alx2 = alt2.selectColumns ( j );
			if ( alx==alx2 ) {
				foundcol=1;
				break;
			}
		}
		if ( !foundcol ) {
			if ( verbose ) {
				printf ( "isConferenceFoldover: no foldover for row %d\n", i );
			}
			return false;
		}
	}
	return true;
}

conference_transformation_t reduceConferenceTransformation ( const array_link &al, int verbose )
{
	const int nr = al.n_rows;
	const int nc=al.n_columns;
	const int nn = 2* ( nr+nc );
	/// create graph
	array_link G ( 2* ( nr+nc ), 2* ( nr+nc ), array_link::INDEX_DEFAULT );
	G.setconstant ( 0 );

	if ( verbose )
		printf ( "reduceConference: %d, %d\n", nr, nc );

	std::vector<int> colors ( 2* ( nr+nc ) );

	const int roffset0=0;
	const int roffset1=nr;
	const int coffset0=2*nr;
	const int coffset1=2*nr+nc;

	/*
	Add edges as follows:
	(1)  r[i]--r'[i] for i=0..nr-1;  c[j]--c'[j] for j=0..nc-1.
	(2)  r[i]--c[j] and r'[i]--c'[j] for all A[i,j] = +1
	(3)  r[i]--c'[j] and r'[i]--c[j] for all A[i,j] = -1.
	Zeros in A don't cause any edges.
	*/

	// set colors
	for ( int i=0; i<coffset0; i++ )
		colors[i]=0;
	for ( int i=coffset0; i<coffset0+2*nc; i++ )
		colors[i]=1;

	// (1)
	for ( int i=0; i<nr; i++ )
		G.atfast ( roffset0+i, i+roffset1 ) =1;
	for ( int i=0; i<nc; i++ )
		G.atfast ( coffset0+i, i+coffset1 ) =1;

	// (2), (3)
	for ( int c=0; c<nc; c++ ) {
		for ( int r=0; r<nr; r++ ) {
			if ( al.atfast ( r,c ) ==1 ) {
				G.atfast ( roffset0+r, coffset0+c ) =1;
				G.atfast ( roffset1+r, coffset1+c ) =1;
			} else {
				if ( al.atfast ( r,c ) ==-1 ) {
					G.atfast ( roffset0+r, coffset1+c ) =1;
					G.atfast ( roffset1+r, coffset0+c ) =1;
					//G.atfast ( coffset0+c, roffset1+r ) =1;
				}
			}
		}
	}

	// make symmetryic
	/*
	for ( int i=0; i<nn; i++ )
		for ( int j=i; j<nn; j++ )
			G.atfast ( j,i ) =G.atfast ( i, j );
	*/
	const int nrg = G.n_rows;
	for ( int i=0; i<nn; i++ ) {

		array_t *x = G.array+i*nrg; // offset to column
		for ( int j=i; j<nn; j++ ) {
			x[j] =G.array[i+j*nrg];
		}
	}

	if ( verbose>=3 ) {
		printf ( "reduceConference: incidence graph:\n" );
		printf ( "   2x%d=%d row vertices and 2x%d=%d column vertices\n", nr, 2*nr, nc, 2*nc );
		G.showarray();
	}
	/// call nauty
	const std::vector<int> tr = nauty::reduceNauty ( G, colors, verbose>=2 );
	const std::vector<int> tri = invert_permutation ( tr );
	const std::vector<int> trx=tri;

	// extract transformation
	if ( verbose>=2 ) {
		if ( verbose>=3 ) {
			array_link Gx = transformGraph ( G, tri, 0 );
			printfd ( "transformed graph\n" );
			Gx.showarray();
		}
		std::vector<int> tr1 = std::vector<int> ( trx.begin () , trx.begin () + 2*nr );
		std::vector<int> tr2 = std::vector<int> ( trx.begin () +coffset0, trx.end() );
		printf ( "  row vertex transformations: " );
		display_vector ( tr1 );
		printf ( "\n" );
		printf ( "  col vertex transformations: " );
		display_vector ( tr2 );
		printf ( "\n" );
	}

	// ...

	// define conference matrix transformation object....
	conference_transformation_t t ( al );

	// extract transformation
	std::vector<int> rr ( nr );
	for ( int i=0; i<nr; i++ ) {
		rr[i] = std::min ( trx[i], trx[i+nr] );
	}

	if ( verbose>=2 ) {
		printf ( "rr: " );
		print_perm ( rr );
	}

	t.rperm = invert_permutation ( argsort ( rr ) );

	for ( int i=0; i<nr; i++ ) {
		t.rswitch[ t.rperm[i]] = 2* ( trx[i]<trx[i+nr] )-1;
	}

	std::vector<int> cc ( nc );
	for ( int i=0; i<nc; i++ ) {
		cc[i] = std::min ( trx[coffset0+i], trx[coffset0+i+nc] );
	}
	t.cperm = invert_permutation ( argsort ( cc ) );

	for ( int i=0; i<nc; i++ ) {
		t.cswitch[ t.cperm[i]] = 2* ( trx[coffset0+i]< trx[coffset0+i+nc] ) -1;
	}

	if ( verbose>=2 ) {
		printf ( "transform: \n" );
		t.show();
	}
	return t;
}

array_link reduceConference ( const array_link &al, int verbose )
{
	const int nr = al.n_rows;
	const int nc=al.n_columns;
	const int nn = 2* ( nr+nc );
	/// create graph
	array_link G ( 2* ( nr+nc ), 2* ( nr+nc ), array_link::INDEX_DEFAULT );
	G.setconstant ( 0 );

	if ( verbose )
		printf ( "reduceConference: %d, %d\n", nr, nc );

	std::vector<int> colors ( 2* ( nr+nc ) );

	const int roffset0=0;
	const int roffset1=nr;
	const int coffset0=2*nr;
	const int coffset1=2*nr+nc;

	/*
	Add edges as follows:
	(1)  r[i]--r'[i] for i=0..nr-1;  c[j]--c'[j] for j=0..nc-1.
	(2)  r[i]--c[j] and r'[i]--c'[j] for all A[i,j] = +1
	(3)  r[i]--c'[j] and r'[i]--c[j] for all A[i,j] = -1.
	Zeros in A don't cause any edges.
	*/

	// set colors
	for ( int i=0; i<coffset0; i++ )
		colors[i]=0;
	for ( int i=coffset0; i<coffset0+2*nc; i++ )
		colors[i]=1;

	// (1)
	for ( int i=0; i<nr; i++ )
		G.atfast ( roffset0+i, i+roffset1 ) =1;
	for ( int i=0; i<nc; i++ )
		G.atfast ( coffset0+i, i+coffset1 ) =1;

	// (2), (3)
	for ( int r=0; r<nr; r++ ) {
		for ( int c=0; c<nc; c++ ) {
			if ( al.atfast ( r,c ) ==1 ) {
				G.atfast ( roffset0+r, coffset0+c ) =1;
				G.atfast ( roffset1+r, coffset1+c ) =1;
			}
			if ( al.atfast ( r,c ) ==-1 ) {
				G.atfast ( roffset0+r, coffset1+c ) =1;
				G.atfast ( roffset1+r, coffset0+c ) =1;
				//G.atfast ( coffset0+c, roffset1+r ) =1;
			}
		}
	}

	// make symmetryic
	/*
	for ( int i=0; i<nn; i++ )
		for ( int j=i; j<nn; j++ )
			G.atfast ( j,i ) =G.atfast ( i, j );
	*/
	const int nrg = G.n_rows;
	for ( int i=0; i<nn; i++ ) {

		array_t *x = G.array+i*nrg; // offset to column
		for ( int j=i; j<nn; j++ ) {
			x[j] =G.array[i+j*nrg];
		}
	}

	if ( verbose>=3 ) {
		printf ( "reduceConference: incidence graph:\n" );
		printf ( "   2x%d=%d row vertices and 2x%d=%d column vertices\n", nr, 2*nr, nc, 2*nc );
		G.showarray();
	}
	/// call nauty
	const std::vector<int> tr = nauty::reduceNauty ( G, colors, verbose>=2 );
	const std::vector<int> tri = invert_permutation ( tr );
	const std::vector<int> trx=tri;

	// extract transformation
	if ( verbose>=2 ) {
		if ( verbose>=3 ) {
			array_link Gx = transformGraph ( G, tri, 0 );
			printfd ( "transformed graph\n" );
			Gx.showarray();
		}
		std::vector<int> tr1 = std::vector<int> ( trx.begin () , trx.begin () + 2*nr );
		std::vector<int> tr2 = std::vector<int> ( trx.begin () +coffset0, trx.end() );
		printf ( "  row vertex transformations: " );
		display_vector ( tr1 );
		printf ( "\n" );
		printf ( "  col vertex transformations: " );
		display_vector ( tr2 );
		printf ( "\n" );
	}

	// ...

	// define conference matrix transformation object....
	conference_transformation_t t ( al );

	// extract transformation
	std::vector<int> rr ( nr );
	for ( int i=0; i<nr; i++ ) {
		rr[i] = std::min ( trx[i], trx[i+nr] );
	}

	if ( verbose>=2 ) {
		printf ( "rr: " );
		print_perm ( rr );
	}

	t.rperm = invert_permutation ( argsort ( rr ) );

	for ( int i=0; i<nr; i++ ) {
		t.rswitch[ t.rperm[i]] = 2* ( trx[i]<trx[i+nr] )-1;
	}

	std::vector<int> cc ( nc );
	for ( int i=0; i<nc; i++ ) {
		cc[i] = std::min ( trx[coffset0+i], trx[coffset0+i+nc] );
	}
	t.cperm = invert_permutation ( argsort ( cc ) );

	for ( int i=0; i<nc; i++ ) {
		t.cswitch[ t.cperm[i]] = 2* ( trx[coffset0+i]< trx[coffset0+i+nc] ) -1;
	}

	if ( verbose>=2 ) {
		printf ( "transform: \n" );
		t.show();
	}
	array_link alx = t.apply ( al );
	return alx;

}

// return vector of length n with specified positions set to one
cperm get_comb ( cperm p, int n, int zero=0, int one=1 )
{
	cperm c ( n, zero );
	//for ( int i=0; i<n ; i++ )
	//	c[i]=zero;
	for ( size_t i=0; i<p.size() ; i++ )
		c[p[i]]=one;
	return c;
}

// set vector of length n with specified positions set to one
inline void get_comb ( const cperm &p, int n, int zero, int one, cperm &c )
{
	//std::fill(c.begin(), c.end(), zero);
	for ( int i=0; i<n ; i++ )
		c[i]=zero;
	for ( size_t i=0; i<p.size() ; i++ )
		c[p[i]]=one;
}

/// return copy of vector with zero inserted at specified position
inline cperm insertzero ( const cperm &c, int pos, int value=0 )
{
	cperm cx ( c.size() +1 );
	std::copy ( c.begin(), c.begin() +pos, cx.begin() );
	cx[pos]=value;
	std::copy ( c.begin() +pos, c.end(), cx.begin() +pos+1 );
	return cx;
}


/** Return all admissible columns (first part) for a conference array in normal form
 *
 *
 **/
std::vector<cperm> get_first ( int N, int extcol, int verbose=1 )
{
	int k1=-1;
	int n1=-1;
	int k = extcol;

	int q, q1, q2, v;
	getConferenceNumbers ( N, k, q, q1, q2, v );

	int haszero=extcol<q+2;
	if ( haszero ) {

		n1=q-1;
		//k1=n1/2;
	} else {
		//printf ( "conference array: extcol %d, N %d, n1 %d, not implemented...\n", extcol, N, n1 );
		//k1 = n1/2;
		n1=q;
	}

	cperm c ( q1 );
	for ( int i=0; i<q1 ; i++ )
		c[i]=i;


	int nc = ncombs<long> ( n1, q1 );
	if ( verbose>=2 )
		printf ( "get_first: conference array: extcol %d: N %d, n1 %d, q %d, v %d, q1 %d, q2 %d, nc %d\n", extcol, N, n1, q, v, q1, q2, nc );

	std::vector<cperm> ff;
	for ( long j=0; j<nc; j++ ) {
		cperm cc =get_comb ( c, n1, -1, 1 );

		cc=insertzero ( cc, 0, 1 );
		cc=insertzero ( cc, 1, v );

		if ( haszero )
			cc=insertzero ( cc, extcol );
		//printfd("get_first: add element of size %d =  2 + %d\n", cc.size(), q );
		ff.push_back ( cc );

		if ( j+1<nc ) {
			next_comb ( c, q1, n1 );
		}
	}

	return ff;
}

/** Return all admissible columns (block two) for a conference array in normal form */
std::vector<cperm> get_second ( int N, int extcol, int target, int verbose=0 )
{
	//verbose=2;
	if ( verbose )
		printfd ( "get_second: N %d, extcol %d, target %d\n" );
	int k = extcol;
	int q, q1, q2, v;
	getConferenceNumbers ( N, k, q, q1, q2, v );

	int n1=-1;
	int haszero=extcol>=q+2;
	if ( verbose )
		printf ( "get_second: extcol: %d, q %d, haszero %d\n", extcol, q, haszero );

	if ( haszero ) {
		//k1=n1/2;
		n1=q-1;
	} else {
		//k1 = n1- ( n1-target ) /2;
		n1=q;
	}
	int qx=q2;

	cperm c ( qx );
	for ( int i=0; i<qx ; i++ )
		c[i]=i;


	int nc = ncombs<long> ( n1, qx );
	if ( verbose )
		printf ( "get_second: N %d, n1 %d, qx %d, target %d, nc %d\n", N, n1, qx, target, nc );
	std::vector<cperm> ff;
	cperm ccx ( n1 );
	cperm cc;
	for ( long j=0; j<nc; j++ ) {
		//printf("ccc: "); display_vector(cc); printf("\n");
		//printf("ccx: "); display_vector(ccx); printf("\n");

		if ( haszero ) {
			get_comb ( c, n1, -1, 1, ccx );
			cc=insertzero ( ccx, extcol- ( q+2 ) );
		} else {
			cc =get_comb ( c, n1, -1, 1 );
		}
		if ( verbose>=2 ) {
			printfd ( "add element of size %d =   %d\n", cc.size(), q );
			display_vector ( cc );
			printf ( "\n" );
			printf ( "c: " );
			display_vector ( c );
			printf ( "\n" );
		}

		ff.push_back ( cc );
		if ( n1>0 ) // guard
			next_comb ( c, qx, n1 );
	}

	return ff;
}

/// calculate inner product between two permutations
int innerprod ( const cperm &a, const array_link &al, int col )
{
	int ip=0;
	size_t nn = a.size();
	const array_t *b = al.array+col*al.n_rows;

	for ( size_t i=0; i<nn; i++ ) {
		ip+= a[i] * b[i];
	}
	return ip;
}


/// calculate inner product between two permutations
int innerprod ( const cperm &a, const cperm &b )
{
	int ip=0;
	size_t nn = b.size();
	for ( size_t i=0; i<nn; i++ ) {
		//printf("innerprod %d: %d += %d + %d\n", (int)i, ip, a[i], b[i] );
		ip+= a[i] * b[i];
	}
	return ip;
}

/// helper function, return true if a candidate extensions satisfies the symmetry test
int satisfy_symm ( const cperm &c, const std::vector<int>  & check_indices, int rowstart, int rowend )
{
	//return true; // hack
//	int k = sd.rowvalue.n_columns-1;

	for ( int i=rowstart+1; i<rowend; i++ ) {
		if ( check_indices[i] ) {
			if ( ( ( unsigned char ) c[i-1] ) > ( ( unsigned char ) c[i] ) ) {
				// discard
				return false;
			}
		}
	}
	// accept
	return true;
}

/// helper function, return true if a candidate extensions satisfies the symmetry test
int satisfy_symm ( const cperm &c, const std::vector<int>  & check_indices, int rowstart )
{
	//return true; // hack
//	int k = sd.rowvalue.n_columns-1;

	for ( size_t i=rowstart; i<c.size()-1; i++ ) {
		if ( check_indices[i+1] ) {
			if ( ( ( unsigned char ) c[i] ) > ( ( unsigned char ) c[i+1] ) ) {
				// discard
				return false;
			}
		}
	}
	// accept
	return true;
}

/// helper function, return true if a candidate extensions satisfies the symmetry test
int satisfy_symm ( const cperm &c, const symmdata & sd, int rowstart )
{
	const int verbose=0;

	if ( verbose>=2 ) {
		printf ( "satisfy_symm: sd: " );
		sd.show();
	}
	//return true; // hack
	int k = sd.rowvalue.n_columns-1;

	if ( verbose ) {
		printf ( "satisfy_symm: " );
		display_vector ( c );
		printf ( "\n" );
	}
	for ( size_t i=rowstart; i<c.size()-1; i++ ) {
		// FIXME: use the sd.checkIdx() for this
		if ( sd.rowvalue.atfast ( i, k ) ==sd.rowvalue.atfast ( i+1, k ) ) {
			//if ( c[i]<c[i+1] && c[i]!=0 && c[i+1]!=0 ) {
			//printf("c[i] %d, (char)c[i] %d\n", c[i], (unsigned char)c[i]);
			if ( ( ( unsigned char ) c[i] ) > ( ( unsigned char ) c[i+1] ) ) {
				// discard

				if ( verbose ) {
					printf ( "satisfy_symm: perm: " );
					display_vector ( c );
					printf ( "\n" );
					printf ( "  discard i %d, k %d, c[i]=%d:   %d %d\n", ( int ) i, k, c[i], sd.rowvalue.atfast ( i, k ), sd.rowvalue.atfast ( i+1, k ) );
				}
				return false;
			}
		}
	}
	if ( verbose>=2 ) {
		printf ( "satisfy_symm: return true\n" );
	}
	return true;
}

/// return column of an array in cperm format
cperm getColumn ( const array_link &al, int c )
{
	cperm cx ( al.n_rows );
	std::copy ( al.array+c*al.n_rows, al.array+ ( c+1 ) *al.n_rows, cx.begin() );
	return cx;
}

// return true if the extension column satisfies the inner product check
int ipcheck ( const cperm &col, const array_link &al, int cstart, int verbose )
{
	for ( int c=cstart; c<al.n_columns; c++ ) {
		if ( innerprod ( col, al, c ) !=0 ) {
			if ( verbose ) {
				printf ( "ipcheck: column %d to %d (inclusive), failed at col %d\n", c, cstart, al.n_columns+1 );
			}
			return false;
		}
	}
	return true;
}

int maxz ( const array_link &al, int k )
{
	int maxzidx=-1;
	const int nr=al.n_rows;
	if ( k==-1 ) {
		for ( int k=0; k<al.n_columns; k++ ) {
			for ( int r=nr-1; r>=maxzidx; r-- ) {
				//printf("r k %d %d\n", r, k); al.show();
				if ( al._at ( r, k ) ==0 ) {
					maxzidx = std::max ( maxzidx, r );
				}
			}
		}
		return maxzidx;
	} else {
		for ( int r=nr-1; r>=0; r-- ) {
			if ( al._at ( r, k ) ==0 ) {
				return r;
			}
		}
	}
	return maxzidx ;
}

/// filter candidate extensions based on symmetry propery
std::vector<cperm> filterCandidatesSymm ( const std::vector<cperm> &extensions, const array_link &als, int verbose )
{
	const int N = als.n_rows;

	int mval=0;
	if ( N%4==2 ) {
		// S is symmetric
		mval=1;
	} else {
		// else S is anti-symmetric
		mval=-1;
	}
	if ( verbose>=2 )
		printf ( "N %d, mval %d\n", N, mval );

	const int k = als.n_columns;
	cperm tmp ( N );
	for ( int i=0; i<k; i++ )
		tmp[i] = als.at ( k, i );

	std::vector<cperm> e ( 0 );
	for ( size_t i=0; i<extensions.size(); i++ ) {
		const cperm &ex = extensions[i];

		int good=1;
		for ( int x=2; x<k; x++ ) {
			if ( tmp[x]!=mval*ex[x] ) {
				good=0;
				if ( k>3 && verbose>=3 ) {
					printf ( "discard extension %d: N %d, k %d: x %d\n", ( int ) i, N, k, x );
					printf ( "tmp: " );
					printf_vector<signed char> ( tmp, "%d " );
					printf ( "\n" );
					printf ( "ex: " );
					printf_vector<signed char> ( ex, "%d " );
					printf ( "\n" );
				}
				break;
			}
		}
		if ( good )
			e.push_back ( extensions[i] );
	}
	if ( verbose>=1 )
		printf ( "filterCandidatesSymm: k %d, filter %d/%d\n", k, ( int ) e.size(), ( int ) extensions.size() );
	return e;

}

/// filter candidate extensions on J3 value (only pairs with extension candidate are checked)
std::vector<cperm> filterJ3 ( const std::vector<cperm> &extensions, const array_link &als, int verbose )
{

	const int N = als.n_rows;

	array_link dtable = createJ2tableConference ( als );

	if ( verbose>=2 ) {
		printf ( "array + dtable\n" );
		als.showarray();
		printfd ( "dtable:\n" );
		dtable.showarray();
	}

	int nc = dtable.n_columns;

	if ( verbose>=1 ) {
		printf ( "filterJ3: array %dx%d, nc %d\n", N, als.n_columns, nc );
	}

	std::vector<cperm> e2 ( 0 );
	for ( size_t i=0; i<extensions.size(); i++ ) {
		const cperm &c = extensions[i];

		//std::vector<int> cx(c.begin(), c.end() ); printf("i %d: ", (int)i); print_perm(cx);

		int jv=0;
		for ( int idx1=0; idx1<nc; idx1++ ) {
			jv=0;

			const array_t *o1 = dtable.array+dtable.n_rows*idx1;
			for ( int xr=0; xr<N; xr++ ) {

				jv += ( o1[xr] ) * ( c[xr] );
			}

			if ( jv!=0 )
				break;
		}

		if ( jv==0 )
			e2.push_back ( c );
	}

	if ( verbose>=1 ) {
		printf ( "filterJ3: %ld -> %ld extensions\n", ( long ) extensions.size(), ( long ) e2.size() );
	}
	return e2;
}



/** filter conferece matrix extension candidates
 *
 * Filtering is based in symmetry and ip
 */
std::vector<cperm> filterDconferenceCandidates ( const std::vector<cperm> &extensions, const array_link &als, int filtersymm, int filterip, int verbose )
{
//	symmetry_group rs = als.row_symmetry_group();
	symmdata sd ( als );
	DconferenceFilter dfilter ( als, filtersymm, filterip );
	dfilter.filterfirst=1;

	if ( verbose>=2 )
		sd.show ( 1 );

	std::vector<cperm> e2 ( 0 );
	for ( size_t i=0; i<extensions.size(); i++ ) {

		if ( dfilter.filter ( extensions[i] ) ) {
			e2.push_back ( extensions[i] );

		}

	}
	return e2;
}


/** filter conferece matrix extension candidates
 *
 * Filtering is based in symmetry and ip
 */
std::vector<cperm> filterCandidates ( const std::vector<cperm> &extensions, const array_link &als, int filtersymm, int filterip, int verbose )
{
	symmetry_group rs = als.row_symmetry_group();
	symmdata sd ( als );


	if ( verbose>=2 )
		sd.show ( 1 );

	std::vector<cperm> e2 ( 0 );
	for ( size_t i=0; i<extensions.size(); i++ ) {

		if ( filtersymm ) {
			if ( ! satisfy_symm ( extensions[i], sd ) ) {
				if ( verbose>=2 ) {
					printf ( "filterCandidates: reject due to row symm: " );
					display_vector ( extensions[i] );
					printf ( "\n" );
				}
				continue;
			}
		}
		if ( filterip ) {

			// perform inner product check for all columns
			if ( ! ipcheck ( extensions[i], als ) ) {
				if ( verbose>=2 ) {
					printf ( "   extension " );
					display_vector ( extensions[i] );
					printf ( "\n" );
					printf ( "filterCandidates: reject due to innerproduct (extension %d)\n", ( int ) i );
					ipcheck ( extensions[i], als, 2, 1 );
				}
				continue;
			}
		}
		e2.push_back ( extensions[i] );
	}
	return e2;
}

std::vector<cperm> generateConferenceExtensions ( const array_link &al, const conference_t & ct, int kz, int verbose , int filtersymm, int filterip )
{
	if ( ct.ctype==conference_t::DCONFERENCE ) {
		return generateDoubleConferenceExtensions ( al, ct, verbose,filtersymm,filterip );
	}

	if ( ct.itype==CONFERENCE_RESTRICTED_ISOMORPHISM ) {
		return generateConferenceRestrictedExtensions ( al, ct, kz, verbose,filtersymm,filterip );
	}

	conference_extend_t ce;
	std::vector<cperm> extensions ( 0 );
	const int N = ct.N;
	const int extcol=al.n_columns;

	// loop over all possible first combinations
	std::vector<cperm> ff = get_first ( N, kz, verbose );

	if ( verbose>=2 ) {
		for ( size_t i=0; i<ff.size(); i++ ) {
			printf ( "extend1 %d: N %d: ", ( int ) i, N );
			display_vector ( ff[i] );
			printf ( "\n" );
		}
	}

	ce.first=ff;
	ce.second=ff;

	array_link als = al.selectFirstColumns ( extcol );

	cperm c0 = getColumn ( al, 0 );
	cperm c1 = getColumn ( al, 1 );
	for ( size_t i=0; i<ce.first.size(); i++ ) {
		int ip = innerprod ( c0, ce.first[i] );
		//int ip = innerprod(c1, ce.first[i]);
		//printf("extend1 %d: inner product %d\n", (int)i, ip);

		// TODO: cache this function call
		int target = -ip;

		std::vector<cperm> ff2 = get_second ( N, kz, target, verbose>=2 );
		ce.second=ff2;

		//printfd("ce.second[0] "); display_vector( ce.second[0]); printf("\n");

		for ( size_t j=0; j<ff2.size(); j++ ) {
			cperm c = ce.combine ( i, j );

#ifdef OADEBUG
#else
			if ( 1 ) {
				extensions.push_back ( c );
				continue;
			}
#endif
			int ip0 = innerprod ( c0, c );
			int ip1 = innerprod ( c1, c );
			//printf("extend %d: N %d ", (int)i, N); display_vector(c);	 printf("\n");

			if ( verbose>=3 ) {
				printf ( "extend_conference %d: ip %d %d\n", ( int ) i, ip0, ip1 );
			}

			if ( verbose>=2 ) {
				//alx.showarraycompact();
				array_link ecol=al.selectFirstColumns ( 1 );
				array_link alx = hstack ( al, ecol );
				alx.setColumn ( kz, c );
				alx.showarray();
			}
			// add array to good set if ip2 is zero
			if ( ip0==0 && ip1==0 ) {
				extensions.push_back ( c );
			} else {
				printfd ( "huh?" );

				printf ( " ip0 ip1 %d %d\n" , ip0, ip1 );
				al.show();
				printf ( "  c: %d\n", ( int ) c.size() );
				array_link alx = hstack ( al, c );
				alx.showarray();
			}
		}
	}

	ce.extensions=extensions;
	if ( verbose>=2 )
		printf ( "generateConferenceExtensions: after generation: found %d extensions\n", ( int ) extensions.size() );
	// perform row symmetry check


	std::vector<cperm> e2 = filterCandidates ( extensions, al,  filtersymm,  filterip,  verbose );

	if ( verbose>=1 )
		printf ( "extend_conference: symmetry check %d + ip filter %d: %d->%d\n", filtersymm, filterip, ( int ) extensions.size(), ( int ) e2.size() );

	ce.extensions=e2;

	return e2;
}

int nOnesFirstColumn ( const array_link &al )
{
	int n=0;
	for ( int i=0; i<al.n_rows; i++ )
		n+= al.atfast ( i,0 ) ==1;
	return n;
}

indexsort rowsorter ( const array_link &al )
{

	std::vector<mvalue_t<int> > rr;
	for ( int i=0; i<al.n_rows; i++ ) {
		mvalue_t<int> m;
		for ( int k=0; k<al.n_columns; k++ )
			m.v.push_back ( al.at ( i, k ) );
		rr.push_back ( m );
	}
	indexsort is ( rr );
	return rr;
}


void showCandidates ( const std::vector<cperm> &cc )
{
	for ( size_t i=0; i<cc.size(); i++ ) {
		printf ( "%d: ", ( int ) i );
		print_cperm ( cc[i] );
		printf ( "\n" );
	}
}

struct branch_t {
	int row;
	int rval;
	int nvals[3];	/// number of 0, 1, and -1
};

const int bvals[3] = {0,1,-1}; // these are ordered

template<class object_t>
class lightstack_t
{
	object_t *stack;
private:
	int size;
	int n;
public:

	lightstack_t ( int sz ) : size ( sz ), n ( 0 ) {
		stack = new object_t[sz];
	}
	~lightstack_t() {
		delete [] stack;
	}

	bool empty() const {
		return n==0;
	}
	void push ( const object_t &o ) {
#ifdef OADEBUG
		assert ( this->n<this->size );
#endif
		this->stack[n] = o;
		n++;
	}
	object_t & top() const {
		assert ( n>0 );
		return this->stack[n-1];
	}
	void pop() {
		n--;
	}
};


void inflateCandidateExtensionHelper ( std::vector<cperm> &list, const cperm &basecandidate,  cperm &candidate, int block, const array_link &al,
									   const symmetry_group &alsg, const std::vector<int> & check_indices, const conference_t & ct, int verbose , const DconferenceFilter &filter, long &ntotal )
{
	const symmdata &sd = filter.sd;

	//int lastcol = al.n_columns-1;
	//symmetry_group alsg = al.row_symmetry_group(); //(sd.rowvalue.selectColumns(lastcol));
	// TODO: inline symmetry checks
	int nblocks = alsg.ngroups;


	if ( block==nblocks ) {
		// TODO: make this loop in n-1 case?

		ntotal++;
		// TODO: this can probably be a restricted filter
		if ( filter.filter ( candidate ) ) {
			list.push_back ( candidate );
		}
		return;
	}


	//printfd("sg.gstart.size() %d, block %d: blocksize %d\n", sg.gstart.size(), block, sg.gsize[block]);
	const int blocksize = alsg.gsize[block];

	if ( blocksize==1 ) {
		// easy case
		inflateCandidateExtensionHelper ( list, basecandidate, candidate, block+1, al, alsg, check_indices, ct, verbose, filter,ntotal );
		return;
	}
	int gstart = alsg.gstart[block];
	int gend = alsg.gstart[block+1];

	if ( block<=-1 ) {
		printfd ( "row: %d to %d\n", gstart, gend );
		cperm tmp ( candidate.begin() +gstart, candidate.begin() +gend );
		printf ( "   current perm: " );
		print_cperm ( tmp );
		printf ( "\n" );
	}

	unsigned long iter=0;
	std::sort ( candidate.begin() +gstart, candidate.begin() +gend );
	do {
		if ( block<=4 && blocksize>1 && 0 ) {
			printf ( "  block %d, iter %ld\n", block, iter );
		}
		//cout << s1 << endl;
		iter++;
		
		// TODO: smart symmetry generation
		if (satisfy_symm(candidate, check_indices, gstart, gend)) {
		inflateCandidateExtensionHelper ( list, basecandidate, candidate, block+1, al, alsg, check_indices, ct, verbose, filter,ntotal );
		}
		// TODO: run inline filter
	} while ( std::next_permutation ( candidate.begin() +gstart, candidate.begin() +gend ) );

}

std::vector<cperm> inflateCandidateExtension ( const cperm &basecandidate,  const array_link &als,  const array_link &alx, const conference_t & ct, int verbose , const DconferenceFilter &filter )
{
	long ntotal=0;
	symmetry_group alsg = als.row_symmetry_group(); //filter.sd.rowvalue.selectColumns(col));
	symmetry_group alxsg = alx.row_symmetry_group(); //filter.sd.rowvalue.selectColumns(col));

	const std::vector<int> check_indices = alxsg.checkIndices();
	
	//printf ( "inflateCandidateExtension: symmetry group\n" ); alsg.show();

	cperm candidate = basecandidate;
	int block=0;
	std::vector<cperm> cc;
	inflateCandidateExtensionHelper ( cc, basecandidate, candidate, block, als, alsg, check_indices, ct, verbose, filter, ntotal );

	if ( verbose>=2 ) {
		printfd ( "generated %ld/%ld candidates \n", ( long ) cc.size(), ntotal );
	}
	return cc;
}


std::vector<cperm> generateDoubleConferenceExtensions ( const array_link &al, const conference_t & ct, int verbose , int filtersymm, int filterj2, int filterj3, int filtersymminline )
{
	//verbose=2;
	//FIXME filtersymm=0;

	printf ( "generateDoubleConferenceExtensions: filters: symmetry %d, symmetry inline %d, j2 %d, j3 %d\n", filtersymm, filtersymminline, filterj2, filterj3 );

	const int N = al.n_rows;
	DconferenceFilter dfilter ( al, filtersymm, filterj2 );

	std::vector<int> sidx = dfilter.sd.checkIdx();
	if ( verbose>=2 ) {
		printfd ( "sidx: " );
		print_perm ( sidx );
		printf ( "\n" );
	}
	std::vector<cperm> cc;
	cperm c ( al.n_rows );

	//std::stack <branch_t> branches;
	lightstack_t<branch_t> branches ( 3*N );

	c[0]=1;

	// push initial branches
	branch_t b1 = {0, 1, {2,N/2-2,N/2-1} };
	branches.push ( b1 );
	branch_t b0 = {0, 0, {1,N/2-1,N/2-1} };
	branches.push ( b0 );
	double t0=get_time_ms();

#ifdef OADEBUG
	std::vector<long> nb ( N+1 );
#endif

	// FIXME: do faster inline checks (e.g. abort with partial symmetry, take combined J2 check with many zeros)
	long n=0;
	do {

		branch_t b = branches.top();
#ifdef OADEBUG
		nb[b.row]++;
#endif
		branches.pop(); // FIXME: use reference and pop later
		if ( verbose>=3 && 0 ) {
			printf ( "branch: row %d, val %d, nums %d %d %d\n", b.row, b.rval, b.nvals[0], b.nvals[1],b.nvals[2] );
			for ( int x=b.row+1; x<N; x++ )
				c[x]=-9;
			c[b.row]=b.rval;
			printf ( "   " );
			print_cperm ( c );
			printf ( "\n" );
		}
		c[b.row]=b.rval;

		if ( b.row==dfilter.inline_row && filterj3 ) {
			// FIXME: inline_row can be one earlier?
			if ( ! dfilter.filterJ3inline ( c ) )
				continue;
		}
		if ( b.row==N-1 ) {
			n++;
			//verbose=2;
			//dfilter.filterReason(c);
			if ( dfilter.filter ( c ) ) {
				if ( verbose>=2 || 0 ) {
					printf ( "## push candindate   : " );
					print_cperm ( c );
					printf ( "\n" );
				}
				cc.push_back ( c );
			} else {
				if ( verbose>=2 || 0 ) {
					printf ( "## discard candindate: " );
					print_cperm ( c );
					printf ( "\n" );
				}
			}
			continue;
		}
		int istart=0;
		const int iend = 3;
		if ( sidx[b.row+1] && filtersymminline ) {
			if ( c[b.row]==-1 ) {
				istart=2;

				if ( verbose>=3 ) {
					printfd ( "symmetry: istart %d\n", istart );

					printf ( "row %d: ", b.row );
					print_cperm ( c );
					printf ( "\n" );
				}
			}
			if ( c[b.row]==1 && 1 ) {
				istart=1;
				//printfd("symmetry: istart %d\n", istart);
			}
		}
		for ( int i=istart; i<iend; i++ ) {

			if ( b.nvals[i]==0 )
				continue;


			//c[b.row]= bvals[i];
			// checks
			if ( ! ( 1 ) )
				continue;


			branch_t bnew ( b );
			bnew.row++;
			bnew.rval = bvals[i];
			bnew.nvals[i]--;
			//printf("push new branch: i %d\n", i);
			// FIXME: make direct push possible
			// FIXME: can we eliminate the branches object altogether?
			branches.push ( bnew );
		}

	} while ( ! branches.empty() )
		;

#ifdef OADEBUG
	if ( 1 ) {
		printf ( "branch count:\n" );
		for ( int i=0; i<=N; i++ ) {
			printf ( "  %d: %ld\n", i, nb[i] );
		}
	}
#endif
	if ( verbose || 1 ) {
		printfd ( "generateDoubleConferenceExtensions: generated %ld/%ld/%ld perms (len %ld)\n", ( long ) cc.size(), n, factorial<long> ( c.size() ), ( long ) c.size() );
		//al.show();
		//al.transposed().showarray(); showCandidates ( cc );
	}

	return cc;
}


std::vector<cperm> generateDoubleConferenceExtensions2 ( const array_link &al, const conference_t & ct, int verbose , int filtersymm, int filterip, int filterj3 )
{
	assert ( ct.itype==CONFERENCE_RESTRICTED_ISOMORPHISM );

	int j1zero = ct.j1zero;

	std::vector<cperm> cc;

	const int N = ct.N;
	cperm c ( N );

	DconferenceFilter dfilter ( al, filtersymm, filterip );
	dfilter.filterfirst=1;
	dfilter.filterj3=filterj3;
	unsigned long n=0;
	for ( int i=0; i<N-2; i++ ) {
		// fill initial permutation
		std::fill ( c.begin(), c.end(), -1 );
		c[0]=0;
		c[1]=0;
		for ( int k=2; k<i+2; k++ )
			c[k]=1;

		std::sort ( c.begin(), c.end() );

		if ( j1zero && i!= ( N-2 ) /2 )
			continue;

		do {
			//cout << s1 << endl;
			n++;

			if ( dfilter.filter ( c ) ) {
				cc.push_back ( c );
			}
		} while ( std::next_permutation ( c.begin(), c.end() ) );
	}

	//printfd ( "generateDoubleConferenceExtensions: before filter generated %d/%ld perms (len %ld)\n", n, factorial<long> ( c.size() ), ( long ) c.size() );
	//cc= filterDconferenceCandidates ( cc, al, filtersymm,  filterip, verbose );
	if ( verbose || 1 ) {
		printfd ( "generateDoubleConferenceExtensions: generated %ld/%ld/%ld perms (len %ld)\n", ( long ) cc.size(), n, factorial<long> ( c.size() ), ( long ) c.size() );
		//al.show();
		//al.transposed().showarray(); showCandidates ( cc );
	}
	return cc;
}

std::vector<cperm> generateConferenceRestrictedExtensions ( const array_link &al, const conference_t & ct, int kz, int verbose , int filtersymm, int filterip )
{

	const int extcol=al.n_columns;
//const int N = al.n_rows;
	const int N = ct.N;

	// special case
	if ( extcol==1 ) {
		std::vector<cperm> ee;

		// we have no values +1 in the first column and
		int no = nOnesFirstColumn ( al );
		int n1=no-1;
		int n2 = N-n1-2;

		// in the second column we start with [1,0]^T and then in block1: k1 values +1, block2: k2 values +1
		// we must have k2 = k1+(n2-n1)/2 = k1 +N -2n1-2

		cperm cc ( N );
		cc[0]=1;
		cc[1]=0;
		for ( int k1=0; k1<=n1; k1++ ) {
			int k2 = k1+ ( n2-n1 ) /2;
			if ( k2>n2 )
				continue;
			if ( k2<0 )
				continue;
			printf ( "generateConferenceRestrictedExtensions: column 1: n1 %d, n2 %d, k1 %d, k2 %d\n", n1, n2, k1, k2 );

			std::fill ( cc.begin() +2, cc.end(), -1 );

			for ( int i=2; i<2+k1; i++ )
				cc[i]=1;
			for ( int i=2+n1; i<2+n1+k2; i++ )
				cc[i]=1;

			ee.push_back ( cc );
		}
		return ee;
	}

	std::vector<int> moi; // indices of -1 in first column
	for ( int i=0; i<N; i++ ) {
		if ( al.atfast ( i,0 ) ==-1 )
			moi.push_back ( i );
	}
	array_link alx = al.clone();

	// multiply
	for ( size_t i =0; i<moi.size(); i++ ) {
		alx.negateRow ( moi[i] );
	}

	// sort rows of array
	indexsort is = rowsorter ( alx );

	// now get candidate columns for the normal case, afterwards convert then using the rowsorter and row negations

	printfd ( "FIXME: factor next block into a function (also in the other function)\n" );

	// loop over all possible first combinations
	std::vector<cperm> ff = get_first ( N, kz, verbose );

	if ( verbose>=2 ) {
		for ( size_t i=0; i<ff.size(); i++ ) {
			printf ( "extend1 %d: N %d: ", ( int ) i, N );
			display_vector ( ff[i] );
			printf ( "\n" );
		}
	}

	conference_extend_t ce;
	std::vector<cperm> extensions ( 0 );

	ce.first=ff;
	ce.second=ff;

	array_link als = al.selectFirstColumns ( extcol );

	cperm c0 = getColumn ( al, 0 );
	cperm c1 = getColumn ( al, 1 );
	for ( size_t i=0; i<ce.first.size(); i++ ) {
		int ip = innerprod ( c0, ce.first[i] );
		//int ip = innerprod(c1, ce.first[i]);
		//printf("extend1 %d: inner product %d\n", (int)i, ip);

		// TODO: cache this function call
		int target = -ip;

		std::vector<cperm> ff2 = get_second ( N, kz, target, verbose>=2 );
		ce.second=ff2;

		//printfd("ce.second[0] "); display_vector( ce.second[0]); printf("\n");

		for ( size_t j=0; j<ff2.size(); j++ ) {
			cperm c = ce.combine ( i, j );


			extensions.push_back ( c );
			continue;

		}
	}

	ce.extensions=extensions;
	if ( verbose>=2 )
		printf ( "generateConferenceExtensions: after generation: found %d extensions\n", ( int ) extensions.size() );
	// perform row symmetry check


	std::vector<cperm> e2 = filterCandidates ( extensions, al,  filtersymm,  filterip,  verbose );

	if ( verbose>=1 )
		printf ( "extend_conference: symmetry check %d + ip filter %d: %d->%d\n", filtersymm, filterip, ( int ) extensions.size(), ( int ) e2.size() );

	ce.extensions=e2;

	return e2;
}

int selectZmax ( int maxzpos, const conference_t::conference_type &ctype, const array_link &al, int extcol )
{
	if ( maxzpos<0 ) {
		switch ( ctype ) {
		case conference_t::CONFERENCE_NORMAL:
			maxzpos = al.n_rows-1;
			break;
		case conference_t::CONFERENCE_DIAGONAL:

			maxzpos = extcol; // maxzval+2;
			//printf("ct.ctype==conference_t::conference_t::CONFERENCE_DIAGONAL: maxzpos %d/%d, extcol %d\n", maxzpos, al.n_rows-1, extcol);
			break;
		case conference_t::DCONFERENCE:
			maxzpos = al.n_rows-1;

			break;
		default
				:
			printfd ( "not implemented...\n" );
			maxzpos = al.n_rows-1;
		}



	}
	return maxzpos;
}

conference_extend_t extend_double_conference_matrix ( const array_link &al, const conference_t & ct, int extcol, int verbose, int maxzpos )
{
	conference_extend_t ce;
	ce.extensions.resize ( 0 );

	const int N = ct.N;
	const int k = extcol;
	const int maxzval = maxz ( al );


	if ( verbose )
		printf ( "--- extend_double_conference_matrix: extcol %d, maxz %d, itype %d ---\n", extcol, maxzval, ct.itype );

	int filterip=1;
	int filtersymm=1;
	std::vector<cperm> cc = generateDoubleConferenceExtensions ( al, ct, verbose, filtersymm, filterip );

	if ( ct.j3zero ) {
		//printfd("filter on j3 values\n");
		cc = filterJ3 ( cc, al, verbose );
	}

	ce.extensions = cc; //.insert ( ce.extensions.end(), cc.begin(), cc.end() );


	return ce;
}

conference_extend_t extend_conference_matrix ( const array_link &al, const conference_t & ct, int extcol, int verbose, int maxzpos )
{
	conference_extend_t ce;
	ce.extensions.resize ( 0 );

	const int N = ct.N;
	const int k = extcol;
	const int maxzval = maxz ( al );

	if ( verbose )
		printf ( "--- extend_conference_matrix: extcol %d, maxz %d, itype %d ---\n", extcol, maxzval, ct.itype );

	const int zstart=maxzval+1;

	maxzpos = selectZmax ( maxzpos, ct.ctype, al, extcol );

	for ( int ii=zstart; ii<maxzpos+1; ii++ ) {
		if ( verbose>=2 )
			printf ( "array: kz %d: generate\n", ii );
		std::vector<cperm> extensionsX  = generateConferenceExtensions ( al, ct, ii, verbose, 1, 1 );

		if ( verbose>=2 )
			printf ( "array: kz %d: %d extensions\n", ii, ( int ) extensionsX.size() );
		ce.extensions.insert ( ce.extensions.end(), extensionsX.begin(), extensionsX.end() );
	}

	return ce;
}

conference_extend_t extend_conference_matrix ( const array_link &al, const conference_t & ct, int extcol, int verbose, int maxzpos, const conf_candidates_t &cande )
{
	conference_extend_t ce;
	ce.extensions.resize ( 0 );

	const int N = ct.N;
	const int k = extcol;
	const int maxzval = maxz ( al );

	if ( verbose )
		printf ( "--- extend_conference_matrix: extcol %d, maxz %d ---\n", extcol, maxzval );

	const int zstart=maxzval+1;

	maxzpos = selectZmax ( maxzpos, ct.ctype, al, extcol );

	//for ( int ii=maxzval+1; ii<std::min<int>(al.n_rows, maxzval+2); ii++ ) {
	for ( int ii=zstart; ii<maxzpos+1; ii++ ) {
		if ( verbose>=2 )
			printf ( "array: kz %d: generate\n", ii );
		//std::vector<cperm> extensionsX  = generateConferenceExtensions ( al, ct, ii, verbose, 1, 1 );
		std::vector<cperm> extensionsX;

		if ( ct.ctype==conference_t::CONFERENCE_DIAGONAL ) {
			//printf("filter on symmetry time\n");
			extensionsX = filterCandidatesSymm ( cande.ce[ii],  al, verbose );
			extensionsX = filterCandidates ( extensionsX,  al,1, 1, verbose );
		} else {
			extensionsX  = filterCandidates ( cande.ce[ii],  al,1, 1, verbose );
		}
		if ( verbose>=2 )
			printf ( "array: kz %d: %d extensions\n", ii, ( int ) extensionsX.size() );
		ce.extensions.insert ( ce.extensions.end(), extensionsX.begin(), extensionsX.end() );
	}

	return ce;
}


/// sort rows of an array based on the zero elements
array_link sortrows ( const array_link al )
{
	size_t nr=al.n_rows;
	// initialize original index locations
	std::vector<size_t> idx ( nr );
	for ( size_t i = 0; i != nr; ++i )
		idx[i] = i;

	//compfunc = ..;
	// sort indexes based on comparing values in v
// sort(idx.begin(), idx.end(), compfunc );

	printfd ( "not implemented...\n" );
	return al;
}



/**
void test_comb ( int n, int k )
{
	std::vector<int> c ( k );
	for ( int i=0; i<k ; i++ )
		c[i]=i;

	int nc = ncombs<long> ( n, k );

	for ( long j=0; j<nc; j++ ) {
		std::vector<int> cc =get_comb ( c, n );
		display_vector ( cc );
		printf ( "\n" );
		next_comb ( c, k, n );
	}
}
*/

template<typename T>
size_t vectorsizeof ( const typename std::vector<T>& vec )
{
	return sizeof ( T ) * vec.size();
}


conf_candidates_t generateCandidateExtensions ( const conference_t ctype, int verbose=1, int ncstart=3 )
{

	conf_candidates_t cande;

	cande.ce.resize ( ctype.N );

	array_link al = ctype.create_root();
	array_link al3 = ctype.create_root_three();
	int ncmax=ctype.N;
	if ( ctype.ctype==conference_t::CONFERENCE_DIAGONAL ) {
		ncmax=ncstart;
	}

	for ( int extcol=ncstart-1; extcol<ncmax; extcol++ ) {
		std::vector<cperm> ee;

		{
			if ( extcol==2 )
				ee = generateConferenceExtensions ( al, ctype, extcol, 0, 0, 1 );
			else
				ee= generateConferenceExtensions ( al3, ctype, extcol, 0, 0, 1 );
		}
		//printf("al3:\n"); al3.showarray();

		if ( ( long ) vectorsizeof ( ee ) > ( long ( 1 ) *1024*1024*1024 ) / ( long ) ctype.N ) {
			printfd ( "generateCandidateExtensions: set of generated candidates too large, aborting" );
			assert ( 0 );
			exit ( 0 );
		}
		cande.ce[extcol] = ee;
	}


	cande.info ( verbose );

	return cande;
}



arraylist_t extend_double_conference ( const arraylist_t &lst, const conference_t ctype, int verbose )
{
	// TODO: cache candidate extensions
	arraylist_t outlist;
	if ( verbose>=2 ) {
		printfd ( "extend_double_conference: start %d\n", ( int ) lst.size() );
	}

	int vb=std::max ( 0, verbose-1 );

	int ncstart=3;
	if ( lst.size() >0 )
		ncstart=lst[0].n_columns+1;


	for ( size_t i=0; i<lst.size(); i++ ) {
		const array_link &al = lst[i];
		int extcol=al.n_columns;
		conference_extend_t ce = extend_double_conference_matrix ( al, ctype, extcol, vb, -1 );


		arraylist_t ll = ce.getarrays ( al );
		const int nn = ll.size();

		outlist.insert ( outlist.end(), ll.begin(), ll.end() );

		if ( verbose>=2 || ( verbose>=1 && ( i%100==0 || i==lst.size()-1 ) ) ) {
			printf ( "extend_conference: extended array %d/%d to %d arrays\n", ( int ) i, ( int ) lst.size(), nn );
			fflush ( 0 );
		}
	}
	return outlist;
}

arraylist_t extend_conference_restricted ( const arraylist_t &lst, const conference_t ctype, int verbose )
{
	arraylist_t outlist;

	if ( verbose>=2 ) {
		printfd ( "extend_conference: start %d\n", ( int ) lst.size() );
	}

	int vb=std::max ( 0, verbose-1 );

	int ncstart=3;
	if ( lst.size() >0 )
		ncstart=lst[0].n_columns+1;


	for ( size_t i=0; i<lst.size(); i++ ) {
		const array_link &al = lst[i];
		int extcol=al.n_columns;
		conference_extend_t ce = extend_conference_matrix ( al, ctype, extcol, vb, -1 );


		arraylist_t ll = ce.getarrays ( al );
		const int nn = ll.size();

		outlist.insert ( outlist.end(), ll.begin(), ll.end() );

		if ( verbose>=2 || ( verbose>=1 && ( i%100==0 || i==lst.size()-1 ) ) ) {
			printf ( "extend_conference: extended array %d/%d to %d arrays\n", ( int ) i, ( int ) lst.size(), nn );
			fflush ( 0 );
		}
	}
	return outlist;
}

arraylist_t extend_conference ( const arraylist_t &lst, const conference_t ctype, int verbose )
{
	arraylist_t outlist;

	if ( verbose>=2 ) {
		printfd ( "extend_conference: start %d\n", ( int ) lst.size() );

	}

	int vb=std::max ( 0, verbose-1 );

	int ncstart=3;
	if ( lst.size() >0 )
		ncstart=lst[0].n_columns+1;

	conf_candidates_t cande = generateCandidateExtensions ( ctype, verbose>=2, ncstart );

	for ( size_t i=0; i<lst.size(); i++ ) {
		const array_link &al = lst[i];
		int extcol=al.n_columns;
		conference_extend_t ce = extend_conference_matrix ( al, ctype, extcol, vb, -1, cande );

		arraylist_t ll = ce.getarrays ( al );
		const int nn = ll.size();

		outlist.insert ( outlist.end(), ll.begin(), ll.end() );

		if ( verbose>=2 || ( verbose>=1 && ( i%100==0 || i==lst.size()-1 ) ) ) {
			printf ( "extend_conference: extended array %d/%d to %d arrays\n", ( int ) i, ( int ) lst.size(), nn );
			fflush ( 0 );
		}
	}
	return outlist;
}


std::pair<arraylist_t, std::vector<int> > selectConferenceIsomorpismHelper ( const arraylist_t lst, int verbose, matrix_isomorphism_t itype )
{
	const int nn = lst.size();

	arraylist_t lstr;
	arraylist_t lstgood;

	// safety check
	if ( lst.size() >0 ) {
		if ( lst[0].min() <-1 ) {
			printfd ( "error: arrays should have positive integer values\n" );
			arraylist_t lstgood;
			std::vector<int> cidx;
			return std::pair<arraylist_t, std::vector<int> > ( lstgood, cidx );

		}
	}
	for ( int i=0; i< ( int ) lst.size(); i++ ) {
		if ( verbose>=1 && ( i%5000==0 || i== ( int ) lst.size()-1 ) )
			printf ( "selectConferenceIsomorpismClasses: reduce %d/%d\n", i, ( int ) lst.size() );
		array_link alx;

		switch ( itype ) {
		case CONFERENCE_ISOMORPHISM: {
			alx= reduceConference ( lst[i], verbose>=2 );
		}
		break;
		case CONFERENCE_RESTRICTED_ISOMORPHISM: {
			arraydata_t arrayclass ( 3, lst[i].n_rows, 1, lst[i].n_columns );
			//printfd("run %d ", i); arrayclass.show();
			array_transformation_t t = reduceOAnauty ( lst[i]+1, verbose>=2, arrayclass );
			alx=t.apply ( lst[i]+1 ) + ( - 1 );
			break;
		}
		default
				:
			printfd ( "error: isomorphism type not implemented\n" );
			break;
		}
		lstr.push_back ( alx );
	}

	// perform stable sort
	indexsort sortidx ( lstr );

	const std::vector<int> &idx = sortidx.indices;

	std::vector<int> cidx ( nn );

	array_link prev;

	if ( lst.size() >0 )
		prev= lst[0];
	prev.setconstant ( -10 );

	int ci=-1;
	for ( size_t i=0; i<idx.size(); i++ ) {
		array_link al=lstr[idx[i]];
		if ( al!=prev ) {
			// new isomorphism class
			if ( verbose>=2 )
				printf ( "selectConferenceIsomorpismClasses: representative %d: index %d\n", ( int ) lstgood.size(), ( int ) idx[i] );

			lstgood.push_back (	lst[idx[i]] );
			prev=al;
			ci++;
		}
		cidx[i]=ci;
	}

	if ( verbose )
		myprintf ( "selectConferenceIsomorpismClasses: select classes %d->%d\n", ( int ) lst.size(), ( int ) lstgood.size() );

	return std::pair<arraylist_t, std::vector<int> > ( lstgood, cidx );
}

std::vector<int> selectConferenceIsomorpismIndices ( const arraylist_t lst, int verbose,  matrix_isomorphism_t itype )
{

	std::pair<arraylist_t, std::vector<int> > pp = selectConferenceIsomorpismHelper ( lst, verbose, itype ) ;
	return pp.second;
}

arraylist_t selectConferenceIsomorpismClasses ( const arraylist_t lst, int verbose, matrix_isomorphism_t itype )
{

	std::pair<arraylist_t, std::vector<int> > pp = selectConferenceIsomorpismHelper ( lst, verbose , itype ) ;
	return pp.first;
}
/*
bool compareLMC0x ( const array_link &alL, const array_link &alR )
{
	array_link L = alL;
	array_link R = alR;

	assert ( alL.n_rows==alR.n_rows );
	assert ( alL.n_columns==alR.n_columns );

	size_t nn = alL.n_columns*alL.n_rows;
	for ( size_t i=0; i<nn; i++ ) {
		if ( L.array[i]==0 )
			L.array[i]=-100;
		if ( R.array[i]==0 )
			R.array[i]=-100;
	}
	return L < R;
}
*/

/// return true of alL is smaller than alR in LMC-0 ordering
bool compareLMC0 ( const array_link &alL, const array_link &alR )
{
	assert ( alL.n_rows==alR.n_rows );
	assert ( alL.n_columns==alR.n_columns );

	for ( int c=0; c<alL.n_columns; c++ ) {
		const array_t *al = alL.array + c*alL.n_rows;
		const array_t *ar = alR.array + c*alR.n_rows;

		// check position of zero(s) in column c
		for ( int r=0; r<alL.n_rows; r++ ) {
			if ( al[r]==0 &&  ar[r]!=0 )
				return true;
			if ( al[r]!=0 && ar[r]==0 )
				return false;
		}

		int zl = maxz ( alL, c );
		int zr = maxz ( alR, c );

		if ( zl<zr )
			return true;
		if ( zl>zr )
			return false;

		// zero is at same position(s) in column, let LMC ordering decide
		for ( int r=0; r<alL.n_rows; r++ ) {
			if ( al[r]> ar[r] )
				return true;	// note the reversed sign here
			if ( al[r]< ar[r] )
				return false;	// note the reversed sign here
		}
	}
	// the arrays are equal
	return false;
}

bool compareLMC0_1 ( const array_link &alL, const array_link &alR )
{
	assert ( alL.n_rows==alR.n_rows );
	assert ( alL.n_columns==alR.n_columns );

	for ( int c=0; c<alL.n_columns; c++ ) {
		// check position of zero in column c
		int zl = maxz ( alL, c );
		int zr = maxz ( alR, c );

		if ( zl<zr )
			return true;
		if ( zl>zr )
			return false;

		// zero is at same position in column, let LMC ordering decide
		const array_t *al = alL.array + c*alL.n_rows;
		const array_t *ar = alR.array + c*alR.n_rows;
		for ( int r=0; r<alL.n_rows; r++ ) {
			if ( al[r]> ar[r] )
				return true;	// note the reversed sign here
			if ( al[r]< ar[r] )
				return false;	// note the reversed sign here
		}
	}
	// the arrays are equal
	return false;
}


arraylist_t sortLMC0 ( const arraylist_t &lst )
{
	arraylist_t outlist = lst;
	sort ( outlist.begin(), outlist.end(), compareLMC0 );
	return outlist;
}

conference_options::conference_options ( int maxpos )
{
	maxzpos=-1;
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
