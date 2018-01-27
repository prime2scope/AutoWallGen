
#ifndef BRUSH_CLASS_H
#define BRUSH_CLASS_H

/***************************************************************

Programmer: Ben Stockwell


This file defines the Brush and Plane class
(Corners is unused)

The Brush and Plane Classes are intended to represent the 
structure found in a VMF file, where planes are defined by 
3 verts with various other attributes

And a Brush is defined by some set of Planes as well as other
attributes

These classes and their methods are defined here


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

// Global Variables
static int FLOOR_THICKNESS = 16;
static int WALL_THICKNESS = 16;
static int WALL_HEIGHT = 128;

static int BRUSH_ID = 1000;
static int PLANE_ID = 1100;


// This class is not currently used in any meaningful way
// may have had a purpose at some point, but not currently
class Corners
	{
	//friend ostream & operator<<(ostream & lhs, const Plane& rhs);

	public:

		// Overloaded assignment Operator for planes
		Corners& operator=(const Corners& other);

		// Datamembers for the Corners Class

		double crn_vrts[4][2] = { {2.421,2.421}, {2.421,2.421}, {2.421,2.421}, {2.421,2.421}}; // 2D Array, 4 sets of 2 XY pairs
		                                                                                       // initialized to ARBITRARY VALUE for comparision
																															// passed in planes will never have a decimal vert caue they get rounded
		// Pointers for individual XYZ values for easier use/readability in code
		double* v1_X = &crn_vrts[0][0];
		double* v2_X = &crn_vrts[1][0];
		double* v3_X = &crn_vrts[2][0];
		double* v4_X = &crn_vrts[3][0];

		double* v1_Y = &crn_vrts[0][1];
		double* v2_Y = &crn_vrts[1][1];
		double* v3_Y = &crn_vrts[2][1];
		double* v4_Y = &crn_vrts[3][1];

	};


// Used to represent a single Plane of some brush
// Planes are represented by 3 vertices, the order should be clockwise when looking from the outside
class Plane
	{
	//friend ostream & operator<<(ostream & lhs, const Plane& rhs);

	public:

		// Clears all data members of the plane to 0
		void clear();

		// String to hold Uaxis and Vaxis for Textures
		string u_ax = "[0 .69 0 0] 0.25";
		string v_ax = "[0 0 -.69 0] 0.25";

		// Overloaded assignment Operator for planes
		Plane& operator=(const Plane& other);
		bool operator==(const Plane& rhs) const;

		// Datamembers for the Brush Class
		int plane_id = 0; // Stores the plane ID found in the VMF file

		int is_mface = 0; // Stores the plane ID found in the VMF file

		double plane_verts[3][3] = { {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}}; // 2D Array, 3 sets of 3 XYZ cordinates that together define a single plane

		// Pointers for individual XYZ values for easier use/readability in code
		double* X_1 = &plane_verts[0][0];
		double* X_2 = &plane_verts[1][0];
		double* X_3 = &plane_verts[2][0];

		double* Y_1 = &plane_verts[0][1];
		double* Y_2 = &plane_verts[1][1];
		double* Y_3 = &plane_verts[2][1];

		double* Z_1 = &plane_verts[0][2];
		double* Z_2 = &plane_verts[1][2];
		double* Z_3 = &plane_verts[2][2];


		// Corners of the plane
		//Corners CRNR;

	};


// Used to represent a single "solid" object, a 3 dimensional geometric shape as it is represented in the VMF file
// Brush is a term used by valve: https://developer.valvesoftware.com/wiki/Brush
class Brush
	{
	public:

		Brush& operator=(const Brush& other);

		// Datamembers for the Brush Class

		// Holds the IDs which will be used in the VMF file
		int solid_id = 0;
		int vis_group_id = 800;

		// A vector of Planes, a brush can have a variable number of Planes
		vector<Plane> solid_plane;

		Plane br_seed;

		// Corners of the plane, not used currently
		Corners CRNR;

	};



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods

/***************************************************************
Method: Brush& Brush::operator=(const Brush& other)

Use: Overloads assignment operator for use with Brush objects

Parameters:

Returns: Nothing, simply prints

***************************************************************/

Brush& Brush::operator=(const Brush& other)
	{
		if (this == &other) // if objects address is the same, returns
			{
				return *this;
			}
		Plane t1;

		//this->solid_id = other.solid_id; // copy data member
		this->solid_id = BRUSH_ID; // copy data member
		BRUSH_ID += 3;

		//this->vis_group_id = other.vis_group_id; // copy data member
		this->vis_group_id = 800;

		//for (int i = 0; i < int(other.solid_plane.size()); i++)
		for (int i = 0; i < 6; i++)
			{
				t1 = other.solid_plane[i];
				this->solid_plane.push_back(t1);
			}

		return *this;
	}


/***************************************************************
Method: Plane& Plane::operator=(const Plane& other)

Use: Overloads assignment operator for use with Plane objects

***************************************************************/



Plane& Plane::operator=(const Plane& other)
	{
		if (this == &other) // if objects address is the same, returns
			{
				return *this;
			}

		this->u_ax = other.u_ax; // copy data member
		this->v_ax = other.v_ax; // copy data member

		this->plane_id = other.plane_id; // copy data member
		this->is_mface = other.is_mface; // copy data member

		for (int i = 0; i < 3; i++)
			{
				for (int x = 0; x < 3; x++)
					{
						this->plane_verts[i][x] = other.plane_verts[i][x];
					}
			}

		return *this;
	}


/***************************************************************
Method: void Plane::clear()

Use: clears all data members of the plane to 0

Parameters: none

Returns: Nothing, simply clears the plane

***************************************************************/


void Plane::clear()
	{
		//if (this == &other) // if objects address is the same, returns
		//	{
		//		return *this;
		//	}

		plane_id = 0; // copy data member
		is_mface = 0; // copy data member

		for (int i = 0; i < 3; i++)
			{
				for (int x = 0; x < 3; x++)
					{
						plane_verts[i][x] = 0.0;
					}
			}

	}


/***************************************************************
Method: Plane& Plane::operator=(const Plane& other)

Use: Overloads assignment operator for use with Plane objects

***************************************************************/


bool Plane::operator==(const Plane& other) const
	{
		if (this == &other) // if objects address is the same, returns
			{
				return 1;
			}

		if ( plane_id == other.plane_id && is_mface == other.is_mface )
			{
				for (int i = 0; i < 3; i++)
					{
						for (int x = 0; x < 3; x++)
							{
								if (plane_verts[i][x] != other.plane_verts[i][x])
									{
										return 0;
									}
							}
					}
				return 1;
			}
		else
			{
				return 0;
			}
	}


/***************************************************************
Method: Corner& Corner::operator=(const Corner& other)

Use: Overloads assignment operator for use with Corner objects

***************************************************************/


Corners& Corners::operator=(const Corners& other)
	{
		if (this == &other) // if objects address is the same, returns
			{
				return *this;
			}

		for (int i = 0; i < 4; i++)
			{
				for (int x = 0; x < 2; x++)
					{
						this->crn_vrts[i][x] = other.crn_vrts[i][x];
					}
			}

		return *this;
	}



/***********************************************************************
FRIEND
FUNCTION:	ostream& operator<<(ostream& lhs, const Plane& rhs)

USE: Overloads the stream insertion operator for use with the
	 Plane class, used to "cout" a Plane objects

PARAMETERS: The first is a reference to an ostream object, representing
		    the left operand of the stream insertion expression.
		    The second is a reference to a constant Plane object,
			representing the right operand of the expression.

RETURNS: A reference to an ostream object (i.e., the first parameter)

NOTE: Prints the elements of the Plane

***********************************************************************/


ostream& operator<<(ostream& lhs, const Plane& rhs)
	{
		size_t current, i; // declare temp variables

		for (int i = 0; i < 3; i++)
			{
				lhs << "(";
				for (int x = 0; x < 3; x++)
					{
						lhs << rhs.plane_verts[i][x];
						if (x < 2)
							{
								lhs << " ";
							}
					}
				lhs << ")";
				if (i < 2)
					{
						lhs << " ";
					}
			}
	/*
		for (current = rhs.qFront, i = 0; i < rhs.qSize; ++i)
			{
				// Print queue element at subscript i
				lhs << rhs.qAr[current] << " ";
				// Increment i, wrapping around to front of queue array if necessary
				current = (current + 1) % rhs.qCap;
			}
	*/
		return lhs; // return the ostream object
	}


/***********************************************************************
FRIEND
FUNCTION:	ostream& operator<<(ostream& lhs, const Corner& rhs)

USE: Overloads the stream insertion operator for use with the
	 Corner class, used to "cout" a Corner objects

PARAMETERS: The first is a reference to an ostream object, representing
		    the left operand of the stream insertion expression.
		    The second is a reference to a constant Corner object,
			representing the right operand of the expression.

RETURNS: A reference to an ostream object (i.e., the first parameter)

NOTE: Prints the elements of the Corner

***********************************************************************/


ostream& operator<<(ostream& lhs, const Corners& rhs)
	{
		size_t current, i; // declare temp variables
		
		for (int i = 0; i < 4; i++)
			{
				//lhs << "(";
				for (int x = 0; x < 2; x++)
					{
						lhs << rhs.crn_vrts[i][x];
						if (x < 1)
							{
								lhs << ", ";
							}
					}
				//lhs << ")";
				if (i < 3)
					{
						//lhs << " ";
						lhs << endl;
					}
			}

		return lhs; // return the ostream object
	}


#endif