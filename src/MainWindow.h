/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the MIT license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ScrollView.h>
#include <Window.h>

#include "litehtml.h"
#include "containers/haiku/container_haiku.h"

class MainWindow : public BWindow
{
public:
						MainWindow();
			void		Load(const char* filePathOrUrl, const char* masterStylesPath, const char* userStylesPath);

	// BWindow overrides
			void		MessageReceived(BMessage *msg) override;
	virtual void		ScreenChanged(BRect	screenSize, color_space	depth) override;
	virtual void		FrameResized(float newWidth, float newHeight) override;

protected:
    void                UpdateScrollBars();

private:
	LiteHtmlView* 		fHtmlView;
    BScrollView*        fScrollView;
    BScrollBar*         fScrollBarHorizontal;
    BScrollBar*         fScrollBarVertical;
	BString		        fDataReceived;
};

#endif
