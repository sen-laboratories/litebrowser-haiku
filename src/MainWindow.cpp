/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the MIT license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#include "MainWindow.h"

#include <Application.h>

#include <Url.h>
#include <private/netservices/UrlProtocolDispatchingListener.h>
#include <private/netservices2/HttpSession.h>
#include <private/netservices2/HttpRequest.h>
#include <private/netservices2/HttpResult.h>
#include <private/netservices2/NetServicesDefs.h>

#include <LayoutBuilder.h>
#include <ScrollBar.h>
#include <String.h>
#include <Window.h>
#include <TextView.h>

#include <iostream>

using namespace BPrivate::Network;

MainWindow::MainWindow(litehtml::formatting_context* ctx)
	:	BWindow(BRect(100,100,500,400),"litebrowser",B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE),
	fContext(ctx),
	fHtmlView(NULL),
	vScroll(NULL),
	hScroll(NULL),
	fDataReceived()
{
	BRect bounds(Bounds());
	// change bounds for scrollbar size

	bounds.bottom = bounds.bottom - 20;
	bounds.right = bounds.right - 20;

	BRect hBounds(bounds);
	hBounds.InsetBy(10,10);
	fHtmlView = new LiteHtmlView(hBounds, "html");
	fHtmlView->SetContext(ctx);

	BGroupLayout* vGroup = new BGroupLayout(B_VERTICAL, 0);
	vGroup->SetInsets(-1, -1, -1, -1);
	SetLayout(vGroup);

	BGroupLayout* hGroup = new BGroupLayout(B_HORIZONTAL, 0);
	hGroup->SetInsets(0, -1, -1, -1); // hides scroll bar borders
	BView* hView = new BView("hview", 0, hGroup);

	BLayoutItem* htmlView = hGroup->AddView(fHtmlView);
	htmlView->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH,
		B_ALIGN_USE_FULL_HEIGHT));

	vScroll = new BScrollBar("htmlscroll",
		fHtmlView, 0, 100, B_VERTICAL);
	vScroll->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	BLayoutItem* liScrollBar = hGroup->AddView(vScroll);
	liScrollBar->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT,
		B_ALIGN_USE_FULL_HEIGHT));

	BLayoutItem* hGroupView = vGroup->AddView(hView);
	hGroupView->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH,
		B_ALIGN_USE_FULL_HEIGHT));

	hScroll = new BScrollBar("hhtmlscroll",
		fHtmlView,0,100,B_HORIZONTAL);
	hScroll->SetResizingMode(B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	BLayoutItem* liHScrollBar = vGroup->AddView(hScroll);
	liHScrollBar->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH,
		B_ALIGN_BOTTOM));
	// TODO not full width, minux 20 px due to document resize handle

	//BTextView* txt = new BTextView("text");
	//vGroup->AddView(txt);
	//txt->SetText("Some text here");

	fHtmlView->StartWatching(this, M_HTML_RENDERED);

	UpdateScrollbars();
}

void MainWindow::Load(const char* filePathOrUrl)
{
    fHtmlView->RenderUrl(filePathOrUrl);
}

void
MainWindow::MessageReceived(BMessage *msg)
{
	int32 originalWhat;
	int8 protocolWhat;
	BString str;
	switch (msg->what)
	{
		case B_URL_PROTOCOL_NOTIFICATION:
		{
			std::cout << "  UrlRequest update received" << std::endl;
			// data loading, request finished, or download progress
			if (B_OK == msg->FindInt8("be:urlProtocolMessageType",&protocolWhat))
			{
				switch (protocolWhat)
				{
					// TODO don't assume it's just a web page data
					//      - could be image, css, etc.
					case B_URL_PROTOCOL_DOWNLOAD_PROGRESS:
					{
						std::cout << "  Data received" << std::endl;
						if (B_OK == msg->FindString("url:data",&str))
						{
							fDataReceived += str;
						}
						break;
					}
					case B_URL_PROTOCOL_REQUEST_COMPLETED:
					{
						std::cout << "  BUrlRequest complete" << std::endl;
						// TODO check success flag
                        fHtmlView->RenderHtml(fDataReceived);    // FIXME: not needed, at least for local files
                        fDataReceived.Truncate(0);
						break;
					}
					default:
					{
						// ignore
					}
				}
			}
			break;
		}
		case B_OBSERVER_NOTICE_CHANGE:
		{
			if (B_OK == msg->FindInt32(B_OBSERVE_ORIGINAL_WHAT,&originalWhat))
			{
				switch (originalWhat)
				{
					case M_HTML_RENDERED:
					{
						std::cout << "HTML_RENDERED received" << std::endl;
						UpdateScrollbars();
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		default:
		{
			std::cout << "Message received: '" << msg->what
					  << "' (HTML Rendered: '" << M_HTML_RENDERED
					  << "')" << std::endl;
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void
MainWindow::ScreenChanged(BRect	screenSize, color_space	depth)
{
	fHtmlView->Invalidate();
	UpdateScrollbars();
}

void
MainWindow::FrameResized(float newWidth, float newHeight)
{
	fHtmlView->Invalidate();
	UpdateScrollbars();
}

void
MainWindow::UpdateScrollbars()
{
	// For size of doc and length randered
	float pWidth,pHeight;
	fHtmlView->GetPreferredSize(&pWidth,&pHeight);
	// actual current size of view
	BRect size(fHtmlView->Bounds());
	// now set scroll bars
	// Don't forget the 10px insets for the view on each side
	hScroll->SetRange(0,pWidth <= size.Width() ? 0 : pWidth - size.Width() + 20);
	hScroll->SetSteps(10,size.Width());
	vScroll->SetRange(0,pHeight <= size.Height() ? 0 : pHeight - size.Height() + 20);
	vScroll->SetSteps(10,size.Height());
}
