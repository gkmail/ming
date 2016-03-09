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

#ifndef _M_FUNCTION_H_
#define _M_FUNCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_hash.h"

/**Variant type.*/
typedef enum {
	M_VAR_VAR, /**< Real variant.*/
	M_VAR_ARG  /**< Argument.*/
} M_VarType;

/**Variant information.*/
typedef struct {
	M_HashNode node;  /**< Hash table node.*/
	M_Quark    quark; /**< The name of the variant.*/
	uint16_t   id;    /**< The variant's index.*/
	uint16_t   type;  /**< The variant type.*/
} M_VarInfo;

/**Native function.*/
#define M_FUNC_FL_NATIVE 1

/**Function.*/
struct M_Function_s {
	M_Module  *module;   /**< The module contains this function.*/
	uint32_t   flags;    /**< The function's flags.*/
	union {
		struct {
			M_Hash    var_hash; /**< The variant hash table.*/
			uint8_t   nframe;   /**< Operated frame count.*/
			uint16_t  bc_len;   /**< Byte code length.*/
			uint8_t  *bc;       /**< Byte code buffer.*/
		} bc;
		/**Native function.*/
		M_Result (*native)(M_Value thisv, uint32_t argc, const M_Value *argv,
					uint32_t retc, M_Value *retv);
	} f;
};

#ifdef __cplusplus
}
#endif

#endif
