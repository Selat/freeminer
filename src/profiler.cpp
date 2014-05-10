/*
This file is part of Freeminer.

Freeminer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Freeminer  is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Freeminer.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "profiler.h"
#include "util/string.h"
#include "gettime.h"
#include "log.h"

u8 ScopeProfilerTree::m_current_depth = 0;
std::vector <std::string> ScopeProfilerTree::m_messages;
bool ScopeProfilerTree::m_has_children;

ScopeProfilerTree::ScopeProfilerTree(const std::string& message) :
	m_start_time(getTime(PRECISION_MICRO)),
	m_current_message(message)
{
	m_has_children = false;
	++m_current_depth;
	if(m_messages.size() <= m_current_depth) {
		m_messages.resize(m_current_depth + 1);
	}
}

ScopeProfilerTree::~ScopeProfilerTree()
{
	--m_current_depth;
	if(m_has_children) {
		std::string &tmp_s = m_messages[m_current_depth];
		for(u8 i = 0; i < m_current_depth; ++i) {
			tmp_s += "| ";
		}
		tmp_s += "/ " + m_current_message + "\n";
		tmp_s += m_messages[m_current_depth + 1];
		for(u8 i = 0; i < m_current_depth; ++i) {
			tmp_s += "| ";
		}
		tmp_s += "\\ " + m_current_message + ", took " + getTimeString() + "\n";
		for(u8 i = m_current_depth + 1; i < m_messages.size(); ++i) {
			m_messages[i].clear();
		}
	} else {
		std::string tmp_s;
		for(u8 i = 0; i < m_current_depth; ++i) {
			tmp_s += "| ";
		}
		m_messages[m_current_depth] += tmp_s + "+ " + m_current_message + ", took " + getTimeString() + "\n";
	}
	if(m_current_depth == 0) {
		infostream << m_messages[0] << std::endl;
		for(u8 i = 0; i < m_messages.size(); ++i) {
			m_messages[i].clear();
		}
	}
	m_has_children = true;
}

std::string ScopeProfilerTree::getTimeString()
{
	u32 delta_time = getTime(PRECISION_MICRO) - m_start_time;
	if(delta_time >= 1000) {
		return itos(delta_time / 1000) + " ms";
	} else {
		return itos(delta_time) + " us";
	}
}
