// TODO




#if 0
void Layer::SaveToStream(TMStream * iStream, bool BlocksSection) const
{
	SF_THISFN_STACK(Name.c_str());

	for (Primitives::const_iterator itr = Objects.begin(); itr != Objects.end(); ++itr)
	{
		if ((*itr)->ParentBlockDef == NULL)
		{
			(*itr)->SaveToStream(iStream, BlocksSection);
			if ((*itr)->Extended != NULL)
			{
				(*itr)->Extended->SaveToStream(iStream);
			}
		}
	}  // for i - Object[]
}





void Drawing::SaveToStream(TMStream * iStream)
{
	SF_THISFN_STACK("");

	double MinX = 1e16;
	double MinY = MinX;
	double MinZ = MinX;
	double MaxX = -1e16;
	double MaxY = MaxX;
	double MaxZ = MaxX;

	// fix names of layers/blocks, only upper letters, numbers and underscope are allowed up to 31 characters under DXF version 2000
	DXFFixLayerNames(Layers);
	DXFFixBlockNames(BlockDefinitions);

	{
		SF_THISFN_STACK("Find all used block definitions");
		// find all used block definitions:
		for (BlockDefinitions::const_iterator itr = BlockDefinitions.begin(); itr != BlockDefinitions.end(); ++itr)
		{
			(*itr)->IsUsed = false;
		}
		for (Layers::const_iterator itr = Layers.begin(); itr != Layers.end(); ++itr)
		{
			Primitives & Objects = (*itr)->Objects;
			for (Primitives::const_iterator itrO = Objects.begin(); itrO != Objects.end(); ++itrO)
			{
				Primitive * const primitive = *itrO;
				if (primitive->ObjType == otBlock)
				{
					static_cast<Block *>(primitive)->Definition->IsUsed = true;
				}
			}  // for itrO - Objects[]
		}  // for itr - Layers[]
	}

	{
		SF_THISFN_STACK("Get drawing extents");
		// find one object and set extents to it:
		for (Layers::const_iterator itr = Layers.begin(); itr != Layers.end(); ++itr)
		{
			const Primitives & Objects = (*itr)->Objects;
			if (!Objects.empty())
			{
				MinX = Objects[0u]->x;
				MaxX = MinX;
				MinY = Objects[0u]->y;
				MaxY = MinY;
				MinZ = Objects[0u]->z;
				MaxZ = MinZ;
				break;
			}
		}  // for i

		// Get extents:
		for (Layers::const_iterator itr = Layers.begin(); itr != Layers.end(); ++itr)
		{
			(*itr)->GetExtent(&MinX, &MaxX, &MinY, &MaxY, &MinZ, &MaxZ);
		}
	}

	int ID = 1;
	const double wid = (MaxX - MinX) * 1.05;
	const double hei = (MaxY - MinY) * 1.05;

	DXFWriteStreamStr(iStream, 0, "SECTION");
	DXFWriteStreamStr(iStream, 2, "HEADER");

	/*
	// make it an acad-2000 file:
	// _X 2012_02_15: Doesn't work because acad2k requires lots of object handles etc.
	DXFWriteStreamStr(iStream, 9, "$ACADVER");
	DXFWriteStreamStr(iStream, 1, "AC1015");
	*/

	SaveCodepageToStream(iStream);

	DXFWriteStreamStr(iStream, 9, "$EXTMIN");
	DXFWriteStreamDbl(iStream, 10, MinX);
	DXFWriteStreamDbl(iStream, 20, MinY);
	DXFWriteStreamDbl(iStream, 30, MinZ);
	DXFWriteStreamStr(iStream, 9, "$EXTMAX");
	DXFWriteStreamDbl(iStream, 10, MaxX);
	DXFWriteStreamDbl(iStream, 20, MaxY);
	DXFWriteStreamDbl(iStream, 30, MaxZ);

	DXFWriteStreamStr(iStream, 0, "ENDSEC");
	DXFWriteStreamStr(iStream, 0, "SECTION");
	DXFWriteStreamStr(iStream, 2, "TABLES");

	// VPORT table
	DXFWriteStreamStr(iStream, 0, "TABLE");
	DXFWriteStreamStr(iStream, 2, "VPORT");
	DXFWriteStreamInt(iStream, 70, 3);  // max number of entries in the table (DXF doc)
	DXFWriteStream(iStream, 5, ID++);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTable");
	DXFWriteStreamStr(iStream, 0, "VPORT");
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTableRecord");
	DXFWriteStreamStr(iStream, 100, "AcDbViewportTableRecord");  // R13 stuff
	DXFWriteStreamStr(iStream, 2, "*ACTIVE");
	DXFWriteStreamInt(iStream, 70, 0);  // flags (read from DXF)
	DXFWriteStreamDbl(iStream, 10, 0.0);  // vport bottleft (2D), ignored, ACAD 2k crashes if negative!
	DXFWriteStreamDbl(iStream, 20, 0.0);
	DXFWriteStreamDbl(iStream, 11, 1.0);  // vport topright (2D), ignored
	DXFWriteStreamDbl(iStream, 21, 1.0);
	DXFWriteStreamDbl(iStream, 12, (MinX + MaxX) / 2);  // vport center (2D), DCS(???)
	DXFWriteStreamDbl(iStream, 22, (MinY + MaxY) / 2);
	DXFWriteStreamDbl(iStream, 13, 0.0);  // snap base (2D)
	DXFWriteStreamDbl(iStream, 23, 0.0);
	DXFWriteStreamDbl(iStream, 14, 1.0);  // snap spacing (2D)
	DXFWriteStreamDbl(iStream, 24, 1.0);
	DXFWriteStreamDbl(iStream, 15, 0.0);  // grid spacing (2D)
	DXFWriteStreamDbl(iStream, 25, 0.0);
	DXFWriteStreamDbl(iStream, 16, 0.0);  // view direction (3D)
	DXFWriteStreamDbl(iStream, 26, 0.0);
	DXFWriteStreamDbl(iStream, 36, 1.0);
	DXFWriteStreamDbl(iStream, 17, 0.0);  // view target (3D)
	DXFWriteStreamDbl(iStream, 27, 0.0);
	DXFWriteStreamDbl(iStream, 37, 0.0);
	DXFWriteStreamDbl(iStream, 40, hei);  // height
	DXFWriteStreamDbl(iStream, 41, (hei > 1e-10) ? (wid / hei) : 1.0);  // aspect ratio
	DXFWriteStreamDbl(iStream, 42, 50.0);  // lens length (?)
	DXFWriteStreamDbl(iStream, 43, 0.0);  // front clipping plane (?)
	DXFWriteStreamDbl(iStream, 44, 0.0);  // back clipping plane (?)
	DXFWriteStream(iStream, 50, 0);  // snap rotation angle
	DXFWriteStream(iStream, 51, 0);  // view twist angle
	DXFWriteStreamInt(iStream, 71, 0);  // viewmode variable
	DXFWriteStreamInt(iStream, 72, 100);  // circle zoom percent
	DXFWriteStreamInt(iStream, 73, 1);  // fast zoom setting
	DXFWriteStreamInt(iStream, 74, 1);  // UCSICON setting
	DXFWriteStreamInt(iStream, 75, 0);  // snap
	DXFWriteStreamInt(iStream, 76, 0);  // grid
	DXFWriteStreamInt(iStream, 77, 0);  // snap style
	DXFWriteStreamInt(iStream, 78, 0);  // snap isopair
	DXFWriteStreamStr(iStream, 0, "ENDTAB");

	// LTYPE table:
	DXFWriteStreamStr(iStream, 0, "TABLE");
	DXFWriteStreamStr(iStream, 2, "LTYPE");
	DXFWriteStream(iStream, 5, ID++);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTable");
	DXFWriteStreamInt(iStream, 70, 1);
	DXFWriteStreamStr(iStream, 0, "LTYPE");
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTableRecord");
	DXFWriteStreamStr(iStream, 100, "AcDbLinetypeTableRecord");  // R13 stuff
	DXFWriteStreamStr(iStream, 2, "CONTINUOUS");
	DXFWriteStreamInt(iStream, 70, 0);
	DXFWriteStreamStr(iStream, 3, "Solid line");
	DXFWriteStreamInt(iStream, 72, 65);
	DXFWriteStreamInt(iStream, 73, 0);
	DXFWriteStreamStr(iStream, 40, "0.0");
	DXFWriteStreamStr(iStream, 0, "ENDTAB");

	// STYLE table:
	DXFWriteStreamStr(iStream, 0, "TABLE");
	DXFWriteStreamStr(iStream, 2, "STYLE");
	DXFWriteStream(iStream, 5, ID++);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTable");
	DXFWriteStreamInt(iStream, 70, 1);
	DXFWriteStreamStr(iStream, 0, "STYLE");
	DXFWriteStream(iStream, 5, 11);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTableRecord");
	DXFWriteStreamStr(iStream, 100, "AcDbTextStyleTableRecord");
	DXFWriteStreamStr(iStream, 2, "Standard");
	DXFWriteStreamInt(iStream, 70, 0);
	DXFWriteStreamDbl(iStream, 40, 0.0);
	DXFWriteStreamDbl(iStream, 41, 1.0);
	DXFWriteStream(iStream, 50, 0);
	DXFWriteStreamInt(iStream, 71, 0);
	DXFWriteStreamStr(iStream, 42, "0.2");
	DXFWriteStreamStr(iStream, 3, "ARIAL.TTF");
	DXFWriteStreamStr(iStream, 0, "ENDTAB");

	DXFWriteStreamStr(iStream, 0, "TABLE");
	DXFWriteStreamStr(iStream, 2, "LAYER");
	DXFWriteStream(iStream, 5, ID++);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTable");
	DXFWriteStreamInt(iStream, 70, static_cast<int>(Layers.size()));
	for (Layers::const_iterator itr = Layers.begin(); itr != Layers.end(); ++itr)
	{
		SF_ASSERT(!(*itr)->Name.empty());
		DXFWriteStreamStr(iStream, 100, "AcDbSymbolTableRecord");
		DXFWriteStreamStr(iStream, 100, "AcDbLayerTableRecord");  // R13 stuff
		DXFWriteStreamStr(iStream, 0, "LAYER");
		DXFWriteStreamStr(iStream, 2, (*itr)->Name);
		DXFWriteStreamInt(iStream, 70, 0);
		DXFWriteStream(iStream, 62, 7);
		DXFWriteStreamStr(iStream, 6, "CONTINUOUS");
	}
	DXFWriteStreamStr(iStream, 0, "ENDTAB");

	DXFWriteStreamStr(iStream, 0, "TABLE");
	DXFWriteStreamStr(iStream, 2, "APPID");
	DXFWriteStream(iStream, 5, ID++);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTable");
	DXFWriteStreamInt(iStream, 70, 1);
	DXFWriteStreamStr(iStream, 0, "APPID");
	DXFWriteStream(iStream, 5, 12);
	DXFWriteStreamStr(iStream, 100, "AcDbSymbolTableRecord");
	DXFWriteStreamStr(iStream, 100, "AcDbRegAppTableRecord");
	DXFWriteStreamStr(iStream, 2, "ACAD");
	DXFWriteStreamInt(iStream, 70, 0);
	DXFWriteStreamStr(iStream, 0, "APPID");
	DXFWriteStream(iStream, 5, 13);
	DXFWriteStreamStr(iStream, 100, "AcDbRegAppTableRecord");
	DXFWriteStreamStr(iStream, 2, "SITEFLOW"); // TODO: Replace hard-coded name with ability to add user App ID
	DXFWriteStreamInt(iStream, 70, 0);
	DXFWriteStreamStr(iStream, 0, "ENDTAB");
	DXFWriteStreamStr(iStream, 0, "ENDSEC");

	// save block definitions:
	DXFWriteStreamStr(iStream, 0, "SECTION");
	DXFWriteStreamStr(iStream, 2, "BLOCKS");
	for (BlockDefinitions::const_iterator itr = BlockDefinitions.begin(); itr != BlockDefinitions.end(); ++itr)
	{
		if ((*itr)->IsUsed)
		{
			(*itr)->SaveHeaderToStream(iStream);
		}
	}  // for i
	DXFWriteStreamStr(iStream, 0, "ENDSEC");

	DXFWriteStreamStr(iStream, 0, "SECTION");
	DXFWriteStreamStr(iStream, 2, "ENTITIES");

	for (Layers::const_iterator itr = Layers.begin(); itr != Layers.end(); ++itr)
	{
		(*itr)->SaveToStream(iStream, false);
	}

	DXFWriteStreamStr(iStream, 0, "ENDSEC");
	DXFWriteStreamStr(iStream, 0, "EOF");
}





void Arc::SaveToStream(TMStream * iStream, bool BlocksSection) const
{
	UNREFERENCED_PARAMETER(BlocksSection);

	DXFWriteStreamStr(iStream, 0, "ARC");
	DXFWriteStreamStr(iStream, 8, ParentLayer->Name);
	DXFWriteStreamColor(iStream, Color);
	DXFWriteStreamDbl(iStream, 10, x);
	DXFWriteStreamDbl(iStream, 20, y);
	DXFWriteStreamDbl(iStream, 30, z);
	DXFWriteStreamDbl(iStream, 40, Diameter);
	DXFWriteStream(iStream, 50, StartAngle);
	DXFWriteStream(iStream, 51, EndAngle);
}





#endif  // 0
