/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 * $Log$
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.14  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include "../headers/geomgraph.h"
#include "../headers/indexQuadtree.h"

namespace geos {

EdgeList::EdgeList(){
	edges=new vector<Edge*>();
	index=new Quadtree();
}

EdgeList::~EdgeList(){
	delete edges;
	delete index;
}
/**
 * Insert an edge unless it is already in the list
 */
void EdgeList::add(Edge *e) {
	edges->push_back(e);
	index->insert(e->getEnvelope(),e);
}

void EdgeList::addAll(vector<Edge*> *edgeColl) {
	for (int i=0; i<(int)edgeColl->size();i++) {
		add((*edgeColl)[i]);
	}
}

vector<Edge*>* EdgeList::getEdges() {
	return edges;
}

// <FIX> fast lookup for edges
/**
* If there is an edge equal to e already in the list, return it.
* Otherwise return null.
* @return  equal edge, if there is one already in the list
*          null otherwise
*/
Edge* EdgeList::findEqualEdge(Edge *e) {
	vector<void*> *testEdges=index->query(e->getEnvelope());
	for (int i=0; i<(int)testEdges->size();i++) {
		Edge* testEdge=(Edge*) (*testEdges)[i];
		if (testEdge->equals(e)) return testEdge;
	}
	return NULL;
}

Edge* EdgeList::get(int i) {
	return (*edges)[i];
}

/**
 * If the edge e is already in the list, return its index.
 * @return  index, if e is already in the list
 *          -1 otherwise
 */
int EdgeList::findEdgeIndex(Edge *e) {
	for (int i=0; i<(int)edges->size();i++) {
		if ( (*edges)[i]->equals(e) )
            return i;
	}
	return -1;
}

string EdgeList::print() {
	string out="MULTILINESTRING ( ";
	for(unsigned int j=0; j<edges->size();j++) {
        Edge *e=(*edges)[j];
		if (j>0) out+=",";
		out+="(";
		const CoordinateList* pts=e->getCoordinates();
		for(int i=0; i<pts->getSize();i++) {
			if (i>0) out+=",";
			out+=pts->getAt(i).toString();
		}
		out+=")";
	}
	out+=")  ";
	return out;
}

}

