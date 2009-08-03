//
// C++ Interface: qprojectm
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
	signals:
		void presetSwitchedSignal(bool hardCut, unsigned int index) const;
	public slots:


};
#endif
