//---------------------------------------------------------------------------//
//                        ROJPRouter.cpp -
//      The junction-percentage router
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2004/02/02 16:20:16  dkrajzew
// catched the problems with dead end edges
//
// Revision 1.3  2004/01/28 14:19:16  dkrajzew
// allowed to specify the maximum edge number in a route by a factor
//
// Revision 1.2  2004/01/26 09:58:15  dkrajzew
// sinks are now simply marked as these instead of the usage of a further container
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <router/RONet.h>
#include "ROJPRouter.h"
#include "ROJPEdge.h"
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROJPRouter::ROJPRouter(RONet &net)
    : myNet(net)
{
    myMaxEdges = (int) (
        ((float) net.getEdgeNo()) *
         OptionsSubSys::getOptions().getFloat("max-edges-factor"));
}


ROJPRouter::~ROJPRouter()
{
}


ROEdgeVector
ROJPRouter::compute(ROEdge *from, ROEdge *to, long time,
                    bool continueOnUnbuild)
{
    return jpCompute(static_cast<ROJPEdge*>(from), time, continueOnUnbuild);
}


ROEdgeVector
ROJPRouter::jpCompute(ROJPEdge *from, long time, bool continueOnUnbuild)
{
    ROEdgeVector ret;
    ROJPEdge *current = from;
    // route until a sinks has been found
    while(  current!=0
            &&
            current->getType()!=ROEdge::ET_SINK
            &&
            ret.size()<myMaxEdges) {
        ret.add(current);
        time += current->getDuration(time);
        current = current->chooseNext(time);
    }
    // check whether no valid ending edge was found
    if(ret.size()>=myMaxEdges) {
        MsgHandler *mh = 0;
        if(continueOnUnbuild) {
            mh = MsgHandler::getWarningInstance();
        } else {
            mh = MsgHandler::getErrorInstance();
        }
        mh->inform(string("The route starting at edge '") + from->getID()
            + string("' could not be closed."));
    }
    // append the sink
    if(current!=0) {
        ret.add(current);
    }
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


