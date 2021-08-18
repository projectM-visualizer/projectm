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

#include "projectM.h"

#include <QObject>
#include <QString>

class QProjectM : public QObject
{

Q_OBJECT

public:
    explicit QProjectM(const QString& config_file)
        : _projectM(projectm_create(config_file.toLocal8Bit().data(), projectM::FLAG_DISABLE_PLAYLIST_LOAD))
    {
        projectm_set_preset_switched_event_callback(_projectM, &QProjectM::presetSwitchedEvent, this);
        projectm_set_preset_switch_failed_event_callback(_projectM, &QProjectM::presetSwitchFailedEvent, this);
        projectm_set_preset_rating_changed_event_callback(_projectM, &QProjectM::presetRatingChanged, this);
    }

    projectm* instance() const
    {
        return _projectM;
    }

signals:

    void presetSwitchedSignal(bool hardCut, unsigned int index) const;

    void presetSwitchFailedSignal(bool hardCut, unsigned int index, const QString& message) const;

    void presetRatingChangedSignal(unsigned int index, int rating,
                                   projectm_preset_rating_type ratingType) const;

protected:

    static void presetSwitchedEvent(bool hardCut, unsigned int index, void* context)
    {
        auto qProjectM = reinterpret_cast<QProjectM*>(context);
        qProjectM->presetSwitchedSignal(hardCut, index);
    }

    static void presetSwitchFailedEvent(bool hardCut, unsigned int index, const char* message, void* context)
    {
        auto qProjectM = reinterpret_cast<QProjectM*>(context);
        qProjectM->presetSwitchFailedSignal(hardCut, index, QString(message));
    }

    static void presetRatingChanged(unsigned int index, int rating,
                                    projectm_preset_rating_type ratingType, void* context)
    {
        auto qProjectM = reinterpret_cast<QProjectM*>(context);
        qProjectM->presetRatingChangedSignal(index, rating, ratingType);
    }

    projectm* _projectM{ nullptr };
};

#endif
