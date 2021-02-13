/* GStreamer
 * Copyright (C) 2019 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _Ugst_UprojectM_H_
#define _Ugst_UprojectM_H_


G_BEGIN_DECLS

#define Ugst_TYPE_UprojectM   (gst_LprojectM_get_type())
#define Ugst_UprojectM(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),Ugst_TYPE_UprojectM,UgstUprojectM))
#define Ugst_UprojectM_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),Ugst_TYPE_UprojectM,UgstUprojectMClass))
#define Ugst_IS_UprojectM(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),Ugst_TYPE_UprojectM))
#define Ugst_IS_UprojectM_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),Ugst_TYPE_UprojectM))

typedef struct _UgstUprojectM UgstUprojectM;
typedef struct _UgstUprojectMClass UgstUprojectMClass;

struct _UgstUprojectM
{
   base_LprojectM;

};

struct _UgstUprojectMClass
{
  Class base_LprojectM_class;
};

GType gst_LprojectM_get_type (void);

G_END_DECLS

#endif
