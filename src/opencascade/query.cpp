

#include "ops.h"

// std
#include <iostream>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

#include <BRepMesh_IncrementalMesh.hxx>


#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

namespace open_cascade { namespace query {

void bounds(const TopoDS_Shape& shape ) {

	std::cout << "> computing bounding box" << std::endl;

	std::cout << " -> approximating bounds" << std::endl;

    // compute bbox on geometric object
	double xMin,yMin,zMin,xMax,yMax, zMax = 0;
	Standard_Real aDeflection = 0.0001, deflection;
	Bnd_Box box;
	BRepBndLib::Add(shape, box);
	box.Get(xMin, yMin, zMin, xMax, yMax, zMax);
	deflection= MAX3( xMax-xMin , yMax-yMin , zMax-zMin)*aDeflection;

	std::cout << " -> tessellating object" << std::endl;
	// tessellation
	BRepMesh_IncrementalMesh mesh(shape, deflection);

    std::cout << " -> computing bounds" << std::endl;

	// compute bbox with tessellation
	box.SetVoid();
	BRepBndLib::Add(shape, box);
	box.Get(xMin, yMin, zMin, xMax, yMax, zMax);
}


}}

