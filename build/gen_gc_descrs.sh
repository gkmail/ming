#!/bin/bash

HEADER=$1

TYPES=`grep -o 'M_GC_OBJ_[a-zA-Z]*,' $HEADER | sed s/M_GC_OBJ_// | sed s/,//`

function gen_func()
{
cat<<EOF
#define M_CASE_$1(c)\\
	case c:\\
		if (gc_obj_descrs[c].$1)\\
			gc_obj_descrs[c].$1(ptr);\\
		break;

static inline void
gc_$1 (M_GCObjType type, void *ptr)
{
	switch (type) {
EOF

grep -o 'M_GC_OBJ_[a-zA-Z]*,' $HEADER | sed s/,// | sed s/.*/M_CASE_$1\(\&\)/

cat <<EOF
	}
}

EOF
}

cat <<EOF
static const M_GCObjDescr
gc_obj_descrs[] = {
EOF

for t in $TYPES; do
	l=`echo $t | sed s'/.*/\L&/'`
	echo "{ M_GC_"$t"_FLAGS, M_GC_"$t"_SIZE, gc_"$l"_scan, gc_"$l"_final},"
done

cat <<EOF
{},
};

EOF

gen_func scan
gen_func final
