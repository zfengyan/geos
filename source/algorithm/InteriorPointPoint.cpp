#include "../headers/geosAlgorithm.h"
#include "../headers/platform.h"
#include <typeinfo>

namespace geos {

InteriorPointPoint::InteriorPointPoint(const Geometry *g) {
	interiorPoint=NULL;
	minDistance=DoubleInfinity;
	auto_ptr<Point> p(g->getCentroid());
	centroid=p->getCoordinate();
	add(g);
}

InteriorPointPoint::~InteriorPointPoint() {
}

/**
* Tests the point(s) defined by a Geometry for the best inside point.
* If a Geometry is not of dimension 0 it is not tested.
* @param geom the geometry to add
*/
void InteriorPointPoint::add(const Geometry *geom) {
	if (typeid(*geom)==typeid(Point)) {
		add(geom->getCoordinate());
	} else if ((typeid(*geom)==typeid(GeometryCollection)) ||
				(typeid(*geom)==typeid(MultiPoint)) ||
				(typeid(*geom)==typeid(MultiPolygon)) ||
				(typeid(*geom)==typeid(MultiLineString))) {
		GeometryCollection *gc=(GeometryCollection*) geom;
		for(int i=0;i<gc->getNumGeometries();i++) {
			add(gc->getGeometryN(i));
		}
	}
}

void InteriorPointPoint::add(const Coordinate *point) {
	double dist=point->distance(*centroid);
	if (dist<minDistance) {
		interiorPoint=point;
		minDistance=dist;
	}
}

const Coordinate* InteriorPointPoint::getInteriorPoint() const {
	return interiorPoint;
}
}

