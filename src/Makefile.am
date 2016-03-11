lib_LTLIBRARIES=libming.la
libming_la_SOURCES=\
	m_startup.c\
	m_log.c\
	m_gc.c\
	m_gc_map.c\
	m_gc_obj.c\
	m_gc_root.c\
	m_gc_buf.c\
	m_thread.c

m_gc_descrs.c: ../include/m_gc.h
	../build/gen_gc_descrs.sh $< > $@

m_gc_obj.c: m_gc_descrs.c
