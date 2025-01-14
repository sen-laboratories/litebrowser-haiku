/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the MIT license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#include "App.h"
#include "MainWindow.h"

App::App(const char* filePathOrUrl, const char* masterStylesPath, const char* userStylesPath)
	:	BApplication("application/x-vnd.af-litebrowser")
{
	MainWindow *mainwin = new MainWindow();
	if (NULL != filePathOrUrl)
	{
		mainwin->Load(filePathOrUrl, masterStylesPath, userStylesPath);
	}
	mainwin->Show();
}

int main (int argc, char *argv[])
{
	const char* filePathOrUrl = NULL;
    const char* masterCssPath = NULL;
    const char* userCssPath   = NULL;

	if (argc > 1)
	{
		filePathOrUrl = argv[1];
        if (argc > 2) {
            masterCssPath = argv[2];
            if (argc > 3) {
                userCssPath = argv[3];
            }
        }
	}
	App *app = new App(filePathOrUrl, masterCssPath, userCssPath);
	app->Run();
	delete app;
	return 0;
}
