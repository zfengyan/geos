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
* Revision 1.2  2004/07/02 13:28:28  strk
* Fixed all #include lines to reflect headers layout change.
* Added client application build tips in README.
*
* Revision 1.1  2004/04/07 06:55:50  ybychkov
* "operation/linemerge" ported from JTS 1.4
*
*
**********************************************************************/


#include <geos/opLinemerge.h>
#include <geos/util.h>
#include <typeinfo>

namespace geos {

void LineMerger::add(vector<Geometry*> *geometries) {
	for(int i=0;i<(int)geometries->size();i++) {
		Geometry *geometry=(*geometries)[i];
		add(geometry);
	}
}

LineMerger::LineMerger() {
	graph=new LineMergeGraph();
	mergedLineStrings=NULL;
	edgeStrings=NULL;
	factory=NULL;
}

LineMerger::~LineMerger() {
	delete graph;
//	delete mergedLineStrings;
//	delete edgeStrings;
}


LMGeometryComponentFilter::LMGeometryComponentFilter(LineMerger *newLm) {
	lm=newLm;
}
void LMGeometryComponentFilter::filter_rw(Geometry *geom) {
	if (typeid(*geom)==typeid(LineString)) {
		lm->add((LineString*)geom);
	}	
}
/**
* Adds a Geometry to be processed. May be called multiple times.
* Any dimension of Geometry may be added; the constituent linework will be
* extracted.
*/  
void LineMerger::add(Geometry *geometry) {
	geometry->apply_rw(new LMGeometryComponentFilter(this));
}

void LineMerger::add(LineString *lineString) {
	if (factory==NULL) {
		factory=lineString->getFactory();
	}
	graph->addEdge(lineString);
}

void LineMerger::merge() {
	if (mergedLineStrings!=NULL) { 
		return;
	}

	edgeStrings=new vector<EdgeString*>();
	buildEdgeStringsForObviousStartNodes();
	buildEdgeStringsForIsolatedLoops();
	mergedLineStrings=new vector<LineString*>();    
	for (int i=0;i<(int)edgeStrings->size();i++) {
		EdgeString *edgeString=(*edgeStrings)[i];
		mergedLineStrings->push_back(edgeString->toLineString());
	}    
}

void LineMerger::buildEdgeStringsForObviousStartNodes() {
	buildEdgeStringsForNonDegree2Nodes();
}
void LineMerger::buildEdgeStringsForIsolatedLoops() {
	buildEdgeStringsForUnprocessedNodes();
}  

void LineMerger::buildEdgeStringsForUnprocessedNodes() {
	vector<planarNode*> *nodes=graph->getNodes();
	for (int i=0;i<(int)nodes->size();i++) {
		planarNode *node=(*nodes)[i];
		if (!node->isMarked()) { 
			Assert::isTrue(node->getDegree()==2);
			buildEdgeStringsStartingAt(node);
			node->setMarked(true);
		}
	}
}

void LineMerger::buildEdgeStringsForNonDegree2Nodes() {
	vector<planarNode*> *nodes=graph->getNodes();
	for (int i=0;i<(int)nodes->size();i++) {
		planarNode *node=(*nodes)[i];
		if (node->getDegree()!=2) { 
			buildEdgeStringsStartingAt(node);
			node->setMarked(true);
		}
	}
}

void LineMerger::buildEdgeStringsStartingAt(planarNode *node) {
	vector<planarDirectedEdge*> *edges=node->getOutEdges()->getEdges();
	for (int i=0;i<(int)edges->size();i++) {
		LineMergeDirectedEdge *directedEdge=(LineMergeDirectedEdge*) (*edges)[i];
		if (directedEdge->getEdge()->isMarked()) {
			continue;
		}
		edgeStrings->push_back(buildEdgeStringStartingWith(directedEdge));
	}
}

EdgeString* LineMerger::buildEdgeStringStartingWith(LineMergeDirectedEdge *start) {    
	EdgeString *edgeString = new EdgeString(factory);
	LineMergeDirectedEdge *current=start;
	do {
		edgeString->add(current);
		current->getEdge()->setMarked(true);
		current=current->getNext();      
	} while (current!=NULL && current!=start);
	return edgeString;
}
/**
* Returns the LineStrings built by the merging process.
*/
vector<LineString*>* LineMerger::getMergedLineStrings() {
	merge();
	return mergedLineStrings;
}
}
