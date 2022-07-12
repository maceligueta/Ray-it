#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cassert>





namespace Dxf
{





// fwd:
using Coord = double;
using Color = int;
class Drawing;





/** The "default" color for an object is to use the layer's color. */
static const Color COLOR_BYLAYER = -1;

/** The "default" color for objects within a block definition is to use the block's color. */
static const Color COLOR_BYBLOCK = 0;

/** The default style, if not given to a constructor. */
static const int STYLE_DEFAULT = 0;

/** The default width, if not given to a constructor. */
static const Coord WIDTH_DEFAULT = 1;

/** The default Z coord, if not given to a constructor. */
static const Coord Z_DEFAULT = 0;





/** Text align flags */
enum Alignment
{
	// Horizontal alignment:
	alLeft     = 0,
	alHCenter  = 1,
	alRight    = 2,

	// Vertical alignment:
	alTop      = 0x300,
	alVCenter  = 0x200,
	alBottom   = 0x100,
	alBaseline = 0,
};





/** Type of the individual dxf object (subclass).
Used instead of RTTI for speed. */
enum ObjectType
{
	otError,
	otLine,
	otPolyline,
	otLWPolyline,
	otPolygon,
	otSolid,
	otCircle,
	otSimpleEllipse,
	otHatch,
	otArc,
	otText,
	otBlock,
	otVertex,
	otPoint,
};





/** The coords of a single point in the 3D space. */
class Coords
{
public:
	Coord mX;
	Coord mY;
	Coord mZ;

	/** Creates a new isntance with only 2D coords (Z unused). */
	Coords(Coord && aX, Coord && aY):
		mX(std::move(aX)),
		mY(std::move(aY)),
		mZ(Z_DEFAULT)
	{
	}

	/** Creates a new isntance with 3D coords. */
	Coords(Coord && aX, Coord && aY, Coord && aZ):
		mX(std::move(aX)),
		mY(std::move(aY)),
		mZ(std::move(aZ))
	{
	}

	/** Creates a new isntance with only 2D coords (Z unused). */
	Coords(const Coord & aX, const Coord & aY):
		mX(aX),
		mY(aY),
		mZ(Z_DEFAULT)
	{
	}

	/** Creates a new isntance with 3D coords. */
	Coords(const Coord & aX, const Coord & aY, const Coord & aZ):
		mX(aX),
		mY(aY),
		mZ(aZ)
	{
	}

	/** Creates a copy of the other instance. */
	Coords(const Coords & aOther) = default;

	/** Move-constructs from the other instance. */
	Coords(Coords && aOther) = default;

	/** Default copy-assignment operator. */
	Coords & operator = (const Coords & aOther) = default;

	/** Default move-assignment operator. */
	Coords & operator = (Coords && aOther) = default;

	/** Returns whether the other coords are not equal to this. */
	bool operator != (const Coords & aOther) const
	{
		return (mX != aOther.mX) || (mY != aOther.mY) || (mZ != aOther.mZ);
	}

	/** Returns whether the other coords are equal to this. */
	bool operator == (const Coords & aOther) const
	{
		return (mX == aOther.mX) && (mY == aOther.mY) && (mZ == aOther.mZ);
	}

	Coords operator + (const Coords & aOther) const
	{
		return {mX + aOther.mX, mY + aOther.mY, mZ + aOther.mZ};
	}

	Coords operator - (const Coords & aOther) const
	{
		return {mX - aOther.mX, mY - aOther.mY, mZ - aOther.mZ};
	}
};





/** A simple axis-aligned bounding box that can also be empty.
Provides operations to modify and query the box. */
class Extent
{
	bool mIsEmpty;
	Coords mMin;
	Coords mMax;


public:

	/** The default constructor, creates an empty instance. */
	Extent():
		mIsEmpty(true),
		mMin(0, 0),
		mMax(0, 0)
	{
	}

	/** Constructs a new instance from the two extreme points. */
	Extent(Coords && aMin, Coords && aMax):
		mIsEmpty(false),
		mMin(std::move(aMin)),
		mMax(std::move(aMax))
	{
		assert(aMin.mX <= aMax.mX);
		assert(aMin.mY <= aMax.mY);
		assert(aMin.mZ <= aMax.mZ);
	}

	/** Constructs a new instance from the two extreme points. */
	Extent(const Coords & aMin, const Coords & aMax):
		mIsEmpty(false),
		mMin(aMin),
		mMax(aMax)
	{
		assert(aMin.mX <= aMax.mX);
		assert(aMin.mY <= aMax.mY);
		assert(aMin.mZ <= aMax.mZ);
	}

	/** Constructs a new instance from a single point. */
	Extent(const Coords & aPt):
		mIsEmpty(false),
		mMin(aPt),
		mMax(aPt)
	{
	}

	/** Copy-constructor */
	Extent(const Extent & aOther) = default;

	/** Move-constructor */
	Extent(Extent && aOther) = default;

	Extent & operator = (const Extent & aOther) = default;

	// TODO: Other constructors

	bool isEmpty() const { return mIsEmpty; }
	const Coords & minCoord() const { return mMin; }
	const Coords & maxCoord() const { return mMax; }

	/** Expands the Extent so that it contains aPt as well. */
	inline void expandTo(const Coords & aPt)
	{
		if (mIsEmpty)
		{
			mMin = aPt;
			mMax = aPt;
			mIsEmpty = false;
			return;
		}

		if (aPt.mX < mMin.mX)
		{
			mMin.mX = aPt.mX;
		}
		else if (aPt.mX > mMax.mX)
		{
			mMax.mX = aPt.mX;
		}
		if (aPt.mY < mMin.mY)
		{
			mMin.mY = aPt.mY;
		}
		else if (aPt.mY > mMax.mY)
		{
			mMax.mY = aPt.mY;
		}
		if (aPt.mZ < mMin.mZ)
		{
			mMin.mZ = aPt.mZ;
		}
		else if (aPt.mZ > mMax.mZ)
		{
			mMax.mZ = aPt.mZ;
		}
	}

	/** Expands the Extent so that it contains the specified extent as well. */
	inline void expandTo(const Extent & aOther)
	{
		if (mIsEmpty)
		{
			*this = aOther;
			return;
		}
		if (aOther.mIsEmpty)
		{
			return;
		}

		mMin.mX = std::min(mMin.mX, aOther.mMin.mX);
		mMin.mY = std::min(mMin.mY, aOther.mMin.mY);
		mMin.mZ = std::min(mMin.mZ, aOther.mMin.mZ);
		mMax.mX = std::max(mMax.mX, aOther.mMax.mX);
		mMax.mY = std::max(mMax.mY, aOther.mMax.mY);
		mMax.mZ = std::max(mMax.mZ, aOther.mMax.mZ);
	}

	// TODO: Other modifiers

	// TODO: Queries
};





class Attrib
{
public:
	std::string mName;
	std::string mValue;
	Coord mFontSize;

	Attrib(const std::string & aName, const std::string & aValue, Coord && aFontSize = 1);
};





class ExtendedData
{
public:
	std::string mString;
	std::string mApplicationName;
	bool mControlString;
	std::string mLayerName;
	std::string mBinaryData;
	int mDatabaseHandle;
	Coords mPos;
	Coords mWorldSpacePosition;
	Coords mWorldSpaceDisplacement;
	Coords mWorldDirection;
	Coord mReal;
	Coord mDistance;
	Coord mScaleFactor;
	unsigned short int mInteger;
	int mLong;

	ExtendedData();
};





/** The base type for all objects within the Drawing / Layer / BlockDefinition. */
class Primitive
{
public:
	ObjectType mObjectType;
	Coords mPos;
	Color mColor;
	Coord mWidth;
	std::vector<Attrib> mAttribs;
	// std::unique_ptr<ExtendedData> mExtended;

	/** Creates a new empty instance of the specified type.
	Used mainly by the parser. */
	Primitive(ObjectType aObjectType);

	/** Creates a new instance of the specified type and the specified coords and color. */
	Primitive(ObjectType aObjectType, Coords && aPos, Color aColor = COLOR_BYLAYER, Coord aWidth = WIDTH_DEFAULT);

	/** Deletes the instance. */
	virtual ~Primitive() {}

	/** Returns the axis-aligned bounding box of this primitive.
	Descendants are expected to provide real implementations returning the true extent of the object. */
	virtual Extent extent() const;
};

using PrimitivePtr = std::shared_ptr<Primitive>;
using PrimitivePtrs = std::vector<PrimitivePtr>;





/** Representation of the VERTEX dxf data. */
class Vertex:
	public Primitive
{
	using Super = Primitive;


public:

	Coord mBulge;


	/** Creates a new empty instance.
	Used mainly by the parser. */
	Vertex():
		Super(otVertex)
	{
	}

	/** Creates a new instance at the specified coords. */
	explicit Vertex(Coords && aPos):
		Super(otVertex, std::move(aPos))
	{
	}
};





/** Representation of the POINT dxf data. */
class Point:
	public Primitive
{
	using Super = Primitive;


public:

	Point(Coords && aPos = {0, 0}):
		Primitive(otPoint, std::move(aPos))
	{
	}
};





/** Representation of an axis-aligned 2D ellipse. */
class AxisAligned2DEllipse:
	public Primitive
{
	using Super = Primitive;


public:

	Coord mDiameterX, mDiameterY;


	AxisAligned2DEllipse(Coords && aPos, Coord aDiameterX, Coord aDiameterY);

	// Primitive overrides:
	virtual Extent extent() const override;
};





/** Representation of the LINE dxf data. */
class Line:
	public Primitive
{
	using Super = Primitive;


public:

	/** The coords of the second point. */
	Coords mPos2;

	int mStyle;


	/** Creates a new empty instance.
	Used mainly by the parser. */
	Line():
		Super(otLine),
		mPos2(0, 0)
	{
	}

	Line(Coords && aPos1, Coords && aPos2, Color aColor = COLOR_BYLAYER, int aStyle = 0, Coord && aWidth = 0);

	// Primitive overrides:
	virtual Extent extent() const override;
};





/** Representation of a simple 2D circle.
The Primitive's mPos specifies the circle's center. */
class Circle:
	public Primitive
{
	using Super = Primitive;


public:

	/** The radius of the circle. */
	Coord mRadius;


	/** Creates a new empty instance, with zero radius.
	Used mainly bby the parser. */
	Circle();

	/** Creates a new initialized instance. */
	Circle(Coords && aCenterPos, Coord && aRadius, Color aColor = COLOR_BYLAYER);

	// Primitive overrides:
	virtual Extent extent() const override;
};





/** Representation of an arc.
The Primitive's mPos specifies the arc's center. */
class Arc:
	public Primitive
{
	using Super = Primitive;


public:

	Coord mRadius;
	Coord mStartAngle;  ///< In degrees
	Coord mEndAngle;    ///< In degrees


	/** Creates a new empty instance, with zero radius and angles.
	Used mainly by the parser. */
	Arc();

	/** Creates a new initialized instance. */
	Arc(Coords && aCenterPos, Coord && aRadius, Coord && aStartAngle, Coord  && aEndAngle, Color aColor = COLOR_BYLAYER);

	// Primitive overrides:
	virtual Extent extent() const override;
};





class Text:
	public Primitive
{
	using Super = Primitive;


public:

	/** The raw text stored in the DXF.
	May contain formatting instructions. */
	std::string mRawText;

	/** The angle of the text, in degrees. */
	Coord mAngle;

	Coord mSize;

	/** The oblique (italics) angle, in degrees. */
	Coord mOblique;

	int mAlignment;

	Coord mThickness;


	/** Creates a new empty instance.
	Used mainly by the parser. */
	Text():
		Super(otText),
		mAngle(0),
		mSize(1),
		mOblique(0)
	{
	}

	/** Creates an instance holding a copy of the specified text. */
	Text(Coords && aPos, const std::string & aRawText, Coord && aSize, Coord && aAngle = 0, Color aColor = COLOR_BYLAYER, int aAlignment = alHCenter);

	/** Creates an instance by move-constructing the specified text. */
	Text(Coords && aPos, std::string && aRawText, Coord && aSize, Coord && aAngle = 0, Color aColor = COLOR_BYLAYER, int aAlignment = alHCenter);

	// Primitive overrides:
	virtual Extent extent() const override;
};





/** Contains the definition of a single block ("insert"). */
class BlockDefinition
{
public:
	PrimitivePtrs mObjects;
	std::string mName;

	/** Creates a new instance by copying the name. */
	explicit BlockDefinition(const std::string & Name);

	/** Creates a new instance by move-constructing the name. */
	explicit BlockDefinition(std::string && aName);
} ;

using BlockDefinitions = std::vector<std::shared_ptr<BlockDefinition>>;





/** Representation of the block ("insert") that makes a "stamp" from a predefined list of elements. */
class Block:
	public Primitive
{
	using Super = Primitive;


public:

	std::shared_ptr<BlockDefinition> mDefinition;
	Coord mAngle;
	Coords mScale;

	Block(Coords && aPos, std::shared_ptr<BlockDefinition> && aDefinition, Coord && aAngle, Coord aScaleMaster);

	// Primitive overrides:
	virtual Extent extent() const override;
};





/** Common ancestor for objects that hold a variable number of vertices in them: Polyline, LWPolyline and Polygon.
Handles the storage of the vertices.
All vertices are stored in mVertices; the coords stored in the base Primitive's mPos are not used. */
class MultiVertex:
	public Primitive
{
	using Super = Primitive;


public:

	std::vector<Vertex> mVertices;


	/** Creates an empty instance. */
	MultiVertex(ObjectType aObjectType, Color aColor = COLOR_BYLAYER, Coord aWidth = 1):
		Super(aObjectType, {0, 0}, aColor, aWidth)
	{
	}

	/** Adds a vertex with the specified coords.
	Returns the added vertex. */
	void addVertex(Coords && aCoords);

	/** Adds the specified vertex. */
	void addVertex(Vertex && aVertex);

	/** Removes any vertices that have the same coords as their direct predecessor. */
	void removeDuplicateVertices();

	// Primitive overrides:
	virtual Extent extent() const override;
};





enum PolylineFlags
{
	plfNone = 0,
	plfClosedPolyline = 1,
	plfCurveFitVertices = 2,
	plfSplineFitVertices = 4,
	plf3DPolyline = 8,
	plf3DPolygonMesh = 16,
	plfPolygonMeshClosed = 32,
	plfPolyfaceMesh = 64,
	plfGeneratedLinePattern = 128
} ;





/** Representation of the POLYLINE dxf data. */
class Polyline:
	public MultiVertex
{
	using Super = MultiVertex;


public:

	/** Bitwise combination of PolylineFlags. */
	int mFlags;


	/** Creates an empty polyline. */
	explicit Polyline(Color aColor = COLOR_BYLAYER, Coord aWidth = 0):
		Super(otPolyline, aColor, aWidth),
		mFlags(plfNone)
	{
	}
};





/** Representation of the LWPOLYLINE (lightweight polyline) dxf data. */
class LWPolyline:
	public MultiVertex
{
	using Super = MultiVertex;


public:

	/** Bitwise combination of PolylineFlags. */
	int mFlags;


	/** Creates an empty instance. */
	LWPolyline(Color aColor = COLOR_BYLAYER, Coord aWidth = 0):
		Super(otLWPolyline, aColor, aWidth),
		mFlags(0)
	{
	}
};





/** Representation of a 2D polygon dxf data. */
class Polygon:
	public MultiVertex
{
	using Super = MultiVertex;


public:

	/** Creates an empty instance */
	Polygon(Color aColor = COLOR_BYLAYER, Coord aWidth = 0):
		Super(otPolygon, aColor, aWidth)
	{
	}
};





/** Represents a SOLID dxf object (filled 2D polygon), specified using 3 or 4 points. */
class Solid:
	public Primitive
{
	using Super = Primitive;


public:

	/** Coords of the second point. */
	Coords mPos2;

	/** Coords of the third point. */
	Coords mPos3;

	/** Coords of the fourth point.
	Only valid if mIsTetra is true. */
	Coords mPos4;

	/** True if using 4 points, false if only using 3 points. */
	bool mIsTetra;


	/** Creates a new 3-point solid. */
	Solid(Coords && aPos1, Coords && aPos2, Coords && aPos3, Color aColor = COLOR_BYLAYER);

	/** Creates a new 4-point solid. */
	Solid(Coords && aPos1, Coords && aPos2, Coords && aPos3, Coords && aPos4, Color aColor = COLOR_BYLAYER);

	/** Returns true if using 4 points, false if only using 3 points. */
	inline bool isTetra() const { return mIsTetra; }

	// Primitive overrides:
	virtual Extent extent() const override;
} ;





/** Represents a 3D polygon, using 4 points. */
class TetraFace:
	public Primitive
{
	using Super = Primitive;


public:

	/** Coords of the second point. */
	Coords mPos2;

	/** Coords of the third point. */
	Coords mPos3;

	/** Coords of the fourth point. */
	Coords mPos4;

	/** Specifies which edges should be drawn (???) */
	int mVisibleEdges;


	/** Creates a new 4-point solid. */
	TetraFace(Coords && aPos1, Coords && aPos2, Coords && aPos3, Coords && aPos4, Color aColor = COLOR_BYLAYER);
};





/** Represents an entire layer of a drawing.
Contains (and owns) the objects that belong to this layer. */
class Layer
{
protected:
	/** The objects contained within the layer. */
	PrimitivePtrs mObjects;

	/** The parent drawing.
	Used mainly for block definitions. */
	Drawing & mParentDrawing;

	/** The default color to use (when object's color is BY_LAYER)*/
	Color mDefaultColor;

	/** The layer's name. */
	std::string mName;

	/** The (cached) extent of all the objects in this layer.
	The extent is updated upon adding an object and explicitly via updateExtent(). */
	Extent mExtent;


public:

	/** Creates a new empty layer of the specified name. */
	Layer(Drawing & aParentDrawing, const std::string & aName);

	/** Removes all objects from this layer. */
	void clear();


	/** Recalculates the mExtent from all object of this layer.
	This is only needed when an object is modified *after* being added via addObject(). */
	void updateExtent();

	/** Removes the object at the specified index and returns the pointer to it.
	Ignored if the index is invalid. */
	PrimitivePtr removeObjByIndex(size_t aIndex);

	/** Removes the specified object.
	Ignored if the object is not present in the layer. */
	void removeObj(Primitive * aObject);

	/** Adds the specified object to the layer.
	If the object is modified after this call, you should call updateExtent(). */
	void addObject(PrimitivePtr && aObject);

	/** Adds the specified object to the layer.
	If the object is modified after this call, you should call updateExtent(). */
	void addObject(const PrimitivePtr & aObject);

	const PrimitivePtrs & objects() const { return mObjects; }
	const Drawing & parentDrawing() const { return mParentDrawing; }
	Color defaultColor() const { return mDefaultColor; }
	const std::string & name() const { return mName; }

	/** Returns the currently cached extent of the layer. */
	const Extent & extent() const { return mExtent; }

	void setDefaultColor(Color aColor) { mDefaultColor = aColor; }
	void setName(const std::string & aName) { mName = aName; }
} ;





/** Represents the entire Dxf drawing.
Contains the layers for the drawing, and block definitions*/
class Drawing
{
public:

	using LayerAlreadyExists = std::runtime_error;
	using NoSuchLayer = std::runtime_error;
	using BlockDefinitionAlreadyExists = std::runtime_error;

	/** All layers within the drawing.
	The order of the layers is important. */
	std::vector<std::shared_ptr<Layer>> mLayers;

	/** All the BlockDefinitions within the drawing. */
	std::map<std::string, std::shared_ptr<BlockDefinition>> mBlockDefinitions;


	/** Creates a new empty instance. */
	Drawing()
	{
	}

	/** Removes all layers and block definitions. */
	void clear();

	/** Adds a new empty layer of the specified name.
	If there already is a layer of the name, throws a LayerAlreadyExists exception. */
	std::shared_ptr<Layer> addLayer(const std::string & aName);

	/** Returns the specified layer.
	If there's no such layer, returns nullptr. */
	std::shared_ptr<Layer> layerByName(const std::string & aName) const;

	/** Adds a new BlockDefinition.
	If there already is a BlockDefinition of the specified name, throws a BlockDefinitionAlreadyExists exception. */
	void addBlockDefinition(std::string && aName, std::shared_ptr<BlockDefinition> aBlockDefinition);

	/** Returns the specified BlockDefinition.
	If there's no such BlockDefinition, returns nullptr. */
	std::shared_ptr<BlockDefinition> blockDefinitionByName(const std::string & aName) const;

	const std::vector<std::shared_ptr<Layer>> & layers() const { return mLayers; }
} ;





/** The DXF color values, as 0x00BBGGRR constants, indexed by the color index from the file format. */
extern const uint32_t gColors[];

/** Number of entries in the gColors array. */
extern const size_t gNumColors;





#if 0
CONSTEXPR COLORREF DXFColorToRGB(Color iColor) CONST_FUNCTION;
std::string DXFFixLayerName(const std::string & iName);
void    DXFFixLayerName(std::string & Name);
std::string DXFFixBlockName(const std::string & iName);
void    DXFFixBlockName(std::string & Name);
void DXFFixLayerNames(Layers & iLayers);
void DXFFixBlockNames(BlockDefinitions & iBlocks);

double  DXFGetRelativeTextWidth(const std::string & iText); // gets string width relative to its height (that is, multiply by font-height to get the actual width). Only an approximation!

void CreateArrow(Layer * iParent, Coord ix1, Coord iy1, Coord ix2, Coord iy2, Coord iHeadLength, Color iColor = DXF_COLOR_BYLAYER, int iStyle = 0, Coord iWidth = 0);
void CreateTextArrow(Layer * iParent, double & textX, double & textY, double & textW, double & textA, Coord ix1, Coord iy1, Coord idx, Coord idy, bool fromEnd, double textMoveBy, Coord iHeadLength, const std::string & iCaption, Coord iSize, Color iColor = DXF_COLOR_BYLAYER, int iStyle = 0, Coord iWidth = 0);

// Color combo support:
void      DXF_FillColorCombo(HWND iCombo, Color iCurrentColor, LPCTSTR iTxtByLayer, LPCTSTR iTextByBlock, LPCTSTR iTextGeneric);
Color DXF_GetColorComboVal(HWND iCombo);
void      DXF_DrawColorComboItem(HWND iCombo, LPDRAWITEMSTRUCT dis);  // processed from inside WM_DRAWITEM
#endif





}  // namespace Dxf
