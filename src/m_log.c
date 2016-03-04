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

#define M_LOG_TAG "log"

#include <m_log.h>

#define M_LOG_FL_TAG   1
#define M_LOG_FL_FILE  2
#define M_LOG_FL_FUNC  4
#define M_LOG_FL_LINE  8
#define M_LOG_FL_DATE  16
#define M_LOG_FL_TIME  32
#define M_LOG_FL_USEC  64
#define M_LOG_FL_LEVEL 128
#define M_LOG_FL_ALL  0xffffffff

static pthread_mutex_t lock;
static FILE      *log_fp;
static M_LogLevel log_level;
static uint32_t   log_flags;

void
m_log_startup (void)
{
	const char *val;

	/*Mutex initialize.*/
	pthread_mutex_init(&lock, NULL);

	/*Open log file.*/
	val = getenv("M_LOG_FILE");
	if (val) {
		log_fp = fopen(val, "ab");
	} else {
		log_fp = NULL;
	}

	if (!log_fp) {
		log_fp = stdout;
	}

	/*Get log level.*/
	val = getenv("M_LOG_LEVEL");
	if (val) {
#define LOG_LEVEL_CHECK(name)\
		if (!strcasecmp(val, #name)) {\
			log_level = M_LOG_LEVEL_##name;\
		} else

		LOG_LEVEL_CHECK(ALL)
		LOG_LEVEL_CHECK(DEBUG)
		LOG_LEVEL_CHECK(INFO)
		LOG_LEVEL_CHECK(WARNING)
		LOG_LEVEL_CHECK(ERROR)
		LOG_LEVEL_CHECK(FATAL)
		LOG_LEVEL_CHECK(NONE)
		{
			log_level = M_LOG_LEVEL_ALL;
		}
	} else {
		log_level = M_LOG_LEVEL_NONE;
	}

	/*Get log pattern.*/
	val = getenv("M_LOG_PATTERN");
	if (val) {
		log_flags = 0;

#define LOG_FLAG_CHECK(name, ch)\
		if (strchr(val, ch)) {\
			log_flags |= M_LOG_FL_##name;\
		}

		LOG_FLAG_CHECK(TAG,  'T')
		LOG_FLAG_CHECK(LEVEL,'L')
		LOG_FLAG_CHECK(FILE, 'F')
		LOG_FLAG_CHECK(FUNC, 'f')
		LOG_FLAG_CHECK(LINE, 'l')
		LOG_FLAG_CHECK(DATE, 'd')
		LOG_FLAG_CHECK(TIME, 't')
		LOG_FLAG_CHECK(USEC, 'u')
	} else {
		log_flags = M_LOG_FL_ALL;
	}
}

void
m_log_shutdown (void)
{
	if (log_fp && (log_fp != stdout)) {
		fclose(log_fp);
	}

	pthread_mutex_destroy(&lock);
}

void
m_log (M_LogLevel level, const char *tag, const char *file, const char *func,
		int line, const char *fmt, ...)
{
	va_list ap;
	M_Bool sep = M_FALSE;

	if (level < log_level)
		return;

	pthread_mutex_lock(&lock);

	/*Output level information.*/
	if (log_flags & M_LOG_FL_LEVEL) {
		char ch;

		switch (level) {
			case M_LOG_LEVEL_DEBUG:
				ch = 'D';
				break;
			case M_LOG_LEVEL_INFO:
			default:
				ch = 'I';
				break;
			case M_LOG_LEVEL_WARNING:
				ch = 'W';
				break;
			case M_LOG_LEVEL_ERROR:
				ch = 'E';
				break;
			case M_LOG_LEVEL_FATAL:
				ch = 'F';
				break;
		}

		fprintf(log_fp, "%c", ch);
		sep = M_TRUE;
	}

	/*Output tag.*/
	if (log_flags & M_LOG_FL_TAG) {
		if (sep)
			fprintf(log_fp, "/");

		fprintf(log_fp, "%8s", tag);
		sep = M_TRUE;
	}

	/*Output filename.*/
	if (log_flags & M_LOG_FL_FILE) {
		if (sep)
			fprintf(log_fp, ":");

		fprintf(log_fp, "\"%15s\"", file);
		sep = M_TRUE;
	}

	/*Output function name.*/
	if (log_flags & M_LOG_FL_FUNC) {
		if (sep)
			fprintf(log_fp, " ");

		fprintf(log_fp, "(%10s)", func);
		sep = M_TRUE;
	}

	/*Output line number*/
	if (log_flags & M_LOG_FL_LINE) {
		if (sep)
			fprintf(log_fp, " ");

		fprintf(log_fp, "%5d", line);
		sep = M_TRUE;
	}

	/*Output date and time.*/
	if (log_flags & (M_LOG_FL_DATE | M_LOG_FL_TIME | M_LOG_FL_USEC)) {
		struct timespec ts;
		struct tm *date;

		if (sep)
			fprintf(log_fp, " ");

		clock_gettime(CLOCK_REALTIME, &ts);

		if (log_flags & (M_LOG_FL_DATE | M_LOG_FL_TIME)) {
			date = localtime(&ts.tv_sec);

			if (log_flags & M_LOG_FL_DATE) {
				fprintf(log_fp, "%04d-%02d-%02d", date->tm_year + 1900,
							date->tm_mon + 1,
							date->tm_mday);
			}

			if (log_flags & M_LOG_FL_TIME) {
				if (log_flags & M_LOG_FL_DATE)
					fprintf(log_fp, " ");

				fprintf(log_fp, "%02d:%02d:%02d", date->tm_hour,
							date->tm_min,
							date->tm_sec);
			}
		}

		if (log_flags & M_LOG_FL_TIME) {
			fprintf(log_fp, ".%06d", ts.tv_nsec / 1000);
		} else {
			if (log_flags & M_LOG_FL_DATE)
				fprintf(log_fp, " ");

			fprintf(log_fp, "%10u", ts.tv_nsec / 1000 + ts.tv_sec * 1000000);
		}

		sep = M_TRUE;
	}

	if (sep)
		fprintf(log_fp, ": ");

	/*Output message.*/
	va_start(ap, fmt);
	vfprintf(log_fp, fmt, ap);
	va_end(ap);

	fprintf(log_fp, "\n");

	pthread_mutex_unlock(&lock);
}

