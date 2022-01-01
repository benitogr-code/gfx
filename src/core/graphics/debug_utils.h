#pragma once

void glCheckError_(const char *file, int line);

#define GL_CHECK_ERROR() glCheckError_(__FILE__, __LINE__)
