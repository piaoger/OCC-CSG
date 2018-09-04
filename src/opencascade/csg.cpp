
#include "ops.h"
// CSG operators
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>


// see the following links for new boolean features used below:
	// https://www.opencascade.com/sites/default/files/documents/release_notes_7.3.0.pdf
// https://dev.opencascade.org/index.php?q=node/1056

namespace open_cascade { namespace ops { namespace csg {

	static const double fuzzyValue = 0.0001;

TopoDS_Shape unite(const TopoDS_Shape& s1, const TopoDS_Shape& s2) {
	BRepAlgoAPI_Fuse op(s1, s2);
	op.SetUseOBB(true);
	op.SetRunParallel(false);
	op.SetFuzzyValue(fuzzyValue);
	return op.Shape(); 
}

TopoDS_Shape subtract(const TopoDS_Shape& s1, const TopoDS_Shape& s2) {
	BRepAlgoAPI_Cut op(s1, s2);
	op.SetUseOBB(true);
	op.SetRunParallel(false);
	op.SetFuzzyValue(fuzzyValue);
	return op.Shape(); 
}

TopoDS_Shape intesect(const TopoDS_Shape& s1, const TopoDS_Shape& s2) {
	BRepAlgoAPI_Common op(s1, s2);
	op.SetUseOBB(true);
	op.SetRunParallel(false);
	op.SetFuzzyValue(fuzzyValue);
	return op.Shape(); 
}

}}}