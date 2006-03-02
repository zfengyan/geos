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
 **********************************************************************
 *
 * Last port: operation/buffer/SubgraphDepthLocater.java rev. 1.5 (JTS-1.7)
 *
 **********************************************************************/

#include <geos/opBuffer.h>

#ifndef DEBUG
#define DEBUG 0
#endif

using namespace geos::geomgraph;
using namespace geos::noding;
using namespace geos::algorithm;
using namespace geos::operation::overlay;

namespace geos {
namespace operation { // geos.operation
namespace buffer { // geos.operation.buffer

/*public*/
int
SubgraphDepthLocater::getDepth(Coordinate &p)
{
	vector<DepthSegment*> stabbedSegments;
	findStabbedSegments(p, stabbedSegments);

	// if no segments on stabbing line subgraph must be outside all others->
	if (stabbedSegments.size()==0) return 0;

	sort(stabbedSegments.begin(), stabbedSegments.end(), DepthSegmentLT);
	DepthSegment *ds=stabbedSegments[0];
	int ret = ds->leftDepth;
#if DEBUG
	cerr<<"SubgraphDepthLocater::getDepth("<<p.toString()<<"): "<<ret<<endl;
#endif

	for (vector<DepthSegment *>::iterator
		it=stabbedSegments.begin(), itEnd=stabbedSegments.end();
		it != itEnd;
		++it)
	{
		delete *it;
	}

	return ret;
}

/*private*/
void
SubgraphDepthLocater::findStabbedSegments(Coordinate &stabbingRayLeftPt,
			std::vector<DepthSegment*>& stabbedSegments)
{
	unsigned int size = subgraphs->size();
	for (unsigned int i=0; i<size; ++i)
	{
		BufferSubgraph *bsg=(*subgraphs)[i];

		// optimization - don't bother checking subgraphs
		// which the ray does not intersect
		Envelope *env = bsg->getEnvelope();
		if ( stabbingRayLeftPt.y < env->getMinY()
			|| stabbingRayLeftPt.y > env->getMaxY() )
				continue;

		findStabbedSegments(stabbingRayLeftPt, bsg->getDirectedEdges(),
			stabbedSegments);
	}
	//return stabbedSegments;
}

/*private*/
void
SubgraphDepthLocater::findStabbedSegments( Coordinate &stabbingRayLeftPt,
	vector<DirectedEdge*>* dirEdges,
	vector<DepthSegment*>& stabbedSegments)
{

	/**
	 * Check all forward DirectedEdges only. This is still general,
	 * because each Edge has a forward DirectedEdge.
	 */
	for (unsigned int i=0; i<dirEdges->size(); ++i)
	{
		DirectedEdge *de=(*dirEdges)[i];
		if (!de->isForward())
			continue;
		findStabbedSegments(stabbingRayLeftPt, de, stabbedSegments);
	}
}

/*private*/
void
SubgraphDepthLocater::findStabbedSegments( Coordinate &stabbingRayLeftPt,
	DirectedEdge *dirEdge,
	vector<DepthSegment*>& stabbedSegments)
{
	const CoordinateSequence *pts=dirEdge->getEdge()->getCoordinates();

// It seems that LineSegment is *very* slow... undef this
// to see yourself
// LineSegment has been refactored to be mostly inline, still
// it makes copies of the given coordinates, while the 'non-LineSemgent'
// based code below uses pointers instead. I'll kip the SKIP_LS
// defined until LineSegment switches to Coordinate pointers instead.
//
#define SKIP_LS 1

	int n = pts->getSize()-1;
	for (int i=0; i<n; ++i) {
#ifndef SKIP_LS
		seg.p0=pts->getAt(i);
		seg.p1=pts->getAt(i + 1);
#if DEBUG
		cerr << " SubgraphDepthLocater::findStabbedSegments: segment " << i
			<< " (" << seg << ") ";
#endif

#else
		const Coordinate *low=&(pts->getAt(i));
		const Coordinate *high=&(pts->getAt(i+1));
		const Coordinate *swap=NULL;

#endif

#ifndef SKIP_LS
		// ensure segment always points upwards
		//if (seg.p0.y > seg.p1.y)
		{
			seg.reverse();
#if DEBUG
			cerr << " reverse (" << seg << ") ";
#endif
		}
#else
		if (low->y > high->y)
		{
			swap=low;
			low=high;
			high=swap;
		}
#endif

		// skip segment if it is left of the stabbing line
		// skip if segment is above or below stabbing line
#ifndef SKIP_LS
		double maxx=max(seg.p0.x, seg.p1.x);
#else
		double maxx=max(low->x, high->x);
#endif
		if (maxx < stabbingRayLeftPt.x)
		{
#if DEBUG
			cerr<<" segment is left to stabbing line, skipping "<<endl;
#endif
			continue;
		}

		// skip horizontal segments (there will be a non-horizontal
		// one carrying the same depth info
#ifndef SKIP_LS
		if (seg.isHorizontal())
#else
		if (low->y == high->y)
#endif
		{
#if DEBUG
			cerr<<" segment is horizontal, skipping "<<endl;
#endif
			continue;
		}

		// skip if segment is above or below stabbing line
#ifndef SKIP_LS
		if (stabbingRayLeftPt.y < seg.p0.y ||
			stabbingRayLeftPt.y > seg.p1.y)
#else
		if (stabbingRayLeftPt.y < low->y ||
			stabbingRayLeftPt.y > high->y)
#endif
		{
#if DEBUG
			cerr<<" segment above or below stabbing line, skipping "<<endl;
#endif
			continue;
		}

		// skip if stabbing ray is right of the segment
#ifndef SKIP_LS
		if (CGAlgorithms::computeOrientation(seg.p0, seg.p1,
#else
		if (CGAlgorithms::computeOrientation(*low, *high,
#endif
				stabbingRayLeftPt)==CGAlgorithms::RIGHT)
		{
#if DEBUG
			cerr<<" stabbing ray right of segment, skipping"<<endl;
#endif
			continue;
		}

#ifndef SKIP_LS
		// stabbing line cuts this segment, so record it
		int depth=dirEdge->getDepth(Position::LEFT);
		// if segment direction was flipped, use RHS depth instead
		if (! (seg.p0==pts->getAt(i)))
			depth=dirEdge->getDepth(Position::RIGHT);
#else
		int depth = swap ?
			dirEdge->getDepth(Position::RIGHT)
			:
			dirEdge->getDepth(Position::LEFT);
#endif

#if DEBUG
	cerr<<" depth: "<<depth<<endl;
#endif

#ifdef SKIP_LS
		seg.p0 = *low;
		seg.p1 = *high;
#endif

		DepthSegment *ds=new DepthSegment(seg, depth);
		stabbedSegments.push_back(ds);
	}
}

} // namespace geos.operation.buffer
} // namespace geos.operation
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.19  2006/03/02 09:46:04  strk
 * cleaned up debugging lines
 *
 * Revision 1.18  2006/03/01 17:16:39  strk
 * LineSegment class made final and optionally (compile-time) inlined.
 * Reduced heap allocations in Centroid{Area,Line,Point} and InteriorPoint{Area,Line,Point}.
 *
 * Revision 1.17  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.16  2006/01/31 19:07:34  strk
 * - Renamed DefaultCoordinateSequence to CoordinateArraySequence.
 * - Moved GetNumGeometries() and GetGeometryN() interfaces
 *   from GeometryCollection to Geometry class.
 * - Added getAt(int pos, Coordinate &to) funtion to CoordinateSequence class.
 * - Reworked automake scripts to produce a static lib for each subdir and
 *   then link all subsystem's libs togheter
 * - Moved C-API in it's own top-level dir capi/
 * - Moved source/bigtest and source/test to tests/bigtest and test/xmltester
 * - Fixed PointLocator handling of LinearRings
 * - Changed CoordinateArrayFilter to reduce memory copies
 * - Changed UniqueCoordinateArrayFilter to reduce memory copies
 * - Added CGAlgorithms::isPointInRing() version working with
 *   Coordinate::ConstVect type (faster!)
 * - Ported JTS-1.7 version of ConvexHull with big attention to
 *   memory usage optimizations.
 * - Improved XMLTester output and user interface
 * - geos::geom::util namespace used for geom/util stuff
 * - Improved memory use in geos::geom::util::PolygonExtractor
 * - New ShortCircuitedGeometryVisitor class
 * - New operation/predicate package
 *
 * Revision 1.15  2005/11/08 20:12:44  strk
 * Memory overhead reductions in buffer operations.
 *
 * Revision 1.14  2005/07/11 10:27:14  strk
 * Fixed initializzazion lists
 *
 * Revision 1.13  2005/06/30 18:31:48  strk
 * Ported SubgraphDepthLocator optimizations from JTS code
 *
 * Revision 1.12  2005/06/28 21:13:43  strk
 * Fixed a bug introduced by LineSegment skip - made LineSegment skip a compile-time optione
 *
 * Revision 1.11  2005/06/27 21:58:31  strk
 * Bugfix in DepthSegmentLT as suggested by Graeme Hiebert
 *
 * Revision 1.10  2005/06/27 21:24:54  strk
 * Fixed bug just-introduced with optimization.
 *
 * Revision 1.9  2005/06/27 21:21:21  strk
 * Reduced Coordinate copies due to LineSegment overuse
 *
 * Revision 1.8  2005/05/23 15:13:00  strk
 * Added debugging output
 *
 * Revision 1.7  2005/05/20 16:15:41  strk
 * Code cleanups
 *
 * Revision 1.6  2005/05/19 10:29:28  strk
 * Removed some CGAlgorithms instances substituting them with direct calls
 * to the static functions. Interfaces accepting CGAlgorithms pointers kept
 * for backward compatibility but modified to make the argument optional.
 * Fixed a small memory leak in OffsetCurveBuilder::getRingCurve.
 * Inlined some smaller functions encountered during bug hunting.
 * Updated Copyright notices in the touched files.
 *
 * Revision 1.5  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added CoordinateArraySequenceFactory::instance() function.
 *
 * Revision 1.4  2004/07/02 13:28:28  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.3  2004/05/05 12:29:44  strk
 * memleak fixed in ::getDepth
 *
 * Revision 1.2  2004/05/03 22:56:44  strk
 * leaks fixed, exception specification omitted.
 *
 * Revision 1.1  2004/04/10 08:40:01  ybychkov
 * "operation/buffer" upgraded to JTS 1.4
 *
 *
 **********************************************************************/

