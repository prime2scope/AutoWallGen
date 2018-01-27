

/*********************************************************************************
AutoWallGen

November 2016

Programmer: Ben Stockwell


This program reads a VMF file (created in hammer), then attemps to
create walls for every vertical plane it finds in the VMF file.

A new brush will be created for every vertical plane, a default
of 16 units above that plane, and 16 units toward the
inside of the plane that is being used as the seed.

The output is a New VMF file, which contains both the original
layout from the Input, as well as the New walls that were generated


*********************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <tgmath.h>

#include <stdexcept>

#include <string>
#include <cstring>
#include <vector>

#include <iomanip>
#include <algorithm>

#include <cstdlib>
#include <cctype>

#include "BrushClass.h"
#include "BrushUtility.h"


using namespace std;



int main(int argc, char* argv[])
	{
		// Vector of Brushes to store the INPUT layout
		vector<Brush> layout_brushes;

		// Vector of Brushes to store the OUTPUT Walls
		vector<Brush> wall_brushes;

		// Declare vectors to hold the input
		vector<string> line_vec;
		vector<string> line_vec_raw;


		// Sends ALL cout output to the log file
		freopen("DEBUG_AutoWallGen.log","w",stdout); // Uncomment to send all output to file

		// Check if they did a drag and drop, argv 1 should have the filename and path
		if ( argc == 2 )
			{
				// Set the gloabal file name to the dropped file
				INPUT_FILE_NAME = argv[1];
			}

		// Prompt the User for a file name, and what size they want the generated walls
		// Currently only asks for wall sizes, etc
		file_prompt();


		// Get all lines from the VMF File
		// Store a Vector of "cleaned" lines to use for other processing
		// Sotre a Vector of Raw lines to be used with the OUTPUT VMF file
		get_lines(line_vec, line_vec_raw);

		// Output the collected lines for debug purposes
		debug_print(line_vec, line_vec_raw);


		// Loop through the Lines in the Vector
		// Extract the information about each brush and add it to the layout_brushes vector
		build_layout_vec(line_vec, layout_brushes);

		// Shows the difference between the floor layout brush, and the desired layout output
		// Tried to used to create function to generate output
		//print_brush_dif(layout_brushes);

		// Loops through all of the brushes vertical planes, and creates and saves Wall brushes
		// Also hides intersections between walls
		generate_walls(layout_brushes, wall_brushes);

		// Output all the newly Created Walls
		print_brush(wall_brushes);

		// Output a new VMF file containing the INPUT layout, and the newly created walls
		output_vmf(line_vec_raw, wall_brushes);


		// Print to Cerr so it doesn't go to the log file
		cerr << endl << " -------- AutoWallGen Complete -------- " << endl << endl << endl;
		cerr << "  Input File :     " << INPUT_FILE_NAME << endl << endl;
		cerr << " Output File :     " << OUTPUT_FILE_NAME << endl << endl;

		getchar(); // Wait for Enter key press to close program
		getchar(); // added second because other prompt skips first
		return 0;
	}


/***************************************************************
Method: void get_lines( vector<string>& wget )

Use: gets the lines from the input file, cleans them, and puts
	  them in the Vector
	  
	  Also fills the Raw vector used when outputing the new VMF
	  
Parameters: Takes a ref to a vector of strings

Returns: Nothing just fills the vector

***************************************************************/

void get_lines( vector<string>& wget, vector<string>& rawVec )
	{
		// try to Open the file
		ifstream infile;
		infile.open(INPUT_FILE_NAME);

		// Strings for the input
		string gword,
				 clean_word;

		// Check if the file faild to open
		if( infile.fail() )
			{
				cerr << "Error: Input file '" << INPUT_FILE_NAME << "' did NOT open!" << endl;
				cerr << "Error: Input file '" << INPUT_FILE_NAME << "' did NOT open!" << endl;
				cerr << "Error: Input file '" << INPUT_FILE_NAME << "' did NOT open!" << endl;
				cerr << "Program will now terminate. (press enter)" << endl;
				getchar();
				exit(-1);
			}

		// Loop through and fill the vector
		for (int x = 1; getline(infile,gword); x++)
			{
				// Removes certain punctuation and extra spacing
				clean_entry(gword, clean_word);

				// if not blank/empty - add it to the vector
				if( clean_word.length() )
					{
						wget.push_back(clean_word);
					}

				// if the original word is not blank/empty - add it to the vector
				if( gword.length() )
					{
						rawVec.push_back(gword);
					}
			}
	}



/*********************************************************************************
Method: void build_layout_vec( vector<string>& vec_raw, vector<Brush>& vec_layout )

Use: Takes a Vector of Strings, and a Vector of Brush class objects,
	 extracts the meaningful data from the Vector of Strings and stores
	 that data in the Vector of Brush class objects

	 This function reads the data collected from the file, and creates Brush Objects
	 from that data. Then the brushes are stored in a Vector.
	 
Parameters: Takes a Vector of Strings, and a Vector of Brush class objects

Returns: Nothing, simply takes the input and builds it into the
		 Vector of Brushes

*********************************************************************************/

void build_layout_vec( vector<string>& vec_raw, vector<Brush>& vec_layout )
	{
		int i = 0,  // Variable integers for subscripts
		    x = -1,
		    k = -1,
		    v = 0,
		    f = 0,
		    g = 0,
		    d = 0,
		    o = 0,
			 size = 0,
			 bracket_depth = 0;

		// Temp Brush object to push back into vecotr of brushes
		Brush temp_brush;
      Plane t_pl;

		string tword,  // temporary strings for operations
				 //tword2,
			    tword_previous,
			    temp_int ("");

		string temp_int_arr[9];

		//char * temp_int_ar = new char [temp_int.length()+1]; // uncomment for dynamic resizing

		cout << endl << " ------------------------------------------------- ";
		cout << endl << " Converting Input VMF layout to Brush Class Object " << endl;

		// Create the iterator and get the First String
		vector<string>::const_iterator vec_it_1;
		vec_it_1 = vec_raw.begin();

		// Loop through the Strings from the file
		// and set the datamembers of the brush object accordingly
		while (vec_it_1 != vec_raw.end())
			{
				tword = *vec_it_1;

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Check the string and set the Bracket Depth
				if (tword[0] == '{') // Check the string and set the Bracket Depth
					{
						bracket_depth++;
						//cout << endl << "--- DEPTH IS " << bracket_depth << endl; // uncomment to check the bracket depth
						if (bracket_depth == 2) // Reset the Plane Subscript value
							{
								cout << endl << "  solid # " << x+2 << endl; // uncomment to check the bracket depth
							}
						if (bracket_depth == 3) // Reset the Plane Subscript value
							{
								if(k < 5)
									{
										cout << "      plane # " << k+2 << " "; // uncomment to check the bracket depth
									}
							}
					}
				else if (tword[0] == '}') // Check the string and set the Bracket Depth
					{
						bracket_depth--;
						//cout << endl << "--- DEPTH IS " << bracket_depth << endl; // uncomment to check the bracket depth
						if (bracket_depth < 2) // Reset the Plane Subscript value
							{
								k = -1;
							}
					}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Check the string to see if it is the ID value
				if ( tword.find("id") == 0 ) // If it is the ID value, find the digit in the string
					{
						for(i = 0, size = tword.size(); i < size; i++) // Loop through and find the integer for ID
							{
								if(isdigit(tword[i]))
									{
										temp_int = temp_int + tword[i]; // Add all the digits in the string to a temp string
										//size = tword.size();
									}
							}
						// If the braket depth is 2 then, it must be a "solid" type ID - so store it in solid_id
						if (bracket_depth == 2)
							{
								x++; // increment to a new solid subscript

								temp_brush.solid_id = stoi(temp_int, nullptr, 10); // convert number in the string to an integer, and store it in the Brush object
								vec_layout.push_back(temp_brush); // Push the temp brush object INTO the vector, dynamically - automatically makes room - incresses memory size
							}
						else if (bracket_depth == 3) // If the braket depth is 3 then, it must be a "plane" type ID - so store it in the plane_id of the most recent solid
							{
								k++; // increment to a new plane subscript
								// Old code - not dynamic, doesnt adjust size for more or less planes
                        //vec_layout[x].solid_plane[k].plane_id = stoi(temp_int, nullptr, 10); // convert number in the string to an integer, and store it in the Brush object
                        t_pl.plane_id = stoi(temp_int, nullptr, 10); // convert number in the string to an integer, and store it in the Brush object
                        vec_layout[x].solid_plane.push_back(t_pl);
                     }
						temp_int.clear(); // Clear the temporary integer string
					}

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Check the string to see if it has the PLANE verticies value
				if ( tword.find("plane") == 0 ) // If it has Plane in it, loop through and store the vertices in the object
					{
						//cout << tword << endl;

						for(i = 0, size = tword.size(); i < size; i++) // Loop through and find the integer for ID
							{
								if(isdigit(tword[i]) || tword[i] == '-')
									{
										while ( isdigit(tword[i]) || tword[i] == '-' && !isblank(tword[i]) || tword[i] == '.')
											{
												temp_int_arr[v] = temp_int_arr[v] + tword[i]; // Add all the digits in the string to a temp string
												i++;
											}
										v++;
									}
							}

						if (bracket_depth == 3) // If the braket depth is 3 then, it must be a "plane"  - so store it in the vectors for the plane
							{
								for (f = 0, d = 0; f < 3; f++)
									{
										for (g = 0; g < 3; g++)
										{
											//cout << " x : " << x << " k : " << k << " f : " << f << " g : " << g << " d : " << d << endl; // uncomment to check the subscripts for each variable
											//cout << endl << " int_arr = " << temp_int_arr[d];
											vec_layout[x].solid_plane[k].plane_verts[f][g] = round(stod(temp_int_arr[d], nullptr)); // convert number in the string to an integer, and store it in the Brush object
											d++;
										}
									}
								d = 0;
							}
						v = 0; // Reset the V subscript
                	////////////////////////////////////////////////////////////////////////////////////////////////
                	cout << vec_layout[x].solid_plane[k] << endl;

					/* Uncomment to check if the values are being passed to the object, correctly
						for (f = 0; f < 3; f++)
							{
								cout << "(";
								for (g = 0; g < 3; g++)
								{
									//cout << " x : " << x << " k : " << k << " f : " << f << " g : " << g << " d : " << d << endl;
									//cout << "gsub: " << g << " = " << vec_layout[x].solid_plane[k].plane_verts[f][g] << endl; // Print values in the Brush Class Object
									//cout << setw(6);
									//cout << " ";
									cout << vec_layout[x].solid_plane[k].plane_verts[f][g]; // Print values in the Brush Class Object
									if (g < 2)
										{
											cout << " ";
										}
								}
								cout << ") ";
							}
							cout << endl;
					*/
                	////////////////////////////////////////////////////////////////////////////////////////////////
						 // Clear the temporary integer string
						for ( o = 0; o < 9; o++)
						{
							//cout << "tempint_arr sub : " << o << " is " << temp_int_arr[o] << endl; //uncommnet to print the values store in temp int before they get deleted
							temp_int_arr[o].clear();
						}
					}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////



				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Check the string to see if it has the uaxis value
				if ( tword.find("uaxis") == 0 ) // If it has uaxis in it, loop through and store the vertices in the object
					{
						//cout << tword << endl;
							 // set i to the position of the bracket
						for(i = tword.find("["), size = tword.size(); i < size; i++) // Loop through and find the integer for ID
							{
								//
								temp_int_arr[0] = temp_int_arr[0] + tword[i]; // Add everthing from the bracket to the end, to the string
								//v++;
							}

						if (bracket_depth == 3) // If the braket depth is 3 then, it must be a "uaxis"  - so store it in the vectors for the uaxis
							{
								//for (f = 0, d = 0; f < 3; f++)
								//	{
								//		for (g = 0; g < 3; g++)
								//		{
											//cout << " x : " << x << " k : " << k << " f : " << f << " g : " << g << " d : " << d << endl; // uncomment to check the subscripts for each variable
											//cout << endl << " int_arr = " << temp_int_arr[d];
											//vec_layout[x].solid_plane[k].plane_verts[f][g] = round(stod(temp_int_arr[d], nullptr)); // convert number in the string to an integer, and store it in the Brush object
											vec_layout[x].solid_plane[k].u_ax = temp_int_arr[0]; // convert number in the string to an integer, and store it in the Brush object
								//			d++;
								//		}
								//	}
								//d = 0;
							}
						//v = 0; // Reset the V subscript
                	////////////////////////////////////////////////////////////////////////////////////////////////
                	cout << vec_layout[x].solid_plane[k].u_ax << endl;

                	////////////////////////////////////////////////////////////////////////////////////////////////
						 // Clear the temporary integer string
						//for ( o = 0; o < 9; o++)
						//{
							//cout << "tempint_arr sub : " << o << " is " << temp_int_arr[o] << endl; //uncommnet to print the values store in temp int before they get deleted
							temp_int_arr[0].clear();
						//}
					}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Check the string to see if it has the vaxis value
				if ( tword.find("vaxis") == 0 ) // If it has vaxis in it, loop through and store the vertices in the object
					{
						//cout << tword << endl;
							 // set i to the position of the bracket
						for(i = tword.find("["), size = tword.size(); i < size; i++) // Loop through and find the integer for ID
							{
								//
								temp_int_arr[0] = temp_int_arr[0] + tword[i]; // Add everthing from the bracket to the end, to the string
								//v++;
							}

						if (bracket_depth == 3) // If the braket depth is 3 then, it must be a "vaxis"  - so store it in the vectors for the vaxis
							{
								//for (f = 0, d = 0; f < 3; f++)
								//	{
								//		for (g = 0; g < 3; g++)
								//		{
											//cout << " x : " << x << " k : " << k << " f : " << f << " g : " << g << " d : " << d << endl; // uncomment to check the subscripts for each variable
											//cout << endl << " int_arr = " << temp_int_arr[d];
											//vec_layout[x].solid_plane[k].plane_verts[f][g] = round(stod(temp_int_arr[d], nullptr)); // convert number in the string to an integer, and store it in the Brush object
											vec_layout[x].solid_plane[k].v_ax = temp_int_arr[0]; // convert number in the string to an integer, and store it in the Brush object
								//			d++;
								//		}
								//	}
								//d = 0;
							}
						//v = 0; // Reset the V subscript
                	////////////////////////////////////////////////////////////////////////////////////////////////
                	cout << vec_layout[x].solid_plane[k].v_ax << endl;

                	////////////////////////////////////////////////////////////////////////////////////////////////
						 // Clear the temporary integer string
						//for ( o = 0; o < 9; o++)
						//{
							//cout << "tempint_arr sub : " << o << " is " << temp_int_arr[o] << endl; //uncommnet to print the values store in temp int before they get deleted
							temp_int_arr[0].clear();
						//}
					}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////


				*vec_it_1++; //Increment to the next String in the Vector
				tword_previous = tword;  // Set the previous string to current string, then re-loop

			}

	}



/***************************************************************
Method: void generate_walls( vector<Brush>& vec_layout )

Use: Callls all the generate wall functions for a vector of brush objects

Parameters: Takes a reference to a vector of brushes

Returns: Nothing, simply prints

***************************************************************/


void generate_walls( vector<Brush>& vec_layout, vector<Brush>& vec_walls )
	{
		//cout << endl << " ----------------------------- " << endl;
		//cout << endl << "     Generate Walls Called     " << endl;
		//cout << endl << endl << endl;

		Plane t1; // temp plane to use pointers

		// Print the XY pairs to
		print_xy_pairs( vec_layout );

		// Loop through the brushes in the vector
		for (int i = 0; i < int(vec_layout.size()); i++)
			{
				// Loop through the planes of the brush, call the gen_wall function
				// for each VERTICAL face, which will be used as a seed for a new brush
				for (int x = 0; x < int(vec_layout[i].solid_plane.size()); x++)
					{
						t1 = vec_layout[i].solid_plane[x];

						// Only process the Vertical walls, where at least 1 of the XY pairs is the same
						// So the only other change could be in the Z axis
						// xy1 to xy3 does not need to be compared because of the clockwise order
						if ( ( *t1.X_1 == *t1.X_2 && *t1.Y_1 == *t1.Y_2 ) || ( *t1.X_2 == *t1.X_3 && *t1.Y_2 == *t1.Y_3 ) )
							{
								// Use the vertical Plane as a Seed, Create a new brush, and add it to the Vector of Walls
								gen_wall( vec_layout[i].solid_plane[x], vec_walls );
							}
					}
			}


		// Remove the Intersecting walls
		// and generate new disjoint walls as needed
		remove_intersect( vec_walls );

	}


/**********************************************************************************
Method: void gen_wall( const Plane& plane_seed, vector<Brush>& vec_walls )

Use: This function takes a "Vertical Plane" being passed in and generates a new
	  set of 6 planes from it, which make a single "Brush" wall. The new Brush is
	  Then stored in the Vector "vec_walls" for further processing.

Parameters: Takes a reference to a Plane, aka the "seed", and a vector of brushes
			   to store the newly generated Walls

Returns: Nothing, simply prints

**********************************************************************************/



void gen_wall( const Plane& plane_seed, vector<Brush>& vec_walls )
	{
		// Variable integers for subscripts
		int x = 0,
			 i = 0,
		    k = 0,
		    f = 0,
		    g = 0;

		int face_dir = 0; // Used in decisions about which way the face is facing
					         // See face_dir_get() function description

		// Declare Temp brush to push PLANES into
		Brush t_brush;

		// Declare TEMP planes to perform operations on
		Plane t_pl;      // Main Side     this is the main side used to construct ALL other sides and make decisions about which directions things are
		Plane t_pl_tp;   // Top           this is the Z-VERTICAL face - Shud have positive Z vector (its direction should be UP)
		Plane t_pl_op;   // Opposite      this is the OPPOSITE side to the MAIN side - just flip the order of verts and add or subtract wall thickness
		Plane t_pl_dn;   // Down          this is is the side of the brush that faces DOWN - opposite of the TOP brush
		Plane t_pl_s1;   // Side1         this might be facing Y+ or Y- depending on what direction the Main Side is facing
		Plane t_pl_s2;   // Side2         same as Side1 (but facing opposite direction) - this might be facing Y+ or Y- depending on what direction the Main Side is facing


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// WARNING!
		// WARNING!
		// The following code is a bit messy
		// a lot of the decision statements were constructed out of Trial and Error
		//
		// The vertices in the Plane Seed can come in any of 4 different Orders
		// (think of all groups of 3 consecutive corners on a plain)
		// We can assume that at least 1 pair of vertices will align vertically (required on the input file)
		// apart from that, the combination can be skewed in a few ways ( more than 2 unique Z values)
		//
		// These decisions attempt to accomodate for all those possibilities
		//
		// This could be cleaned up a lot, sorry for the mess!
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		t_pl = plane_seed; // Get the plane passed in to the function, which will be used as a seed to generate all other planes

		face_dir = face_dir_get(t_pl); // Initializing FACE_DIR to hold the direction of the face for decisions
		cout << endl << "     fdir set to : " << face_dir;

		cout << endl  << endl  << endl << "         ----------- mSide -------       " << endl << endl;

		cout << "wall b4" << endl;
		cout << t_pl << endl;
		//t_pl.is_mface = 1;

		// Decision about Which verticies to change to make the first face
		if ( *t_pl.X_1 == *t_pl.X_2 && *t_pl.Y_1 == *t_pl.Y_2 )
			{
				if ( *t_pl.Z_1 > *t_pl.Z_2 )
					{
						*t_pl.Z_1 += WALL_HEIGHT;
						*t_pl.Z_2 += FLOOR_THICKNESS;
						*t_pl.Z_3 += FLOOR_THICKNESS;
					}
				else if ( *t_pl.Z_1 < *t_pl.Z_2 )
					{
						*t_pl.Z_1 += FLOOR_THICKNESS;
						*t_pl.Z_2 += WALL_HEIGHT;
						*t_pl.Z_3 += WALL_HEIGHT;
					}
			}
		else if ( *t_pl.X_3 == *t_pl.X_2 && *t_pl.Y_3 == *t_pl.Y_2 )
			{
				if ( *t_pl.Z_3 > *t_pl.Z_2 )
					{
						*t_pl.Z_1 += FLOOR_THICKNESS;
						*t_pl.Z_2 += FLOOR_THICKNESS;
						*t_pl.Z_3 += WALL_HEIGHT;
					}
				else if ( *t_pl.Z_3 < *t_pl.Z_2 )
					{
						*t_pl.Z_1 += WALL_HEIGHT;
						*t_pl.Z_2 += WALL_HEIGHT;
						*t_pl.Z_3 += FLOOR_THICKNESS;
					}
			}
		else
			{
				cerr << endl << "-----------------------------------------------------------------------------------------------------" << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << t_pl << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << t_pl << endl;
				cerr << endl << "-----------------------------------------------------------------------------------------------------" << endl;
			}

		cout << endl << endl;
		cout << "wall after" << endl;
		cout << t_pl << endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Top Side
		cout << endl << "         ----------- top ---------       " << endl << endl;
		t_pl_tp = t_pl;

		//rev_pl(t_pl_tp); // Reverse the order of the verts so the plain faces up clockwise

		cout << "wall b4" << endl;
		cout << t_pl_tp << endl;


		if ( *t_pl_tp.X_1 == *t_pl_tp.X_2 && *t_pl_tp.Y_1 == *t_pl_tp.Y_2 )
			{
				if ( *t_pl_tp.Z_1 > *t_pl_tp.Z_2 )
					{
						cout << endl << "  c1       changing " << *t_pl_tp.X_1 << " + " << WALL_THICKNESS;
						cout << endl << "  c1       changing " << *t_pl_tp.Z_2 << " + " << WALL_HEIGHT;
						cout << endl << "  c1       changing " << *t_pl_tp.Z_3 << " + " << WALL_HEIGHT;
						//*t_pl_tp.X_1 += WALL_THICKNESS; // Replaced by decision statement below
						*t_pl_tp.Z_2 += WALL_HEIGHT;
						*t_pl_tp.Z_3 += WALL_HEIGHT;
						// Below is Decision on WHICH DIRECTION the face is pointing

						if ( face_dir == 1){
							*t_pl_tp.X_1 += WALL_THICKNESS; cout << endl << " c1 ";}
						else if ( face_dir == 2){
							*t_pl_tp.X_1 -= WALL_THICKNESS; cout << endl << " c2 ";}
						else if ( face_dir == 3){
							*t_pl_tp.Y_1 += WALL_THICKNESS; cout << endl << " c3 ";}
						else if ( face_dir == 4){
							*t_pl_tp.Y_1 -= WALL_THICKNESS; cout << endl << " c4 ";}
						else{
							cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}


					}
				else if ( *t_pl_tp.Z_1 < *t_pl_tp.Z_2 )
					{
						cout << endl << "  c2       changing " << *t_pl_tp.X_1 << " + " << WALL_THICKNESS;
						cout << endl << "  c2       changing " << *t_pl_tp.Z_2 << " + " << WALL_HEIGHT;
						cout << endl << "  c2       changing " << *t_pl_tp.Z_3 << " + " << WALL_HEIGHT;
						*t_pl_tp.Z_1 += WALL_HEIGHT;
						//*t_pl_tp.X_1 += WALL_THICKNESS;
						// Below is Decision on WHICH DIRECTION the face is pointing

						if ( face_dir == 1){
							*t_pl_tp.X_1 += WALL_THICKNESS; cout << endl << " c1 ";}
						else if ( face_dir == 2){
							*t_pl_tp.X_1 -= WALL_THICKNESS; cout << endl << " c2 ";}
						else if ( face_dir == 3){
							*t_pl_tp.Y_1 += WALL_THICKNESS; cout << endl << " c3 ";}
						else if ( face_dir == 4){
							*t_pl_tp.Y_1 -= WALL_THICKNESS; cout << endl << " c4 ";}
						else{
							cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}


						rev_pl(t_pl_tp);
					}
			}
		else if ( *t_pl_tp.X_3 == *t_pl_tp.X_2 && *t_pl_tp.Y_3 == *t_pl_tp.Y_2 )
			{
				if ( *t_pl_tp.Z_3 > *t_pl_tp.Z_2 )
					{
						cout << endl << "  c3       changing " << *t_pl_tp.X_1 << " + " << WALL_THICKNESS;
						cout << endl << "  c3       changing " << *t_pl_tp.Z_2 << " + " << WALL_HEIGHT;
						cout << endl << "  c3       changing " << *t_pl_tp.Z_3 << " + " << WALL_HEIGHT;
						*t_pl_tp.Z_1 += WALL_HEIGHT;
						*t_pl_tp.Z_2 += WALL_HEIGHT;
						//*t_pl_tp.X_3 += WALL_THICKNESS;
						// Below is Decision on WHICH DIRECTION the face is pointing
						if ( face_dir == 1){
							*t_pl_tp.X_3 += WALL_THICKNESS; cout << endl << " c1 ";}
						else if ( face_dir == 2){
							*t_pl_tp.X_3 -= WALL_THICKNESS; cout << endl << " c2 ";}
						else if ( face_dir == 3){
							*t_pl_tp.Y_3 += WALL_THICKNESS; cout << endl << " c3 ";}
						else if ( face_dir == 4){
							*t_pl_tp.Y_3 -= WALL_THICKNESS; cout << endl << " c4 ";}
						else{
							cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}


					}
				else if ( *t_pl_tp.Z_3 < *t_pl_tp.Z_2 )
					{
						cout << endl << "  c4       changing " << *t_pl_tp.X_1 << " + " << WALL_THICKNESS;
						cout << endl << "  c4       changing " << *t_pl_tp.Z_2 << " + " << WALL_HEIGHT;
						cout << endl << "  c4       changing " << *t_pl_tp.Z_3 << " + " << WALL_HEIGHT;
						*t_pl_tp.Z_3 += WALL_HEIGHT;
						//*t_pl_tp.X_3 += WALL_THICKNESS;

						// Below is Decision on WHICH DIRECTION the face is pointing
						if ( face_dir == 1){
							*t_pl_tp.X_3 += WALL_THICKNESS; cout << endl << " c1 ";}
						else if ( face_dir == 2){
							*t_pl_tp.X_3 -= WALL_THICKNESS; cout << endl << " c2 ";}
						else if ( face_dir == 3){
							*t_pl_tp.Y_3 += WALL_THICKNESS; cout << endl << " c3 ";}
						else if ( face_dir == 4){
							*t_pl_tp.Y_3 -= WALL_THICKNESS; cout << endl << " c4 ";}
						else{
							cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}



						rev_pl(t_pl_tp);
					}
			}
		else
			{
				cerr << endl << "-----------------------------------------------------------------------------------------------------" << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << t_pl << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << t_pl << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The Corner Verticies of EACH brush MUST be aligned VERTICALLY" << endl;
				cerr << "Brush Verticies = " << t_pl << endl;
				cerr << "ERROR! - COULD NOT FIND VERTICAL PAIR - The top face and the bottom face of each brush must have their corners aligned" << endl;
			}

		cout << endl << "wall after" << endl;
		cout << t_pl_tp << endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Opposite Side
		cout << endl << "         ----------- opst --------       " << endl << endl;

		t_pl_op = t_pl;
		//rev_pl(t_pl_op);

		cout << "wall b4" << endl;
		cout << t_pl_op << endl;

		//t_pl_op.is_mface = 1;

		if ( face_dir == 1){
			*t_pl_op.X_1 += WALL_THICKNESS;
			*t_pl_op.X_2 += WALL_THICKNESS;
			*t_pl_op.X_3 += WALL_THICKNESS;
			rev_pl(t_pl_op); cout << endl << " c1 ";}
		else if ( face_dir == 2){
			*t_pl_op.X_1 -= WALL_THICKNESS;
			*t_pl_op.X_2 -= WALL_THICKNESS;
			*t_pl_op.X_3 -= WALL_THICKNESS;
			rev_pl(t_pl_op); cout << endl << " c2 ";}
		else if ( face_dir == 3){
			*t_pl_op.Y_1 += WALL_THICKNESS;
			*t_pl_op.Y_2 += WALL_THICKNESS;
			*t_pl_op.Y_3 += WALL_THICKNESS;
			rev_pl(t_pl_op); cout << endl << " c3 ";}
		else if ( face_dir == 4){
			*t_pl_op.Y_1 -= WALL_THICKNESS;
			*t_pl_op.Y_2 -= WALL_THICKNESS;
			*t_pl_op.Y_3 -= WALL_THICKNESS;
			rev_pl(t_pl_op); cout << endl << " c4 ";}
		else{
			cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}



		cout << "wall after" << endl;
		cout << t_pl_op << endl;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Down Side
		cout << endl << "         ----------- down --------       " << endl << endl;

		t_pl_dn = t_pl_tp;
		rev_pl(t_pl_dn);

		cout << "wall b4" << endl;
		cout << t_pl_dn << endl;


		t_pl_dn.plane_verts[0][2] -= WALL_HEIGHT;
		t_pl_dn.plane_verts[1][2] -= WALL_HEIGHT;
		t_pl_dn.plane_verts[2][2] -= WALL_HEIGHT;

		cout << "wall after" << endl;
		cout << t_pl_dn << endl;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Side1
		cout << endl << "         ----------- side1 -------       " << endl << endl;

		t_pl_s1 = t_pl_tp;


		cout << "wall b4" << endl;
		cout << t_pl_s1 << endl;
		//cout << distance_get( *t_pl_s1.X_1, *t_pl_s1.X_2, *t_pl_s1.Y_1, *t_pl_s1.Y_2 ) << " distance is " << endl;


		if ( distance_get( *t_pl_s1.X_1, *t_pl_s1.X_2, *t_pl_s1.Y_1, *t_pl_s1.Y_2 ) == WALL_THICKNESS )
			{
				*t_pl_s1.X_3 = *t_pl_s1.X_2;   //t_pl_s1.plane_verts[1][0];
				*t_pl_s1.Y_3 = *t_pl_s1.Y_2;   //t_pl_s1.plane_verts[1][1];
				*t_pl_s1.Z_3 = *t_pl_s1.Z_2;   //t_pl_s1.plane_verts[1][2];
				*t_pl_s1.Z_3 -= WALL_HEIGHT;
				rev_pl(t_pl_s1);
			}
		else if ( distance_get( *t_pl_s1.X_3, *t_pl_s1.X_2, *t_pl_s1.Y_3, *t_pl_s1.Y_2 ) == WALL_THICKNESS )
			{
				*t_pl_s1.X_1 = *t_pl_s1.X_2;   //t_pl_s1.plane_verts[1][0];
				*t_pl_s1.Y_1 = *t_pl_s1.Y_2;   //t_pl_s1.plane_verts[1][1];
				*t_pl_s1.Z_1 = *t_pl_s1.Z_2;   //t_pl_s1.plane_verts[1][2];
				*t_pl_s1.Z_1 -= WALL_HEIGHT;
				rev_pl(t_pl_s1);
			}

		/*
		for (i = 0; i < 3; i++)
			{
				if ( t_pl_s1.plane_verts[i][1] != *t_pl_s1.Y_2 || t_pl_s1.plane_verts[i][2] != *t_pl_s1.Z_2 && ( face_dir == 1 || face_dir == 2 ) )
					{                            //t_pl_s1.plane_verts[1][1]                      t_pl_s1.plane_verts[1][2]
						//cout << endl << "  c0        sub =  " << i << " + " << endl;
						t_pl_s1.plane_verts[i][0] = *t_pl_s1.X_2;   //t_pl_s1.plane_verts[1][0];
						t_pl_s1.plane_verts[i][1] = *t_pl_s1.Y_2;   //t_pl_s1.plane_verts[1][1];
						t_pl_s1.plane_verts[i][2] = *t_pl_s1.Z_2;   //t_pl_s1.plane_verts[1][2];
						t_pl_s1.plane_verts[i][2] -= WALL_HEIGHT;
						i = 4;
					}
				else if ( t_pl_s1.plane_verts[i][0] != *t_pl_s1.X_2 || t_pl_s1.plane_verts[i][2] != *t_pl_s1.Z_2 && ( face_dir == 3 || face_dir == 4 ) )
					{                            //t_pl_s1.plane_verts[1][1]                      t_pl_s1.plane_verts[1][2]
						//cout << endl << "  c0        sub =  " << i << " + " << endl;
						t_pl_s1.plane_verts[i][0] =  *t_pl_s1.X_2;   //t_pl_s1.plane_verts[1][0];
						t_pl_s1.plane_verts[i][1] =  *t_pl_s1.Y_2;   //t_pl_s1.plane_verts[1][1];
						t_pl_s1.plane_verts[i][2] =  *t_pl_s1.Z_2;   //t_pl_s1.plane_verts[1][2];
						t_pl_s1.plane_verts[i][2] -= WALL_HEIGHT;
						i = 4;
					}
			}

		rev_pl(t_pl_s1);
		*/


		cout << "wall after" << endl;
		cout << t_pl_s1 << endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Side2
		cout << endl << "         ----------- side2 -------       " << endl << endl;

		t_pl_s2 = t_pl_tp; // Copy the TOP face into SIDE 2


		cout << "wall b4" << endl;
		cout << t_pl_s2 << endl; // Display SIDE 2 plane vertices

		for (i = 0; i < 3; i++) // Loop through all the verticies
			{                   // if the current vertex Y & Z value == the CENTER vertex Y & Z value
				if ( t_pl_s2.plane_verts[i][1] == *t_pl_s2.Y_2 && t_pl_s2.plane_verts[i][2] == *t_pl_s2.Z_2 && ( face_dir == 1 || face_dir == 2 ) )
					{ // if its X Y and Z vert 1
						if (i == 0) // If the subscript was 0, then we REPLACE THE VERTEX with the X, Y and Z from Vert Sub 2
							{        // Assuming the center vertex will ALWAYS be part of the matching pair
								//cout << endl << "  c1        changing sub " << i << " + " << endl;
								*t_pl_s2.X_2 = *t_pl_s2.X_3;
								*t_pl_s2.Y_2 = *t_pl_s2.Y_3;
								*t_pl_s2.Z_2 = *t_pl_s2.Z_3;
								*t_pl_s2.Z_2 -= WALL_HEIGHT;
								*t_pl_s2.X_1 = *t_pl_s2.X_3;
								*t_pl_s2.Y_1 = *t_pl_s2.Y_3;
								*t_pl_s2.Z_1 = *t_pl_s2.Z_3;
								*t_pl_s2.Z_1 -= WALL_HEIGHT;
								//t_pl_s2.plane_verts[0][0] += WALL_THICKNESS;

								if ( face_dir == 1){
									*t_pl_s2.X_1 += WALL_THICKNESS; cout << endl << " c1 ";}
								else if ( face_dir == 2){
									*t_pl_s2.X_1 -= WALL_THICKNESS; cout << endl << " c2 ";}
								else if ( face_dir == 3){
									*t_pl_s2.Y_1 += WALL_THICKNESS; cout << endl << " c3 ";}
								else if ( face_dir == 4){
									*t_pl_s2.Y_1 -= WALL_THICKNESS; cout << endl << " c4 ";}
								else{
									cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}

								i = 4;
							}
						if (i == 2) // If the subscript was 2, then we REPLACE THE VERTEX with the X, Y and Z from Vert Sub 0
							{
								//cout << endl << "  c2        changing sub " << i << " + " << endl;
								*t_pl_s2.X_2 = *t_pl_s2.X_1;
								*t_pl_s2.Y_2 = *t_pl_s2.Y_1;
								*t_pl_s2.Z_2 = *t_pl_s2.Z_1;
								*t_pl_s2.Z_2 -= WALL_HEIGHT;
								*t_pl_s2.X_3 = *t_pl_s2.X_1;
								*t_pl_s2.Y_3 = *t_pl_s2.Y_1;
								*t_pl_s2.Z_3 = *t_pl_s2.Z_1;
								*t_pl_s2.Z_3 -= WALL_HEIGHT;
								//t_pl_s2.plane_verts[2][0] += WALL_THICKNESS;

								if ( face_dir == 1){
									*t_pl_s2.X_3 += WALL_THICKNESS; cout << endl << " c1 ";}
								else if ( face_dir == 2){
									*t_pl_s2.X_3 -= WALL_THICKNESS; cout << endl << " c2 ";}
								else if ( face_dir == 3){
									*t_pl_s2.Y_3 += WALL_THICKNESS; cout << endl << " c3 ";}
								else if ( face_dir == 4){
									*t_pl_s2.Y_3 -= WALL_THICKNESS; cout << endl << " c4 ";}
								else{
									cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}

								i = 4;
							}
					}
				if ( t_pl_s2.plane_verts[i][0] == *t_pl_s2.X_2 && t_pl_s2.plane_verts[i][2] == *t_pl_s2.Z_2 && ( face_dir == 3 || face_dir == 4 ) )
					{ // if its X Y and Z vert 1
						if (i == 0) // If the subscript was 0, then we REPLACE THE VERTEX with the X, Y and Z from Vert Sub 2
							{        // Assuming the center vertex will ALWAYS be part of the matching pair
								//cout << endl << "  c1        changing sub " << i << " + " << endl;
								*t_pl_s2.X_2 = *t_pl_s2.X_3;
								*t_pl_s2.Y_2 = *t_pl_s2.Y_3;
								*t_pl_s2.Z_2 = *t_pl_s2.Z_3;
								*t_pl_s2.Z_2 -= WALL_HEIGHT;
								*t_pl_s2.X_1 = *t_pl_s2.X_3;
								*t_pl_s2.Y_1 = *t_pl_s2.Y_3;
								*t_pl_s2.Z_1 = *t_pl_s2.Z_3;
								*t_pl_s2.Z_1 -= WALL_HEIGHT;
								//t_pl_s2.plane_verts[0][0] += WALL_THICKNESS;

								if ( face_dir == 1){
									*t_pl_s2.X_1 += WALL_THICKNESS; cout << endl << " c1 ";}
								else if ( face_dir == 2){
									*t_pl_s2.X_1 -= WALL_THICKNESS; cout << endl << " c2 ";}
								else if ( face_dir == 3){
									*t_pl_s2.Y_1 += WALL_THICKNESS; cout << endl << " c3 ";}
								else if ( face_dir == 4){
									*t_pl_s2.Y_1 -= WALL_THICKNESS; cout << endl << " c4 ";}
								else{
									cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}

								i = 4;
							}
						if (i == 2) // If the subscript was 2, then we REPLACE THE VERTEX with the X, Y and Z from Vert Sub 0
							{
								//cout << endl << "  c2        changing sub " << i << " + " << endl;
								*t_pl_s2.X_2 = *t_pl_s2.X_1;
								*t_pl_s2.Y_2 = *t_pl_s2.Y_1;
								*t_pl_s2.Z_2 = *t_pl_s2.Z_1;
								*t_pl_s2.Z_2 -= WALL_HEIGHT;
								*t_pl_s2.X_3 = *t_pl_s2.X_1;
								*t_pl_s2.Y_3 = *t_pl_s2.Y_1;
								*t_pl_s2.Z_3 = *t_pl_s2.Z_1;
								*t_pl_s2.Z_3 -= WALL_HEIGHT;
								//t_pl_s2.plane_verts[2][0] += WALL_THICKNESS;

								if ( face_dir == 1){
									*t_pl_s2.X_3 += WALL_THICKNESS; cout << endl << " c1 ";}
								else if ( face_dir == 2){
									*t_pl_s2.X_3 -= WALL_THICKNESS; cout << endl << " c2 ";}
								else if ( face_dir == 3){
									*t_pl_s2.Y_3 += WALL_THICKNESS; cout << endl << " c3 ";}
								else if ( face_dir == 4){
									*t_pl_s2.Y_3 -= WALL_THICKNESS; cout << endl << " c4 ";}
								else{
									cout << endl << " ERROR - face_dir int - failed to initilize to meaningful value" << endl;}

								i = 4;
							}
					}
			}

		rev_pl(t_pl_s2);

		cout << "wall after" << endl;
		cout << t_pl_s2 << endl;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Set the Values for the Uaxis
		// Controls how textures are displayed

		t_pl_tp.u_ax = "[1 0 0 0] 0.25";      // If they are parallel to the brush face
		t_pl_dn.u_ax = "[1 0 0 0] 0.25";      // the map will get Errors when you try to compile

		t_pl.u_ax = plane_seed.u_ax;         // These should work since they come from the original brush
      t_pl_op.u_ax = plane_seed.u_ax;

		t_pl_s1.u_ax = "[.696969 .69 0 0] 0.25"; // .696969 is just an arbitrary value, set hoping it wont be parallel with a brush face
		t_pl_s2.u_ax = "[.696969 .69 0 0] 0.25";

		/////////////////////////////////////////////////////////////////////////////////////

		// Set the Values for the Uaxis
		// Controls how textures are displayed
		t_pl_tp.v_ax = "[0 -1 0 0] 0.25";      // If they are parallel to the brush face
		t_pl_dn.v_ax = "[0 -1 0 0] 0.25";      // the map will get Errors when you try to compile

		t_pl.v_ax = plane_seed.v_ax;      // These should work since they come from the original brush
      t_pl_op.v_ax = plane_seed.v_ax;

		t_pl_s1.v_ax = "[0 0 -.6969 0] 0.25"; // -1.1 is just an arbitrary value, set hoping it wont be parallel with a brush face
		t_pl_s2.v_ax = "[0 0 -.6969 0] 0.25"; // -1 should work for a verticle face... maybe


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Set the Main Face
      t_pl.is_mface = 1;
      t_pl_op.is_mface = 1;

		// Set Brush ID
		t_brush.solid_id = BRUSH_ID++;
		BRUSH_ID += 3;

		// Add the Brush to the Vector
		vec_walls.push_back(t_brush);

		//vec_walls[0].solid_id = 600;

		// Adjust Plane IDs
		t_pl_tp.plane_id = PLANE_ID++;
		PLANE_ID += 3;
		t_pl_dn.plane_id = PLANE_ID++;
		PLANE_ID += 3;
		t_pl.plane_id = PLANE_ID++;
		PLANE_ID += 3;
		t_pl_op.plane_id = PLANE_ID++;
		PLANE_ID += 3;
		t_pl_s1.plane_id = PLANE_ID++;
		PLANE_ID += 3;
		t_pl_s2.plane_id = PLANE_ID++;
		PLANE_ID += 3;

		// Add each Planes to the Brush that was just added to the Vector
		vec_walls.back().solid_plane.push_back(t_pl_tp);
		vec_walls.back().solid_plane.push_back(t_pl_dn);
		vec_walls.back().solid_plane.push_back(t_pl);
		vec_walls.back().solid_plane.push_back(t_pl_op);
		vec_walls.back().solid_plane.push_back(t_pl_s1);
		vec_walls.back().solid_plane.push_back(t_pl_s2);
		// Save the Seed that was Used in the brush
		vec_walls.back().br_seed = plane_seed;

		cout << endl << "=== DONE BRUSH ===" << endl;

		//cout << "m_face set to  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ - " << t_pl.is_mface << endl;

		/*
		cout << endl << "-- FACE DIR main = " << face_dir_get(t_pl);
		cout << endl << "-- FACE DIR opps = " << face_dir_get(t_pl_op);
		cout << endl << "-- FACE DIR ypos = " << face_dir_get(t_pl_s1);
		cout << endl << "-- FACE DIR yneg = " << face_dir_get(t_pl_s2);
		cout << endl;
		*/

	}



/*********************************************************************
Method: void remove_intersect( vector<Brush>& vec_walls )

Use: This functions attemps to Remove and correct any intersections
	  it finds between brushs. It compares the "Main Face" of each
	  brush to the "Main Face" of every other brush. (the main face
	  is set when the brush is made in gen_wall)

	  If the brush is found to Intersect, it's "visgroup" ID is set
	  this value will be used by the hammer editor to show/hide
	  any of the brushes that were effected by this function

	  First, it checks if the planes are a "perfect match", if they
	  line up perfectly with each other - the planes are hidden
	  and no more action is taken. (the person making the layout
	  is responsible for intentionally creating intersections)

	  Next, If the planes line up, but are not exactly the same size,
	  then replacement brushes must be created only at the positions
	  where the faces do not meet up. This is done by comparing the
	  Min and Max X or Y values for each plane.

	  (This may not work for all cases where brushes meet diagonally)
	  (eg. if the brushes meet diagonally, but one is longer, a new
	   shorter brush will not be made, probably)
	  
Parameters: Takes a reference to a vector of strings

Returns: Nothing, simply prints

*********************************************************************/


void remove_intersect( vector<Brush>& vec_walls )
	{
		Plane t1; // first temp plane to compare to second
		Plane t2; // second temp plane to compare to first
		double temp_val;

		//Brush t_brush; // Declare Temp brush to push PLANES into
		//Plane tp_up; // temp up face
		//Plane tp_dn; // temp down face
		//Plane tp_sd1; // temp side face
		//Plane tp_sd2; // temp side face
		//Plane tp_sd3; // temp side face
		//Plane tp_sd4; // temp side face

		Plane plane_seed;
		Plane t2_seed;
		Plane t1_seed;

		cout << endl << " ----------------------------- " << endl << endl;

		// Main loop gets a "Main Face" from a wall, then compares it to all other Main Faces
		// if the faces line up in some way, appropriate action is taken
		for (int i = 0; i < int(vec_walls.size()); i++)
			{
				for (int x = 0; x < 6; x++)
					{
						if ( vec_walls[i].vis_group_id != 802)
							{
								t1 = vec_walls[i].solid_plane[x]; // Initialize TEMP Plane to perform comparisons on, to see if
								if ( t1.is_mface == 1 ) // If the Brush Face was Specified as a "Main Face" aka not a top bottom or side 1 or 2
									{                    // Then we compare it against ALL other "Main Faces"

										////////////////////////////////////////////////////////////////////////////////////////////////////////////////
										// Inner Loop //////////////////////////////////////////////////////////////////////////////////////////////////
										////////////////////////////////////////////////////////////////////////////////////////////////////////////////
										for (int i2 = 0; i2 < int(vec_walls.size()); i2++)
											{
												for (int x2 = 0; x2 < 6; x2++)
													{
														//
														if ( vec_walls[i2].vis_group_id != 802 )
															{
																t2 = vec_walls[i2].solid_plane[x2]; // Initialize TEMP Plane to perform comparisons on, to see if
																if ( t2.plane_id != t1.plane_id ) // if the plane ID is NOT the same as plane 1
																	{
																		// if the plane is a "Main Face" - set when it was created
																		if ( t2.is_mface == 1 )
																			{
																				//cout << i << " " << i2 << endl << "t1 " << t1 << endl << "t2 " << t2 << endl << endl;
																				//cout << "face align " << face_align(t1,t2) << " |  uniq t1 " << uniq_Z_vals(t1) << " |  uniq t2 " << uniq_Z_vals(t2) << endl;
																				//cout << endl;

																				// If the faces have the SAME LENGTH
																				if ( get_mf_length(t1) == get_mf_length(t2) )
																					{
																						//cout << t1 << endl << t2 << endl;

																						// if the LARGEST and SMALLEST XYZ values for each face are the same, then Hide both faces
																						// (only need largest or smallest Z)
																						if ( get_largest_x(t1)  == get_largest_x(t2)  &&
																							  get_smallest_x(t1) == get_smallest_x(t2) &&
																							  get_largest_y(t1)  == get_largest_y(t2)  &&
																							  get_smallest_y(t1) == get_smallest_y(t2) &&
																							  get_largest_z(t1)  == get_largest_z(t2)  &&
																							  get_smallest_z(t1) == get_smallest_z(t2) )
																							{
																								//cout << "EQUATION for t2 - PASS :   " << *t2.Y_1 << " = " << get_slope_inv( t2 ) << " * " << *t2.X_1 << " + " << get_b_value( t2 ) << endl;
																								//cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								// Hide BOTH faces if they are the same face
																								vec_walls[i2].vis_group_id = 802;
																								vec_walls[i].vis_group_id = 802;
																							}
																					}
																				else if ( get_largest_y(t1)  == get_largest_y(t2)  &&
																							 get_smallest_y(t1) == get_smallest_y(t2) &&
																							 get_largest_z(t1)  == get_largest_z(t2)  &&
																							 get_smallest_z(t1) == get_smallest_z(t2) &&
																							 (face_align(t1,t2)) && uniq_Z_vals(t1) == uniq_Z_vals(t2) )
																					{
																						//temp_val = (( (get_slope_inv(t2)) * *t2.X_1 ) + get_b_value(t2) );

																						// t1 is the SMALLER plane
																						// t2 is the BIGGER plane
																						if ( get_largest_x(t1) < get_largest_x(t2) && get_smallest_x(t1) > get_smallest_x(t2) )
																							{
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest X from T2 should = Smallest X from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_L(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest X from T2 should = Smallest X from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_R(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																						// if the Planes are the same size on the X axis
																						else if ( get_largest_x(t1) == get_largest_x(t2) && get_smallest_x(t1) > get_smallest_x(t2) ) // t1 is the SMALLER plane
																							{                                                                                          // t2 is the BIGGER plane
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest X from T2 should = Smallest X from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_L(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																						else if ( get_largest_x(t1) < get_largest_x(t2) && get_smallest_x(t1) == get_smallest_x(t2) ) // t1 is the SMALLER plane
																							{                                                                                          // t2 is the BIGGER plane
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest X from T2 should = Smallest X from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_R(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																					}
																				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																				else if ( get_largest_x(t1)  == get_largest_x(t2)  &&
																						    get_smallest_x(t1) == get_smallest_x(t2) &&
																							 get_largest_z(t1)  == get_largest_z(t2)  &&
																							 get_smallest_z(t1) == get_smallest_z(t2) &&
																							 (face_align(t1,t2)) && uniq_Z_vals(t1) == uniq_Z_vals(t2) )
																					{
																						//temp_val = (( (get_slope_inv(t2)) * *t2.X_1 ) + get_b_value(t2) );
																						if ( get_largest_y(t1) < get_largest_y(t2) && get_smallest_y(t1) > get_smallest_y(t2) ) // t1 is the SMALLER plane
																							{                                                                                    // t2 is the BIGGER plane
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest Y from T2 should = Smallest Y from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_yL(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest Y from T2 should = Smallest Y from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_yR(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																						else if ( get_largest_y(t1) == get_largest_y(t2) && get_smallest_y(t1) > get_smallest_y(t2) ) // t1 is the SMALLER plane
																							{                                                                                          // t2 is the BIGGER plane
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest Y from T2 should = Smallest Y from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_yL(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																						else if ( get_largest_y(t1) < get_largest_y(t2) && get_smallest_y(t1) == get_smallest_y(t2) ) // t1 is the SMALLER plane
																							{                                                                                          // t2 is the BIGGER plane
																								cout << "t1 = " << t1 << endl << "t2 = " << t2 << endl << endl;

																								vec_walls[i].vis_group_id = 802;    // Set the brush Visgroup to intersecting walls group
																								vec_walls[i2].vis_group_id = 802;   // Set the brush Visgroup to intersecting walls group

																								t2_seed = vec_walls[i2].br_seed;    // initialize the seeds to pass to the prep_seed function
																								t1_seed = vec_walls[i].br_seed;     // initialize the seeds to pass to the prep_seed function

																								plane_seed.clear();
																								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
																								// the Largest Y from T2 should = Smallest Y from T1
																								cout << "b4 prep   " << plane_seed << endl;
																								prep_seed_yR(t1_seed, t2_seed, plane_seed);       // call prep_seed to setup the seed
																								cout << "aftr prep " << plane_seed << endl;

																								gen_wall( plane_seed, vec_walls );

																								vec_walls.back().vis_group_id = 803;
                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
																							}
																					}
																			}
																	}
															}
													}
										////////////////////////////////////////////////////////////////////////////////////////////////////////////////
										// End Inner ///////////////////////////////////////////////////////////////////////////////////////////////////
										////////////////////////////////////////////////////////////////////////////////////////////////////////////////

										 //cout << " B value is " << get_b_value( t1 ) << endl;
										 //cout << "EQUATION :                 Y = " << get_slope_inv( t1 ) << " * X + " << get_b_value( t1 ) << endl;
											}
									}
							}
					}
			}
	}




/***************************************************************
Method: void print_vector_nofilt(const vector<string>& vecprin )

Use: Outputs the new VMF file using the original file as the base,
	  loops through the original file to find the correct place
	  to add the new walls.

***************************************************************/

void output_vmf(  const vector<string>& vecprin, vector<Brush>& vec_walls )
	{
		 // Uncomment parts with outfv to generate lines of code for the Wall Generate functions
		//ofstream outfv ("output_vec_walls_1.vmf");

		// Get the Incremented File name, and open the file
		OUTPUT_FILE_NAME = IncFilename(OUTPUT_FILE_STR);
		ofstream outfv (OUTPUT_FILE_NAME);

		// Check if the output file fails to open
		if (outfv.fail())
			{
				cerr << "Error: Output file failed open";
				cerr << "Program will now terminate. (press enter)" << endl;
				getchar();
				exit(-2);
			}

		int bracket_depth = 0;
		int brkD_prev1 = 0;
		int brkD_prev2 = 0;

		string tword; // temporary strings for operations
		string t_prv_1; // temporary strings for operations
		string t_prv_2; // temporary strings for operations
		vector<string>::const_iterator vec_it_1;
		vec_it_1 = vecprin.begin();

		// This loops through each line of the original File input to find the correct place to add the new walls
		while (vec_it_1 != vecprin.end())
			{
					t_prv_2 = t_prv_1;
					t_prv_1 = tword;
					tword = *vec_it_1;

					if ( tword.find("{") < 300 ) // Check the string and set the Bracket Depth
						{
							brkD_prev2 = brkD_prev1;
							brkD_prev1 = bracket_depth;
							bracket_depth++;
							//cout << endl << "--- DEPTH IS " << bracket_depth << endl; // uncomment to check the bracket depth
						}
					else if ( tword.find("}") < 300 ) // Check the string and set the Bracket Depth
						{
							brkD_prev2 = brkD_prev1;
							brkD_prev1 = bracket_depth;
							bracket_depth--;
							//cout << endl << "--- DEPTH IS " << bracket_depth << endl; // uncomment to check the bracket depth
						}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					if ( bracket_depth == 0 && t_prv_2.find("visgroups") == 0 )
						{
							outfv << "	visgroup" << endl;                      // Create a NEW Visgroup for the output walls
							outfv << "	{" << endl;
							outfv << "		\"name\" \"Output_Walls\"" << endl;
							outfv << "		\"visgroupid\" \"800\"" << endl;     // This ASSUMES that visgroupid 800 will not be taken
							outfv << "		\"color\" \"182 111 188\"" << endl;
								outfv << "		visgroup" << endl;                      // Create a NEW Visgroup for the input walls
								outfv << "		{" << endl;
								outfv << "			\"name\" \"Intersect_Walls_New\"" << endl;
								outfv << "			\"visgroupid\" \"803\"" << endl;     // This ASSUMES that visgroupid 802 will not be taken
								outfv << "			\"color\" \"131 176 97\"" << endl;
								outfv << "		}" << endl;
							outfv << "	}" << endl;

							outfv << "	visgroup" << endl;                      // Create a NEW Visgroup for the input walls
							outfv << "	{" << endl;
							outfv << "		\"name\" \"Input_Layout\"" << endl;
							outfv << "		\"visgroupid\" \"801\"" << endl;     // This ASSUMES that visgroupid 801 will not be taken
							outfv << "		\"color\" \"131 176 97\"" << endl;
							outfv << "	}" << endl;

							outfv << "	visgroup" << endl;                      // Create a NEW Visgroup for the input walls
							outfv << "	{" << endl;
							outfv << "		\"name\" \"Intersect_Walls\"" << endl;
							outfv << "		\"visgroupid\" \"802\"" << endl;     // This ASSUMES that visgroupid 802 will not be taken
							outfv << "		\"color\" \"131 176 97\"" << endl;
							outfv << "	}" << endl;

						}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					// Insert NEW WALL SOLIDS at the END of the last solid in the file
					if ( brkD_prev2 == 2 && brkD_prev1 == 1 && bracket_depth == 0 && tword.find("}") < 100)
						{
							//cout << endl << " brkD_prev2 " << brkD_prev2 << endl;
							//cout << " brkD_prev1 " << brkD_prev1 << endl;
							//cout << " bracket_depth " << bracket_depth << endl;

							for( int i = 0; i <int(vec_walls.size()); i++)
								{
									outfv << "	solid" << endl;
									outfv << "	{" << endl;
									outfv << "		\"id\" \"" << vec_walls[i].solid_id << "\"" << endl;

									for ( int x = 0; x < int(vec_walls[i].solid_plane.size()); x++ )
										{
											outfv << "		side" << endl;
											outfv << "		{" << endl;
											outfv << "			\"id\" \"" << vec_walls[i].solid_plane[x].plane_id << "\"" << endl;
											outfv << "			\"plane\" \"" << vec_walls[i].solid_plane[x] << "\"" << endl;
											outfv << "			\"material\" \"COLORS/RED\"" << endl;

											//outfv << "			\"uaxis\" \"[0 1 0 0] 0.25\"" << endl;          // Earlier Version
											//outfv << "			\"vaxis\" \"[0 0 -1 0] 0.25\"" << endl;         //

											outfv << "			\"uaxis\" \"" << vec_walls[i].solid_plane[x].u_ax << "\"" << endl;
											outfv << "			\"vaxis\" \"" << vec_walls[i].solid_plane[x].v_ax << "\"" << endl;

											outfv << "			\"rotation\" \"0\"" << endl;
											outfv << "			\"lightmapscale\" \"16\"" << endl;
											outfv << "			\"smoothing_groups\" \"0\"" << endl;
											outfv << "		}" << endl;
										}
									outfv << "		editor" << endl;
									outfv << "		{" << endl;
									outfv << "			\"color\" \"0 174 119\"" << endl;
									//outfv << "			\"visgroupid\" \"800\"" << endl;
									outfv << "			\"visgroupid\" \"" << vec_walls[i].vis_group_id << "\"" << endl;
									if ( vec_walls[i].vis_group_id == 803 ) // if the brush is in the Intersect_Walls_New Visgroup, Add it to the Output_walls Visgroup
										{
											outfv << "			\"visgroupid\" \"800\"" << endl;
										}
									if ( vec_walls[i].vis_group_id == 800 || vec_walls[i].vis_group_id == 803 ) // if it is in Either of these visgroups
										{                                                                        // set it to be Visible when the file is opened
											outfv << "			\"visgroupshown\" \"1\"" << endl;
										}
									else if ( vec_walls[i].vis_group_id == 802 )                 // If it is in this visgroup
										{                                                         // set it to be hidden when the file is opened
											outfv << "			\"visgroupshown\" \"0\"" << endl;
										}
									outfv << "			\"visgroupautoshown\" \"1\"" << endl; // I dont know what this does in a VMF file
									outfv << "		}" << endl;
									outfv << "	}" << endl;
								}
						}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					outfv << *vec_it_1++ << endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}

	}


	