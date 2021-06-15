/********
*
*  scene_io.h
*
*  Description:
*    Contains data structures for reading and writing scene descriptions.
*    The scene descriptions are originally from Inventor applications
*    (a toolkit provided by SGI), however rather than forcing you to know
*    how to write an Inventor application, we convert them for you
*    to the much simpler data structures below.
*
*    Do not modify the data structures in this file.
*    You should define your own structures, and write a procedure that
*    that initializes them by walking through the structures defined here.
*
*  Authors: Brian L. Curless, Eric Veach
*  Date of Version: January 22, 1995
*
*********/

#ifndef _SCENE_IO_
#define _SCENE_IO_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <stdio.h>


    /* Definitions of fundamental types a la Heckbert	*/
    /*  in Glassner''s _Introduction_to_Ray_Tracing_	*/

typedef float Flt;	    /* Inventor requires float not double!!	*/
			    /*   Could be coerced to double with effort */
typedef Flt Vec[3];	    /* Vector in 3-space (nx, ny, nz)		*/
typedef Vec Point;	    /* Point in 3-space (x, y, z)		*/


typedef struct SphereIO {
    Point origin;   /* Origin of the sphere/ellipsoid		*/
    Flt radius;	    /* Half the sum of the axis lengths		*/
    
    Vec xaxis;	    /* Direction of the x axis			*/
    Flt xlength;    /* Length of the x axis			*/
    Vec yaxis;	    /* Direction of the y axis			*/
    Flt ylength;    /* Length of the y axis			*/
    Vec zaxis;	    /* Direction of the z axis			*/
    Flt zlength;    /* Length of the z axis			*/
} SphereIO;

    /* Definition of a polygon */

typedef struct PolygonIO {
    long numVertices;		/* Number of vertices			*/
    struct VertexIO *vert;	/* Vertex array				*/
} PolygonIO;


    /* Definition of a polygonal set.					*/
    /* We support 3 types:						*/
    /*									*/
    /*   Triangle mesh --						*/
    /*     Consists only of triangles.  There is not necessarily any	*/
    /*	   structure to the triangles; although the triangles will	*/
    /*     usually be from a single modelling primitive.		*/
    /*									*/
    /*   Face set --							*/
    /*	   A set of polygons with any number of sides.  They will	*/
    /*	   always have per-face normals and per-object materials.	*/
    /*									*/
    /*   Quad mesh --							*/
    /*	   All "polygons" have four sides, but are NOT necessarily	*/
    /*     planar, i.e. the 4 vertices of each quad may not lie in	*/
    /*     the same plane.  						*/

enum PolySetType {POLYSET_TRI_MESH, POLYSET_FACE_SET, POLYSET_QUAD_MESH};


    /* Objects (polygonal sets in particular) may have normals */
    /*   at every vertex for smooth shading of meshed objects  */
    /*   or at every face for sharp-edged shading as in boxes  */

enum NormType {PER_VERTEX_NORMAL, PER_FACE_NORMAL};


    /* We support two kinds of material bindings:			*/
    /*	PER_OBJECT    one material for the whole object			*/
    /*  PER_VERTEX    a material is defined at each vertex, and you	*/
    /*			must interpolate across each face.		*/
    /*									*/
    /* PER_VERTEX obviously only applies to polygonal objects.		*/

enum MaterialBinding {PER_OBJECT_MATERIAL, PER_VERTEX_MATERIAL};


typedef struct PolySetIO {
    enum PolySetType		type;
    enum NormType		normType;
    enum MaterialBinding	materialBinding;
    int /* boolean */		hasTextureCoords;
    
    /* Sometimes it may be useful to know when the polygonal set is
     * organized into a rectangular pattern.  If the primitives are
     * are grouped into rows of equal length, "rowSize" gives the
     * number of primitives in each row, otherwise "rowSize" is 0.
     * The number of rows is (numPolys / rowSize), an integer.
     */
    long rowSize;     	    /* Number of primitives per row		*/
    long numPolys;	    /* Number of polygons		   	*/
    struct PolygonIO *poly; /* Polygonal array			   	*/
} PolySetIO;


#define COMPOSER_EXPORT_REQUESTED	0
#define COMPOSER_EXPORT_SENDING		1
#define COMPOSER_EXPORT_DONE		2

#define COMPOSER_DEFAULT_EXPORT_NAME	"composer.out"


#endif


