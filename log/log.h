/**
 * @file   log.h
 * @brief  Simple logger for MESCC.
 * @author Miguel I. Garcia Lopez / FloppySoftware
 *
 * Simple logger for MESCC (Mike's Enhanced
 * Small C Compiler for Z80 & CP/M).
 *
 * Revisions:
 *  - 25 Dec 2016 : Start.
 *  - 27 Dec 2016 : Documented.
 *
 * Copyright (c) 2016 Miguel I. Garcia Lopez / FloppySoftware.
 *
 * Licensed under the GNU General Public License v3.
 *
 * http://www.floppysoftware.es
 * floppysoftware@gmail.com
 */
#ifndef LOG_H

#define LOG_H

/* Dependencies
   ------------
*/
#include <fileio.h>
#include <fprintf.h>

/* Public defs.
   ------------
*/
#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARNING 2
#define LOG_ERROR   3
#define LOG_FATAL   4

/* Private globals
   ---------------
*/
char *xlog_file;  // Filename
int xlog_level;   // Log level
int xlog_echo;    // Echo to stderr

/**
 * @fn    void LogSetFile(char *fname)
 * @brief Set the filename for logging.
 *
 * @param fname - filename
 */
LogSetFile(fname)
char *fname;
{
	xlog_file = fname;
}

/**
 * @fn    void LogSetLevel(int level)
 * @brief Set the logging level.
 *
 * Set the minimum logging level for next log outputs.
 *
 * @param level - logging level
 */
LogSetLevel(level)
int level;
{
	xlog_level = level;
}

/**
 * @fn    void LogSetEcho(int yesno)
 * @brief Set echo to stderr.
 *
 * Set or unset logging echo to stderr.
 *
 * @param yesno - non zero to set echo, zero to unset
 */
LogSetEcho(yesno)
int yesno;
{
	xlog_echo = yesno;
}

/**
 * @fn    int LogWrite(int level, char *message)
 * @brief Log output.
 *
 * Send a line to the logging file. If the level parameter
 * is equal or greater than the current logging level,
 * the message will be written in the log file. Otherwise,
 * it will be ignored.
 *
 * @param level - log level
 * @param message - log message
 *
 * @return 0 on success, -1 on failure
 */
LogWrite(level, message)
int level; char *message;
{
	FILE *fp;
	char *level_name;

	if(xlog_file)
	{
		if(level >= xlog_level)
		{
			if((fp = fopen(xlog_file, "a")))
			{
				switch(level)
				{
					case LOG_DEBUG   : level_name = "DEBUG";   break;
					case LOG_INFO    : level_name = "INFO";    break;
					case LOG_WARNING : level_name = "WARNING"; break;
					case LOG_ERROR   : level_name = "ERROR";   break;
					case LOG_FATAL   : level_name = "FATAL";   break;
					default	         : return -1;
				}

				// 2 : WARNING | Message
				fprintf(fp, "%d : %-7s | %s\n", level, level_name, message);

				if(xlog_echo)
				{
					fprintf(stderr, "%d : %-7s | %s\n", level, level_name, message);
				}

				if(ferror(fp) || fclose(fp))
				{
					// Failure: can't write
					return -1;
				}

				// Success: done
				return 0;
			}

			// Failure: can't open file
			return -1;
		}

		// Success: ignored
		return 0;
	}

	// Failure: filename not set
	return -1;
}

#endif

