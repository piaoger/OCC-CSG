
#include "ops.h"

// std
#include <iostream>

// fix brep
#include <ShapeFix_Shape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

// primitive objects
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <gp_Circ.hxx>

namespace open_cascade { namespace ops { namespace brep {

TopoDS_Shape create_box(double x1, double y1, double z1, double x2, double y2, double z2) {
	gp_Pnt lowerLeftCornerOfBox(x1,y1,z1);
	gp_Pnt upperRightCornerOfBox(x2,y2,z2);
 	BRepPrimAPI_MakeBox boxMaker(lowerLeftCornerOfBox,upperRightCornerOfBox);
	TopoDS_Shape box = boxMaker.Shape();
	return box;
}

TopoDS_Shape create_sphere(double x1, double y1, double z1, double r) {
	gp_Pnt center(x1,y1,z1);
	BRepPrimAPI_MakeSphere sphereMaker(center,r);
	TopoDS_Shape sphere = sphereMaker.Shape();
	return sphere;
}

TopoDS_Shape create_cylinder(double r, double h) {
	BRepPrimAPI_MakeCylinder cylinderMaker(r,h);
	TopoDS_Shape cylinder = cylinderMaker.Shape();
	return cylinder;
}

TopoDS_Shape create_cylinder(double r, double h, double angle) {
	BRepPrimAPI_MakeCylinder cylinderMaker(r,h,angle);
	TopoDS_Shape cylinder = cylinderMaker.Shape();
	return cylinder;
}

TopoDS_Shape create_cone(double r1, double r2, double h) {
	BRepPrimAPI_MakeCone coneMaker(r1,r2,h);
	TopoDS_Shape cone = coneMaker.Shape();
	return cone;
}

TopoDS_Shape create_cone(double r1, double r2, double h, double angle) {
	BRepPrimAPI_MakeCone coneMaker(r1,r2,h,angle);
	TopoDS_Shape cone = coneMaker.Shape();
	return cone;
}

TopoDS_Shape create_polygons(std::vector<double> const &points, std::vector<std::vector<int>> const &indices) {

    if(points.size()%3!=0) {
		std::cerr << "ERROR: wrong number count, must be multiples of 3, but is " << points.size() << std::endl;
		exit(1);
	}

    size_t numVerts = points.size()/3;
	std::vector<TopoDS_Vertex> vertices(numVerts);

	// converting number list to vertices 
	for(size_t i = 0; i < points.size(); i+=3) {
		gp_XYZ p;
		p.SetCoord(points[i+0], points[i+1], points[i+2]);
		vertices[i/3] = BRepBuilderAPI_MakeVertex(p);
	}

	// creating faces
	std::vector<TopoDS_Face> faces(indices.size());

	for(size_t f_id = 0; f_id < indices.size(); f_id++) {
		BRepBuilderAPI_MakePolygon polyMaker;

		std::vector<int> face_indices = indices[f_id];

		// add vertices to polygon
		for(size_t v_id = 0; v_id < face_indices.size(); v_id++) {
			polyMaker.Add(vertices[ face_indices[v_id] ]);
		}

		polyMaker.Close();

		if(!polyMaker.IsDone()) {
			std::cerr << "ERROR: cannot construct polygon for extrusion. Path invalid (e.g., crossing edges)" << std::endl;
			exit(1);
		}

		TopoDS_Wire wire = polyMaker.Wire();

		if(wire.IsNull()) {
			std::cerr << "ERROR: cannot construct polygon for extrusion. Path invalid (e.g., crossing edges)" << std::endl;
			exit(1);
		}

		TopoDS_Face face = BRepBuilderAPI_MakeFace( wire );

		faces.push_back(face);
		
	} // end for each face

	// sewing faces
	BRepBuilderAPI_Sewing shapeSewer;

	for(size_t f_id = 0; f_id < faces.size(); f_id++) {
		shapeSewer.Add(faces[f_id]);
	}

	std::cout << " -> sewing faces" << std::endl;

	shapeSewer.Perform();
	TopoDS_Shape shape = shapeSewer.SewedShape();

	std::cout << " -> extracting shells" << std::endl;
    
	BRepBuilderAPI_MakeSolid solidmaker;
	TopTools_IndexedMapOfShape shellMap;
	TopExp::MapShapes(shape, TopAbs_SHELL, shellMap);

	unsigned int counter = 0;
	for(int ishell = 1; ishell <= shellMap.Extent(); ++ishell) {
    	const TopoDS_Shell& shell = TopoDS::Shell(shellMap(ishell));
    	solidmaker.Add(shell);
		counter++;
	}

	std::cout << "   -> shells found: " << counter << std::endl;

	std::cout << " -> converting to solid" << std::endl;
	
	TopoDS_Shape solid = solidmaker.Solid();

	std::cout << " -> done." << std::endl;

	return solid;

}


TopoDS_Shape create_circle(double x, double y, double z, double dx, double dy, double dz, double r) {
	gp_Dir dir(dx,dy,dz);
	gp_Pnt point(x,y,z);
	gp_Circ circle(gp_Ax2( point, dir), r);
	BRepBuilderAPI_MakeEdge makeEdge(circle);

    TopoDS_Wire wire = BRepBuilderAPI_MakeWire(makeEdge.Edge());

	TopoDS_Shape shape;

	if( !wire.IsNull()) {
		TopoDS_Shape face = BRepBuilderAPI_MakeFace( wire );
		if(!face.IsNull()) {
			shape = face;
		}
	}

	return shape;
}

TopoDS_Shape create_rect2d(double minX, double minY, double maxX, double maxY) {
	std::vector<double> coords;

	coords.push_back(minX); coords.push_back(minY);
	coords.push_back(maxX); coords.push_back(minY);
	coords.push_back(maxX); coords.push_back(maxY);
	coords.push_back(minX); coords.push_back(maxY);

	return create_polygon2d(coords);
}

TopoDS_Shape create_polygon2d(std::vector<double> const &coords) {
	if(coords.size()%2!=0) {
		std::cerr << "ERROR: wrong number count, must be multiples of 2, but is " << coords.size() << std::endl;
		exit(1);
	}

	size_t numVerts = coords.size()/2;
	std::vector<TopoDS_Vertex> vertices;

	for(size_t i = 0; i < coords.size(); i+=2) {
		gp_XYZ p;
		p.SetCoord(coords[i+0], coords[i+1], 0);
		vertices.push_back(BRepBuilderAPI_MakeVertex(p));
	}

	BRepBuilderAPI_MakePolygon polyMaker;

	for(size_t i = 0; i < numVerts;i++) {
		polyMaker.Add(vertices[i]);
	}

	polyMaker.Close();

	if(!polyMaker.IsDone()) {
		std::cerr << "ERROR: cannot construct polygon. Path invalid (e.g., crossing edges)" << std::endl;
		exit(1);
	}

	TopoDS_Wire wire = polyMaker.Wire();

	if(wire.IsNull()) {
		std::cerr << "ERROR: cannot construct polygon. Path invalid (e.g., crossing edges)" << std::endl;
		exit(1);
	}

	TopoDS_Face face = BRepBuilderAPI_MakeFace( wire );

    return face;
}



TopoDS_Shape extrude_polygon(double ex, double ey, double ez, std::vector<double> const &points) {

    if(points.size()%3!=0) {
		std::cerr << "ERROR: wrong number count, must be multiples of 3, but is " << points.size() << std::endl;
		exit(1);
	}

    size_t numVerts = points.size()/3;
	std::vector<TopoDS_Vertex> vertices(numVerts);

	for(size_t i = 0; i < points.size(); i+=3) {

		gp_XYZ p;
		p.SetCoord(points[i+0], points[i+1], points[i+2]);
		vertices[i/3] = BRepBuilderAPI_MakeVertex(p);
	}

	BRepBuilderAPI_MakePolygon polyMaker;

	for(size_t i = 0; i < numVerts;i++) {
		polyMaker.Add(vertices[i]);
	}

	polyMaker.Close();

	if(!polyMaker.IsDone()) {
		std::cerr << "ERROR: cannot construct polygon for extrusion. Path invalid (e.g., crossing edges)" << std::endl;
		exit(1);
	}

	TopoDS_Wire wire = polyMaker.Wire();

	if(wire.IsNull()) {
		std::cerr << "ERROR: cannot construct polygon for extrusion. Path invalid (e.g., crossing edges)" << std::endl;
		exit(1);
	}

	TopoDS_Face face = BRepBuilderAPI_MakeFace( wire );

    gp_Vec direction;

	direction.SetX(ex);
	direction.SetY(ey);
	direction.SetZ(ez);

	return BRepPrimAPI_MakePrism(face, direction);
}

TopoDS_Shape fix_shape(const TopoDS_Shape& shape) {
	// perform healing in case the boolean operations
	// cause problems
	ShapeFix_Shape fixer(shape);
	fixer.Perform();
	return fixer.Shape();
}


void sew_shape(TopoDS_Shape& shape){
    std::cout << "Going to heal shape!!" << std::endl;

    ShapeFix_Shape fixer(shape);
    fixer.Perform();
    shape = fixer.Shape();

    BRepBuilderAPI_Sewing sew;
    sew.Add(shape);
    sew.Perform();
    shape = sew.SewedShape();
}

}}}