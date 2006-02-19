/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/opOverlay.h>

using namespace geos::algorithm;
using namespace geos::geomgraph;
using namespace geos::geomgraph::index;

namespace geos {
namespace operation { // geos.operation
namespace overlay { // geos.operation.overlay


void
EdgeSetNoder::addEdges(vector<Edge*> *edges)
{
	inputEdges->insert(inputEdges->end(),edges->begin(),edges->end());
}

vector<Edge*>*
EdgeSetNoder::getNodedEdges()
{
	EdgeSetIntersector *esi=new SimpleMCSweepLineIntersector();
	SegmentIntersector *si=new SegmentIntersector(li,true,false);
	esi->computeIntersections(inputEdges,si,true);
	//Debug.println("has proper int = " + si.hasProperIntersection());
	vector<Edge*> *splitEdges=new vector<Edge*>();
	for(int i=0;i<(int)inputEdges->size();i++) {
		Edge* e=(*inputEdges)[i];
		e->getEdgeIntersectionList().addSplitEdges(splitEdges);
	}
	return splitEdges;
}

} // namespace geos.operation.overlay
} // namespace geos.operation
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.8  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.7  2005/11/16 15:49:54  strk
 * Reduced gratuitous heap allocations.
 *
 * Revision 1.6  2004/07/02 13:28:28  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.5  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

