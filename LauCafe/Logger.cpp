#include "Logger.h"
#include <stdio.h>

#define GL_LOG_FILE "debug.log"

#pragma once

#ifndef _INC_STDARG
#define _INC_STDARG

#include <crtdefs.h>

#if !defined (_WIN32)
#error ERROR: Only Win32 target supported!
#endif  /* !defined (_WIN32) */

#include <vadefs.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end

	void _CRTIMP __cdecl _vacopy(_Out_ va_list *, _In_ va_list);
#define va_copy(apd, aps) _vacopy(&(apd), aps)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _INC_STDARG */

bool Log(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
			);
		return false;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	fclose(file);
	return true;
}

void Print_Program_Info_Log(GLuint ShaderProgram) {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetProgramInfoLog(ShaderProgram, max_length, &actual_length, log);
	printf("program info log for GL index %u:\n%s", ShaderProgram, log);
	Log("program info log for GL index %u:\n%s", ShaderProgram, log);
}