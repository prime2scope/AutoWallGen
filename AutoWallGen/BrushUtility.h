

#ifndef BRUSH_UTILITY_H
#define BRUSH_UTILITY_H

/***************************************************************

Programmer: Ben Stockwell

Purpose: Contains various functions used in the program

Sorry for the mess

***************************************************************/

#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>

#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <stdexcept>


using namespace std;

static string INPUT_FILE_NAME = "map_layout.vmf";

static string OUTPUT_FILE_STR = "AutoWall_Output_";
static string OUTPUT_FILE_NAME;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
void build_layout_vec( vector<string>& vect_input, vector<Brush>& vec_layout );
void print_brush_dif( vector<Brush>& vec_layout );
void generate_walls( vector<Brush>& vec_layout, vector<Brush>& vec_walls );

void gen_wall( const Plane& , vector<Brush>& vec_walls );
void remove_intersect( vector<Brush>& vec_walls );
void output_vmf(  const vector<string>& vecprin, vector<Brush>& vec_walls );

void get_lines( vector<string>& wget, vector<string>& rawVec );
void clean_entry( const string& str1, string& str2 );

void debug_print( vector<string> line_vec, vector<string> line_vec_raw );
void print_vector( const vector<string>& vecprin );
void print_vector_nofilt( const vector<string>& vecprin );
void print_brush( const vector<Brush>& vec_walls );
void print_xy_pairs( vector<Brush>& vec_layout );

void set_corners( Brush& );

void file_prompt();
void rev_pl( Plane& ); // Reverses the order of the Verticies in the group of 3 verts

void prep_seed_L( const Plane&, const Plane&, Plane& );
void prep_seed_R( const Plane&, const Plane&, Plane& );
void prep_seed_yL( const Plane&, const Plane&, Plane& );
void prep_seed_yR( const Plane&, const Plane&, Plane& );


string IncFilename(string s);
inline bool FileExist(const std::string& name);

bool face_align( const Plane&, const Plane& );

double get_slope_inv( const Plane& );
double get_b_value( const Plane& );
double get_mf_length( const Plane& );

double distance_get( const double x1, const double x2, const double y1, const double y2 );

double get_smallest_z( const Plane& );   // returns the VALUE of small or largest
double get_largest_z ( const Plane& );   // returns the VALUE of small or largest
double get_smallest_x( const Plane& );   // returns the VALUE of small or largest
double get_largest_x ( const Plane& );   // returns the VALUE of small or largest
double get_smallest_y( const Plane& );   // returns the VALUE of small or largest
double get_largest_y ( const Plane& );   // returns the VALUE of small or largest

int get_smallest_z_sub( const Plane& );  // returns the SUBSCRIPT of small or largest
int get_largest_z_sub ( const Plane& );  // returns the SUBSCRIPT of small or largest
int get_smallest_x_sub( const Plane& );  // returns the SUBSCRIPT of small or largest
int get_largest_x_sub ( const Plane& );  // returns the SUBSCRIPT of small or largest
int get_smallest_y_sub( const Plane& );  // returns the SUBSCRIPT of small or largest
int get_largest_y_sub ( const Plane& );  // returns the SUBSCRIPT of small or largest

int uniq_Z_vals( const Plane& );
int face_dir_get( const Plane& );


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions



/***********************************************************************
FUNCTION:	void set_corners( Brush& brsh )

USE: fills the corners data member of a Brush object

PARAMETERS: Takes a reference to a Brush Object

RETURNS: nothing, directly edits the data member of the object

NOTE: assumes there are only 4 corners

***********************************************************************/

void set_corners( Brush& brsh )
	{
		Plane pln; // temp plane to use double pointers

		int f = 1;

		Corners tmp_crn;

		pln = brsh.solid_plane[0];

		tmp_crn.crn_vrts[0][0] = *pln.X_1; // Set the first corner X and Y values
		tmp_crn.crn_vrts[0][1] = *pln.Y_1;


		for ( int x = 0; x < int(brsh.solid_plane.size()); x++ )
			{
				pln = brsh.solid_plane[x];

				for ( int i = 0; i < 3; i++ )
					{
						if ( ( pln.plane_verts[i][0] != tmp_crn.crn_vrts[0][0] || pln.plane_verts[i][1] != tmp_crn.crn_vrts[0][1] ) && ( pln.plane_verts[i][0] != tmp_crn.crn_vrts[1][0] || pln.plane_verts[i][1] != tmp_crn.crn_vrts[1][1] ) &&
							  ( pln.plane_verts[i][0] != tmp_crn.crn_vrts[2][0] || pln.plane_verts[i][1] != tmp_crn.crn_vrts[2][1] ) && ( pln.plane_verts[i][0] != tmp_crn.crn_vrts[3][0] || pln.plane_verts[i][1] != tmp_crn.crn_vrts[3][1] ) )
							{
								tmp_crn.crn_vrts[f][0] = pln.plane_verts[i][0];
								tmp_crn.crn_vrts[f][1] = pln.plane_verts[i][1];
								f++;
							}
					}
			}

		//cout << endl << "Here is finshed corner plane : " << tmp_crn << endl;

		brsh.CRNR = tmp_crn;

		cout << endl << "                    Here is finshed corner plane : " << endl << brsh.CRNR << endl;

	}


/***************************************************************
Method: double get_mf_length( const Plane& plane_v )

Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/

double get_mf_length( const Plane& plane_v )
	{
		double x1 = 0.0,
			x2 = 0.0,
			y1 = 0.0,
			y2 = 0.0;

		if ( *plane_v.X_2 == *plane_v.X_3 && *plane_v.Y_2 == *plane_v.Y_3 )
			{
				if ( *plane_v.X_1 != *plane_v.X_2 || *plane_v.Y_1 != *plane_v.Y_2)
					{
						x1 = *plane_v.X_1;
						x2 = *plane_v.X_2;
						y1 = *plane_v.Y_1;
						y2 = *plane_v.Y_2;
					}
				else if ( (*plane_v.X_1 == *plane_v.X_2 && *plane_v.X_2 == *plane_v.X_3) && (*plane_v.Y_1 == *plane_v.Y_2 && *plane_v.Y_2 == *plane_v.Y_3) )
					{
						return -99999999.9;
					}
				else
					{
						cout << endl << "gt_mf_len ERROR: Cannot Determine SLOPE for plane - get B val" << plane_v << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane - get B val" << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane - get B val" << endl;
						return -99999999.9;
					}
			}
		else
			{
				//cout << endl << plane_slo.plane_verts[1][0] << " != " << plane_slo.plane_verts[2][0] << " AND " << plane_slo.plane_verts[1][1] <<  " != " << plane_slo.plane_verts[2][1];
				x1 = plane_v.plane_verts[1][0];
				x2 = plane_v.plane_verts[2][0];
				y1 = plane_v.plane_verts[1][1];
				y2 = plane_v.plane_verts[2][1];
			}


		double distance = sqrt(pow(( x2 - x1 ), 2) + pow(( y2 - y1 ), 2));
		return distance;
	}



/***************************************************************
Method: bool face_align( const Plane& t1, const Plane& t2)

Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/

bool face_align( const Plane& t1, const Plane& t2)
	{
		double temp_val;

		//cout << "    bval t1 = " << get_b_value(t1);
		//cout << "    bval t2 = " << get_b_value(t2) << "  slop t1 = " << get_slope_inv(t1) << "  slop t2 = " << get_slope_inv(t2) << endl;
		if ( (get_slope_inv(t2) == get_slope_inv(t1)) && (get_b_value(t1) == get_b_value(t2)) && get_b_value(t1) < 99999.9 && get_b_value(t1) > -99999.9 )
			{
				//cout << get_b_value(t1) << " is equal to " << get_b_value(t2);
				temp_val = (( (get_slope_inv(t2)) * *t2.X_1 ) + get_b_value(t2) );
				//cout << "                                                    tval = " << temp_val << " t2 y1 = " << *t2.Y_1 << endl;
				if ( temp_val == *t2.Y_1 )
					{
						return 1;
					}
			}
		else if ( ((get_slope_inv(t2) != get_slope_inv(t1)) || (get_b_value(t1) != get_b_value(t2))) && (get_b_value(t1) > 99999.9 || get_b_value(t1) < -99999.9) && (get_b_value(t2) > 99999.9 || get_b_value(t2) < -99999.9) )
			{
				temp_val = (( (get_slope_inv(t2)) * *t2.X_1 ) + get_b_value(t2) );
				//cout << "                                                    tval = " << temp_val << " t2 y1 = " << *t2.Y_1 << endl;
				if ( temp_val == *t2.Y_1 )
					{
						return 2;
					}
			}
		else if ( ((get_slope_inv(t2) == get_slope_inv(t1)) || (get_b_value(t1) == get_b_value(t2))) && (get_b_value(t1) > 99999.9 || get_b_value(t1) < -99999.9) && (get_b_value(t2) > 99999.9 || get_b_value(t2) < -99999.9) )
			{
				temp_val = (( (get_slope_inv(t2)) * *t2.X_1 ) + get_b_value(t2) );
				//cout << "                                                    tval = " << temp_val << " t2 y1 = " << *t2.Y_1 << endl;
				if ( temp_val == *t2.Y_1 )
					{
						return 3;
					}
			}
		else
			{
				return 0;
			}
	}


/***************************************************************
Method: void prep_seed( const Plane& t1, const Plane& t2, Plane& p_seed)

Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/


void prep_seed_L( const Plane& t1, const Plane& t2, Plane& p_seed )
	{

		int sx_sub_t1 = get_smallest_x_sub(t1);
		//double bx_sub = get_largest_x_sub(t1);

		p_seed = t2;

		int wall_thick_2 = WALL_THICKNESS;

		// the Largest X from T2 should = Smallest X from T1

		// This decision should cover all possible cominations of 3 verticies, assuming that at least 2 of the vertices are aligned vertically
		if ( *p_seed.X_1 == *p_seed.X_2 && *p_seed.Y_1 == *p_seed.Y_2 ) // if X1 == X2 and Y1 == Y2
			{                                                                // then vertices 1 and 2 are vertically aligned
				if ( *p_seed.Z_1 > *p_seed.Z_2 ) // if Z1 is above Z2
					{
						if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[sx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[sx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[sx_sub_t1][1];
									}
							}
					}
				else if ( *p_seed.Z_1 < *p_seed.Z_2 ) // if Z1 is below Z2
					{
						if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[sx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[sx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[sx_sub_t1][1];
									}
							}
					}
			}
		else if ( *p_seed.X_3 == *p_seed.X_2 && *p_seed.Y_3 == *p_seed.Y_2 )  // else if X3 == X2 and Y3 == Y2
			{                                                                            // then vertices 3 and 2 are vertically aligned
				if ( *p_seed.Z_3 > *p_seed.Z_2 )  // if Z3 is above Z2                  // note, it should not be possible for verts 3 and 1 to be aligned
					{                                                                      // at least it is unecessary for making a layout to a map... you can make inclined planes that still follow the rules
						if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[sx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[sx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[sx_sub_t1][1];
									}
							}
					}
				else if ( *p_seed.Z_3 < *p_seed.Z_2 )  // if Z3 is below Z2
					{
						if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[sx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[sx_sub_t1][0] + wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[sx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[sx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[sx_sub_t1][1];
									}
							}
					}
			}
		else
			{
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << p_seed << endl;
			}

	}


/***************************************************************
Method: void prep_seed( const Plane& t1, const Plane& t2, Plane& p_seed)

Use: Used to prep a Seed plane, to exclude the section that
	  intersects with another brush

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/


void prep_seed_R( const Plane& t1, const Plane& t2, Plane& p_seed)
	{

		int lx_sub_t1 = get_largest_x_sub(t1);
		//double bx_sub = get_largest_x_sub(t1);

		p_seed = t2;

		int wall_thick_2 = WALL_THICKNESS;

		// the Smallest X from T2 should = Largest X from T1

		// This decision should cover all possible combinations of 3 verticies, assuming that at least 2 of the vertices are aligned vertically
		if ( *p_seed.X_1 == *p_seed.X_2 && *p_seed.Y_1 == *p_seed.Y_2 ) // if X1 == X2 and Y1 == Y2
			{                                                                // then vertices 1 and 2 are vertically aligned
				if ( *p_seed.Z_1 > *p_seed.Z_2 ) // if Z1 is above Z2
					{
						if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[lx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[lx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[lx_sub_t1][1];
									}
							}
					}
				else if ( *p_seed.Z_1 < *p_seed.Z_2 ) // if Z1 is below Z2
					{
						if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[lx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[lx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[lx_sub_t1][1];
									}
							}
					}
			}
		else if ( *p_seed.X_3 == *p_seed.X_2 && *p_seed.Y_3 == *p_seed.Y_2 )  // else if X3 == X2 and Y3 == Y2
			{                                                                            // then vertices 3 and 2 are vertically aligned
				if ( *p_seed.Z_3 > *p_seed.Z_2 )  // if Z3 is above Z2                  // note, it should not be possible for verts 3 and 1 to be aligned
					{                                                                      // at least it is unecessary for making a layout to a map... you can make inclined planes that still follow the rules
						if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[lx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[lx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[lx_sub_t1][1];
									}
							}
					}
				else if ( *p_seed.Z_3 < *p_seed.Z_2 )  // if Z3 is below Z2
					{
						if ( *p_seed.X_1 == get_smallest_x(p_seed) )
							{
								*p_seed.X_1 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "a - t2 seed y1 != t1 sx_sub y val" << endl;
										*p_seed.Y_1 = t1.plane_verts[lx_sub_t1][1];
									}
							}
						else if ( *p_seed.X_1 == get_largest_x(p_seed) )
							{
								*p_seed.X_3 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								*p_seed.X_2 = t1.plane_verts[lx_sub_t1][0] - wall_thick_2;
								if ( *p_seed.Y_3 != t1.plane_verts[lx_sub_t1][1] )
									{
										cout << endl << "b - t2 seed y3 != t1 sx_sub y val" << endl;
										*p_seed.Y_3 = t1.plane_verts[lx_sub_t1][1];
										*p_seed.Y_2 = t1.plane_verts[lx_sub_t1][1];
									}
							}
					}
			}
		else
			{
				cout << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cout << "Brush Verticies = " << p_seed << endl;
			}

	}



/***************************************************************
Method: void prep_seed( const Plane& t1, const Plane& t2, Plane& p_seed)

Use: Preps the Seed face for constructing a new brush that does
	  not intersect with the previous brush

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/


void prep_seed_yL( const Plane& t1, const Plane& t2, Plane& p_seed)
	{

		int sy_sub_t1 = get_smallest_y_sub(t1);

		p_seed = t2;

		int wall_thick_2 = WALL_THICKNESS;

		// the Largest Y from T2 should = Smallest Y from T1

		// This decision should cover all possible combinations of 3 verticies, assuming that at least 2 of the vertices are aligned vertically
		if ( *p_seed.X_1 == *p_seed.X_2 && *p_seed.Y_1 == *p_seed.Y_2 ) // if X1 == X2 and Y1 == Y2
			{                                                                // then vertices 1 and 2 are vertically aligned
				if ( *p_seed.Z_1 > *p_seed.Z_2 ) // if Z1 is above Z2
					{
						if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[sy_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[sy_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[sy_sub_t1][0];
									}
							}
					}
				else if ( *p_seed.Z_1 < *p_seed.Z_2 ) // if Z1 is below Z2
					{
						if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[sy_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[sy_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[sy_sub_t1][0];
									}
							}
					}
			}
		else if ( *p_seed.X_3 == *p_seed.X_2 && *p_seed.Y_3 == *p_seed.Y_2 )  // else if X3 == X2 and Y3 == Y2
			{                                                                            // then vertices 3 and 2 are vertically aligned
				if ( *p_seed.Z_3 > *p_seed.Z_2 )  // if Z3 is above Z2                  // note, it should not be possible for verts 3 and 1 to be aligned
					{                                                                      // at least it is unecessary for making a layout to a map... you can make inclined planes that still follow the rules
						if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[sy_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[sy_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[sy_sub_t1][0];
									}
							}
					}
				else if ( *p_seed.Z_3 < *p_seed.Z_2 )  // if Z3 is below Z2
					{
						if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.Y_1 = t1.plane_verts[sy_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[sy_sub_t1][1] + wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[sy_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[sy_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[sy_sub_t1][0];
									}
							}
					}
			}
		else
			{
				cout << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cout << "Brush Verticies = " << p_seed << endl;
			}
	}



/***************************************************************
Method: void prep_seed_yR( const Plane& t1, const Plane& t2, Plane& p_seed)

Use: Preps the Seed face for constructing a new brush that does
	  not intersect with the previous brush

Parameters: Takes a reference to a plane integers

Returns: returns distance as Int

***************************************************************/


void prep_seed_yR( const Plane& t1, const Plane& t2, Plane& p_seed)
	{

		int ly_sub_t1 = get_largest_y_sub(t1);

		p_seed = t2;

		int wall_thick_2 = WALL_THICKNESS;

		// the Smallest Y from T2 should = Largest Y from T1

		// This decision should cover all possible combinations of 3 verticies, assuming that at least 2 of the vertices are aligned vertically
		if ( *p_seed.X_1 == *p_seed.X_2 && *p_seed.Y_1 == *p_seed.Y_2 ) // if X1 == X2 and Y1 == Y2
			{                                                                // then vertices 1 and 2 are vertically aligned
				if ( *p_seed.Z_1 > *p_seed.Z_2 ) // if Z1 is above Z2
					{
						if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[ly_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[ly_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[ly_sub_t1][0];
									}
							}
					}
				else if ( *p_seed.Z_1 < *p_seed.Z_2 ) // if Z1 is below Z2
					{
						if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[ly_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[ly_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[ly_sub_t1][0];
									}
							}
					}
			}
		else if ( *p_seed.X_3 == *p_seed.X_2 && *p_seed.Y_3 == *p_seed.Y_2 )  // else if X3 == X2 and Y3 == Y2
			{                                                                            // then vertices 3 and 2 are vertically aligned
				if ( *p_seed.Z_3 > *p_seed.Z_2 )  // if Z3 is above Z2                  // note, it should not be possible for verts 3 and 1 to be aligned
					{                                                                      // at least it is unecessary for making a layout to a map... you can make inclined planes that still follow the rules
						if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_1 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.X_1 = t1.plane_verts[ly_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[ly_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[ly_sub_t1][0];
									}
							}
					}
				else if ( *p_seed.Z_3 < *p_seed.Z_2 )  // if Z3 is below Z2
					{
						if ( *p_seed.Y_1 == get_smallest_y(p_seed) )
							{
								*p_seed.Y_1 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.Y_1 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "a - t2 seed x1 != t1 sx_sub x val" << endl;
										*p_seed.Y_1 = t1.plane_verts[ly_sub_t1][0];
									}
							}
						else if ( *p_seed.Y_1 == get_largest_y(p_seed) )
							{
								*p_seed.Y_3 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								*p_seed.Y_2 = t1.plane_verts[ly_sub_t1][1] - wall_thick_2;
								if ( *p_seed.X_3 != t1.plane_verts[ly_sub_t1][0] )
									{
										cout << endl << "b - t2 seed x3 != t1 sx_sub x val" << endl;
										*p_seed.X_3 = t1.plane_verts[ly_sub_t1][0];
										*p_seed.X_2 = t1.plane_verts[ly_sub_t1][0];
									}
							}
					}
			}
		else
			{
				cout << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cout << "Brush Verticies = " << p_seed << endl;
			}

	}



/***************************************************************
Method: int uniq_Z_vals( const Plane& plane_v)

Use: returns the number of UNIQUE Z values, used to quick determine
	  if the plane has a Slope on the Z access

Parameters: Takes a reference to a plane

Returns: returns count, the number of unique Z values

***************************************************************/

int uniq_Z_vals( const Plane& plane_v)
	{
      double temp_z = 0.0;

      int count = 0;

		if ( *plane_v.Z_1 != *plane_v.Z_2 ){
		count++;}
		//else{cout << *plane_v.Z_1 << " == " << *plane_v.Z_2 << endl;}

		if ( *plane_v.Z_1 != *plane_v.Z_3 ){
		count++;}
		//else{cout << *plane_v.Z_1 << " == " << *plane_v.Z_3 << endl;}

		if ( *plane_v.Z_2 != *plane_v.Z_3 ){
		count++;}
		//else{cout << *plane_v.Z_2 << " == " << *plane_v.Z_3 << endl;}

      return count;
   }



// Checks if a file exists
inline bool FileExist(const std::string& name)
	{
	    ifstream f(name.c_str());
	    return f.good();
	}

// Used to return an incremented file name
string IncFilename(string s)
	{
		// Base file name
		string fileF = s;
		int n = 1;
		string fileE = ".vmf";

		// Stream to update file name
		ostringstream fname;
		fname << fileF << n << fileE;

		// Get a new incremented file name
		while( FileExist(fname.str()) )
			{
				// Reset the file name it
				fname.str("");
				fname.clear();

				// Increment and try again
				n++;
				fname << fileF << n << fileE;
			}

		// Return the Incremented File Name
		return fname.str();
	}




/***************************************************************
Method: void clean_entry( const string& str1, string& str2 )

Use: It cleans a word from its punctuation marks. The first
	argument is the original word in the input stream and the
	second argument contains the same word after cleaning.

Parameters: Takes a reference string and a clean string

Returns: Nothing, simply cleans the string globally

***************************************************************/

void clean_entry( const string& str1, string& str2 )
	{
		// set the clean string to the input string
		str2 = str1;

		// Loop through and clean the line
		for(int i = 0, size = str2.size(); i < size; i++)
			{
				//str2[i] = tolower(str2[i]); // convert each character to lower

				// Remove punctuation
				if(ispunct(str2[i]))
					{
						//tolower(str2[i]);
						//if ( str2[i] != '{' && str2[i]  != '}' && str2[i]  != '-' && str2[i]  != '.' )
						if ( str2[i] != '{' && str2[i]  != '}' && str2[i]  != '-' && str2[i]  != '.' && str2[i]  != '[' && str2[i]  != ']' ) // included brackets for Uaxis and Vaxis
							{
								//cout << (int)str2[i] << endl;
								str2.erase(i--, 1);
								size = str2.size();
							}
					}
				// Remove punctuation
				if(isspace(str2[i]))
					{
						// if the character is a TAB character, should NOT remove spaces
						if ( (int)str2[i] == 9 )
							{
								// Remove it
								str2.erase(i--, 1);
								size = str2.size();
							}
					}
			}
	}



/***************************************************************
Method:int face_dir_get( const Plane& )

Use: Gets the direction of the face passed in and returns an INT
	  which indicated that direction.
	  1 = -X  or  negative X vector
	  2 = +X  or  positive X vector
	  3 = -Y
	  4 = +Y

Parameters: Takes a reference to a Plane object

Returns: Returns and integer which indicates the face direction
			1 = -X  or  negative X vector
		   2 = +X  or  positive X vector
		   3 = -Y
		   4 = +Y

Note: This is only really helpful if the face is parallel with
		a certain axis. It is used to make a decision about which
		way to move the faces of the walls when Generating walls
		for a layout.

***************************************************************/


int face_dir_get( const Plane& pl_fdir)
	{
		cout << endl << endl << " <--------------------- TESTING PLANE ---------------------> " << endl << "  " << pl_fdir << endl;

		int face_direction = -999;
		Plane t_pl_fdir;
		t_pl_fdir = pl_fdir;
		if ( *t_pl_fdir.Y_2 != *t_pl_fdir.Y_1 )
			{
				cout << endl << "               Pass C1 fdir -------------- <<<<< " << endl;
				if ( *t_pl_fdir.Y_2 < *t_pl_fdir.Y_1 )
					{
						cout << "               Pass C1a fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
							{
								face_direction = 1;
							}
						else if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
							{
								face_direction = 2;
							}
					}
				else if ( *t_pl_fdir.Y_2 > *t_pl_fdir.Y_1 )
					{
						cout << "               Pass C1b fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
							{
								face_direction = 1;
							}
						else if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
							{
								face_direction = 2;
							}
					}
				else
					{
						cout << "               FAIL C1ab fdir -------------- <<<<< " << endl;
					}
			}
		else if ( *t_pl_fdir.Y_2 == *t_pl_fdir.Y_1 )
			{
				cout << endl << "               Pass C2 fdir -------------- <<<<< " << endl;
				if ( *t_pl_fdir.Y_2 < *t_pl_fdir.Y_3 )
					{
						cout << "               Pass C2a fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
							{
								face_direction = 1;
							}
						else if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
							{
								face_direction = 2;
							}
					}
				else if ( *t_pl_fdir.Y_2 > *t_pl_fdir.Y_3 )
					{
						cout << "               Pass C2b fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
							{
								face_direction = 1;
							}
						else if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
							{
								face_direction = 2;
							}
					}
				else
					{
						cout << "               FAIL C2ab fdir -------------- <<<<< " << endl;
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						if ( *t_pl_fdir.X_2 != *t_pl_fdir.X_1 )   // Second part for +Y and -Y
							{
								cout << endl << "               Pass C3_2 fdir -------------- <<<<< " << endl;
								if ( *t_pl_fdir.X_2 < *t_pl_fdir.X_1 )
									{
										cout << "               Pass C3a_2 fdir -------------- <<<<< " << endl;
										//cout << *t_pl_fdir.X_2 << " < " << *t_pl_fdir.X_1 << endl;
										if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
											{
												face_direction = 4;
											}
										else if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
											{
												face_direction = 3;
											}
									}
								else if ( *t_pl_fdir.X_2 > *t_pl_fdir.X_1 )
									{
										cout << "               Pass C3b_2 fdir -------------- <<<<< " << endl;
										//cout << *t_pl_fdir.X_2 << " > " << *t_pl_fdir.X_1 << endl;
										if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
											{
												face_direction = 4;
											}
										else if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
											{
												face_direction = 3;
											}
									}
								else
									{
										cout << "               FAIL C3ab_2 fdir -------------- <<<<< " << endl;
									}
							}
						else if ( *t_pl_fdir.X_2 == *t_pl_fdir.X_1 )
							{
								cout << endl << "               Pass C4_2 fdir -------------- <<<<< " << endl;
								if ( *t_pl_fdir.X_2 < *t_pl_fdir.X_3 )
									{
									cout << "               Pass C4a_2 fdir -------------- <<<<< " << endl;
										if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
											{
												face_direction = 4;
											}
										else if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
											{
												face_direction = 3;
											}
									}
								else if ( *t_pl_fdir.X_2 > *t_pl_fdir.X_3 )
									{
										cout << "               Pass C4b_2 fdir -------------- <<<<< " << endl;
										if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
											{
												face_direction = 4;
											}
										else if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
											{
												face_direction = 3;
											}
									}
								else
									{
										cout << "               FAIL C4ab_2 fdir -------------- <<<<< " << endl;
									}
							}
						else
							{
								cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
								cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
								cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
							}
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
			}
		//cout << "   - fdir set to : " << face_direction << endl;
		else if ( *t_pl_fdir.X_2 != *t_pl_fdir.X_1 )   // Second part for +Y and -Y
			{
				cout << endl << "               Pass C3 fdir -------------- <<<<< " << endl;
				if ( *t_pl_fdir.X_2 < *t_pl_fdir.X_1 )
					{
						cout << "               Pass C3a fdir -------------- <<<<< " << endl;
						//cout << *t_pl_fdir.X_2 << " < " << *t_pl_fdir.X_1 << endl;
						if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
							{
								face_direction = 4;
							}
						else if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
							{
								face_direction = 3;
							}
					}
				else if ( *t_pl_fdir.X_2 > *t_pl_fdir.X_1 )
					{
						cout << "               Pass C3b fdir -------------- <<<<< " << endl;
						//cout << *t_pl_fdir.X_2 << " > " << *t_pl_fdir.X_1 << endl;
						if ( *t_pl_fdir.Z_3 > *t_pl_fdir.Z_2 )
							{
								face_direction = 4;
							}
						else if ( *t_pl_fdir.Z_3 < *t_pl_fdir.Z_2 )
							{
								face_direction = 3;
							}
					}
				else
					{
						cout << "               FAIL C3ab fdir -------------- <<<<< " << endl;
					}
			}
		else if ( *t_pl_fdir.X_2 == *t_pl_fdir.X_1 )
			{
				cout << endl << "               Pass C4 fdir -------------- <<<<< " << endl;
				if ( *t_pl_fdir.X_2 < *t_pl_fdir.X_3 )
					{
					cout << "               Pass C4a fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
							{
								face_direction = 4;
							}
						else if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
							{
								face_direction = 3;
							}
					}
				else if ( *t_pl_fdir.X_2 > *t_pl_fdir.X_3 )
					{
						cout << "               Pass C4b fdir -------------- <<<<< " << endl;
						if ( *t_pl_fdir.Z_1 < *t_pl_fdir.Z_2 )
							{
								face_direction = 4;
							}
						else if ( *t_pl_fdir.Z_1 > *t_pl_fdir.Z_2 )
							{
								face_direction = 3;
							}
					}
				else
					{
						cout << "               FAIL C4ab fdir -------------- <<<<< " << endl;
					}
			}
		else
			{
				cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
				cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
				cout << endl << " ERROR - failed f_dir check -------------- <<<<< " << endl;
			}

		return face_direction;
	}


/***************************************************************
Method: void rev_pl( Plane& pl_rev)

Use: Reverses the order of the Verticies in the group of 3 verts
	  that define a plane.
	  if the verts are clockwise from left to right when looked
	  then that is the direction the plane faces.
	  This essentially is used to REVERSE the direction that the
	  plane is facing.

Parameters: a plane

Returns: returns nothing - edits the plane directly

***************************************************************/

void rev_pl( Plane& pl_rev)
	{
		int i = 0;
		int t_vx[3], t_vy[3], t_vz[3];

		for ( i = 0; i < 3; i++ )
			{
				for (int x = 0; x < 3; x++)
					{
						if ( i == 0 )
							{
								t_vx[x] = pl_rev.plane_verts[0][x];
							}
						else if ( i == 2 )
							{
								t_vz[x] = pl_rev.plane_verts[2][x];
							}
					}
			}
		
		for ( i = 0; i < 3; i++ )
			{
				for (int x = 0; x < 3; x++)
					{
						if ( i == 0 )
							{
								pl_rev.plane_verts[0][x] = t_vz[x];
							}
						else if ( i == 2 )
							{
								pl_rev.plane_verts[2][x] = t_vx[x];
							}
					}
			}
	}


/***************************************************************
Method: int distance_get( const int x1, const int x2, const int y1, const int y2 )

Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double distance_get( const double x1, const double x2, const double y1, const double y2 )
	{
		double distance = sqrt(pow(( x2 - x1 ), 2) + pow(( y2 - y1 ), 2));
		return distance;
	}

/***************************************************************
Method: double get_smallest_*( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double get_smallest_z( const Plane& planex )
   {
      double temp_z = 0.0;
      int small_pose = 0;
      temp_z = planex.plane_verts[0][2];
      //cout << endl << "    temp z is " << temp_z << endl;
      for (int i = 1; i < 3; i++)
         {
         	//cout << endl << "    plane x vert " << i << " Z = " << planex.plane_verts[i][2] << endl;
            if ( planex.plane_verts[i][2] < temp_z )
               {
                  temp_z = planex.plane_verts[i][2];
                  small_pose = i;
                  //cout << endl << "   -- temp z is " << temp_z << endl;
               }
         }
      return planex.plane_verts[small_pose][2];
   }


/***************************************************************
Method: double get_smallest_*( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double get_largest_z( const Plane& planex )
   {
      double temp_z = 0.0;
      int big_pose = 0;
      temp_z = planex.plane_verts[0][2];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][2] > temp_z )
               {
                  temp_z = planex.plane_verts[i][2];
                  big_pose = i;
               }
         }
      return planex.plane_verts[big_pose][2];
   }


/***************************************************************
Method: double get_smallest_x( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double get_smallest_x( const Plane& planex )
   {
      double temp_x = 0.0;
      int small_pose = 0;
      temp_x = planex.plane_verts[0][0];
      //cout << endl << "    temp z is " << temp_x << endl;
      for (int i = 1; i < 3; i++)
         {
         	//cout << endl << "    plane x vert " << i << " Z = " << planex.plane_verts[i][2] << endl;
            if ( planex.plane_verts[i][0] < temp_x )
               {
                  temp_x = planex.plane_verts[i][0];
                  small_pose = i;
                  //cout << endl << "   -- temp z is " << temp_x << endl;
               }
         }
      return planex.plane_verts[small_pose][0];
   }

/***************************************************************
Method: int get_smallest_x_sub( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

int get_smallest_x_sub( const Plane& planex )
   {
      double temp_x = 0.0;
      int small_pose = 0;
      temp_x = planex.plane_verts[0][0];
      //cout << endl << "    temp z is " << temp_x << endl;
      for (int i = 1; i < 3; i++)
         {
         	//cout << endl << "    plane x vert " << i << " Z = " << planex.plane_verts[i][2] << endl;
            if ( planex.plane_verts[i][0] < temp_x )
               {
                  temp_x = planex.plane_verts[i][0];
                  small_pose = i;
                  //cout << endl << "   -- temp z is " << temp_x << endl;
               }
         }
      return small_pose;
   }


/***************************************************************
Method: double get_largest_x( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double get_largest_x( const Plane& planex )
   {
      double temp_x = 0.0;
      int big_pose = 0;
      temp_x = planex.plane_verts[0][0];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][0] > temp_x )
               {
                  temp_x = planex.plane_verts[i][0];
                  big_pose = i;
               }
         }
      return planex.plane_verts[big_pose][0];
   }

/***************************************************************
Method: int get_largest_x_sub( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

int get_largest_x_sub( const Plane& planex )
   {
      double temp_x = 0.0;
      int big_pose = 0;
      temp_x = planex.plane_verts[0][0];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][0] > temp_x )
               {
                  temp_x = planex.plane_verts[i][0];
                  big_pose = i;
               }
         }
      return big_pose;
   }


/***************************************************************
Method: double get_smallest_y( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/


double get_smallest_y( const Plane& planex )
   {
      double temp_y = 0.0;
      int small_pose = 0;
      temp_y = planex.plane_verts[0][1];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][1] < temp_y )
               {
                  temp_y = planex.plane_verts[i][1];
                  small_pose = i;
               }
         }
      return planex.plane_verts[small_pose][1];
   }


/***************************************************************
Method: int get_smallest_y_sub( const Plane& planex )

Use: Returns the subscript of the smallest y value

Parameters: take a ref to a plane

Returns: returns distance as Int

***************************************************************/

int get_smallest_y_sub( const Plane& planex )
   {
      double temp_y = 0.0;
      int small_pose = 0;
      temp_y = planex.plane_verts[0][1];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][1] < temp_y )
               {
                  temp_y = planex.plane_verts[i][1];
                  small_pose = i;
               }
         }
      return small_pose;
   }

/***************************************************************
Method: double get_largest_y( const Plane& planex )
Use: Returns the distance between the 2 points on a 2D plane

Parameters: Takes 4 integers

Returns: returns distance as Int

***************************************************************/

double get_largest_y( const Plane& planex )
   {
      double temp_y = 0.0;
      int big_pose = 0;
      temp_y = planex.plane_verts[0][1];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][1] > temp_y )
               {
                  temp_y = planex.plane_verts[i][1];
                  big_pose = i;
               }
         }
      return planex.plane_verts[big_pose][1];
   }


/***************************************************************
Method: int get_largest_y_sub( const Plane& planex )

Use:  Returns the subscript of the largest y value

Parameters: Takes a ref to a plane

Returns: returns subscript

***************************************************************/


int get_largest_y_sub( const Plane& planex )
   {
      double temp_y = 0.0;
      int big_pose = 0;
      temp_y = planex.plane_verts[0][1];
      for (int i = 1; i < 3; i++)
         {
            if ( planex.plane_verts[i][1] > temp_y )
               {
                  temp_y = planex.plane_verts[i][1];
                  big_pose = i;
               }
         }
      return big_pose;
   }


/***************************************************************

Method: int get_slope( const Plane& )

Use: Gets the inverse slope? maybe

Parameters: Takes a plane

Returns: returns the slope

***************************************************************/


double get_slope_inv( const Plane& plane_v)
	{
		double x1 = 0.0,
				 x2 = 0.0,
				 y1 = 0.0,
				 y2 = 0.0;
		double slope = 0.0;
		//*plane_slo.X_2
		//if ( plane_slo.plane_verts[1][0] == plane_slo.plane_verts[2][0] && plane_slo.plane_verts[1][1] == plane_slo.plane_verts[2][1])
		if ( *plane_v.X_2 == *plane_v.X_3 && *plane_v.Y_2 == *plane_v.Y_3 )
			{
				//cout << endl << plane_v.plane_verts[1][0] << " == " << plane_v.plane_verts[2][0] << " AND " << plane_v.plane_verts[1][1] <<  " == " << plane_v.plane_verts[2][1];
				//if ( plane_v.plane_verts[0][0] != plane_v.plane_verts[1][0] || plane_v.plane_verts[0][1] != plane_v.plane_verts[1][1])
				if ( *plane_v.X_1 != *plane_v.X_2 || *plane_v.Y_1 != *plane_v.Y_2)
					{
						x1 = *plane_v.X_1;
						x2 = *plane_v.X_2;
						y1 = *plane_v.Y_1;
						y2 = *plane_v.Y_2;
					}
				else if ( (*plane_v.X_1 == *plane_v.X_2 == *plane_v.X_3) &&  (*plane_v.Y_1 == *plane_v.Y_2 == *plane_v.Y_3) )
					{
						return -99999999.9;
					}
				else
					{
						cout << endl << "gt_slop_inv ERROR: Cannot Determine SLOPE for plane" << plane_v << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane" << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane" << endl;
						return -99999999.9;
					}
			}
		else
			{
				//cout << endl << plane_slo.plane_verts[1][0] << " != " << plane_slo.plane_verts[2][0] << " AND " << plane_slo.plane_verts[1][1] <<  " != " << plane_slo.plane_verts[2][1];
				x1 = plane_v.plane_verts[1][0];
				x2 = plane_v.plane_verts[2][0];
				y1 = plane_v.plane_verts[1][1];
				y2 = plane_v.plane_verts[2][1];
			}

		slope = ((y2-y1)/(x2-x1));

		//slope = (1 / slope) * (-1);

		return slope;
	}


/***************************************************************

Method: double get_b_value( const Plane& plane_v)

Use: gets the Y intercept 

Parameters: Takes a plane

Returns: returns the Y intercept 

***************************************************************/


double get_b_value( const Plane& plane_v )
	{
		double x1 = 0.0,
			x2 = 0.0,
			y1 = 0.0,
			y2 = 0.0;
		double slope = 0.0;
		double b_value = 0.0;
		//*plane_slo.X_2
		//if ( plane_slo.plane_verts[1][0] == plane_slo.plane_verts[2][0] && plane_slo.plane_verts[1][1] == plane_slo.plane_verts[2][1])
		if ( *plane_v.X_2 == *plane_v.X_3 && *plane_v.Y_2 == *plane_v.Y_3 )
			{
				if ( *plane_v.X_1 != *plane_v.X_2 || *plane_v.Y_1 != *plane_v.Y_2)
					{
						x1 = *plane_v.X_1;
						x2 = *plane_v.X_2;
						y1 = *plane_v.Y_1;
						y2 = *plane_v.Y_2;
					}
				else
					{
						cout << endl << "gt_b_val ERROR: Cannot Determine SLOPE for plane - get B val" << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane - get B val" << endl;
						//cout << endl << " ERROR: Cannot Determine SLOPE for plane - get B val" << endl;
						return -99999999.9;
					}
			}
		else
			{
				//cout << endl << plane_slo.plane_verts[1][0] << " != " << plane_slo.plane_verts[2][0] << " AND " << plane_slo.plane_verts[1][1] <<  " != " << plane_slo.plane_verts[2][1];
				x1 = plane_v.plane_verts[1][0];
				x2 = plane_v.plane_verts[2][0];
				y1 = plane_v.plane_verts[1][1];
				y2 = plane_v.plane_verts[2][1];
			}

		slope = ((y2-y1)/(x2-x1));


		b_value = ( (-1) * ( ( slope * x1 ) - y1 ) );

		//slope = (1 / slope) * (-1);

		return b_value;
	}

/***************************************************************
Method: void print_vector_nofilt(const vector<string>& vecprin )

Use: Prints the words in the vector

Parameters: Takes a reference to a vector of strings

Returns: Nothing, simply prints

***************************************************************/

void print_brush( const vector<Brush>& vec_walls )
	{
		int x = 0, // Variable integers for subscripts
		    k = 0,
		    f = 0,
		    g = 0;
		//cout << endl  << endl << " ------------------------------------------------- ";
		//cout << endl << " Difference between Input brush, and desired Ouput " << endl << endl;
		/* // Uncomment parts with outfv to generate lines of code for the Wall Generate functions
		ofstream outfv ("output_planes_test.txt");
		if (outfv.fail())
			{
				cout<<"outfile failed open - prog terminate";
				exit(-2);
			}
		*/
		for (x = 0; x < int(vec_walls.size()); x++)
			{
				cout << endl << "  Solid # " << x+1 << endl;
				for (k = 0; k < 6; k++)
					{
						cout << "     Plane # " << k+1 << "  ";
						cout << vec_walls[x].solid_plane[k]; // Print values in the Brush Class Object
						cout << endl;
					}
			}

	}


// Prompt the user for a File name and info about how they want the walls
// to be generated
void file_prompt()
	{
		// Used to ask for a Custom file, but Drag and Drop makes this pointless
		//char cust_fileQ = 'n';
		//cout << endl << "Use custom file name? (y or n) ";
		//cin >> cust_fileQ;
		//if (cust_fileQ == 'y' || cust_fileQ == 'Y')
		//	{
		//		cout << endl << "Enter the filename for the layout without quotes, ex. 'layout_raw.vmf' : ";
		//		cin >> INPUT_FILE_NAME;
		//	}
	
		char cust_wall = 'n';
		
		// Prompt the user if they want to set custom wall sizes
		cerr << endl << "Use custom floor/wall parameters? (y or n) ";
		cin >> cust_wall;
		
		// If they say yes, get the parameters they want to use
		if (cust_wall == 'y' || cust_wall == 'Y')
			{
				// Warning that it may not work, brushes will be invalid and wont load in Hammer
				cerr << endl << endl << "Warning - Non-Default Values may not work (result in Invalid Brushes) " << endl << endl;
				
				// This will be used as an offset for how far up the walls will need to be adjusted from the Seed Face
				cerr << endl << "Enter Floor Thickness of the brushes currently in the layout ";
				cerr << endl << "(default = 16) - Enter Floor Thickness: ";
				cin >> FLOOR_THICKNESS;
				
				// Used for thickness of the new walls
				cerr << endl << "Enter your desired Wall Thickness for the generated walls ";
				cerr << endl << "(default = 16) - Desired Wall Thickness: ";
				cin >> WALL_THICKNESS;
				
				// Used for height of the new walls
				cerr << endl << "Enter your desired Wall Height for the generated walls ";
				cerr << endl << "(default = 128) - Desired Wall Height: ";
				cin >> WALL_HEIGHT;

				cerr << endl;
				cerr << endl;
			}
		
	}


// Used to print the INPUT collected from the file
// May be sent to the log file if Enabled
void debug_print( vector<string> line_vec, vector<string> line_vec_raw )
	{
		// Print the info stored in vector WITHOUT FILTERING them
		//print_vector_nofilt(line_vec_raw);
		//print_vector_nofilt(line_vec);

		// Print the info stored in vector
		print_vector(line_vec);
		//print_vector(line_vec_raw); // Print the info stored in vector
	}


/***************************************************************
Method: void print_vector(const vector<string>& vecprin )

Use: Prints the words in the vector

Parameters: Takes a reference to a vector of strings

Returns: Nothing, simply prints

***************************************************************/

void print_vector(const vector<string>& vecprin )
	{
		string tword;
		vector<string>::const_iterator vec_it_1;
		vec_it_1 = vecprin.begin();

		cout << " Words Stored in the Vector: " << endl << endl;
		while (vec_it_1 != vecprin.end())
			{
				tword = *vec_it_1;
				//cout << *vec_it_1++ << endl;
				if ( tword.find("solid") < 400 )
					{
						cout << endl << endl;
						cout << *vec_it_1++ << " ";
						//cout << endl;
					}
				else if ( tword.find("side") < 400 )
					{
						cout << endl << endl;
						cout << "  " << *vec_it_1++ << " ";
						//cout << endl;
					}
				else if ( tword.find("id") == 0)
					{
						//cout << endl;
						cout << *vec_it_1++;
						//cout << endl;
					}
				else if ( tword.find("plane") < 400 )
					{
						cout << endl;
						cout << "    " << *vec_it_1++;
						cout << endl;

					}
				else
					{
						//cout << *vec_it_1++;
						*vec_it_1++;
					}
				//cout << *vec_it_1++;
			}
	}


/***************************************************************
Method: void print_vector_nofilt(const vector<string>& vecprin )

Use: Prints the words in the vector

Parameters: Takes a reference to a vector of strings

Returns: Nothing, simply prints

***************************************************************/

void print_vector_nofilt(const vector<string>& vecprin )
	{
		string tword;
		vector<string>::const_iterator vec_it_1;
		vec_it_1 = vecprin.begin();

		cout << " - NOT FILTERED - Words Stored in the Vector: " << endl << endl;
		while (vec_it_1 != vecprin.end())
			{
				cout << *vec_it_1++ << endl;
			}
	}


// Used to print all the XY pairs
// to see that we are processing Verticle walls only
void print_xy_pairs( vector<Brush>& vec_layout )
	{
		Plane t1; // temp plane to use pointers

		// Loop through the layout to print the XY pairs
		for (int i = 0; i < int(vec_layout.size()); i++)
			{
				// for each PLANE in the brush
				for (int x = 0; x < int(vec_layout[i].solid_plane.size()); x++)
					{
						t1 = vec_layout[i].solid_plane[x];

						// Only print the Vertical walls, where at least 1 of the XY pairs is the same
						// So the only other change could be in the Z axis
						// xy1 to xy3 does not need to be compared because of the clockwise order
						if ( ( *t1.X_1 == *t1.X_2 && *t1.Y_1 == *t1.Y_2 ) || ( *t1.X_2 == *t1.X_3 && *t1.Y_2 == *t1.Y_3 ) )
							{
								cout << endl << "-------  X and Y pairs  -------" << endl;
								cout << "(" << *t1.X_1 << ", " << *t1.Y_1 << ")" << endl;
								cout << "(" << *t1.X_2 << ", " << *t1.Y_2 << ")" << endl;
								cout << "(" << *t1.X_3 << ", " << *t1.Y_3 << ")" << endl;

								//cout << setprecision(9) << endl << "   SLOPE : " << get_slope_inv(vec_layout[i].solid_plane[x]) << endl;
								//if ( get_slope_inv(vec_layout[i].solid_plane[x]) == -0.00 )
								//	 {
								//		  cout << "   SLOPE : " << get_slope_inv(vec_layout[i].solid_plane[x]) << " is = -0 " << endl;
								//	 }
							}
					}
			}

	}


/***************************************************************
Method: void print_brush_dif( vector<Brush>& vect_layout )

Use: Prints the difference between the input brush, and the desired output brushes

Parameters: Takes a reference to a vector of brushes

Returns: Nothing, simply prints

***************************************************************/


void print_brush_dif( vector<Brush>& vec_layout )
	{
		int x = 0, // Variable integers for subscripts
		    k = 0,
		    f = 0,
		    g = 0;
		cout << endl  << endl << " ------------------------------------------------- ";
		cout << endl << " Difference between Input brush, and desired Ouput " << endl << endl;
		/* // Uncomment parts with outfv to generate lines of code for the Wall Generate functions
		ofstream outfv ("output_planes_test.txt");
		if (outfv.fail())
			{
				cout<<"outfile failed open - prog terminate";
				exit(-2);
			}
		*/
		for (x = 0; x < int(vec_layout.size()); x++)
			{
				cout << endl << "  Solid # " << x+1 << endl;
				for (k = 0; k < 6; k++)
					{
						cout << "     Plane # " << k+1 << " ";
						for (f = 0; f < 3; f++)
							{
								cout << "  (";
								//outfv << endl;
								for (g = 0; g < 3; g++)
								{
									//outfv << " vec_walls["<<x<<"].solid_plane["<<k<<"].plane_verts["<<f<<"]["<<g<<"] = (vec_layout["<<x<<"].solid_plane["<<k<<"].plane_verts["<<f<<"]["<<g<<"] + VALUE);" << endl;
									cout << setw(6);
									if ( x == 0 )
										{
											cout << vec_layout[x].solid_plane[k].plane_verts[f][g]; // Print values in the Brush Class Object
										}
									else
										{
											if ( vec_layout[x].solid_plane[k].plane_verts[f][g] == vec_layout[0].solid_plane[k].plane_verts[f][g])
												{
													cout << "   ";
												}
											else if ( vec_layout[x].solid_plane[k].plane_verts[f][g] > vec_layout[0].solid_plane[k].plane_verts[f][g])
												{
													cout << " + ";
												}
											else if ( vec_layout[x].solid_plane[k].plane_verts[f][g] < vec_layout[0].solid_plane[k].plane_verts[f][g])
												{
													cout << " - ";
												}
											//cout << vec_layout[x].solid_plane[k].plane_verts[f][g] - vec_layout[0].solid_plane[k].plane_verts[f][g]; // Print values in the Brush Class Object
										}
								}
								cout << " ) ";
							}
						cout << endl;
					}
			}
	}





#endif
