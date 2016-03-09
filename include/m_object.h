/******************************************************************************
 * Ming: a free scripting language running platform                           *
 *----------------------------------------------------------------------------*
 * Copyright (C) 2016  L+#= +0=1 <gkmail@sina.com>                            *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 *****************************************************************************/

#ifndef _M_OBJECT_H_
#define _M_OBJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_hash.h"

/**The property is an accessor.*/
#define M_PROP_FL_ACCESSOR   1
/**The property is writable.*/
#define M_PROP_FL_WRITABLE   2
/**The property is enumerable.*/
#define M_PROP_FL_ENUMERABLE 4

/**Property.*/
typedef struct {
	M_HashNode node;  /**< Hash table node.*/
	M_Quark    quark; /**< The name of the property.*/
	uint16_t   id;    /**< The property's index.*/
	uint16_t   flags; /**< The property's flags.*/
} M_Property;

/**The object is configurable.*/
#define M_OBJ_FL_CONFIGURABLE 1
/**The object is mutable.*/
#define M_OBJ_FL_MUTABLE      2

/**Object.*/
typedef struct M_Object_s {
	M_Hash   prop_hash; /**< The properties hash table.*/
	M_Value  protov;    /**< The prototype value.*/
	M_Value *v;         /**< The property values.*/
	uint16_t nv;        /**< The number of property values.*/
	uint16_t flags;     /**< The object's flags.*/
};

#ifdef __cplusplus
}
#endif

#endif
