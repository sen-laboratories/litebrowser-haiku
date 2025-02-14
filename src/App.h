/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the MIT license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#ifndef APP_H
#define APP_H

#include <Application.h>
#include <string>

class App : public BApplication
{
public:
	App(const char *filePathOrUrl, const char* masterStylesPath, const char* userStylesPath);
};

#endif
