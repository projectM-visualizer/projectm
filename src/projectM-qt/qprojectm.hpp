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

#ifndef QPROJECTM_HPP
#define QPROJECTM_HPP
#include "projectM.hpp"
#include <QObject>

class QProjectM : public QObject, public projectM {
	
	Q_OBJECT

	public:
		QProjectM(const std::string & config_file):projectM(config_file, projectM::FLAG_DISABLE_PLAYLIST_LOAD) {}

		void presetSwitchedEvent(bool hardCut, unsigned int index) const {
			presetSwitchedSignal(hardCut, index);
		}

		void presetRatingChanged(unsigned int index, int rating,
						PresetRatingType ratingType) const {
			 presetRatingChangedSignal(index, rating, ratingType);
		}

	signals:
		void presetSwitchedSignal(bool hardCut, unsigned int index) const;
		void presetRatingChangedSignal(unsigned int index, int rating,
				PresetRatingType ratingType) const;

	public slots:


};
#endif
