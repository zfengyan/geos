#include "../headers/geom.h"
#include "../headers/operation.h"

namespace geos {

MultiLineString::MultiLineString(){}
MultiLineString::MultiLineString(vector<Geometry *> *lineStrings, PrecisionModel* precisionModel, int SRID):
GeometryCollection(lineStrings, precisionModel,SRID){}
MultiLineString::~MultiLineString(){}

int MultiLineString::getDimension() const {
	return 1;
}

int MultiLineString::getBoundaryDimension() const {
	if (isClosed()) {
		return Dimension::False;
	}
	return 0;
}

string MultiLineString::getGeometryType() const {
	return "MultiLineString";
}

bool MultiLineString::isClosed() const {
	if (isEmpty()) {
		return false;
	}
	for (unsigned int i = 0; i < geometries->size(); i++) {
		if (!((LineString *)(*geometries)[i])->isClosed()) {
			return false;
		}
	}
	return true;
}

bool MultiLineString::isSimple() const {
	auto_ptr<IsSimpleOp> iso(new IsSimpleOp());
	return iso->isSimple(this);
}

Geometry* MultiLineString::getBoundary() const {
	if (isEmpty()) {
		return new GeometryCollection(NULL, precisionModel, SRID);
	}
	GeometryGraph *g=new GeometryGraph(0,this);
	CoordinateList *pts=g->getBoundaryPoints();
	GeometryFactory fact(precisionModel, SRID);
	delete g;
	return fact.createMultiPoint(pts);
}

bool
MultiLineString::equalsExact(const Geometry *other, double tolerance) const
{
    if (!isEquivalentClass(other)) {
      return false;
    }
	return GeometryCollection::equalsExact(other, tolerance);
}
}

