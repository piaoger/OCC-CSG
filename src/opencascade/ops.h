
#include <vector>
#include <string>

#include <TopoDS.hxx>

namespace open_cascade { namespace ops { 


	namespace brep {
		// --------------------------------
		// creation
		// --------------------------------
		// minimal API for primitive objects
		TopoDS_Shape create_box(double x1, double y1, double z1, double x2, double y2, double z2);
		TopoDS_Shape create_sphere(double x1, double y1, double z1, double r);
		TopoDS_Shape create_cylinder(double r, double h);
		TopoDS_Shape create_cylinder(double r, double h, double angle);
		TopoDS_Shape create_cone(double r1, double r2, double h);
		TopoDS_Shape create_cone(double r1, double r2, double h, double angle);
		TopoDS_Shape create_polygons(std::vector<double> const &points, std::vector<std::vector<int>> const &indices);
		TopoDS_Shape create_circle(double x, double y, double z, double dx, double dy, double dz, double r);

		TopoDS_Shape create_polygon2d(std::vector<double>const &coords);
		TopoDS_Shape create_rect2d(double minX, double minY, double maxX, double maxY);
	 
		// extrusion
		TopoDS_Shape extrude_polygon(double ex, double ey, double ez, std::vector<double> const &points);
	 
		void sew_shape(TopoDS_Shape& shape);		
		void fix_shape(TopoDS_Shape& shape);
	}

	// --------------------------------
	// boolean
	// --------------------------------
	namespace csg {
		TopoDS_Shape unite(const TopoDS_Shape& s1, const TopoDS_Shape& s2);
		TopoDS_Shape subtract(const TopoDS_Shape& s1, const TopoDS_Shape& s2);
		TopoDS_Shape intesect(const TopoDS_Shape& s1, const TopoDS_Shape& s2);
	}


 
}}