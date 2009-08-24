/**
 * projectM-qt -- Qt4 based projectM GUI 
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#ifndef NULLABLE_HPP
#define NULLABLE_HPP

template <class Value>
class Nullable {
	public:
		Nullable(const Value & value) : m_value(value), m_hasValue(true) {}
		Nullable() :m_hasValue(false) {}
	
		Nullable & operator=(const Value & value) {
			m_value = value;
			m_hasValue = true;
		}
	
		void nullify() {
			m_hasValue = false;
		}
	
		const Value & value() const { return m_value;}		
		bool hasValue() const { return m_hasValue;}
	private:
		Value m_value;
		bool m_hasValue;
};
#endif
