/*
 * opencog/generate/Frame.cc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "Frame.h"

using namespace opencog;

Frame::Frame(AtomSpace* as)
	: _as(as)
{
}

Frame::~Frame()
{
}

/// Extend unconnected connectors by one step.
/// Return false if not done (more work remains)
/// Else return true if done.
///
/// pol_pairs is a list of polarization pairs, i.e.
/// match pairs of ConnectorDir pairs (from, to) which
/// are to be connected.
bool Frame::extend(const HandlePairSeq& pole_pairs)
{
	if (0 == _open.size()) return true;

	for (const Handle& section : _open)
	{
		extend_one(section, pole_pairs);
	}

	return (0 == _open.size());
}

#define al _as->add_link
#define an _as->add_node

void Frame::extend_one(const Handle& section,
                       const HandlePairSeq& pole_pairs)
{
	printf("duude extend =%s\n", section->to_string().c_str());

	// Connector seq is always second in the outset.
	Handle conseq = section->getOutgoingAtom(1);

	for (const Handle& con : conseq->getOutgoingSet())
	{

		// Nothing to do, if not a connector.
		if (CONNECTOR != con->get_type()) continue;

		// For now, assume only one connector.
		Handle fromdir = con->getOutgoingAtom(1);
		Handle todir;
		for (const HandlePair& popr: pole_pairs)
			if (fromdir == popr.first) { todir = popr.second; break; }

		// A matching pole was not found.
		if (!todir) continue;

		printf("duude connect =%s\n%s\n", todir->to_string().c_str(), con->to_string().c_str());

		// Link type of the desired link to make...
		Handle linkt = con->getOutgoingAtom(0);

		// Find appropriate connector, if it exists
		Handle matching = _as->get_atom(createLink(CONNECTOR, linkt, todir));
		if (!matching) continue;

		// Find all ConnectorSeq with the matching connector in it.
		HandleSeq seqs = matching->getIncomingSetByType(CONNECTOR_SEQ);
		for (const Handle& seq : seqs)
		{
			printf("duude found %s\n", seq->to_string().c_str());

		}
	}
}
